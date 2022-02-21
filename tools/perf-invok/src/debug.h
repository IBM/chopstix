#ifdef DEBUG_MODE
#define DEBUG 1
#else
#define DEBUG 0
#endif

#define debug_print(fmt, ...) \
	do { if (DEBUG) fprintf(stderr, "DEBUG:%s:%d:%s(): " fmt, __FILE__, \
    __LINE__, __func__,##__VA_ARGS__); } while (0)
