#include <stdlib.h>
#include <stdint.h>
#include "tmr_mngr_internal.h"

typedef struct
{
    uint64_t         next_cc_value;
    uint64_t         periodic_value;
    tmr_mngr_mode_t  vrtc_mode;
    tmr_mngr_state_t state;
    bool             reload;
    uint32_t         context;
    uint8_t          timer_id;
} pot_element_t;

#ifdef POT_INCLUDE_COMPARE_FUNCTION
static uint32_t pot_compare_elems(pot_element_t * p1, pot_element_t * p2)
{
    return p1->next_cc_value > p2->next_cc_value ? 0 : 1;
}
#endif

