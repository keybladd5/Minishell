/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isvalidkey.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 19:54:05 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/11 19:54:07 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

int	ft_isvalidkey(char *str)
{
	if (!(ft_isalpha(str[0]) || str[0] == '_'))
		return (0);
	while (*(++str) && *str != '=')
	{
		if (!(ft_isalnum(*str) || *str == '_'))
			return (0);
	}
	return (1);
}
