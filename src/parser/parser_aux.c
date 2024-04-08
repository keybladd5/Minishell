/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_aux.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 11:21:53 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/08 11:21:56 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Inicializa los strucs contenidos en el struc del parser
void	ft_init_data_parser(t_parser *d, t_token **tokens)
{
	d->data_hd_append = malloc(sizeof(t_hd_append));
	if (!d->data_hd_append)
		ft_error_system(MALLOC_ERROR);
	ft_memset(d->data_hd_append, 0, sizeof(t_hd_append));
	d->data_hd_append->fd_append = -1;
	d->data_hd_append->fd_append = -1;
	d->process = 0;
	d->t_tmp = NULL;
	d->flag_input = 0;
	d->flag_output = 0;
	d->data_pipe = malloc(sizeof(t_pipe));
	if (!d->data_pipe)
		ft_error_system(MALLOC_ERROR);
	ft_memset(d->data_pipe, 0, sizeof(t_pipe));
	d->data_pipe->og_stdin = dup(0);
	d->data_pipe->og_stdout = dup(1);
	if (d->data_pipe->og_stdin == -1 || d->data_pipe->og_stdout == -1)
		ft_error_system(DUP_ERROR);
	d->data_redir = malloc(sizeof(t_redir));
	if (!d->data_redir)
		ft_error_system(MALLOC_ERROR);
	ft_memset(d->data_redir, 0, sizeof(t_redir));
	d->data_redir->fd_infile = -1;
	d->data_redir->fd_outfile = -1;
	d->t_current = *tokens;
}

int	aux_parse_last_child(t_parser *d, int *exit_status)
{
	if (d->flag_input == 2)
	{
		ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append);
		return (ft_wait_child_process(exit_status, d->process), 1);
	}
	d->flag_output = selector_output(d);
	if (d->flag_output == 2)
	{
		ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append);
		return (ft_wait_child_process(exit_status, d->process), 1);
	}
	else if (d->data_redir->flag_to_close)
	{
		if (dup2(d->data_pipe->og_stdout, 1) == -1)
			ft_error_system(DUP2_ERROR);
		d->data_redir->flag_to_close = 0;
	}
	return (0);
}
