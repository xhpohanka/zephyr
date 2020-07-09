#ifndef POT_H__
#define POT_H__

#include <stdint.h>
#include <pot_element.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup libpot Partially Ordered Tree Library
 * @{
 * @brief Implementation of partially ordered trees with non-defined element.
 */

#ifndef POT_LOG_DEBUG
#define POT_LOG_DEBUG(...)
#endif

/**
 * @brief Function for initializing the tree
 *
 * @param p_array Pointer to the array of the pointers to the tree nodes.
 * @param size    Size of the array.
 */
void pot_init(pot_element_t ** p_array, uint32_t size);

/**
 * @brief Function for adding node to the tree.
 *
 * @param p_elem Pointer to the element to be added as a node.
 *
 * @retval 0  Success.
 * @retval -1 No space.
 */
int pot_push(pot_element_t * p_elem);

/**
 * @brief Function for getting root node of the tree.
 *
 * @warning Root node is removed from the tree as a consequence.
 *
 * @return Pointer to the root node of the tree.
 */
pot_element_t * pot_pop(void);

/**
 * @brief Function for peeking at the root node of the tree.
 *
 * Node is not removed from the tree.
 *
 * @return Pointer to the root node of the tree.
 */
pot_element_t * pot_get(void);

/**
 * @brief Function for removing node from tree.
 *
 * @retval 0  Success.
 * @retval -1 No node in tree.
 */
int pot_remove(pot_element_t * p_elem);

/**
 *@}
 **/

#ifdef __cplusplus
}
#endif

#endif // POT_H__
