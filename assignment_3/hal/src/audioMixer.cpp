#include "hal/audioMixer.hpp"
#include <alloca.h>  // needed for mixer
#include <limits.h>
#include <pthread.h>
#include <stdbool.h>

#define DEFAULT_VOLUME 80
#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short))  // bytes per sample
#define AUDIOMIXER_MAX_VOLUME 100

AudioMixer::AudioMixer() {
    isRunning = false;
    volume = 0;
    playbackBufferSize = 0;
    playbackBuffer = NULL;
}

void AudioMixer::init(void) {
    setVolume(DEFAULT_VOLUME);
    isRunning = true;

    // Initialize the currently active sound-bites being played
    for (int i = 0; i < MAX_SOUND_BITES; i++) {
        soundBites[i].pSound = NULL;
        soundBites[i].location = -1;
    }

    // Open the PCM output
    int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Configure parameters of PCM output
    err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED, NUM_CHANNELS, SAMPLE_RATE,
                             1,       // Allow software resampling
                             50000);  // 0.05 seconds per buffer
    if (err < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        exit(EXIT_FAILURE);
    }

    // Allocate this software's playback buffer to be the same size as the
    // the hardware's playback buffers for efficient data transfers.
    unsigned long unusedBufferSize = 0;
    snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
    playbackBuffer = (short*)malloc(playbackBufferSize * sizeof(*playbackBuffer));

    // Launch playback thread
    playbackThread = std::thread([this] {
        while (isRunning) {
            // Generate next block of audio
            fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

            // Output the audio
            snd_pcm_sframes_t frames = snd_pcm_writei(handle, playbackBuffer, playbackBufferSize);

            // Check for (and handle) possible error conditions on output
            if (frames < 0) {
                fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
                frames = snd_pcm_recover(handle, frames, 1);
            }
            if (frames < 0) {
                fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n", frames);
                exit(EXIT_FAILURE);
            }
            if (frames > 0 && (unsigned long)frames < playbackBufferSize) {
                printf("Short write (expected %li, wrote %li)\n", playbackBufferSize, frames);
            }
        }
    });
}

// Client code must call freeWaveFileData to free dynamically allocated data.
void AudioMixer::readWaveFileIntoMemory(char* fileName, WaveData* pSound) {
    assert(pSound);

    // The PCM data in a wave file starts after the header:
    const int PCM_DATA_OFFSET = 44;

    // Open the wave file
    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
        exit(EXIT_FAILURE);
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
    pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

    // Search to the start of the data in the file
    fseek(file, PCM_DATA_OFFSET, SEEK_SET);

    // Allocate space to hold all PCM data
    pSound->pData = (short*)malloc(sizeInBytes);
    if (pSound->pData == 0) {
        fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n", sizeInBytes, fileName);
        exit(EXIT_FAILURE);
    }

    // Read PCM data from wave file into memory
    int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
    if (samplesRead != pSound->numSamples) {
        fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n", pSound->numSamples, fileName,
                samplesRead);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void AudioMixer::freeWaveFileData(WaveData* pSound) {
    pSound->numSamples = 0;
    free(pSound->pData);
    pSound->pData = NULL;
}

void AudioMixer::queueSound(WaveData* pSound) {
    // Ensure we are only being asked to play "good" sounds:
    assert(pSound->numSamples > 0);
    assert(pSound->pData);

    // Insert the sound by searching for an empty sound bite spot
    /*
	 * REVISIT: Implement this:
	 * 1. Since this may be called by other threads, and there is a thread
	 *    processing the soundBites[] array, we must ensure access is threadsafe.
	 * 2. Search through the soundBites[] array looking for a free slot.
	 * 3. If a free slot is found, place the new sound file into that slot.
	 *    Note: You are only copying a pointer, not the entire data of the wave file!
	 * 4. After searching through all slots, if no free slot is found then print
	 *    an error message to the console (and likely just return vs asserting/exiting
	 *    because the application most likely doesn't want to crash just for
	 *    not being able to play another wave file.
	 */
    if (!isRunning) return;
    audioMutex.lock();
    for (int i = 0; i < MAX_SOUND_BITES; i++) {
        if (soundBites[i].pSound == NULL) {
            soundBites[i].pSound = pSound;
            soundBites[i].location = 0;
            audioMutex.unlock();
            return;
        }
    }
    audioMutex.unlock();
    perror("No free sound bite slots available to play sound.\n");
}

void AudioMixer::stop(void) {
    printf("Stopping audio...\n");

    // Stop the PCM generation thread
    isRunning = false;
    playbackThread.join();

    // Shutdown the PCM output, allowing any pending sound to play out (drain)
    snd_pcm_drain(handle);
    snd_pcm_close(handle);

    // Free playback buffer
    // (note that any wave files read into WaveData records must be freed
    //  in addition to this by calling freeWaveFileData() on that struct.)
    free(playbackBuffer);
    playbackBuffer = NULL;

    printf("Done stopping audio...\n");
    fflush(stdout);
}

int AudioMixer::getVolume() {
    // Return the cached volume; good enough unless someone is changing
    // the volume through other means and the cached value is out of date.
    return volume;
}

void AudioMixer::setVolume(int newVolume) {
    // Ensure volume is reasonable; If so, cache it for later getVolume() calls.
    if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
        printf("ERROR: Volume must be between 0 and 100.\n");
        return;
    }
    volume = newVolume;

    long min, max;
    snd_mixer_t* volHandle;
    snd_mixer_selem_id_t* sid;
    const char* card = "default";
    const char* selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}

