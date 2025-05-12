/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 15:39:21 by Ilia Munaev       #+#    #+#             */
/*   Updated: 2025/05/12 14:06:05 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Safely frees part of a string array and nullifies it.
 *
 * Frees up to `count` elements in the array and sets each to NULL.
 * Then optionally sets the whole array pointer to NULL.
 *
 * @param arr_ptr Pointer to the string array.
 * @param count Number of strings to free.
 */
void	free_str_array_range(char **arr, size_t count)
{
	size_t	i;

	i = 0;
	if (!arr)
		return ;
	while (i < count)
	{
		free(arr[i]);
		arr[i] = NULL;
		i++;
	}
}

/**
 * @brief Frees a t_mshell_var and sets its pointer to NULL.
 *
 * @param pair Double pointer to the variable structure.
 */
void	free_pair(t_mshell_var **pair)
{
	if (!pair || !*pair)
		return ;
	free((*pair)->key);
	free((*pair)->value);
	free(*pair);
	*pair = NULL;
}
