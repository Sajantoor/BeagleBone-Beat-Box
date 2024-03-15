#include "hal/periodTimer.hpp"

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Inspired by the PeriodTimer code by Brian Fraser
Period::Period(void) {
    s_lock = PTHREAD_MUTEX_INITIALIZER;
    memset(s_eventData, 0, sizeof(s_eventData[0]) * NUM_PERIOD_EVENTS);
    s_initialized = true;
}

void Period::cleanup(void) { s_initialized = false; }

void Period::markEvent(enum Period_whichEvent whichEvent) {
    assert(whichEvent >= 0 && whichEvent < NUM_PERIOD_EVENTS);
    assert(s_initialized);
    timestamps_t* pData = &s_eventData[whichEvent];
    pthread_mutex_lock(&s_lock);
    {
        if (pData->timestampCount < MAX_EVENT_TIMESTAMPS) {
            pData->timestampsInNs[pData->timestampCount] = getTimeInNanoS();
            pData->timestampCount++;
        } else {
            printf("WARNING: No sample space for event collection on %d\n",
                   whichEvent);
        }
    }
    pthread_mutex_unlock(&s_lock);
}

void Period::getStatisticsAndClear(enum Period_whichEvent whichEvent,
                                   Period_statistics_t* pStats) {
    assert(whichEvent >= 0 && whichEvent < NUM_PERIOD_EVENTS);
    assert(s_initialized);
    timestamps_t* pData = &s_eventData[whichEvent];
    pthread_mutex_lock(&s_lock);
    {
        // Compute stats
        updateStats(pData, pStats);

        // Update the "previous" sample (if we have any)
        if (pData->timestampCount > 0) {
            pData->prevTimestampInNs =
                pData->timestampsInNs[pData->timestampCount - 1];
        }

        // Clear
        pData->timestampCount = 0;
    }
    pthread_mutex_unlock(&s_lock);
}

void Period::updateStats(timestamps_t* pData, Period_statistics_t* pStats) {
    long long prevInNs = pData->prevTimestampInNs;

    // Handle startup (no previous sample)
    if (prevInNs == 0) {
        prevInNs = pData->timestampsInNs[0];
    }

    // Find min/max/sum time delta between consecutive samples
    long long sumDeltasNs = 0;
    long long minNs = 0;
    long long maxNs = 0;
    for (int i = 0; i < pData->timestampCount; i++) {
        long long thisTime = pData->timestampsInNs[i];
        long long deltaNs = thisTime - prevInNs;
        sumDeltasNs += deltaNs;

        if (i == 0 || deltaNs < minNs) {
            minNs = deltaNs;
        }
        if (i == 0 || deltaNs > maxNs) {
            maxNs = deltaNs;
        }

        prevInNs = thisTime;
    }

    long long avgNs = 0;
    if (pData->timestampCount > 0) {
        avgNs = sumDeltasNs / pData->timestampCount;
    }

// Save stats
#define MS_PER_NS (1000 * 1000.0)
    pStats->minPeriodInMs = minNs / MS_PER_NS;
    pStats->maxPeriodInMs = maxNs / MS_PER_NS;
    pStats->avgPeriodInMs = avgNs / MS_PER_NS;
    pStats->numSamples = pData->timestampCount;
}

// Timing function
long long Period::getTimeInNanoS(void) {
    struct timespec spec;
    clock_gettime(CLOCK_BOOTTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec + seconds * 1000 * 1000 * 1000;
    assert(nanoSeconds > 0);

    static long long lastTimeHack = 0;
    assert(nanoSeconds > lastTimeHack);
    lastTimeHack = nanoSeconds;

    return nanoSeconds;
}
