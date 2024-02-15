/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Meritxu <Meritxu@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 10:36:31 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 19:02:45 by Meritxu          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//check if the word on the token is built in comand
int ft_is_built_in(t_token **tokens)
{
	if (!*tokens)
		return (1);
	if (!ft_strncmp("exit", (*tokens)->str, 4))
	{
			ft_printf("exit\n");
			exit(0);
	}
	return (0);
}
//aux to malloc size on the executor function
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

//expand env variables on the tokens linked list, works whith a '$' char
//ECHO:
//-Uno o mas strings simples ✅
//-Una o mas variables de entorno ($USER$USER$PWD) ✅
//-Caraccteres + Variable de entorno (AAAA$USER) 
//-Variable de entorno + Caracteres ($USERAAAA)
//-SOLO SIMBOLO $ ✅

void	expansor(t_token **tokens, t_env **env)
{
	t_token *t_current; //token current
	t_env	*e_current; //enviroment current
	char	*tmp;
	char	*str = NULL;
	int		i;
	int		x;

	t_current = *tokens;
	e_current = *env;
	x = 0;

	//hay que comprobar cuando hay caracteres de mas en la variable y los del principio del string
	while (t_current)
	{
		i = 0;
		tmp = t_current->str;
		t_current->str = NULL;
		while (tmp[i])
		{
			if (!ft_strncmp(tmp + i, "$", 1)) //si el primer caracter es un $ recorre el env hasta encontrar el contenido y sustituirlo
			{
				i++;
				if (!tmp[i])
					break;
				x = i;
				while(tmp[x] != '$' && !ft_isspace(tmp[x]) && tmp[x])
					x++;
				while (e_current)
				{
					if (!ft_strncmp(&(tmp[i]), e_current->key_name, x - i))
					{
						str = ft_strdup(e_current->value);
						if (!str)
							exit (MALLOC_ERROR);
						i = x;
						break;
					}
					else
						 e_current = e_current->next;
				}
				if (str)
					t_current->str = ft_strjoin_f(t_current->str, str);
				e_current = *env;
			}
			else
				i++;
		}
		if (!t_current->str)
			t_current->str = tmp;
		else
			free (tmp);
		t_current = t_current->next;
	}
}

//fork, find the absolute path, get the argv to the comand (included comand name) check the acces and exec
void	exec_cmd(t_token **tokens, t_env **env, char **envp, t_pipe *data_pipe)
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
	//dprintf(2, "%d\n", pid);
	if (pid == 0)
	{
		/*int loop = 1;
		while (loop)
			;*/
		if (data_pipe->flag == YES && data_pipe->pipe_counter)
		{
			dup2(data_pipe->pipefd[1], 1);//comunica la salida con la entrada del siguiente proceso
			close(data_pipe->pipefd[1]); //cierra pipes
			close(data_pipe->pipefd[0]);
		}
		while (ft_strncmp("PATH", (*env)->key_name, 4) != 0) //LOCALIZA EL PATH
		{
			*env = (*env)->next;
			if ((*env)->next == NULL)
				exit(1);//  ❌ ESTO NO ESTA BIEN! SI BORRAN LA PATH SIMPLEMENTE NO ENCUENTRA NADA Y SACA EL ERROR PERTIENENTE
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
		while (*tokens && (*tokens)->type == WORD) //llena la matriz con todos los tokens PENDIENTE CAMBIAR
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
	}
	//wait (NULL);//moved this part to the parser
}
