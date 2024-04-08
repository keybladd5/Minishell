/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 13:08:06 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/11 13:08:07 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	*ft_get_limiter(t_token *token)
{
	t_token	*t_current;

	t_current = token;
	while (t_current && t_current->type != LIMITER)
		t_current = t_current->next;
	if (!t_current || t_current->type != LIMITER)
		return (NULL);
	return (t_current->str);
}

//en caso de control+d lanzar directamente el string LIMITER para inalizar!!!!!
void	ft_here_doc(t_token *token, t_hd_append *d_hd, t_pipe *data_pipe)
{
	int		tmp_fd[2];

	if (!d_hd->heredoc_counter)
		return ;
	ft_dup2(data_pipe->og_stdin, 0);
	d_hd->limiter = ft_get_limiter(token);
	if (!d_hd->limiter)
		return ;
	pipe(tmp_fd);
	while (42 - 41)
	{
		d_hd->tmp_input = readline("> ");
		if (!d_hd->tmp_input || (d_hd->tmp_input \
		&& d_hd->limiter && ft_strxcmp(d_hd->tmp_input, d_hd->limiter) == 0))
		{
			free(d_hd->tmp_input);
			break ;
		}
		if (d_hd->tmp_input)
			ft_putendl_fd(d_hd->tmp_input, tmp_fd[1]);
		free(d_hd->tmp_input);
	}
	ft_dup2(tmp_fd[0], 0);
	ft_close2(tmp_fd[0], tmp_fd[1]);
	d_hd->heredoc_counter--;
}
