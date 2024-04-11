/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:13:38 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 15:13:39 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_error_cmd(char *cmd, int type)
{
	ft_putstr_fd("\033[31mminishell: ", 2);
	ft_putstr_fd(cmd, 2);
	if (type == 1)
		ft_putstr_fd(": command not found\x1b[0m\n", 2);
	else if (type == 2)
		ft_putstr_fd(": No such file or directory\x1b[0m\n", 2);
}

void	ft_error_system(int type)
{
	if (type == MALLOC_ERROR)
		perror("system error -> malloc() error\n");
	else if (type == DUP2_ERROR)
		perror("system error -> dup2() error\n");
	else if (type == DUP_ERROR)
		perror("system error -> dup() error\n");
	else if (type == FORK_ERROR)
		perror("system error -> fork() error\n");
	else if (type == CLOSE_ERROR)
		perror("system error -> close() error\n");
	exit(type);
}

int	ft_error_keyname(char *keyname, int type)
{
	if (type == 1)
		ft_putstr_fd("\033[31Minishell: export: `", 2);
	else if (type == 2)
		ft_putstr_fd("\033[31Minishell: unset: `", 2);
	ft_putstr_fd(keyname, 2);
	ft_putstr_fd("': not a valid identifier\x1b[0m\n", 2);
	return (1);
}
