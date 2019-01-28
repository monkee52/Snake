#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(x) (((x) >= 0) ? (x) : -(x))
#define SIGN(x) (((x) > 0) - ((x) < 0))

typedef void (*Hook0)(const char*);
typedef void (*Hook1)(const char*, uint8_t);
typedef void (*Hook2)(const char*, uint8_t, uint8_t);
typedef void (*Hook3)(const char*, uint8_t, uint8_t, uint8_t);
typedef void (*Hook4)(const char*, uint8_t, uint8_t, uint8_t, uint8_t);
typedef void (*HookD)(const char*, void*, uint16_t);

extern Hook1 hook_end_update;
extern Hook1 hook_end_invert;
extern Hook1 hook_end_contrast;

#ifdef __GNUC__
#define PACKED_STRUCT(name) struct __attribute__ ((packed)) name
#else
#define PACKED_STRUCT(name) __pragma(pack(push, 1)) struct name __pragma(pack(pop))
#endif

#define HOOK_DEF(name, fn) hook_##name = fn

#define HOOK_0(name) if (hook_##name != NULL) { hook_##name(__FUNCTION__); }
#define HOOK_1(name, p1) if (hook_##name != NULL) { hook_##name(__FUNCTION__, (p1)); }
#define HOOK_2(name, p1, p2) if (hook_##name != NULL) { hook_##name(__FUNCTION__, (p1), (p2)); }
#define HOOK_3(name, p1, p2, p3) if (hook_##name != NULL) { hook_##name(__FUNCTION__, (p1), (p2), (p3)); }
#define HOOK_4(name, p1, p2, p3, p4) if (hook_##name != NULL) { hook_##name(__FUNCTION__, (p1), (p2), (p3), (p4)); }
#define HOOK_D(name, p1, p2) if (hook_##name != NULL) { hook_##name(__FUNCTION__, (p1), (p2)); }

#endif
