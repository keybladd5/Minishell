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

//unefficient, but this is only works when in a pipeline, the actual cmd is
//canceled by a no_permission doc, but the next cmd need a the read pipe
//to work as bash
void	l_red_out(t_parser *d, t_env **env, char **envp)
{
	pipe(d->data_pipe->pipefd);
	d->process++;
	d->data_pipe->flag = YES;
	executor(NULL, env, envp, d->data_pipe);
	if (dup2(d->data_pipe->pipefd[0], 0)== -1) //tiene que comunicar la tuberia contenga o no contenido siempre en la pipeline 
		ft_error_system(DUP2_ERROR);
	close(d->data_pipe->pipefd[0]);
	close(d->data_pipe->pipefd[1]);
	d->data_pipe->pipe_counter--;
}

// Inicializa los strucs contenidos en el struc del parser, y llama 
//	al typer para setear tipos a los tokens
//	en caso de error pasado por este, pone en NULL la lista de 
//	t_current para evitar la ejecucion
void	ft_init_data_parser(t_parser *d, t_token **tokens,\
		int *exit_status)
{
	d->data_heredoc = malloc(sizeof(t_pipe));
	if (!d->data_heredoc)
		ft_error_system(MALLOC_ERROR);
	d->data_heredoc->heredoc_counter = 0;
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
	d->data_pipe->og_stdin = dup(0);
	d->data_pipe->og_stdout = dup(1);
	if (d->data_pipe->og_stdin == -1 || d->data_pipe->og_stdout == -1)
		ft_error_system(DUP_ERROR);
	d->data_pipe->flag = 0;
	d->data_pipe->pipe_counter = 0;
	d->data_redir->fd_infile = -1;
	d->data_redir->fd_outfile = -1;
	d->t_current = *tokens;
	if (typer_tokens(d->data_redir, &d->t_current, d->data_pipe,\
	 d->data_heredoc, exit_status))
		d->t_current = NULL;
	else
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
			ft_here_doc(tmp, d->data_heredoc, d->data_pipe);
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
