/**
 * @file handle_exit_utils.c
 * @brief Helpers of the handle_exit().
 */
#include "minishell.h"

/**
 * @brief Skips leading whitespace and checks for +/- sign.
 *
 * @param str_ptr  A pointer to the string pointer (because we modify it).
 * @return		 +1 if the sign is positive, -1 if negative.
 */
static int	skip_whitespace_and_sign(const char **str_ptr)
{
	const char	*str;
	int			sign;

	str = *str_ptr;
	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	*str_ptr = str;
	return (sign);
}

/**
 * @brief Checks if adding `digit` to `current_result` would overflow (positive).
 *
 * @param current_result The number so far.
 * @param digit		  The next digit to add.
 * @return true if overflow would occur, false otherwise.
 */
static bool	check_overflow_pos(long long current_result, int digit)
{
	if (current_result > (LLONG_MAX - digit) / 10)
		return (true);
	return (false);
}

/**
 * @brief Checks if adding `digit` to `current_result` would overflow (negative).
 *
 * @param current_result The number so far (still positive if you haven't
 * multiplied by sign).
 * @param digit		  The next digit to add.
 * @return true if overflow would occur, false otherwise.
 */
static bool	check_overflow_neg(long long current_result, int digit)
{
	if (current_result > (-(LLONG_MIN + digit) / 10))
		return (true);
	return (false);
}

long long ft_atoll_exit(const char *str, bool *overflow)
{
	long long	result;
	int			sign;
	int			digit;

	result = 0;
	*overflow = false;
	sign = skip_whitespace_and_sign(&str);
	while (ft_isdigit(*str))
	{
		digit = *str - '0';
		if ((sign == 1 && check_overflow_pos(result, digit))
			|| (sign == -1 && check_overflow_neg(result, digit)))
		{
			*overflow = true;
			return (0);
		}
		result = result * 10 + digit;
		str++;
	}
	if (sign == -1)
		result = -result;
	return (result);
}
