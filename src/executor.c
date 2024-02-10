/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:36:31 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/09 10:36:34 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../inc/minishell.h"


int ft_is_built_in(t_token **tokens)
{
	if (!ft_strncmp("exit", (*tokens)->str, 4))
	{
			ft_printf("exit\n");
			exit(0);
	}
	return (0);
}
int	ft_token_lst_size(t_token *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		lst = lst->next;
		i++;
	}
	return (i);
}
void	executor(t_token **tokens, t_env **env, char **envp)
{
	int i = 0;
	char **path = NULL;
	char *cmd = NULL;
	char *absolute_path = NULL;
	char **cmd_argv = NULL;

	//int	*pipefd[2];
	//pipe(pipefd);
	if (ft_is_built_in(tokens))
		return ;
	int pid = fork();
	//dprintf(0, "%d\n", pid);
	if (pid == 0)
	{
		/*int loop = 1;
		while (loop)
			;*/
		//dup2(pipefd[1], 1);
		while (ft_strncmp("PATH", (*env)->key_name, 4) != 0) //LOCALIZA EL PATH
		{
			*env = (*env)->next;
			if ((*env)->next == NULL)
				exit(1);
		}
		path = ft_split((*env)->value, ':'); //lo splitea
		if (!path)
			exit (MALLOC_ERROR);
		cmd = ft_strjoin("/", (*tokens)->str); //prepara el primer comando con el slash
		if (!cmd)
			exit (MALLOC_ERROR);
		//condicion temporal que acogera en una matriz todos los strings de la lista tokens
		cmd_argv = (char **)malloc(sizeof(char * ) * ft_token_lst_size(*tokens) + 1); //crea la matriz a pasar al execve
		if (!cmd_argv)
			exit(MALLOC_ERROR);
		cmd_argv[ft_token_lst_size(*tokens)] = NULL;
		while (*tokens) //llena la matriz con todos los tokens PENDIENTE CAMBIAR
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
			if (i > 0)
				free(absolute_path);
			absolute_path = ft_strjoin(path[i], cmd);
			if (!absolute_path)
				exit(MALLOC_ERROR);
			if (access(absolute_path, X_OK) == 0) //Checkea a validez de la ruta absoluta
				execve(absolute_path, cmd_argv, envp);
			i++;
		}
	}
	wait (NULL);
}