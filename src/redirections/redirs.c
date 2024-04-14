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

#include "../../inc/minishell.h"

static int	ft_aux_red_in_aux(t_token *t_current)
{
	if (t_current->type == WORD)
		t_current->type = ERROR_WORD;
	else if (t_current->type == RED_IN)
	{
		if (t_current && t_current->next && t_current->next->next \
		&& t_current->next->next->type == WORD)
			return (t_current->next->next->type = ERROR_WORD, 2);
		else if (t_current->next->type == ERROR_FILE)
			return (2);
	}
	return (1);
}
//Funcion que abre los fds en caso de redireccion input, 
//el retorno se usa para iterar en el parser en base a los tokens
//while (dir_doc && dir_doc->type != RED_IN) 
//busca el token '<'. Puede salir con la 
//direccion del token o NULL si no lo ha encontrado
//t_current->type = ERROR_WORD; 
// < test/new << eof cat en este caso esta accion peta 
//if (t_current && t_current->next && t_current->next->next && 
//t_current->next->next->type == WORD) 
//se especifica la ultima condicion para que funncione 
//la combinacion de here_doc y redir_in con el comando al final

int	ft_red_in_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_redir->red_in_counter)
		return (0);
	while (dir_doc && dir_doc->type != RED_IN)
		dir_doc = dir_doc->next;
	if (!dir_doc)
		return (0);
	data_redir->fd_infile = open(dir_doc->next->str, O_RDONLY);
	data_redir->red_in_counter--;
	if (data_redir->fd_infile == -1)
	{
		if (ft_aux_red_in_aux(t_current) == 2)
			return (2);
		else
			return (1);
	}
	ft_dup2(data_redir->fd_infile, 0);
	if (data_pipe->pipe_counter)
		if (ft_close_v2(&(data_redir->fd_infile)))
			return (1);
	return (0);
}

//while (dir_doc && dir_doc->type != RED_OUT) 
//busca el token '>'. Puede salir con la direccion del token o NULL 
//si no lo ha encontrado
//if (dir_doc->type == PIPE)
//esto es para que salga de la funcion y marcar los pipes como delimitadores
int	ft_red_out_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_redir->red_out_counter)
		return (0);
	while (dir_doc && dir_doc->type != RED_OUT)
	{
		if (dir_doc->type == PIPE)
			return (0);
		dir_doc = dir_doc->next;
	}
	if (!dir_doc)
		return (0);
	data_redir->red_out_counter--;
	data_redir->fd_outfile = \
	open(dir_doc->next->str, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (data_redir->fd_outfile == -1 && dir_doc->next->type == ERROR_FILE)
		return (2);
	ft_dup2(data_redir->fd_outfile, 1);
	if (data_pipe->pipe_counter)
		if (ft_close_v2(&(data_redir->fd_outfile)))
			return (1);
	return (0);
}

//Reestablece los fd originales, cierra el resto de fd y 
//libera estructuras de pipes y redirs
void	ft_aux_close(t_pipe *data_pipe, t_redir *data_redir, \
t_hd_append *data_heredoc)
{
	if (!data_pipe->pipe_counter)
	{
		ft_dup2(data_pipe->og_stdin, 0);
		ft_dup2(data_pipe->og_stdout, 1);
	}
	if (data_redir->fd_infile >= 0)
		ft_close(data_redir->fd_infile);
	if (data_redir->fd_outfile >= 0)
		ft_close(data_redir->fd_outfile);
	if (data_pipe->og_stdin >= 0)
		ft_close(data_pipe->og_stdin);
	if (data_pipe->og_stdout >= 0)
		ft_close(data_pipe->og_stdout);
	free(data_pipe);
	free(data_redir);
	free(data_heredoc);
}
