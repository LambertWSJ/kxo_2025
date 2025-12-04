#include <linux/rbtree.h>
#include <linux/vmalloc.h>

#include "ai_game.h"
#include "reinforcement_learning.h"
#include "rl_private.h"
#include "util.h"

struct xo_state {
    u32 table;
    rl_fxp scores[2];
    struct rb_node link;
    struct list_head list;
};

static struct rb_root states;
static LIST_HEAD(orders);
static unsigned long st_map[ST_MAP_SZ] = {0};
static struct xo_state *st_buff;

static void clean_state(void)
{
    struct xo_state *st, *safe;
    u32 pos, i = 0;
    list_for_each_entry_safe(st, safe, &orders, list) {
        if (i >= CLEAN_N_STATES)
            break;

        pos = ((uintptr_t) st - (uintptr_t) st_buff) / sizeof(struct xo_state);
        rb_erase(&st->link, &states);
        list_del(&st->list);
        st->table = st->scores[AGENT_O] = st->scores[AGENT_X] = 0;
        bitmap_clear(st_map, pos, 1);
        i++;
    }
}

static struct xo_state *search_state(const struct rb_root *root,
                                     const u32 table)
{
    struct rb_node *node = root->rb_node;
    while (node) {
        struct xo_state *st = rb_entry(node, struct xo_state, link);
        if (st->table > table)
            node = node->rb_left;
        else if (st->table < table)
            node = node->rb_right;
        else
            return st;
    }
    return NULL;
}

rl_fxp *find_rl_state(const u32 table)
{
    struct rb_root *root = &states;
    struct xo_state *st = search_state(root, table);
    if (likely(st)) {
        list_move_tail(&st->list, &orders);
        return st->scores;
    } else {
        u32 tot = bitmap_weight(st_map, MAX_STATES);
        if (tot >= MAX_STATES - 1)
            clean_state();

        u32 pos = find_first_zero_bit(st_map, MAX_STATES);
        st = st_buff + pos;
        bitmap_set(st_map, pos, 1);

        struct rb_node **new = &(root->rb_node), *parent = NULL;
        while (*new) {
            const struct xo_state *this = rb_entry(*new, struct xo_state, link);
            parent = *new;

            if (this->table > table)
                new = &((*new)->rb_left);
            else if (this->table < table)
                new = &((*new)->rb_right);
        }

        st->table = table;
        st->scores[AGENT_O] =
            fixed_mul_s32(INITIAL_MUTIPLIER, get_score(table, CELL_O));
        st->scores[AGENT_X] =
            fixed_mul_s32(INITIAL_MUTIPLIER, get_score(table, CELL_X));
        INIT_LIST_HEAD(&st->list);
        list_add(&st->list, &orders);
        rb_link_node(&st->link, parent, new);
        rb_insert_color(&st->link, root);
    }

    return st->scores;
}

void free_rl_agent(void)
{
    vfree(st_buff);
}

void init_rl_agent(void)
{
    states = RB_ROOT;
    LIST_HEAD(order);
    size_t nodes_sz = MAX_STATES * sizeof(struct xo_state);
    st_buff = vzalloc(nodes_sz);
}
