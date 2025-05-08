/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Ilia Munaev <ilyamunaev@gmail.com>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 13:12:32 by pvershin          #+#    #+#             */
/*   Updated: 2025/05/08 11:49:31 by Ilia Munaev      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	is_debug_mode(void)
{
	return (getenv("MINISHELL_DEBUG") != NULL);
}

/**
 * @brief Helper to print a formatted debug specifier.
 *
 * Supports %s, %d, %p, and %%.
 *
 * @param format Pointer to format specifier.
 * @param args Active va_list to consume arguments.
 */
static void	print_debug_specifier(const char *format, va_list args)
{
	if (*format == 's')
		printf("%s", va_arg(args, char *));
	else if (*format == 'd')
		printf("%d", va_arg(args, int));
	else if (*format == 'p')
		printf("%p", va_arg(args, void *));
	else if (*format == '%')
		printf("%%");
	else
		printf("%%%c", *format);
}

/**
 * @brief Conditionally prints debug output based on global debug flag.
 *
 * A minimal custom printf that respects %s, %d, %p, and %% formats.
 * Only prints if is_debug_mode() returns true.
 *
 * @param format Format string.
 * @param ... Variable arguments.
 */
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
			print_debug_specifier(format, args);
		}
		else
			putchar(*format);
		format++;
	}
	va_end(args);
}
