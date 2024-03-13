#include "generateBeat.hpp"
#include <common/utils.hpp>

static char BASE_DRUM_FILE[] = "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav";
static char HI_HAT_FILE[] = "beatbox-wav-files/100053__menegass__gui-drum-cc.wav";
static char SNARE_FILE[] = "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav";
static constexpr unsigned int MAX_TEMPO = 300;
static constexpr unsigned int MIN_TEMPO = 40;
static constexpr long long DEFAULT_DELAY = 150;

GenerateBeat::GenerateBeat(AudioMixer* audioMixer) {
    isRunning = false;
    beatType = ROCK_DRUM;  // Default to rock drum beat
    bpm = 120;             // Default to 120 beats per minute
    this->audioMixer = audioMixer;
}

void GenerateBeat::init(void) {
    isRunning = true;
    audioMixer->readWaveFileIntoMemory(BASE_DRUM_FILE, &baseDrumData);
    audioMixer->readWaveFileIntoMemory(HI_HAT_FILE, &hiHatData);
    audioMixer->readWaveFileIntoMemory(SNARE_FILE, &snareData);
    beatThread = std::thread([this] {
        while (isRunning) {
            // Music is often laid out in measures of 4 beats.
            // Each pass through the loop corresponds to a 1/2 beat.
            long long delay = 60000 / bpm / 2;

            switch (beatType) {
                case NONE:
                    sleepForMs(DEFAULT_DELAY);
                    break;
                case ROCK_DRUM:
                    // play bass drum and hi-hat
                    audioMixer->queueSound(&baseDrumData);
                    audioMixer->queueSound(&hiHatData);
                    sleepForMs(delay);

                    // play hi-hat
                    audioMixer->queueSound(&hiHatData);
                    sleepForMs(delay);

                    // play snare and hi-hat
                    audioMixer->queueSound(&hiHatData);
                    audioMixer->queueSound(&snareData);
                    sleepForMs(delay);

                    // play hi-hat
                    audioMixer->queueSound(&hiHatData);
                    sleepForMs(delay);
                    break;
                case CUSTOM:
                    audioMixer->queueSound(&hiHatData);
                    audioMixer->queueSound(&snareData);
                    sleepForMs(delay);

                    audioMixer->queueSound(&baseDrumData);
                    sleepForMs(delay);

                    audioMixer->queueSound(&baseDrumData);
                    sleepForMs(delay);

                    audioMixer->queueSound(&snareData);
                    audioMixer->queueSound(&hiHatData);
                    sleepForMs(delay);
                    break;
                default:
                    perror("Invalid beat type");
                    sleepForMs(DEFAULT_DELAY);
                    break;
            }
        }
    });
}

void GenerateBeat::stop(void) {
    isRunning = false;
    beatThread.join();
    audioMixer->freeWaveFileData(&baseDrumData);
    audioMixer->freeWaveFileData(&hiHatData);
    audioMixer->freeWaveFileData(&snareData);
}

void GenerateBeat::playSnare(void) {
    audioMixer->queueSound(&snareData);
}

void GenerateBeat::playBaseDrum(void) {
    audioMixer->queueSound(&baseDrumData);
}

void GenerateBeat::playHiHat(void) {
    audioMixer->queueSound(&hiHatData);
}

void GenerateBeat::increaseTempo(void) {
    if (bpm < MAX_TEMPO) {
        bpm += 5;
    }
}

void GenerateBeat::decreaseTempo(void) {
    if (bpm > MIN_TEMPO) {
        bpm -= 5;
    }
}

void GenerateBeat::increaseVolume(void) {
    audioMixer->setVolume(audioMixer->getVolume() + 5);
}

void GenerateBeat::decreaseVolume(void) {
    audioMixer->setVolume(audioMixer->getVolume() - 5);
}

void GenerateBeat::setBeatType(BEAT_TYPE beatType) {
    this->beatType = beatType;
}

void GenerateBeat::cycleBeatType(void) {
    this->beatType = static_cast<BEAT_TYPE>((this->beatType + 1) % NUM_BEATS);
}