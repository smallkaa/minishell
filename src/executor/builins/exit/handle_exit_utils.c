/**
 * @file handle_exit_utils.c
 * @brief Helpers of the handle_exit().
 */
#include "minishell.h"

/**
 * @brief Skips leading whitespace characters in a string.
 *
 * @param str The input string.
 * @return A pointer to the first non-whitespace character.
 */
static const char	*skip_whitespace(const char *str)
{
	while (ft_isspace(*str))
		str++;
	return (str);
}

/**
 * @brief Parses the sign of a number and updates the pointer accordingly.
 *
 * @param str The input string.
 * @param sign A pointer to store the sign (1 for positive, -1 for negative).
 * @return A pointer to the first numeric character in the string.
 */
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

/**
 * @brief Converts a string to a long long integer while checking for overflow.
 *
 * If the value exceeds `LLONG_MAX` or `LLONG_MIN`, overflow is detected.
 *
 * @param str The input string containing the number.
 * @param sign The sign of the number (1 or -1).
 * @param overflow Pointer to a boolean flag indicating overflow.
 * @return The converted long long integer or LLONG_MAX/LLONG_MIN in case
 *         of overflow.
 */
static long long	convert_to_ll(const char *str, int sign, bool *overflow)
{
	unsigned long long	res = 0;
	int					digit;

	*overflow = false;
	while (ft_isdigit(*str))
	{
		digit = *str - '0';
		if (res > (unsigned long long)(LLONG_MAX + (sign == -1)))
		{
			*overflow = true;
			return (sign == 1 ? LLONG_MAX : LLONG_MIN);
		}
		res = res * 10 + digit;
		str++;
	}
	return ((long long)(res * sign));
}

/**
 * @brief Converts a string to a long long integer, handling errors and
 *        overflow.
 *
 * @param str The input string.
 * @param overflow Pointer to a boolean flag indicating overflow.
 * @return The converted long long integer or 0 in case of an error.
 */
long long	ft_atoll_exit(const char *str, bool *overflow)
{
	int		sign;

	str = skip_whitespace(str);
	str = parse_sign(str, &sign);
	if (!ft_isdigit(*str))
	{
		*overflow = true;
		return (0);
	}
	return (convert_to_ll(str, sign, overflow));
}
