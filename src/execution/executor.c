/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:36:31 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 19:02:45 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_path_finder(t_env **env, t_token **tokens, t_executor **d_exec)
{
	while (ft_strncmp("PATH", (*env)->key_name, 4) != 0)
	{
		*env = (*env)->next;
		if (!*env || (*env)->next == NULL)
		{
			ft_error_cmd((*tokens)->str, 1);
			exit(127);
		}
	}
	(*d_exec)->path = ft_split((*env)->value, ':');
	if (!(*d_exec)->path)
		ft_error_system(MALLOC_ERROR);
	(*d_exec)->cmd = ft_strjoin("/", (*tokens)->str);
	if (!(*d_exec)->cmd)
		ft_error_system(MALLOC_ERROR);
}

int	ft_exec_cmd_aux(t_executor **d_exec)
{
	int	i;

	i = 0;
	while ((*d_exec)->path[i])
	{
		(*d_exec)->absolute_path = ft_strjoin((*d_exec)->path[i], \
		(*d_exec)->cmd);
		if (!(*d_exec)->absolute_path)
			exit(MALLOC_ERROR);
		if (access((*d_exec)->absolute_path, X_OK) == 0)
			return (1);
		free((*d_exec)->absolute_path);
		i++;
	}
	ft_error_cmd((*d_exec)->cmd_argv[0], 1);
	return (0);
}

void	exec_cmd(t_token **tokens, t_env **env, t_executor *d_exec)
{
	int		i;

	i = 0;
	ft_path_finder(env, tokens, &d_exec);
	d_exec->cmd_argv = (char **)malloc(sizeof(char *) * \
	(ft_token_lst_size(*tokens) + 1));
	if (!d_exec->cmd_argv)
		ft_error_system(MALLOC_ERROR);
	d_exec->cmd_argv[ft_token_lst_size(*tokens)] = NULL;
	while (*tokens)
	{
		d_exec->cmd_argv[i] = ft_strdup((*tokens)->str);
		if (!d_exec->cmd_argv[i])
			exit(MALLOC_ERROR);
		*tokens = (*tokens)->next;
		i++;
	}
	if (ft_exec_cmd_aux(&d_exec))
		execve(d_exec->absolute_path, d_exec->cmd_argv, d_exec->new_envp);
	exit(127);
}

void	ft_child_process(t_executor *d_exec, t_pipe *data_pipe, \
t_token **tokens, t_env **env)
{
	if (d_exec->pid == 0)
	{
		sig_init(0);
		d_exec->new_envp = ft_copy_env(env);
		if (data_pipe->flag == YES)
		{
			ft_dup2(data_pipe->pipefd[1], 1);
			ft_close2(data_pipe->pipefd[1], data_pipe->pipefd[0]);
		}
		if (!tokens || !*tokens)
			exit (0);
		if (ft_is_built_in(tokens))
			exit(ft_exec_builtin(tokens, env, NULL));
		else if ((*tokens)->str[0] == '/')
			ft_exec_absoluthe_path(tokens, d_exec);
		else
			exec_cmd(tokens, env, d_exec);
	}
}

//fork, find the absolute path, get the argv to the comand 
//(included comand name) check the acces and exec
void	executor(t_token **tokens, t_env **env, t_pipe *data_pipe)
{
	t_executor	*d_exec;

	d_exec = malloc(sizeof(t_executor));
	ft_memset(d_exec, 0, sizeof(t_executor));
	if (!tokens || !*tokens)
		return (free(d_exec));
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	d_exec->pid = fork();
	if (d_exec->pid < 0)
		ft_error_system(FORK_ERROR);
	ft_child_process(d_exec, data_pipe, tokens, env);
	free(d_exec);
}
