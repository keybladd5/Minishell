/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_aux.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 13:00:51 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/09 13:00:53 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//wait the end of all the child process, and set the exit_status value
void	ft_wait_child_process(int *exit_status, int process)
{
	int	status;

	while (process)
	{
		wait(&status);
		if (WIFEXITED(status))
			*exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
		{
			if (WTERMSIG(status) == SIGINT)
				*exit_status = 130;
			else if (WTERMSIG(status) == SIGQUIT)
				*exit_status = 131;
		}
		process--;
	}
}

void	ft_aux_exec_absoluthe_path(char **cmd_argv, t_token **tokens)
{
	int	i;

	i = 0;
	while (*tokens)
	{
		if ((*tokens)->str[0] == '/')
		{
			cmd_argv[i] = ft_substr((*tokens)->str, \
			ft_aux_abs((*tokens)->str), ft_strlen((*tokens)->str));
			if (!cmd_argv[i])
				ft_error_system(MALLOC_ERROR);
			*tokens = (*tokens)->next;
			if (!*tokens || (*tokens)->type != WORD)
				break ;
			i++;
		}
		cmd_argv[i] = ft_strdup((*tokens)->str);
		if (!cmd_argv[i])
			ft_error_system(MALLOC_ERROR);
		*tokens = (*tokens)->next;
		i++;
	}
}

void	ft_exec_absoluthe_path(t_token **tokens, t_executor *d_exec)
{
	d_exec->absolute_path = (*tokens)->str;
	d_exec->cmd_argv = (char **)malloc(sizeof(char *) * \
	(ft_token_lst_size(*tokens) + 1));
	if (!d_exec->cmd_argv)
		ft_error_system(MALLOC_ERROR);
	if (access(d_exec->absolute_path, F_OK) != 0)
	{
		ft_error_cmd(d_exec->absolute_path, 2);
		exit(127);
	}
	d_exec->cmd_argv[ft_token_lst_size(*tokens)] = NULL;
	ft_aux_exec_absoluthe_path(d_exec->cmd_argv, tokens);
	if (access(d_exec->absolute_path, X_OK) == 0)
		execve(d_exec->absolute_path, d_exec->cmd_argv, d_exec->new_envp);
	else
	{
		ft_error_cmd(d_exec->cmd_argv[0], 3);
		exit(126);
	}
	ft_error_cmd(d_exec->cmd_argv[0], 1);
	exit(127);
}
