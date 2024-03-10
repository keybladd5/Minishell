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

int typer_tokens(t_redir *data_redir, t_token **t_current, t_pipe *data_pipe, int *exit_status) //se le da el tipo int para el caso "cat <"
{
	int first_token;
	int consecutive_metachar;

	first_token = 1;//variable anadida para el caso "| algo"
	consecutive_metachar = 0;
	while(*t_current)
	{	
		if (!ft_strncmp((*t_current)->str, "|\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			(*t_current)->type = PIPE; //seteo type
			consecutive_metachar++;
			data_pipe->pipe_counter++;
			*t_current = (*t_current)->next;
			if (!*t_current || first_token) //variable anadida para el caso "| algo"
				return (ft_error_syntax(exit_status, PIPE, NULL), 1);
		}
		else if (!ft_strncmp((*t_current)->str, "<\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			(*t_current)->type = RED_IN; //seteo type
			consecutive_metachar++;
			*t_current = (*t_current)->next;
			if (!*t_current || (data_redir->fd_infile = open((*t_current)->str, O_RDONLY)) == -1)
			{
				ft_error_syntax(exit_status, RED_IN, *t_current);
				if (!*t_current)
					return (1);
			}
			data_redir->red_in_counter++;
			if (data_redir->fd_infile >= 0)
			{
				if (close(data_redir->fd_infile) == -1)
					ft_error_system(CLOSE_ERROR);
			}
			data_redir->fd_infile = -1;
			(*t_current)->type = DOC;
			consecutive_metachar = 0;
			*t_current = (*t_current)->next;	
		}
		else if (!ft_strncmp((*t_current)->str, ">\0", 2))
		{
			(*t_current)->type = RED_OUT; //seteo type
			consecutive_metachar++;
			*t_current = (*t_current)->next;
			//para checkear que esto funcione, en la segunda opcion del if, 
			if (!*t_current || (data_redir->fd_outfile = open((*t_current)->str, O_WRONLY | O_CREAT | O_TRUNC, 0666 )) == -1 ) 
			{
				ft_error_syntax(exit_status, RED_OUT, *t_current);
				if (!*t_current) //para que no pete en solo >
					return (1); 
			}
			else
				data_redir->red_out_counter++;
			if (data_redir->fd_outfile >= 0)
			{
				if (close(data_redir->fd_outfile) == -1)
					ft_error_system(CLOSE_ERROR);
			}
			(*t_current)->type = DOC;
			consecutive_metachar = 0;
			*t_current = (*t_current)->next;	
		}
		else
		{
			(*t_current)->type = WORD; //seteo type default
			consecutive_metachar = 0;
			*t_current = (*t_current)->next;
		}
		first_token = 0;
		if (consecutive_metachar > 1)
			return (1);
		
	}
	return (0) ;
}


/*int typer_tokens(t_redir *data_redir, t_token **t_current, t_pipe *data_pipe, int *exit_status)
{
	int first_token;

	first_token = 1;//variable anadida para el caso "| algo"
	while(*t_current)
	{	
		if (!ft_strncmp((*t_current)->str, "|\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			(*t_current)->type = PIPE; //seteo type
			data_pipe->pipe_counter++;
			*t_current = (*t_current)->next;
			if (!*t_current || first_token) //variable anadida para el caso "| algo"
				return (ft_error_syntax(exit_status, PIPE, NULL));
		}
		else if (!ft_strncmp((*t_current)->str, "<\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			(*t_current)->type = RED_IN; //seteo type
			*t_current = (*t_current)->next;
			if (!*t_current || (data_redir->fd_infile = open((*t_current)->str, O_RDONLY)) == -1)
				return (ft_error_syntax(exit_status, RED_IN, *t_current));
			if (data_redir->fd_infile >= 0)
			{
				if (close(data_redir->fd_infile) == -1)
					ft_error_system(CLOSE_ERROR);
			}
			data_redir->fd_infile = -1;
			(*t_current)->type = DOC;
			data_redir->red_in_counter++;
			*t_current = (*t_current)->next;	
		}
		else if (!ft_strncmp((*t_current)->str, ">\0", 2))
		{
			(*t_current)->type = RED_OUT; //seteo type
			*t_current = (*t_current)->next;
			//para checkear que esto funcione, en la segunda opcion del if, 
			if (!*t_current || (data_redir->fd_outfile = open((*t_current)->str, O_WRONLY | O_CREAT | O_TRUNC, 0666 )) == -1 ) 
				return (ft_error_syntax(exit_status, RED_OUT, *t_current));
			if (data_redir->fd_outfile >= 0)
			{
				if (close(data_redir->fd_outfile) == -1)
					ft_error_system(CLOSE_ERROR);
			}
			(*t_current)->type = DOC;
			data_redir->red_out_counter++;
			*t_current = (*t_current)->next;	
		}
		else
		{
			(*t_current)->type = WORD; //seteo type default
			*t_current = (*t_current)->next;
		}
		first_token = 0;
	}
	return (0);
}*/
