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

#include "../inc/minishell.h"

static char *ft_get_limiter(t_token *token)
{
	t_token *t_current;

	t_current = token;
	while (t_current && t_current->type != LIMITER)
		t_current = t_current->next;
	if (!t_current || t_current->type != LIMITER)
		return (NULL);
	return (t_current->str);
}

void ft_protect_here_doc(t_pipe *data_pipe)
{
	if (dup2(data_pipe->og_stdin, 0) == -1)
		ft_error_system(DUP2_ERROR);
}
void ft_here_doc(t_token *token, t_heredoc *data_heredoc, t_pipe *data_pipe)
{
	int		tmp_fd[2];
	char	*tmp_input;
	char	*limiter;

	if (!data_heredoc->heredoc_counter)
		return ;
	ft_protect_here_doc(data_pipe);
	limiter = ft_get_limiter(token);
	if (!limiter)
		return ;
	pipe(tmp_fd);
	while(42 - 41)
	{
		tmp_input = readline("> ");
		if (ft_strxcmp(tmp_input, limiter) == 0)
		{
			free(tmp_input);
			break ;
		}
		ft_putendl_fd(tmp_input, tmp_fd[1]);
		free(tmp_input);
	}
	if (close(tmp_fd[1]) == -1)
		ft_error_system(CLOSE_ERROR);
	if (dup2(tmp_fd[0], 0) == -1)
		ft_error_system(DUP2_ERROR);
	if (close(tmp_fd[0]) == -1)
		ft_error_system(CLOSE_ERROR);
	data_heredoc->heredoc_counter--;
}
