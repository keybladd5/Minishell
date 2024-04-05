/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   typer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:19:30 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 15:19:32 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	typer_pipe(t_token **curr_token, t_parser *d, t_typer *t, int *exit_status)
{
	(*curr_token)->type = PIPE; //seteo type
	t->consecutive_metachar++;
	d->data_pipe->pipe_counter++;
	*curr_token = (*curr_token)->next;
	if (!*curr_token || t->first_token) //variable anadida para el caso "| algo"
		return (ft_error_syntax(exit_status, PIPE, NULL), 1);
	return (0);
}

int	typer_red_in(t_token **curr_token, t_parser *d, int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = RED_IN; //seteo type
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (!*curr_token || (d->data_redir->fd_infile = open((*curr_token)->str, O_RDONLY)) == -1)
	{
		ft_error_syntax(exit_status, RED_IN, *curr_token);
		if (!*curr_token || (*curr_token)->str[0] == '|')
			return ((ft_error_syntax(exit_status, PIPE, NULL), 1));
	}
	d->data_redir->red_in_counter++;
	if (d->data_redir->fd_infile >= 0)
	{
		if (close(d->data_redir->fd_infile) == -1)
			ft_error_system(CLOSE_ERROR);
	}
	d->data_redir->fd_infile = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}

int  typer_red_out(t_token **curr_token, t_parser *d, int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = RED_OUT; //seteo type
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (*curr_token && ft_ismetachar((*curr_token)->str[0]))
		return (ft_error_syntax(exit_status, RED_OUT, *curr_token), 1);
	//para checkear que esto funcione, en la segunda opcion del if, 
	if (!*curr_token || (d->data_redir->fd_outfile = open((*curr_token)->str, O_WRONLY | O_CREAT | O_TRUNC, 0644 )) == -1 ) 
	{
		ft_error_syntax(exit_status, RED_OUT, *curr_token);
		if (!*curr_token) //para que no pete en solo >
			return (1); 
	}
	d->data_redir->red_out_counter++;
	if (d->data_redir->fd_outfile >= 0)
	{
		if (close(d->data_redir->fd_outfile) == -1)
			ft_error_system(CLOSE_ERROR);
	}
	d->data_redir->fd_outfile = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}

int	typer_here_doc(t_token **curr_token, t_parser *d, int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = HERE_DOC; //seteo type
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (!*curr_token || (*curr_token && ft_ismetachar((*curr_token)->str[0]))) 
	{
		ft_error_syntax(exit_status, HERE_DOC, *curr_token);
		if (!*curr_token) //para que no pete en solo <<
			return (1); 
	}
	else
		d->data_hd_append->heredoc_counter++;
	(*curr_token)->type = LIMITER;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}
int typer_append(t_token **curr_token, t_parser *d, int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = APPEND;
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (*curr_token && ft_ismetachar((*curr_token)->str[0]))
		return (ft_error_syntax(exit_status, RED_OUT, *curr_token), 1);
	//para checkear que esto funcione, en la segunda opcion del if, 
	if (!*curr_token || (d->data_hd_append->fd_append = open((*curr_token)->str, O_WRONLY | O_CREAT | O_APPEND, 0644 )) == -1 ) 
	{
		ft_error_syntax(exit_status, APPEND, *curr_token);
		if (!*curr_token) //para que no pete en solo >
			return (1); 
	}
	d->data_hd_append->append_counter++;
	if (d->data_hd_append->fd_append >= 0)
	{
		if (close(d->data_hd_append->fd_append) == -1)
			ft_error_system(CLOSE_ERROR);
	}
	d->data_hd_append->fd_append = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}
//si recibe mode le da tipo, si no no.
void	typer_word(t_token **curr_token, int *consecutive_metachar, int mode)
{
	if (mode == 1)
		(*curr_token)->type = WORD;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
}


//hay que empezar a splitear el codigo en una funcion por cada metacarcter encontrado hasta pode reducir-lo
//se le da el tipo int para el caso "cat <"
//else if (!ft_strncmp((*curr_token)->str, "|\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
//else if (!ft_strncmp((*curr_token)->str, "<\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???

int typer_tokens(t_parser *d, t_token **curr_token, int *exit_status) 
{
	t_typer t;

	while(*curr_token)
	{
		if ((*curr_token)->type == WORD)
			typer_word(curr_token, &t.consecutive_metachar, 0);
		else if (!ft_strncmp((*curr_token)->str, "|\0", 2)) 
			t.cntrl = typer_pipe(curr_token, d, &t, exit_status);
		else if (!ft_strncmp((*curr_token)->str, "<\0", 2)) 
			t.cntrl = typer_red_in(curr_token, d, &t.consecutive_metachar, exit_status);
		else if (!ft_strncmp((*curr_token)->str, ">\0", 2))
			t.cntrl = typer_red_out(curr_token, d, &t.consecutive_metachar, exit_status);
		else if (!ft_strncmp((*curr_token)->str, "<<\0", 2))
			t.cntrl = typer_here_doc(curr_token, d, &t.consecutive_metachar, exit_status);
		else if (!ft_strncmp((*curr_token)->str, ">>\0", 2))
			t.cntrl = typer_append(curr_token, d, &t.consecutive_metachar, exit_status);
		else
			typer_word(curr_token, &t.consecutive_metachar, 1);
		if (t.cntrl == 1)
			return (1);
		t.first_token = 0;
		if (t.consecutive_metachar > 1)
			return (1);
	}
	return (0);
}
