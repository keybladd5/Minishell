/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin_f.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/15 15:03:54 by ade-tole          #+#    #+#             */
/*   Updated: 2024/02/15 15:03:56 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

char	*ft_strjoin_f(char *s1, char *s2)
{
	char	*rtrn;
	size_t	i;
	size_t	j;

	if (!s1)
		rtrn = ft_strdup(s2);
	else
	{
		i = ft_strlen(s1);
		rtrn = (char *)malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) +1));
		if (!rtrn)
			return (0);
		rtrn = ft_memcpy(rtrn, s1, (int)i);
		free(s1);
		j = 0;
		while (s2[j])
		{
			rtrn[i] = s2[j];
			i++;
			j++;
		}
		rtrn[i] = '\0';
	}
	free(s2);
	return (rtrn);
}
