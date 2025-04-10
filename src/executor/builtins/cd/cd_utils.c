/**
 * @file handle_cd_utils.c
 * @brief helpers of handle_cd().
 */
#include "minishell.h"


// bool	is_only_endline(t_cmd *cmd)
// {
// 	if (ft_strchr(cmd->argv[1], '\n'))
// 		return (false);
// 	return (true);
// }

/**
 * @brief Retrieves the current working directory.
 *
 * This function attempts to get the current working directory using `getcwd()`.
 * If `getcwd()` fails, it falls back to retrieving the `$PWD` environment
 * variable.
 *
 * @param cwd Buffer where the current directory path will be stored.
 * @param cmd Pointer to the command structure used to access environment
 *            variables.
 * @return `true` if `getcwd()` succeeds, `false` if it fails and `$PWD`
 *          is used instead.
 *
 * @note Ensure `cwd` has enough space (`MS_PATHMAX`). The function does not
 *       allocate memory.
 */
bool	get_directory(char *cwd, t_cmd *cmd)
{
	char	*temp_pwd;

	if (getcwd(cwd, MS_PATHMAX))
		return (true);
	temp_pwd = ms_getenv(cmd->minishell, "PWD");
	if (temp_pwd)
		ft_strlcpy(cwd, temp_pwd, MS_PATHMAX);
	else
		ft_strlcpy(cwd, "", MS_PATHMAX);
	return (false);
}
