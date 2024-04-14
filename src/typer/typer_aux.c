/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   typer_aux.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 10:09:43 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/08 10:09:45 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	typer_red_in(t_token **curr_token, t_parser *d, \
int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = RED_IN;
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (*curr_token && ft_ismetachar((*curr_token)->str[0]))
		return (ft_error_syntax(exit_status, RED_IN, *curr_token), 1);
	if (*curr_token && (*curr_token)->str)
		d->data_redir->fd_infile = open((*curr_token)->str, O_RDONLY);
	if (!*curr_token || (d->data_redir->fd_infile == -1))
	{
		ft_error_syntax(exit_status, RED_IN, *curr_token);
		if (!*curr_token)
			return (1);
		else if ((*curr_token)->str[0] == '|')
			return ((ft_error_syntax(exit_status, PIPE, NULL), 1));
	}
	d->data_redir->red_in_counter++;
	if (d->data_redir->fd_infile >= 0)
		ft_close(d->data_redir->fd_infile);
	d->data_redir->fd_infile = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}

int	typer_red_out(t_token **curr_token, t_parser *d, \
int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = RED_OUT;
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (*curr_token && ft_ismetachar((*curr_token)->str[0]))
		return (ft_error_syntax(exit_status, RED_OUT, *curr_token), 1);
	if (*curr_token && (*curr_token)->str)
		d->data_redir->fd_outfile \
		= open((*curr_token)->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (!*curr_token || (d->data_redir->fd_outfile == -1))
	{
		ft_error_syntax(exit_status, RED_OUT, *curr_token);
		if (!*curr_token)
			return (1);
	}
	d->data_redir->red_out_counter++;
	if (d->data_redir->fd_outfile >= 0)
		ft_close(d->data_redir->fd_outfile);
	d->data_redir->fd_outfile = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}

int	typer_here_doc(t_token **curr_token, t_parser *d, \
int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = HERE_DOC;
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (!*curr_token || (*curr_token && ft_ismetachar((*curr_token)->str[0])))
	{
		ft_error_syntax(exit_status, HERE_DOC, *curr_token);
		if (!*curr_token)
			return (1);
	}
	else
		d->data_hd_append->heredoc_counter++;
	(*curr_token)->type = LIMITER;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}

int	typer_append(t_token **curr_token, t_parser *d, \
int *consecutive_metachar, int *exit_status)
{
	(*curr_token)->type = APPEND;
	(*consecutive_metachar)++;
	*curr_token = (*curr_token)->next;
	if (*curr_token && ft_ismetachar((*curr_token)->str[0]))
		return (ft_error_syntax(exit_status, RED_OUT, *curr_token), 1);
	if (*curr_token && (*curr_token)->str)
		d->data_hd_append->fd_append \
		= open((*curr_token)->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (!*curr_token || (d->data_hd_append->fd_append == -1))
	{
		ft_error_syntax(exit_status, APPEND, *curr_token);
		if (!*curr_token)
			return (1);
	}
	d->data_hd_append->append_counter++;
	if (d->data_hd_append->fd_append >= 0)
		ft_close(d->data_hd_append->fd_append);
	d->data_hd_append->fd_append = -1;
	if ((*curr_token)->type != ERROR_FILE)
		(*curr_token)->type = DOC;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
	return (0);
}
