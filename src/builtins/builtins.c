/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:20:19 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 20:20:21 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//check if the word on the token is built in comand
int	ft_is_built_in(t_token **tokens)
{
	if (!*tokens)
		return (0);
	if (!ft_strxcmp("echo", (*tokens)->str) || \
	!ft_strxcmp("cd", (*tokens)->str) || \
	!ft_strxcmp("pwd", (*tokens)->str) || \
	!ft_strxcmp("env", (*tokens)->str) || \
	!ft_strxcmp("export", (*tokens)->str) || \
	!ft_strxcmp("exit", (*tokens)->str) || \
	!ft_strxcmp("unset", (*tokens)->str))
		return (1);
	return (0);
}

int	ft_exec_builtin(t_token **tokens, t_env **env, int *exit_status)
{
	if (!ft_strxcmp("echo", (*tokens)->str))
		return (ft_echo((*tokens)->next));
	else if (!ft_strxcmp("cd", (*tokens)->str))
		return (ft_cd((*tokens)->next, *env));
	else if (!ft_strxcmp("pwd", (*tokens)->str))
		return (ft_pwd(*env));
	else if (!ft_strxcmp("env", (*tokens)->str))
		return (ft_env(*env));
	else if (!ft_strxcmp("export\0", (*tokens)->str))
		return (ft_export((*tokens)->next, *env));
	else if (!ft_strxcmp("exit\0", (*tokens)->str))
		return (ft_exit((*tokens)->next, exit_status));
	else if (!ft_strxcmp("unset\0", (*tokens)->str))
		return (ft_unset((*tokens)->next, env));
	return (0);
}
