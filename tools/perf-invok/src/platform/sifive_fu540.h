// ~~~ SiFive FU540 specific values ~~~

// ~~ PMU Events ~~

// Instruction commit events
#define EVENT_BIT_INT_LOAD_RETIRED  (1 << 9)
#define EVENT_BIT_INT_STORE_RETIRED (1 << 10)
#define EVENT_BIT_FLT_LOAD_RETIRED  (1 << 19)
#define EVENT_BIT_FLT_STORE_RETIRED (1 << 20)

// Microarchitectural events
#define EVENT_BIT_DATA_CACHE_BUSY ((1 << 12) | (1 & 0xFF))

// Memory system events
#define EVENT_BIT_DATA_CACHE_MISS ((1 << 9) | (2 & 0xFF))

// ~~ RTC Clock Multiplier ~~
#define TIME_TO_US 1
