/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:12:10 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 15:12:11 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//Funcion que abre los fds en caso de redireccion input, el retorno se usa para iterar en el parser en base a los tokens
int	ft_red_in_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_redir->red_in_counter)
		return (0);
	while (dir_doc && dir_doc->type != RED_IN) //busca el token '<'. Puede salir con la direccion del token o NULL si no lo ha encontrado
		dir_doc = dir_doc->next;
	if (!dir_doc)
		return (0);
	data_redir->fd_infile = open(dir_doc->next->str, O_RDONLY);
	data_redir->red_in_counter--;
	if (dup2(data_redir->fd_infile, 0)== -1)
		ft_error_system(DUP2_ERROR);
	if (data_pipe->pipe_counter)
	{
		close(data_redir->fd_infile);
		data_redir->fd_infile = -1;
		return (1);
	}
	return (0);
}
int	ft_red_out_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_redir->red_out_counter)
		return (0);
	while (dir_doc && dir_doc->type != RED_OUT) //busca el token '>'. Puede salir con la direccion del token o NULL si no lo ha encontrado
	{	
		if (dir_doc->type == PIPE)//esto es para que salga de la funcion y marcar los pipes como delimitadores 
			return (0);
		dir_doc = dir_doc->next;
	}
	if (!dir_doc)
		return (0);
	data_redir->red_out_counter--;
	data_redir->fd_outfile = open(dir_doc->next->str, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	if (dup2(data_redir->fd_outfile, 1) == -1)
		ft_error_system(DUP2_ERROR);
	if (data_pipe->pipe_counter)
	{
		close(data_redir->fd_outfile);
		data_redir->fd_outfile = -1;
		return (1);
	}
	return (0);
}
//Reestablece los fd originales, cierra el resto de fd y libera estructuras de pipes y redirs
void	ft_aux_close(t_pipe *data_pipe, t_redir *data_redir)
{
	if (!data_pipe->pipe_counter)//pedniente modificar ya se hace dup2 de mas
	{
		//dup2(data_pipe->og_stdin, 0);
		//dup2(data_pipe->og_stdout, 1);
		if (dup2(data_pipe->og_stdin, 0) == -1)
			ft_error_system(DUP2_ERROR);
		if (dup2(data_pipe->og_stdout, 1) == -1)
			ft_error_system(DUP2_ERROR);
	}
	close(data_redir->fd_infile);
	close(data_redir->fd_outfile);
	close(data_pipe->og_stdin);
	close(data_pipe->og_stdout);
	free(data_pipe);
	free(data_redir);
}
