/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/17 11:15:57 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/17 11:15:59 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//funcion aux, crea un nuevo token copiando el que recibe por parametro
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

//Crea una copia de los tokens a enviar al executor, SOLO DE LAS WORDS encontradas hasta la pipe
void ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens)
{
	t_token	*tmp;
	t_token	*last;
	t_token	*curr;

	curr = *og_tokens;
	last = NULL;
	tmp = NULL;
	while (curr && curr->type != PIPE)
	{
		if (curr->type == WORD)
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
		}
		curr = curr->next;
	}
}
//unefficient, but this is only works when in a pipeline, the actual cmd is
//canceled by a no_permission doc, but the next cmd need a the read pipe
//to work as bash
void	l_red_out(t_parser *d, t_env **env)
{
	pipe(d->data_pipe->pipefd);
	d->process++;
	d->data_pipe->flag = YES;
	executor(NULL, env, d->data_pipe);
	if (dup2(d->data_pipe->pipefd[0], 0)== -1) //tiene que comunicar la tuberia contenga o no contenido siempre en la pipeline 
		ft_error_system(DUP2_ERROR);
	close(d->data_pipe->pipefd[0]);
	close(d->data_pipe->pipefd[1]);
	d->data_pipe->pipe_counter--;
}

// Inicializa los strucs contenidos en el struc del parser
void	ft_init_data_parser(t_parser *d, t_token **tokens)
{
	d->data_hd_append= malloc(sizeof(t_pipe));
	if (!d->data_hd_append)
		ft_error_system(MALLOC_ERROR);
	d->data_hd_append->heredoc_counter = 0;
	d->data_hd_append->append_counter = 0;
	d->data_hd_append->fd_append = -1;
	d->process = 0;
	d->t_tmp = NULL;
	d->flag_input = 0;
	d->flag_output = 0;
	d->data_pipe = malloc(sizeof(t_pipe));
	if (!d->data_pipe)
		ft_error_system(MALLOC_ERROR);
	d->data_redir = malloc(sizeof(t_redir));
	if (!d->data_redir)
		ft_error_system(MALLOC_ERROR);
	d->data_redir->red_in_counter = 0;
	d->data_redir->red_out_counter = 0;
	d->data_redir->flag_to_close = 0;
	d->data_pipe->og_stdin = dup(0);
	d->data_pipe->og_stdout = dup(1);
	if (d->data_pipe->og_stdin == -1 || d->data_pipe->og_stdout == -1)
		ft_error_system(DUP_ERROR);
	d->data_pipe->flag = 0;
	d->data_pipe->pipe_counter = 0;
	d->data_redir->fd_infile = -1;
	d->data_redir->fd_outfile = -1;
	d->t_current = *tokens;
}

//itera sin modificar los tokens para comprobar prioridad de input en el comando, en caso de error de archivo
//por falta de permisos o inexistente, devuelve un 2 otorgado por ft_red_in_aux;
int selector_input(t_parser *d)
{
	int i;
	t_token *tmp;

	tmp = d->t_current;
	i = 0;
	while (tmp  && tmp->type != PIPE)
	{
		if (tmp->type == HERE_DOC)
		{
			ft_here_doc(tmp, d->data_hd_append, d->data_pipe);
			i = 0;
		}
		else if (tmp->type == RED_IN)
			i = ft_red_in_aux(d->data_redir, tmp,d->data_pipe);
		if (i == 2)
			return (i);
		tmp = tmp->next;
	}
	return (i);
}
//itera sin modificar los tokens para comprobar prioridad de output en el comando, en caso de error de archivo
//por falta de permisos, devuelve un 2 otorgado por ft_red_out_aux o ft_append;
int selector_output(t_parser *d)
{
	int i;
	t_token *tmp;

	tmp = d->t_current;
	i = 0;
	while (tmp  && tmp->type != PIPE)
	{
		if (tmp->type == APPEND)
			i = ft_append(d->data_hd_append, tmp,d->data_pipe);
		else if (tmp->type == RED_OUT)
			i = ft_red_out_aux(d->data_redir, tmp,d->data_pipe);
		if (i == 2)
			return (i);
		tmp = tmp->next;
	}
	return (i);
}
