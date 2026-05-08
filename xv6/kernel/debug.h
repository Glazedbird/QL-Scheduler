// debug.h
#define DEBUG 1

#ifdef DEBUG
#define LOG(fmt, ...) printf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...)
#endif
