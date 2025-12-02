#pragma once

#include "game.h"

typedef int rl_fxp;
#define RL_FIXED_SCALE_BITS 16
#define RL_FIXED_1 (1 << RL_FIXED_SCALE_BITS)
#define RL_FIXED_MAX CLR_SIGN(~0)
#define RL_FIXED_MIN SET_SIGN(0)

// for training
#define INITIAL_MUTIPLIER 0x6 /* 0.0001 */
#define LEARNING_RATE 0x51e   /* 0.02 */
#define NUM_EPISODE 10000
#define EPSILON_GREEDY 0
#define MONTE_CARLO 1

// for Markov decision model
#define GAMMA 0xfd70 /* 0.99 */
#define REWARD_TRADEOFF RL_FIXED_1

// for epsilon greedy
#define EPSILON_START (FIXED_SCALE_BITS >> 1) /* 0.5 */
#define EPSILON_END 0x41                      /* 0.001 */

#define CALC_STATE_NUM(x)                 \
    {                                     \
        x = 1;                            \
        for (int i = 0; i < N_GRIDS; i++) \
            x *= 3;                       \
    }

typedef struct td_agent {
    char player;
    rl_fxp *state_value;
} rl_agent_t;

int table_to_hash(unsigned int table);

int play_rl(unsigned int table, char player);

void init_rl_agent(unsigned int state_num, char player);

void free_rl_agent(unsigned char player);

void update_state_value(const int *after_state_hash,
                        const rl_fxp *reward,
                        int steps,
                        char player);
