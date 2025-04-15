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
// static bool	check_overflow_neg(long long current_result, int digit)
// {
// 	printf("DEBUG: start check_overflow_neg: compare with %lld \n", (-(LLONG_MIN + digit) / 10));
	
// 	printf("DEBUG: check_overflow_neg: current_result = %lld\n", current_result);
// 	printf("DEBUG: check_overflow_neg: digit = %d\n", digit);

// 	if (current_result > (-(LLONG_MIN + digit) / 10))
// 	{
// 		printf("DEBUG: check_overflow_neg: TRUE\n");
// 		return (true);
// 	}
// 	printf("DEBUG: check_overflow_neg: FALSE\n");

// 	return (false);
// }
static bool check_overflow_neg(long long result, int digit)
{
    // For negative numbers, we can go up to an absolute value of 9223372036854775808
    // which is LLONG_MAX + 1 (but that exact number can’t be stored in a signed 64-bit).
    // So we compare carefully in two steps.

    // If result > (LLONG_MAX + 1) / 10, definitely overflow.
    // But we can’t directly write (LLONG_MAX + 1) in 64-bit,
    // so we use (LLONG_MAX / 10) plus an extra check for the last digit.
    if (result > (LLONG_MAX / 10))
        return true;

    // If result == LLONG_MAX / 10, then we must ensure digit ≤ 8
    // because -9223372036854775808 is valid (absolute value = 9223372036854775808).
    if (result == (LLONG_MAX / 10))
    {
        // LLONG_MAX % 10 is 7, so the valid digit for the final negative number is 8.
        if (digit > 8)
            return true;
    }
    return false;
}
long long	ft_atoll_exit(const char *str, bool *overflow)
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

		// printf("DEBUG: ft_atoll_exit: digit = %d\n", digit);
		// printf("DEBUG: ft_atoll_exit: result = %lld\n", result);

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