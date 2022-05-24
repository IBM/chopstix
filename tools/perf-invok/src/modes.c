#include "modes.h"

#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "limits.h"
#include "tool.h"

void measureInvocationPerformance(unsigned long long *addrStart,
                                  unsigned int startPointCount,
                                  unsigned long long *addrEnd,
                                  unsigned int endPointCount) {

    for(int i=0; i<startPointCount; i++) perfInvokSetBreakpoint(addrStart[i]);

    // Loop indefinitely, when the program ends, perf-invok will exit
    // automatically
    while (1) {

        // Continue until the program hits the start breakpoint

        perfInvokContinue();
        perfInvokWait();

        for(int i=0; i<startPointCount; i++) perfInvokResetBreakpoint(addrStart[i]);
        for(int i=0; i<endPointCount; i++) perfInvokSetBreakpoint(addrEnd[i]);

        perfInvokBeginSample();

        // Continue until the program hits the end breakpoint
        perfInvokContinue();
        perfInvokWait();

        perfInvokEndSample();

        for(int i=0; i<endPointCount; i++) perfInvokResetBreakpoint(addrEnd[i]);
        for(int i=0; i<startPointCount; i++) perfInvokSetBreakpoint(addrStart[i]);
    }
}

timer_t timeoutTimer, rateTimer;

// Function that will be called once the timeout expires
static void timeoutHandler(union sigval data) {
    perfInvokExit(EXIT_SUCCESS);
}

// Function that will be called at the user-specified rate
static void rateHandler(union sigval data) {
    perfInvokEndSample();
    perfInvokBeginSample();
}

void measureGlobalPerformance(unsigned int timeout, float rate) {
    struct sigevent timeoutSigev = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_signo = SIGUSR1,
        .sigev_value.sival_int = 0,
        .sigev_notify_function = timeoutHandler,
        .sigev_notify_attributes = NULL,
    };

    timer_create(CLOCK_MONOTONIC, &timeoutSigev, &timeoutTimer);

    struct sigevent rateSigev = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_signo = SIGUSR1,
        .sigev_value.sival_int = 0,
        .sigev_notify_function = rateHandler,
        .sigev_notify_attributes = NULL,
    };

    timer_create(CLOCK_MONOTONIC, &rateSigev, &rateTimer);


    if (timeout > 0 ) {
        fprintf(stderr, "Timeout set to %d seconds\n", timeout);
        struct itimerspec timerVal = {
            .it_interval = {
                .tv_sec = 0,
                .tv_nsec = 0
            },
            .it_value = {
                .tv_sec = timeout,
                .tv_nsec = 0
            }
        };

        int ret = timer_settime(timeoutTimer, 0, &timerVal, NULL);
        if (ret != 0) { perror("ERROR while setting timeout"); exit(EXIT_FAILURE);};
    } else {
        fprintf(stderr, "No timeout set. Waiting process to finish\n");
    }

    if (rate > 0.0) {
        float period = 1.0 / rate;
        long periodSeconds = period;
        long periodNanoseconds = (period - periodSeconds) * 1000000000;

        fprintf(stderr, "Taking measurements at a rate of %fHz (period: %ld seconds, %ld nanoseconds)\n", rate, periodSeconds, periodNanoseconds);
        struct itimerspec timerVal = {
            .it_interval = {
                .tv_sec = periodSeconds,
                .tv_nsec = periodNanoseconds
            },
            .it_value = {
                .tv_sec = periodSeconds,
                .tv_nsec = periodNanoseconds
            }
        };

        int ret = timer_settime(rateTimer, 0, &timerVal, NULL);
        if (ret != 0) { perror("ERROR while setting rate timer"); exit(EXIT_FAILURE);};
    } else {
        fprintf(stderr, "Measuring once from start to finish\n");
    }

    perfInvokBeginSample();

    perfInvokContinue();

    perfInvokWait();

    perfInvokEndSample();
}
