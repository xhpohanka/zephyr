#define POT_INCLUDE_COMPARE_FUNCTION

#include <pot.h>

static pot_element_t ** p_elements;
static uint32_t         arr_size;
static uint32_t         write;

static void rebase_push(int i)
{
    if (i <= 1)
    {
        // do not need to rebase
        POT_LOG_DEBUG("No need to rebase\n");
        return;
    }

    uint8_t curr_elem = i - 1;
    uint8_t prev_elem = (i / 2) - 1;

    pot_element_t * p_prev_elem;

    if (pot_compare_elems(p_elements[curr_elem], p_elements[prev_elem]))
    {
        p_prev_elem = p_elements[prev_elem];
        p_elements[prev_elem] = p_elements[curr_elem];
        p_elements[curr_elem] = p_prev_elem;
        POT_LOG_DEBUG("Recurrence:\n");
        rebase_push(i / 2);
    }
    else
    {
        POT_LOG_DEBUG("No need to swap!\n");
    }
}

static void rebase_pop(int i)
{
    if (i > write)
    {
        // do not need to rebase
        POT_LOG_DEBUG("No need to rebase\n");
        return;
    }

    uint8_t curr_elem = i - 1;
    uint8_t next_elem1 = (i * 2) - 1;
    uint8_t next_elem2 = (i * 2);

    if ((next_elem1 >= write) || (next_elem2 >= write))
    {
        // do not need to rebase
        POT_LOG_DEBUG("No need to rebase\n");
        return;
    }

    pot_element_t * p_curr_elem;

    if (pot_compare_elems(p_elements[next_elem1], p_elements[curr_elem]))
    {
        p_curr_elem = p_elements[curr_elem];
        p_elements[curr_elem] = p_elements[next_elem1];
        p_elements[next_elem1] = p_curr_elem;

        rebase_pop(i * 2);
    }
    if (pot_compare_elems(p_elements[next_elem2], p_elements[curr_elem]))
    {
        p_curr_elem = p_elements[curr_elem];
        p_elements[curr_elem] = p_elements[next_elem2];
        p_elements[next_elem2] = p_curr_elem;

        rebase_pop(i * 2 + 1);

        if (pot_compare_elems(p_elements[next_elem1], p_elements[curr_elem]))
        {
            p_curr_elem = p_elements[curr_elem];
            p_elements[curr_elem] = p_elements[next_elem1];
            p_elements[next_elem1] = p_curr_elem;

            rebase_pop(i * 2);
        }
    }
}

void pot_init(pot_element_t ** p_array, uint32_t size)
{
    arr_size = size;
    p_elements = p_array;
    for (int i = 0; i < arr_size; ++i)
    {
        p_elements[i] = NULL;
    }
    write = 0;
}

int pot_push(pot_element_t * p_elem)
{
    // Check if we can add element
    if ((write >= arr_size) && (p_elem))
    {
        return -1;
    }

    // Add element
    p_elements[write] = p_elem;
    write++;

    // Rebase the tree
    rebase_push(write);
    return 0;
}

pot_element_t * pot_pop(void)
{
    pot_element_t * p_elem = p_elements[0];
    write--;

    p_elements[0] = p_elements[write];
    p_elements[write] = NULL;

    rebase_pop(1);

    return p_elem;
}

pot_element_t * pot_get(void)
{
    if (!p_elements)
    {
        return NULL;
    }
    else
    {
        return p_elements[0];
    }
}

int pot_remove(pot_element_t * p_elem)
{
    int i;
    int found = -1;

    for (i = 0; i < arr_size; ++i)
    {
        if (p_elements[i] == p_elem)
        {
            found = i;
            break;
        }
    }

    if (found >= 0)
    {
        write--;
        p_elements[i] = p_elements[write];
        p_elements[write] = NULL;

        rebase_pop(i + 1);
        return 0;
    }
    return -1;
}
