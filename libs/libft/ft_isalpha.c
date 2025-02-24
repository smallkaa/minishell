/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:03:44 by imunaev-          #+#    #+#             */
/*   Updated: 2025/01/30 23:45:45 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
/**
 * @brief Checks if a character is alphabetic.
 *
 * This function tests whether the given character `c` is an alphabetic
 * character (either lowercase 'a' to 'z' or uppercase 'A' to 'Z').
 *
 * @param c The character to check.
 * @return int Nonzero if the character is alphabetic, 0 otherwise.
 */
int	ft_isalpha(int c)
{
	return (ft_is_lower_alpha(c) || ft_is_upper_alpha(c));
}
