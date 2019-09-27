#ifndef __GR55xx_MP_DEFS_H__
#define __GR55xx_MP_DEFS_H__

#include "py/mpconfig.h"
#include "mpconfigboard.h"
#include "custom_config.h"

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#define TRUE                        1u
#define FALSE                       0u
#define MP_GR5515_STACK_SIZE        CSTACK_HEAP_SIZE

#endif /*__GR55xx_MP_DEFS_H__*/
