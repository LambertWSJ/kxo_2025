#pragma once

#include <linux/types.h>
#include "reinforcement_learning.h"

#define HT_BITS 14
#define MAX_STATES (1 << HT_BITS)
#define ST_MAP_SZ \
    ALIGN(MAX_STATES / sizeof(unsigned long), sizeof(unsigned long))
#define CLEAN_N_STATES (MAX_STATES * 3 / 5)

rl_fxp *find_rl_state(const u32 table);
