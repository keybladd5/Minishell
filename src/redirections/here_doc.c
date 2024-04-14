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

int	ft_aux_here_doc(t_parser *d, int tmp_fd[2], t_token *token)
{
	if (!d->data_hd_append->heredoc_counter)
		return (1);
	ft_dup2(d->data_pipe->og_stdin, 0);
	d->data_hd_append->limiter = ft_get_limiter(token);
	if (!d->data_hd_append->limiter)
		return (1);
	pipe(tmp_fd);
	return (0);
}

//en caso de control+d lanzar directamente el string LIMITER para inalizar!!!!!
void	ft_here_doc(t_token *token, t_parser *d, t_env **env, int *exit_status)
{
	int		tmp_fd[2];

	if (ft_aux_here_doc(d, tmp_fd, token))
		return ;
	while (42 - 41)
	{
		d->data_hd_append->tmp_input = readline("> ");
		if (!d->data_hd_append->tmp_input || (d->data_hd_append->tmp_input \
		&& d->data_hd_append->limiter && ft_strxcmp(\
		d->data_hd_append->tmp_input, d->data_hd_append->limiter) == 0))
		{
			free(d->data_hd_append->tmp_input);
			break ;
		}
		if (d->data_hd_append->tmp_input)
		{
			expansor(&d->data_hd_append->tmp_input, env, *exit_status);
			if (d->data_hd_append->tmp_input)
				ft_putendl_fd(d->data_hd_append->tmp_input, tmp_fd[1]);
		}
		free(d->data_hd_append->tmp_input);
	}
	ft_dup2(tmp_fd[0], 0);
	ft_close2(tmp_fd[0], tmp_fd[1]);
	d->data_hd_append->heredoc_counter--;
}
