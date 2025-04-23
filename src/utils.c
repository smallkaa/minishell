/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:12:32 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/23 13:13:04 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_debug_mode(void)
{
	return (getenv("MINISHELL_DEBUG") != NULL);
}

void	debug_printf(const char *format, ...)
{
	va_list	args;

	if (!is_debug_mode())
		return ;
	va_start(args, format);
	while (*format)
	{
		if (*format == '%' && *(format + 1))
		{
			format++;
			if (*format == 's')
				printf("%s", va_arg(args, char *));
			else if (*format == 'd')
				printf("%d", va_arg(args, int));
			else if (*format == '%')
				printf("%%");
			else
				printf("%%%c", *format);
		}
		else
			putchar(*format);
		format++;
	}
	va_end(args);
}
