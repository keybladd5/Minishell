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

//funcion aux, crea un nuevo token copiando el que recibe por parametro
static t_token *ft_tokendup(t_token *token)
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
//Crea una copia de los tokens a enviar al executor 
static void ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens)
{
	t_token	*tmp = NULL;
	t_token	*last = NULL;
	t_token *curr = NULL;
	curr = *og_tokens;
	while (curr && curr->type != WORD)//para saltar casos como "< test cat" hasta la palabra necesaria para ejecutar como es el cat
		curr = curr->next;
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

//Procesa los tokens de izquierda a derecha uno por uno.
void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_token		*t_current;
	t_token		*t_tmp = NULL;
	t_pipe		*data_pipe;
	t_redir		*data_redir;
	int			process = 0;

	data_pipe = malloc(sizeof(t_pipe));
	if (!data_pipe)
		exit (MALLOC_ERROR);
	data_redir = malloc(sizeof(t_redir));
	if (!data_redir)
		exit (MALLOC_ERROR);
	data_redir->red_in_counter = 0;
	data_redir->red_out_counter = 0;
	data_pipe->og_stdin = dup(0);
	data_pipe->og_stdout = dup(1);
	data_pipe->pipe_counter = 0;
	data_redir->fd_infile = -1;
	data_redir->fd_outfile = -1;
	t_current = *tokens;
	if (typer_tokens(data_redir, t_current, data_pipe, exit_status))
		return (ft_aux_close(data_pipe, data_redir));
	t_current = *tokens;
	if (!data_pipe->pipe_counter && t_current)//SI SOLO HAY UN COMANDO 
	{
		ft_red_in_aux(data_redir, t_current, data_pipe);
		ft_red_out_aux(data_redir, t_current, data_pipe);
		data_pipe->flag = NO;
		ft_tokens_to_exec(&t_current, &t_tmp);
		if (ft_is_built_in(&t_tmp))
		{
			*exit_status = ft_exec_builtin(&t_tmp, env);
			free_tokens(&t_tmp);
			free(data_pipe);
			free(data_redir);
			return ;
		}
		exec_cmd(&t_tmp, env, envp, data_pipe);
		free_tokens(&t_tmp);
		ft_wait_child_process(t_current->str, exit_status, 1);
		ft_aux_close(data_pipe, data_redir);
		return ;
	}
	while(t_current)//PIPELINE
	{
		while ((t_current && t_current->type != WORD) || (t_current->next && t_current->next->type == RED_IN ))//en caso de orden de redireccion necesito colocarme en la palabra a ejecutar
		{
			if (ft_red_in_aux(data_redir, t_current, data_pipe))
				break ;
			/*if (ft_red_out_aux(data_redir, t_current, data_pipe))
				break ;*/
			t_current = t_current->next;
		}
		//entra aqui si hay pipes leidas, y ejecuta hasta nodos hasta la pipe(en el exec)
		if (t_current && data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
		{
			if (ft_red_out_aux(data_redir, t_current, data_pipe))
			{
				data_pipe->flag = NO;
			}
			else
				data_pipe->flag = YES;
			ft_tokens_to_exec(&t_current, &t_tmp);
			pipe(data_pipe->pipefd);
			process++;
			exec_cmd(&t_tmp, env, envp, data_pipe);

			//tiene que comunicar la tuberia contenga o no contenido siempre en la pipeline 
			dup2(data_pipe->pipefd[0], 0); 
			
			close(data_pipe->pipefd[0]);
			close(data_pipe->pipefd[1]);
			data_pipe->pipe_counter--;
			free_tokens(&t_tmp);
			//ft_wait_child_process(t_current->str, exit_status);
		}
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (t_current && !(t_current->type == PIPE))
			t_current = t_current->next;
		//caso del ultimo comando a ejecutar, donde se recoge el exit status
		if (t_current && (t_current->type == PIPE && !data_pipe->pipe_counter))
		{
			data_pipe->flag = NO;
			dup2(data_pipe->og_stdout, 1); //esto ha hecho que funcione "cat tet1 > newfile | wc newfile" como debe
			t_current = t_current->next;
			ft_red_in_aux(data_redir, t_current, data_pipe);
			ft_red_out_aux(data_redir, t_current, data_pipe);
			ft_tokens_to_exec(&t_current, &t_tmp);

			exec_cmd(&t_tmp, env, envp, data_pipe);
			process++;
			free_tokens(&t_tmp);
			ft_aux_close(data_pipe, data_redir);
			ft_wait_child_process(t_current->str, exit_status, process);
			return ;
		}
		if (t_current)
			t_current = t_current->next;
	}
	//SIN ESTO DA LEAKS CUANDO EL INPUT ESTA VACIO. COMPPROBAR QUE NO HAYA DOBLE FREE!!
	free(data_pipe);
	free(data_redir);
}

//El problema actual es que no se puede ejecutar el REDIR_IN en cualquier parte de la pipeline substituyendo los fds
