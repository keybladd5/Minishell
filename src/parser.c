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
	{
		if (curr->type == PIPE)
			return ;
		curr = curr->next;
	}
	while (curr && curr->type == WORD)
	{
		tmp = ft_tokendup(curr);
		if (!*new_tokens)
			*new_tokens = tmp;
		else
		{
			if (last)
				last->next = tmp;
		}	
		last = tmp;
		curr = curr->next;
	}
}

void	ft_init_data_parser(t_parser *data, t_token **tokens, int *exit_status)
{
	data->process = 0;
	data->t_tmp = NULL;
	data->data_pipe = malloc(sizeof(t_pipe));
	if (!data->data_pipe)
		ft_error_system(MALLOC_ERROR);
	data->data_redir = malloc(sizeof(t_redir));
	if (!data->data_redir)
		ft_error_system(MALLOC_ERROR);
	data->data_redir->red_in_counter = 0;
	data->data_redir->red_out_counter = 0;
	data->data_pipe->og_stdin = dup(0);
	data->data_pipe->og_stdout = dup(1);
	if (data->data_pipe->og_stdin == -1 || data->data_pipe->og_stdout == -1)
		ft_error_system(DUP_ERROR);
	data->data_pipe->pipe_counter = 0;
	data->data_redir->fd_infile = -1;
	data->data_redir->fd_outfile = -1;
	data->t_current = *tokens;
	if (typer_tokens(data->data_redir, &data->t_current, data->data_pipe, exit_status))
		data->t_current = NULL;
	else
		data->t_current = *tokens;
}
//Procesa los tokens de izquierda a derecha uno por uno.
void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_parser	*data;

	data = malloc(sizeof(t_parser));
	if (!data)
		ft_error_system(MALLOC_ERROR);
	ft_init_data_parser(data, tokens, exit_status);
	if (!data->data_pipe->pipe_counter && data->t_current)//SI SOLO HAY UN COMANDO 
	{
		ft_red_in_aux(data->data_redir, data->t_current,data->data_pipe);
		ft_red_out_aux(data->data_redir, data->t_current, data->data_pipe);
		data->data_pipe->flag = NO;
		ft_tokens_to_exec(&data->t_current, &data->t_tmp);
		if (ft_is_built_in(&data->t_tmp))
		{
			*exit_status = ft_exec_builtin(&data->t_tmp, env);
			free_tokens(&data->t_tmp);
			free(data->data_pipe);
			free(data->data_redir);
			return ;
		}
		executor(&data->t_tmp, env, envp, data->data_pipe);
		free_tokens(&data->t_tmp);
		ft_wait_child_process(data->t_current->str, exit_status, 1);
		ft_aux_close(data->data_pipe, data->data_redir);
		free(data);
		return ;
	}
	while(data->t_current)//PIPELINE
	{
		while ((data->t_current && data->t_current->type != WORD) || (data->t_current &&\
		 data->t_current->next && data->t_current->next->type == RED_IN) )//en caso de orden de redireccion necesito colocarme en la palabra a ejecutar
		{
			if (ft_red_in_aux(data->data_redir, data->t_current, data->data_pipe))
			{
				data->data_pipe->flag = NO;
				break ;
			}
			data->t_current = data->t_current->next;
		}
		//entra aqui si hay pipes leidas, y ejecuta hasta nodos hasta la pipe(en el exec)
		if (data->t_current && data->data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
		{
			if (ft_red_out_aux(data->data_redir, data->t_current, data->data_pipe))
				data->data_pipe->flag = NO;
			else
				data->data_pipe->flag = YES;
			ft_tokens_to_exec(&data->t_current, &data->t_tmp);
			pipe(data->data_pipe->pipefd);
			data->process++;
			executor(&data->t_tmp, env, envp, data->data_pipe);
			if (dup2(data->data_pipe->pipefd[0], 0)== -1) //tiene que comunicar la tuberia contenga o no contenido siempre en la pipeline 
				ft_error_system(DUP2_ERROR);
			close(data->data_pipe->pipefd[0]);
			close(data->data_pipe->pipefd[1]);
			data->data_pipe->pipe_counter--;
			free_tokens(&data->t_tmp);
			
		}
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (data->t_current && !(data->t_current->type == PIPE))
			data->t_current = data->t_current->next;
		//caso del ultimo comando a ejecutar, donde se recoge el exit status
		if (data->t_current && (data->t_current->type == PIPE && !data->data_pipe->pipe_counter))
		{
			data->data_pipe->flag = NO;
			if (dup2(data->data_pipe->og_stdout, 1) == -1) //esto ha hecho que funcione "cat tet1 > newfile | wc newfile" como debe
				ft_error_system(DUP2_ERROR);
			data->t_current = data->t_current->next;
			ft_red_in_aux(data->data_redir, data->t_current, data->data_pipe);
			ft_red_out_aux(data->data_redir, data->t_current, data->data_pipe);
			ft_tokens_to_exec(&data->t_current, &data->t_tmp);
			executor(&data->t_tmp, env, envp, data->data_pipe);
			data->process++;
			free_tokens(&data->t_tmp);
			if (dup2(data->data_pipe->og_stdin, 0) == -1) //esto ha hecho que funcione "cat tet1 > newfile | wc newfile" como debe
				ft_error_system(DUP2_ERROR);
			ft_aux_close(data->data_pipe, data->data_redir);
			ft_wait_child_process(data->t_current->str, exit_status, data->process);
			return (free(data));
		}
		if (data->t_current)
			data->t_current = data->t_current->next;
	}
	//SIN ESTO DA LEAKS CUANDO EL INPUT ESTA VACIO. COMPPROBAR QUE NO HAYA DOBLE FREE!!
	free(data->data_pipe);
	free(data->data_redir);
	free(data);
}

