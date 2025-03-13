/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoll.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imunaev- <imunaev-@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 11:15:22 by imunaev-          #+#    #+#             */
/*   Updated: 2025/03/13 12:00:10 by imunaev-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static const char	*skip_spaces(const char *str)
{
	while (ft_isspace(*str))
		str++;
	return (str);
}

static const char	*parse_sign(const char *str, int *sign)
{
	*sign = 1;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			*sign = -1;
		str++;
	}
	return (str);
}

static long long	parse_number(const char *str, int sign, int *overflow)
{
	long long	result;
	int			digit;

	result = 0;
	*overflow = 0;
	while (ft_isdigit(*str))
	{
		digit = *str - '0';
		if (sign == 1 && result > (LLONG_MAX - digit) / 10)
		{
			*overflow = 1;
			return (LLONG_MAX);
		}
		if (sign == -1 && result > (-(LLONG_MIN + digit)) / 10)
		{
			*overflow = 1;
			return (LLONG_MIN);
		}
		result = result * 10 + digit;
		str++;
	}
	return (sign * result);
}

long long	ft_atoll(const char *str)
{
	int			sign;
	int			overflow;
	long long	result;

	str = skip_spaces(str);
	str = parse_sign(str, &sign);
	result = parse_number(str, sign, &overflow);
	if (overflow)
	{
		if (sign == 1)
			return (LLONG_MAX);
		else
			return (LLONG_MIN);
	}
	return (result);
}
