/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 11:42:36 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/08 11:42:38 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_close(int fd)
{
	if (close(fd) == -1)
		ft_error_system(CLOSE_ERROR);
}

void	ft_close2(int fd1, int fd2)
{
	ft_close(fd1);
	ft_close(fd2);
}

//close the fd, set the fd to -1, and returns 1
int	ft_close_v2(int *fd)
{
	ft_close(*fd);
	*fd = -1;
	return (1);
}

void	ft_dup2(int fd1, int fd2)
{
	if (dup2(fd1, fd2) == -1)
		ft_error_system(DUP2_ERROR);
}

//free the list do it by lexer
void	free_tokens(t_token **head)
{
	t_token	*current;

	while (*head)
	{
		current = (*head)->next;
		free((*head)->str);
		free(*head);
		*head = current;
	}
	*head = NULL;
}
