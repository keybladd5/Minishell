/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:36:31 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 19:02:45 by Meritxu          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

void 	ft_wait_child_process(int *exit_status, int process)
{
	int 	status;

	while (process)
	{
		wait(&status);
		if (WIFEXITED(status))
			*exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status)) //esta opcion no sera funcional hasta implementar signals en procesos hijos
		{
			if (WTERMSIG(status) == SIGINT)
				*exit_status = 130;
			else if (WTERMSIG(status) == SIGQUIT)
				*exit_status = 131;
		}
		process--;
	}
}

//NO FUNCIONA !!!
char **ft_copy_env(t_env **env)
{
	int i;
	char **new_env;
	t_env **ref;

	i = 0;
	ref = env;
	while(*env)
	{
		*env = (*env)->next;
		i++;
	}
	*env = *ref;
	new_env = (char **)malloc(sizeof(char *) * (i + 1));
	i = 0;
	while(*env)
	{
		new_env[i] = ft_strdup((*env)->value);
		if (!new_env[i])
			ft_error_system(MALLOC_ERROR);
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}

int		ft_aux_abs(char *str)
{
	int	i;

	i = 0;
	while(str[i])
		i++;
	while (str[i] != '/')
		i--;
	return (i+1);
}

//aux to malloc size on the executor function
int		ft_token_lst_size(t_token *lst)
{
	int	i;

	i = 0;
	while (lst && lst->type == WORD)//PARA QUE SOLO CUENTE HASTA LA PIPE SI HAY
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

void	ft_exec_absoluthe_path(t_token **tokens, char **envp)
{
	char 	**cmd_argv;
	char	*absolute_path;
	int 	i;

	i = 0;
	absolute_path = (*tokens)->str; //setemos la variable directamente sin buscar en path
	cmd_argv = (char **)malloc(sizeof(char * ) * (ft_token_lst_size(*tokens) + 1)); //crea la matriz a pasar al execve
	if (!cmd_argv)
		ft_error_system(MALLOC_ERROR);
	if (access(absolute_path, X_OK) != 0)
	{
		ft_error_cmd(cmd_argv[0]);
		exit(127);
	}
	cmd_argv[ft_token_lst_size(*tokens)] = NULL;
	while (*tokens) //llena la matriz con todos los tokens PENDIENTE CAMBIAR
	{
		if ((*tokens)->str[0] == '/')
		{
			cmd_argv[i] = ft_substr((*tokens)->str, ft_aux_abs((*tokens)->str), ft_strlen((*tokens)->str));
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
	i = 0;
	if (access(absolute_path, X_OK) == 0) //Checkea a validez de la ruta absoluta, si no lo es en el input es un NULL y no entra
		execve(absolute_path, cmd_argv, envp);
	ft_error_cmd(cmd_argv[0]);
	exit(127);
}

void 	exec_cmd(t_token **tokens, t_env **env, char **envp)
{
	char 	**path;
	char	**cmd_argv;
	char 	*cmd;
	char	*absolute_path;
	int		i;

	i = 0;
	while (ft_strncmp("PATH", (*env)->key_name, 4) != 0) //LOCALIZA EL PATH si la flag no esta
	{
		*env = (*env)->next;
		if ((*env)->next == NULL)
			exit(127);
	}
	path = ft_split((*env)->value, ':'); //lo splitea
	if (!path) 
		ft_error_system(MALLOC_ERROR);
	cmd = ft_strjoin("/", (*tokens)->str); //prepara el primer comando con el slash
	if (!cmd)
		ft_error_system(MALLOC_ERROR);
	cmd_argv = (char **)malloc(sizeof(char * ) * (ft_token_lst_size(*tokens) + 1)); //crea la matriz a pasar al execve
	if (!cmd_argv)
		ft_error_system(MALLOC_ERROR);
	cmd_argv[ft_token_lst_size(*tokens)] = NULL;
	while (*tokens)
	{
		cmd_argv[i] = ft_strdup((*tokens)->str);
		if (!cmd_argv[i])
			exit(MALLOC_ERROR);
		*tokens = (*tokens)->next;
		i++;
	}
	i = 0;
	while (path[i]) //Checkea el access de ese primer token recibido 
	{
		absolute_path = ft_strjoin(path[i], cmd);
		if (!absolute_path)
			exit(MALLOC_ERROR);
		if (access(absolute_path, X_OK) == 0) //Checkea a validez de la ruta absoluta
			execve(absolute_path, cmd_argv, envp);
		free(absolute_path);
		i++;
	}
	ft_error_cmd(cmd_argv[0]);
	exit(127);
}

//fork, find the absolute path, get the argv to the comand (included comand name) check the acces and exec
void	executor(t_token **tokens, t_env **env, char **envp, t_pipe *data_pipe)
{
	int	pid;
	char **new_envp;
	
	pid = fork();
	new_envp = NULL;
	//dprintf(2, "%d\n", pid);
	if (pid < 0)
		ft_error_system(FORK_ERROR);
	sig_init(0);
	if (pid == 0)
	{
		/*int loop = 1;
		while (loop)
		;*/
		new_envp = ft_copy_env(env);// en FASE DE TESTEO
		if (data_pipe->flag == YES) 
		{
			if (dup2(data_pipe->pipefd[1], 1) == -1)
				ft_error_system(DUP2_ERROR);
			close(data_pipe->pipefd[1]);
			close(data_pipe->pipefd[0]);
		}
		if (!tokens || !*tokens)
			exit (1) ;
		if (ft_is_built_in(tokens))
			exit(ft_exec_builtin(tokens, env));
		else if ((*tokens)->str[0] ==  '/')
			ft_exec_absoluthe_path(tokens, envp);
		else
			exec_cmd(tokens, env, envp);
	}
}
