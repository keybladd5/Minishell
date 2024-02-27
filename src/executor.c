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
int	ft_aux_abs(char *str)
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
int	ft_token_lst_size(t_token *lst)
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

void	expansor(t_token **tokens, t_env **env, int exit_status)
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
						break ;
					}
					else if (!ft_strncmp(&(tmp[i]), "?", x - i))
					{
						str = ft_itoa(exit_status);
						if (!str)
							exit (MALLOC_ERROR);
						i = x;
						break ;
					}
					else
						 e_current = e_current->next;
				}
				if (str && e_current)
				{
					t_current->str = ft_strjoin_free(t_current->str, str);
					if (!t_current->str)
						exit(MALLOC_ERROR);
				}
				else if (!e_current)
					i = x;
				e_current = *env;
			}
			else
			{
				i++;
				if (tmp[i] == '$')
				{
					t_current->str = ft_substr(tmp, 0, i);
					if (!t_current->str)
						exit(MALLOC_ERROR);
				}
			}
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
	int 	i = 0;
	int 	flag_absoluthepath = 0;
	char 	**path = NULL;
	char 	*cmd = NULL;
	char 	*absolute_path = NULL;
	char 	**cmd_argv = NULL;
	t_env	*e_current = *env;

	int pid = fork();
	if (pid < 0)
	{
		perror("Fork error 😓");
		exit(MALLOC_ERROR);
	}
	//dprintf(2, "%d\n", pid);
	sig_init(0);//cambio anadido pendiente analizar🐸
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
			if (ft_is_built_in(tokens))
				exit(ft_exec_builtin(tokens, env));
		}
		if ((*tokens)->str[0] ==  '/')//en caso de ser posible ruta absoluta
		{
			flag_absoluthepath = 1; //flag activada
			absolute_path = (*tokens)->str; //setemos la variable directamente sin buscar en path
			if (access(absolute_path, X_OK) != 0)
				exit(127);
		}
		while (ft_strncmp("PATH", e_current->key_name, 4) != 0 && !flag_absoluthepath) //LOCALIZA EL PATH si la flag no esta
		{
			e_current = e_current->next;
			if (e_current->next == NULL)
				exit(127);
		}
		path = ft_split(e_current->value, ':'); //lo splitea
		if (!path)
			exit (MALLOC_ERROR);
		cmd = ft_strjoin("/", (*tokens)->str); //prepara el primer comando con el slash
		if (!cmd)
			exit (MALLOC_ERROR);
		//condicion temporal que acogera en una matriz todos los strings de la lista tokens
		cmd_argv = (char **)malloc(sizeof(char * ) * (ft_token_lst_size(*tokens) + 1)); //crea la matriz a pasar al execve
		if (!cmd_argv)
			exit(MALLOC_ERROR);
		cmd_argv[ft_token_lst_size(*tokens)] = NULL;
		while (*tokens) //llena la matriz con todos los tokens PENDIENTE CAMBIAR
		{
			if (flag_absoluthepath)
			{
				cmd_argv[i] = ft_substr((*tokens)->str, ft_aux_abs((*tokens)->str), ft_strlen((*tokens)->str));
				if (!cmd_argv[i])
					exit(MALLOC_ERROR);
				*tokens = (*tokens)->next;
				flag_absoluthepath = 0;
				if (!*tokens || (*tokens)->type != WORD)
					break ;
				i++;
			}
			cmd_argv[i] = ft_strdup((*tokens)->str);
			if (!cmd_argv[i])
				exit(MALLOC_ERROR);
			*tokens = (*tokens)->next;
			i++;
		}
		i = 0;
		if (access(absolute_path, X_OK) == 0) //Checkea a validez de la ruta absoluta, si no lo es en el input es un NULL y no entra
				execve(absolute_path, cmd_argv, envp);
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
		exit(127);
	}
}
