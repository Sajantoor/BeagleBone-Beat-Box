#ifndef _PERIOD_TIMER_HPP_
#define _PERIOD_TIMER_HPP_

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Module to record and report the timing of periodic events.
//     Inspired by the PeriodTimer code by Brian Fraser
// Usage:
//  1. Define your Period_whichEvent enums of interest
//     (ensure NUM_PERIOD_EVENTS is last).
//  2. Call Period_markEvent() periodically to mark each
//     occurrence of the event. For example, call this function
//     each time you sample the A2D.
//  3. Call getStatisticsAndClear() to get the statistics for
//     an event of interest. Calling this will clear the
//     data collected for this event (but not others).
//     For example, call this function once a second to get timing
//     information to print to the screen.

// Maximum number of timestamps to record for a given event.
#define MAX_EVENT_TIMESTAMPS (1024 * 4)

enum Period_whichEvent {
    PERIOD_EVENT_AUDIO,
    PERIOD_EVENT_ACCELEROMETER,
    NUM_PERIOD_EVENTS
};

typedef struct {
    int numSamples;
    double minPeriodInMs;
    double maxPeriodInMs;
    double avgPeriodInMs;
} Period_statistics_t;

typedef struct {
    // Store the timestamp samples each time we mark an event.
    long timestampCount;
    long long timestampsInNs[MAX_EVENT_TIMESTAMPS];

    // Used for recording the event between analysis periods.
    long long prevTimestampInNs;
} timestamps_t;

/**
 * This class is responsible for recording and reporting the timing of periodic
 * events.
 */
class Period {
   public:
    // Initialize/cleanup the module's data structures.
    Period(void);
    void cleanup(void);

    // Record the current time as a timestamp for the
    // indicated event. This allows later calls to
    // Period_getStatisticsAndClear() to access these timestamps
    // and compute the timing statistics for this periodic event.
    void markEvent(enum Period_whichEvent whichEvent);

    // Fill the `pStats` struct, which must be allocated by the calling
    // code, with the statistics about the periodic event `whichEvent`.
    // This function is threadsafe, and may be called by any thread.
    // Calling this function will, after it computes the timing
    // statistics, clear the data stored for this event.
    void getStatisticsAndClear(enum Period_whichEvent whichEvent,
                               Period_statistics_t* pStats);

   private:
    void updateStats(timestamps_t* pData, Period_statistics_t* pStats);
    long long getTimeInNanoS(void);
    timestamps_t s_eventData[NUM_PERIOD_EVENTS];
    pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;
    bool s_initialized = false;
};

#endif
