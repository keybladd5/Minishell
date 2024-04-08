/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/08 13:18:36 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/08 13:18:39 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_remove_token(t_token **tokens, t_token **curr_token)
{
	t_token	*tmp_current;

	tmp_current = NULL;
	if (*tokens == *curr_token)
	{
		*tokens = (*curr_token)->next;
		tmp_current = *curr_token;
		*curr_token = (*curr_token)->next;
	}
	else
	{
		while (*tokens)
		{
			if ((*tokens)->next == *curr_token)
			{
				tmp_current = *curr_token;
				(*tokens)->next = (*curr_token)->next;
				*curr_token = (*curr_token)->next;
				break ;
			}
			*tokens = (*tokens)->next;
		}
	}
	free (tmp_current->str);
	free (tmp_current);
}
