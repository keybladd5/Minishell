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


void parser(t_token **tokens, t_env **env, char **envp)
{
	t_token		*t_current;
	t_token		*t_tmp = NULL;
	t_pipe		*data_pipe = malloc(sizeof(t_pipe));
	int			status = 0;
	
	data_pipe->og_stdin = dup(0);
	data_pipe->og_stdout = dup(1);
	data_pipe->pipe_counter = 0;
	t_current = *tokens;

	//el siguiente bucle da valor a la variable type de cada nodo de la lista obtenida por el lexer, llamada tokens
	while(t_current)
	{	
		if (!ft_strncmp(t_current->str, "|", 1)) //si encuentras pipe
		{
				t_current->type = PIPE; //seteo type
				data_pipe->pipe_counter++;
				t_current = t_current->next;
				if (!t_current)
				{
					ft_printf("\033[31mminishell: syntax error near unexpected token `|'\x1b[0m\n");
					while(*env)
					{
						if (ft_strncmp((*env)->key_name, "?", 1) == 0)
							break ;
						*env = (*env)->next;
					}
					free((*env)->value);
					(*env)->value = ft_strdup("258");
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
	//se realiza el siguien te proceso para copiar la lista hasta la pipe y mandar solo eso. Se almacena en t_tmp
	t_current = *tokens; //vuelve al princio de la lista
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
	}
	t_current = *tokens;
	//ahora ya esta toda la lista seteada, debo abrir los pipes necesarios y llamar a ejecutar los procesos.
	while(t_current)//para en un futuro si hay mas pipes
	{
		//Condicion por si solo hay 1 comando
		if (!data_pipe->pipe_counter && t_current->type == WORD)
		{
			data_pipe->flag = NO;
			ft_tokens_to_exec(&t_current, &t_tmp);
			exec_cmd(&t_tmp, env, envp, data_pipe);
			wait (&status);
			free_tokens(&t_tmp);
			if (WIFEXITED(status) != 0)
			{	
				while(*env)
				{
					if (ft_strncmp((*env)->key_name, "?", 1) == 0)
						break ;
					*env = (*env)->next;
				}
				free((*env)->value);
				(*env)->value = ft_itoa(WEXITSTATUS(status));
				ft_printf("minishell %s: command not found", t_tmp->str);
				return ;
			}
		}
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
		while (!(t_current->type == PIPE))
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

//este parser solo manda la lista a partir del comando que se quiere ejecutar, y ya en el executor se pone el stop de iteracion de la lista donde se encuentra el pipe
//Propuesta-> crear una copia de los nodos en una nueva lista hasta encontrar la pipe y mandarla al executor, todo des de el parser.
//hace falta revisar que sucede con los pipes cuando se interrumpe con una senyal.
