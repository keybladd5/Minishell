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
	//ahora ya esta toda la lista seteada, debo abrir los pipes necesarios y llamar a ejecutar los procesos.
	t_current = *tokens; //vuelve al princio de la lista
	if (data_pipe->pipe_counter)//si hay minimo 1 pipe leido
	{
		pipe(data_pipe->pipefd); //Crea pipe para comunicar solo 2 comandos
		exec_cmd(tokens, env, envp, data_pipe); //ejecuta el comando y en el proceso hijo comunica la salida con el pipe
		dup2(data_pipe->pipefd[0], 0); //comunica la entrada con la salida del anterior proceso
		close(data_pipe->pipefd[0]);// cierra pipes
		close(data_pipe->pipefd[1]);
		data_pipe->pipe_counter--;
	}	
	while(t_current)//para en un futuro si hay mas pipes
	{
		if (!data_pipe->pipe_counter && t_current->type == PIPE)
		{
			t_current = t_current->next;	
			exec_cmd(&t_current, env, envp, data_pipe);
			return ;
		}

		t_current = t_current->next;
	}
}
