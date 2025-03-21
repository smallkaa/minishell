/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realloc.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/20 12:50:07 by imunaev-          #+#    #+#             */
/*   Updated: 2025/03/20 12:53:32 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/**
 * @brief Resizes a previously allocated memory block.
 *
 * This function reallocates a new memory block of size `new_size`,
 * preserving the contents of the original block up to `old_size` bytes.
 * If `ptr` is NULL, it behaves like `malloc(new_size)`.
 * If `new_size` is 0, it frees `ptr` and returns NULL.
 * If the allocation fails, it returns NULL without modifying `ptr`.
 *
 * @param ptr Pointer to the previously allocated memory.
 * @param old_size The current size of the allocated memory in bytes.
 * @param new_size The desired new size of the memory block in bytes.
 * @return void* Pointer to the newly allocated memory block,
 *         or NULL on failure.
 */
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (!ptr)
		return (malloc(new_size));
	if (new_size <= old_size)
		return (ptr);
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	ft_memcpy(new_ptr, ptr, old_size);
	free(ptr);
	return (new_ptr);
}
