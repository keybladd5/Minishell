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

void ft_here_doc(t_token *token, t_heredoc *data_heredoc)
{
	int		tmp_fd[2];
	char	*tmp_input;

	if (!data_heredoc->heredoc_counter)
		return ;
	pipe(tmp_fd);
	while(42 - 41)
	{
		tmp_input = readline(">");
		if (ft_strxcmp(tmp_input, token->str) == 0)
		{
			break ;
			ft_putendl_fd(tmp_input, tmp_fd[1]);
			free(tmp_input);
		}
	}
	if (close(tmp_fd[1]) == -1)
		ft_error_system(CLOSE_ERROR);
	if (dup2(tmp_fd[0], 0) == -1)
		ft_error_system(DUP2_ERROR);
	if (close(tmp_fd[0]) == -1)
		ft_error_system(CLOSE_ERROR);
	data_heredoc->heredoc_counter--;
}
