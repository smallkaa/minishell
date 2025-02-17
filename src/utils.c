/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 11:49:46 by imunaev-          #+#    #+#             */
/*   Updated: 2025/02/17 11:49:53 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error(char *cmd, const char *msg)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	ft_putstr_fd((char *)msg, STDERR_FILENO);
}
