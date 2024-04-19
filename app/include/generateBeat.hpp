#ifndef _GENERATE_BEAT_HPP_
#define _GENERATE_BEAT_HPP_

#include <thread>

#include "hal/audioMixer.hpp"
#include "socket.hpp"

typedef enum { NONE, ROCK_DRUM, CUSTOM, NUM_BEATS } BEAT_TYPE;

/**
 * Interface for handling the beat generation and updating the audio mixer,
 * tempo, and volume. The beat generation is done in a separate thread.
 */
class GenerateBeat {
   private:
    bool isRunning;
    unsigned int bpm;
    AudioMixer* audioMixer;
    BEAT_TYPE beatType;
    std::thread beatThread;
    WaveData baseDrumData;
    WaveData hiHatData;
    WaveData snareData;
    Socket* socket;

   public:
    GenerateBeat(AudioMixer* audioMixer, Socket* socket);
    // init() must be called before any other functions,
    // stop() must be called last to stop beat generation
    void init(void);
    void stop(void);

    // Play sound
    void playSnare(void);
    void playBassDrum(void);
    void playHiHat(void);

    // Control the beat's tempo (in bpm) in range [40, 300] BPM (inclusive);
    // default 120 BPM.
    void increaseTempo(void);
    void decreaseTempo(void);
    unsigned int getTempo(void);

    // Control the beat's volume in range [0, 100] (inclusive); default 80.
    void increaseVolume(void);
    void decreaseVolume(void);
    int getVolume(void);

    // Set beat
    void setBeatType(BEAT_TYPE beatType);
    void cycleBeatType(void);
    BEAT_TYPE getBeatType(void);
};

#endif