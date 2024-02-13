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

void parser(t_token **tokens, t_env **env, char **envp)
{
	t_token		*t_current;
	t_pipe		*data_pipe = malloc(sizeof(t_pipe));
	
	data_pipe->og_stdin = dup(0);
	data_pipe->og_stdout = dup(1);
	data_pipe->pipe_counter = 0;
	t_current = *tokens;
	//el siguiente bucle da valor a la variable type de cada nodo de la lista obtenida por el lexer, llamada tokens
	while(t_current)
	{	
			//creas pipe y conectas salida y entrada con primer comando y segundo comando
			//hay que marcar de alguna forma que todo los anteriores nodos se deben mandar al executor y que seran el primer comando
			//todo lo que se encuentre despues hasta final de string o siguiente pipe seran el segundo comando
		if (!ft_strncmp(t_current->str, "|", 1)) //si encuentras pipe
		{
				t_current->type = PIPE; //seteo type
				data_pipe->pipe_counter++;
				t_current = t_current->next;
		}
		else
		{
			t_current->type = WORD; //seteo type default
			t_current = t_current->next;
		}
	}
	t_current = *tokens; //vuelve al princio de la lista
	/*if (data_pipe->pipe_counter)//si hay minimo 1 pipe leido
	{
		data_pipe->flag = YES;
		pipe(data_pipe->pipefd); //Crea pipe para comunicar solo 2 comandos
		exec_cmd(tokens, env, envp, data_pipe); //ejecuta el comando y en el proceso hijo comunica la salida con el pipe
		dup2(data_pipe->pipefd[0], 0); //comunica la salida del cmd ejecutado a la pipe
		close(data_pipe->pipefd[0]);// cierra pipes
		close(data_pipe->pipefd[1]);
		data_pipe->pipe_counter--;
		wait (NULL);
	}*/
	//ahora ya esta toda la lista seteada, debo abrir los pipes necesarios y llamar a ejecutar los procesos.
	while(t_current)//para en un futuro si hay mas pipes
	{
		//Condicion por si solo hay 1 comando
		if (!data_pipe->pipe_counter && t_current->type == WORD)
		{
			data_pipe->flag = NO;	
			exec_cmd(&t_current, env, envp, data_pipe);
			wait (NULL);
			return ;
		}
		//entra aqui si hay pipes leidas, y ejecuta hasta nodos hasta la pipe(en el exec)
		if (t_current->type == WORD && data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
		{
			data_pipe->flag = YES;
			pipe(data_pipe->pipefd); //Crea pipe para comunicar solo 2 comandos
			exec_cmd(tokens, env, envp, data_pipe); //ejecuta el comando y en el proceso hijo comunica la salida con el pipe
			dup2(data_pipe->pipefd[0], 0); //comunica la salida del cmd ejecutado a la pipe
			close(data_pipe->pipefd[0]);// cierra pipes
			close(data_pipe->pipefd[1]);
			data_pipe->pipe_counter--;
			wait (NULL);
		}
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (!(t_current->type == PIPE))
			t_current = t_current->next;
		if (!data_pipe->pipe_counter && t_current->type == PIPE)
		{
			data_pipe->flag = NO;
			t_current = t_current->next;	
			exec_cmd(&t_current, env, envp, data_pipe);
			wait (NULL);
			dup2(data_pipe->og_stdin, 0);
			dup2(data_pipe->og_stdout, 1);
			close(data_pipe->og_stdin);
			close(data_pipe->og_stdout);
			return ;
		}
		t_current = t_current->next;
	}
}
