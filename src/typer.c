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

#include "../inc/minishell.h"

int typer_tokens(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe, int *exit_status)
{
	while(t_current)
	{	
		if (!ft_strncmp(t_current->str, "|\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			t_current->type = PIPE; //seteo type
			data_pipe->pipe_counter++;
			t_current = t_current->next;
			if (!t_current)
				return (ft_error_syntax(exit_status, PIPE, NULL));
		}
		else if (!ft_strncmp(t_current->str, "<\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			t_current->type = RED_IN; //seteo type
			t_current = t_current->next;
			if (!t_current || (data_redir->fd_infile = open(t_current->str, O_RDONLY)) == -1)
				return (ft_error_syntax(exit_status, RED_IN, t_current));
			close(data_redir->fd_infile);
			data_redir->fd_infile = -1;
			t_current->type = DOC;
			data_redir->red_in_counter++;
			t_current = t_current->next;	
		}
		else if (!ft_strncmp(t_current->str, ">\0", 2))
		{
			t_current->type = RED_OUT; //seteo type
			t_current = t_current->next;
			//para checkear que esto funcione, en la segunda opcion del if, 
			if (!t_current || (data_redir->fd_outfile = open(t_current->str, O_WRONLY | O_CREAT | O_TRUNC, 0666 )) == -1 ) 
				return (ft_error_syntax(exit_status, RED_OUT, t_current));
			close(data_redir->fd_outfile);
			t_current->type = DOC;
			data_redir->red_out_counter++;
			t_current = t_current->next;	
		}
		else
		{
			t_current->type = WORD; //seteo type default
			t_current = t_current->next;
		}
	}
	return (0);
}