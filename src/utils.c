/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 11:49:46 by imunaev-          #+#    #+#             */
/*   Updated: 2025/02/26 12:36:40 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	print_error_exit(char *cmd, int exit_status)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
	exit(exit_status);
}

void	print_error(char *cmd)
{
	ft_putstr_fd(cmd, STDERR_FILENO);
	perror(": ");
}