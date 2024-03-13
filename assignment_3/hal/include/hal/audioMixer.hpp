// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef _AUDIO_MIXER_HPP_
#define _AUDIO_MIXER_HPP_

#include <alsa/asoundlib.h>
#include <stdbool.h>

#include <mutex>
#include <thread>

#define MAX_SOUND_BITES 30

typedef struct {
    int numSamples;
    short* pData;
} WaveData;

typedef struct {
    // A pointer to a previously allocated sound bite (WaveData struct).
    // Note that many different sound-bite slots could share the same pointer
    // (overlapping cymbal crashes, for example)
    WaveData* pSound;

    // The offset into the pData of pSound. Indicates how much of the
    // sound has already been played (and hence where to start playing next).
    int location;
} PlaybackSound;

class AudioMixer {
   private:
    bool isRunning;
    int volume;
    snd_pcm_t* handle;
    std::mutex audioMutex;
    std::thread playbackThread;
    unsigned long playbackBufferSize;
    short* playbackBuffer;
    void fillPlaybackBuffer(short* buff, int size);
    PlaybackSound soundBites[MAX_SOUND_BITES];

   public:
    AudioMixer();
    // init() must be called before any other functions,
    // stop() must be called last to stop playback threads and free memory.
    void init(void);
    void stop(void);

    // Read the contents of a wave file into the pSound structure. Note that
    // the pData pointer in this structure will be dynamically allocated in
    // readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
    void readWaveFileIntoMemory(char* fileName, WaveData* pSound);
    void freeWaveFileData(WaveData* pSound);

    // Queue up another sound bite to play as soon as possible.
    void queueSound(WaveData* pSound);

    // Get/set the volume.
    // setVolume() function posted by StackOverflow user "trenki" at:
    // http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
    int getVolume(void);
    void setVolume(int newVolume);
};
#endif