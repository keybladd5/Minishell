/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:59:32 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 20:59:34 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	ft_echo(t_token *tokens)
{
	t_token	*t_current;
	int		flag;

	t_current = tokens;
	flag = 0;
	if (!tokens)
		return (ft_putstr_fd("\n", 1), 0);
	while (t_current && !ft_strncmp("-n", t_current->str, 2) && \
	!ft_strchr(t_current->str, ' '))
	{
		flag = 1;
		t_current = t_current->next;
	}
	while (t_current)
	{
		ft_putstr_fd(t_current->str, 1);
		if (t_current->next)
			ft_putstr_fd(" ", 1);
		t_current = t_current->next;
	}
	if (!flag)
		ft_putstr_fd("\n", 1);
	return (0);
}
