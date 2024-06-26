/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   append.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 12:57:47 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/19 12:57:49 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//if (dir_doc->type == PIPE)
//esto es para que salga de la funcion y marcar los pipes como delimitadores
//while (dir_doc && dir_doc->type != APPEND) 
//busca el token '>'. Puede salir con la direccion del token o 
//NULL si no lo ha encontrado
int	ft_append(t_hd_append *data_hd_append, \
t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_hd_append->append_counter)
		return (0);
	while (dir_doc && dir_doc->type != APPEND)
	{
		if (dir_doc->type == PIPE)
			return (0);
		dir_doc = dir_doc->next;
	}
	if (!dir_doc)
		return (0);
	data_hd_append->append_counter--;
	if (dir_doc->next && dir_doc->next->str)
		data_hd_append->fd_append = \
		open(dir_doc->next->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (data_hd_append->fd_append == -1 && \
	dir_doc->next->type == ERROR_FILE)
		return (2);
	ft_dup2(data_hd_append->fd_append, 1);
	if (data_pipe->pipe_counter)
		if (ft_close_v2(&(data_hd_append->fd_append)))
			return (1);
	return (0);
}