// Fill the `buff` array with new PCM values to output.
//    `buff`: buffer to fill with new PCM data from sound bites.
//    `size`: the number of values to store into playbackBuffer
void AudioMixer::fillPlaybackBuffer(short* buff, int size) {
    /*
	 * 1. Wipe the playbackBuffer to all 0's to clear any previous PCM data.
	 *    Hint: use memset()
	 * 2. Since this is called from a background thread, and soundBites[] array
	 *    may be used by any other thread, must synchronize this.
	 * 3. Loop through each slot in soundBites[], which are sounds that are either
	 *    waiting to be played, or partially already played:
	 *    - If the sound bite slot is unused, do nothing for this slot.
	 *    - Otherwise "add" this sound bite's data to the play-back buffer
	 *      (other sound bites needing to be played back will also add to the same data).
	 *      * Record that this portion of the sound bite has been played back by incrementing
	 *        the location inside the data where play-back currently is.
	 *      * If you have now played back the entire sample, free the slot in the
	 *        soundBites[] array.
	 *
	 * Notes on "adding" PCM samples:
	 * - PCM is stored as signed shorts (between SHRT_MIN and SHRT_MAX).
	 * - When adding values, ensure there is not an overflow. Any values which would
	 *   greater than SHRT_MAX should be clipped to SHRT_MAX; likewise for underflow.
	 * - Don't overflow any arrays!
	 * - Efficiency matters here! The compiler may do quite a bit for you, but it doesn't
	 *   hurt to keep it in mind. Here are some tips for efficiency and readability:
	 *   * If, for each pass of the loop which "adds" you need to change a value inside
	 *     a struct inside an array, it may be faster to first load the value into a local
	 *      variable, increment this variable as needed throughout the loop, and then write it
	 *     back into the struct inside the array after. For example:
	 *           int offset = myArray[someIdx].value;
	 *           for (int i =...; i < ...; i++) {
	 *               offset ++;
	 *           }
	 *           myArray[someIdx].value = offset;
	 *   * If you need a value in a number of places, try loading it into a local variable
	 *          int someNum = myArray[someIdx].value;
	 *          if (someNum < X || someNum > Y || someNum != Z) {
	 *              someNum = 42;
	 *          }
	 *          ... use someNum vs myArray[someIdx].value;
	 *
	 */
    audioMutex.lock();
    memset(buff, 0, size * sizeof(short));

    for (int i = 0; i < MAX_SOUND_BITES; i++) {
        if (soundBites[i].pSound != NULL) {
            int location = soundBites[i].location;
            int numSamples = soundBites[i].pSound->numSamples;
            short* pData = soundBites[i].pSound->pData;
            int curSample = 0;
            int mixedSample = 0;

            for (int j = 0; j < size; j++) {
                // If the sound bite has been played, free the slot
                if (location >= numSamples) {
                    soundBites[i].pSound = NULL;
                    soundBites[i].location = -1;
                    break;
                }

                // try to mix the sound bite with the current buffer
                curSample = pData[location];
                mixedSample = buff[j] + curSample;

                // check for overflow
                if (mixedSample > SHRT_MAX) {
                    buff[j] = SHRT_MAX;
                } else if (mixedSample < SHRT_MIN) {
                    buff[j] = SHRT_MIN;
                } else {
                    buff[j] = (short)mixedSample;
                }
                location++;
            }

            soundBites[i].location = location;
        }
    }
    audioMutex.unlock();
}
