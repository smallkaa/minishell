/**
 * @file exit_utils.c
 * @brief Helper functions for implementing the `exit` built-in command.
 *
 * Provides numeric parsing and overflow checking for validating and converting
 * user input passed to `exit`. These helpers ensure compliance with
 * Bash behavior when handling large or invalid exit codes.
 */
#include "minishell.h"

/**
 * @brief Skips leading whitespace and determines the sign of a number.
 *
 * Advances the input string pointer past any leading whitespace characters.
 * If the next character is '+' or '-', the function adjusts the sign accordingly
 * and moves the pointer past it.
 *
 * @param str_ptr A pointer to a string pointer that will be updated in-place.
 * @return `1` if the number is positive, `-1` if it is negative.
 *
 * @note The input pointer is modified to point to the first numeric digit.
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
 * @brief Checks for potential overflow when adding a digit to a positive result.
 *
 * Used to safely build positive numbers during string-to-integer conversion.
 * Ensures that adding a digit will not exceed `LLONG_MAX`.
 *
 * @param current_result The number accumulated so far.
 * @param digit The next digit to be added.
 * @return `true` if the addition would cause overflow, `false` otherwise.
 */
static bool	check_overflow_pos(long long current_result, int digit)
{
	if (current_result > (LLONG_MAX - digit) / 10)
		return (true);
	return (false);
}

/**
 * @brief Checks for potential overflow when adding a digit to a negative result.
 *
 * Ensures that the negative number being built will not exceed `LLONG_MIN`
 * (in absolute value),
 * by comparing against `LLONG_MAX` limits (due to signed integer
 * representation).
 *
 * @param result The current positive magnitude of the number being parsed.
 * @param digit The next digit to consider.
 * @return `true` if overflow would occur, `false` otherwise.
 */
static bool	check_overflow_neg(long long result, int digit)
{
	if (result > (LLONG_MAX / 10))
		return (true);
	if (result == (LLONG_MAX / 10))
	{
		if (digit > 8)
			return (true);
	}
	return (false);
}

/**
 * @brief Converts a numeric string to `long long` with overflow detection.
 *
 * This is a custom version of `atoll()` that supports:
 * - Skipping leading whitespace.
 * - Optional '+' or '-' sign.
 * - Overflow detection using safe arithmetic logic.
 *
 * If overflow occurs during parsing, the function sets `*overflow` to `true`
 * and returns 0.
 *
 * @param str The string to convert.
 * @param overflow A pointer to a boolean flag that will be set on overflow.
 * @return The parsed signed integer, or `0` if overflow is detected.
 */
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
