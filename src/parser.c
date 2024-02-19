/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 11:19:02 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/12 11:19:04 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"
//funcion aux del prototipo para modificar el parser
t_token *ft_tokendup(t_token *token)
{
	t_token *new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		exit(MALLOC_ERROR);
	new_token->str = ft_strdup(token->str);
	if (!new_token->str)
		exit (MALLOC_ERROR);
	new_token->type = token->type;
	new_token->next = NULL;
	return (new_token);
}
//funcion aux del prototipo para modificar el parser 2
void ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens)
{
	t_token	*tmp = NULL;
	t_token	*last = NULL;
	t_token *curr = NULL;
	curr = *og_tokens;
	while (curr && curr->type == WORD)
	{
		tmp = ft_tokendup(curr);
		if (!*new_tokens)
			*new_tokens = tmp;
		else
			last->next = tmp;	
		last = tmp;
		curr = curr->next;
	}
}


void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_token		*t_current;
	t_token		*t_tmp = NULL;
	t_pipe		*data_pipe = malloc(sizeof(t_pipe));
	t_env		*e_current;
	
	data_pipe->og_stdin = dup(0);
	data_pipe->og_stdout = dup(1);
	data_pipe->pipe_counter = 0;
	t_current = *tokens;
	e_current = *env;

	//el siguiente bucle da valor a la variable type de cada nodo de la lista obtenida por el lexer, llamada tokens
	while(t_current)
	{	
		if (!ft_strncmp(t_current->str, "|", 1)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
				t_current->type = PIPE; //seteo type
				data_pipe->pipe_counter++;
				t_current = t_current->next;
				if (!t_current)
				{
					ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `|'\x1b[0m\n", 2);
					while(e_current)
					{
						if (ft_strncmp(e_current->key_name, "?", 1) == 0)
							break ;
						e_current = e_current->next;
					}
					if (!e_current)
						return ;
					free(e_current->value);
					e_current->value = ft_strdup("258");
					return ;
				}

		}
		else
		{
			t_current->type = WORD; //seteo type default
			t_current = t_current->next;
		}
	}

	//BUCLE prototipo para modificar las acciones del parser, pendiente de propuesta
	//se realiza el siguiente proceso para copiar la lista hasta la pipe y mandar solo eso. Se almacena en t_tmp
	/*t_current = *tokens; //vuelve al princio de la lista
	while (t_current)
	{
		if (t_current->type == PIPE)
			t_current = t_current->next;
		ft_printf("---COPY LINKED LIST TO SEND IN EXEC---\n");
		ft_tokens_to_exec(&t_current, &t_tmp);
		print_tokens(&t_tmp);
		free_tokens(&t_tmp);
		if (!t_current)
			break;
		while (t_current && t_current->type == WORD)
			t_current = t_current->next;
	}*/
	
	//ahora ya esta toda la lista seteada, debo abrir los pipes necesarios y llamar a ejecutar los procesos.
	t_current = *tokens;
	e_current = *env;
	//Condicion por si solo hay 1 comando
	if (!data_pipe->pipe_counter && t_current && t_current->type == WORD)
		{
			data_pipe->flag = NO;
			ft_tokens_to_exec(&t_current, &t_tmp);
			if (ft_is_built_in(&t_tmp))
			{
				*exit_status = ft_exec_builtin(&t_tmp, env);
				return ;
			}
			exec_cmd(&t_tmp, env, envp, data_pipe);
			wait (exit_status);
			free_tokens(&t_tmp);
			*exit_status = WIFEXITED(*exit_status);
			while (e_current)
			{
				if (ft_strncmp((e_current)->key_name, "?", 1) == 0)
					break ;
				e_current = e_current->next;
				if (!e_current)
					return ;
			}
			free(e_current->value);
			e_current->value = ft_itoa(WEXITSTATUS(*exit_status));
			if (*exit_status != 0)
			{
				ft_putstr_fd("minishell ", 2);
				ft_putstr_fd(t_current->str, 2);
				ft_putendl_fd(": command not found", 2);
			}
		}
	else
	{
		while(t_current)//para en un futuro si hay mas pipes
		{
			//entra aqui si hay pipes leidas, y ejecuta hasta nodos hasta la pipe(en el exec)
			if (t_current->type == WORD && data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
			{
				ft_tokens_to_exec(&t_current, &t_tmp);
				data_pipe->flag = YES;
				pipe(data_pipe->pipefd); //Crea pipe para comunicar solo 2 comandos
				exec_cmd(&t_tmp, env, envp, data_pipe); //ejecuta el comando y en el proceso hijo comunica la salida con el pipe
				dup2(data_pipe->pipefd[0], 0); //comunica la salida del cmd ejecutado a la pipe
				close(data_pipe->pipefd[0]);// cierra pipes
				close(data_pipe->pipefd[1]);
				data_pipe->pipe_counter--;
				wait (NULL);
				free_tokens(&t_tmp);
			}
			//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
			while (t_current && !(t_current->type == PIPE))
				t_current = t_current->next;
			//caso del ultimo comando a ejecutar, donde se recoge el exit status
			if (!data_pipe->pipe_counter && t_current->type == PIPE)
			{
				data_pipe->flag = NO;
				t_current = t_current->next;
				ft_tokens_to_exec(&t_current, &t_tmp);	
				exec_cmd(&t_tmp, env, envp, data_pipe);
				wait (NULL);
				dup2(data_pipe->og_stdin, 0);
				dup2(data_pipe->og_stdout, 1);
				close(data_pipe->og_stdin);
				close(data_pipe->og_stdout);
				free_tokens(&t_tmp);
				return ;
			}
			t_current = t_current->next;
		}
	}
}

//este parser solo manda la lista a partir del comando que se quiere ejecutar, y ya en el executor se pone el stop de iteracion de la lista donde se encuentra el pipe
//Propuesta-> crear una copia de los nodos en una nueva lista hasta encontrar la pipe y mandarla al executor, todo des de el parser.
//hace falta revisar que sucede con los pipes cuando se interrumpe con una senyal.
