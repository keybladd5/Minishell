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
	(*curr_token)->type = PIPE;
	t->c_m++;
	d->data_pipe->pipe_counter++;
	*curr_token = (*curr_token)->next;
	if (!*curr_token || t->first_token)
		return (ft_error_syntax(exit_status, PIPE, NULL), 1);
	return (0);
}

//si recibe mode le da tipo, si no no.
void	typer_word(t_token **curr_token, int *consecutive_metachar, int mode)
{
	if (mode == 1 && (*curr_token)->type != ERROR_WORD)
		(*curr_token)->type = WORD;
	*consecutive_metachar = 0;
	*curr_token = (*curr_token)->next;
}

int	typer_tokens(t_parser *d, t_token **curr_token, int *exit_status)
{
	t_typer	t;

	while (*curr_token)
	{
		if ((*curr_token)->type == WORD)
			typer_word(curr_token, &t.c_m, 0);
		else if (!ft_strncmp((*curr_token)->str, "|\0", 2))
			t.cntrl = typer_pipe(curr_token, d, &t, exit_status);
		else if (!ft_strncmp((*curr_token)->str, "<\0", 2))
			t.cntrl = typer_red_in(curr_token, d, &t.c_m, exit_status);
		else if (!ft_strncmp((*curr_token)->str, ">\0", 2))
			t.cntrl = typer_red_out(curr_token, d, &t.c_m, exit_status);
		else if (!ft_strncmp((*curr_token)->str, "<<\0", 2))
			t.cntrl = typer_here_doc(curr_token, d, &t.c_m, exit_status);
		else if (!ft_strncmp((*curr_token)->str, ">>\0", 2))
			t.cntrl = typer_append(curr_token, d, &t.c_m, exit_status);
		else
			typer_word(curr_token, &t.c_m, 1);
		if (t.cntrl == 1)
			return (1);
		t.first_token = 0;
		if (t.c_m > 1)
			return (1);
	}
	return (0);
}
