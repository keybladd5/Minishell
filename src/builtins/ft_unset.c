/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 21:04:53 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 21:04:54 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_remove_env_node(t_token *tokens, t_env **env, t_env **curr_env, \
	t_env **tmp_to_delete)
{
	if (!ft_strxcmp(tokens->str, (*curr_env)->key_name))
	{
		*tmp_to_delete = *curr_env;
		(*env) = (*env)->next;
	}
	else
	{
		while (*curr_env)
		{
			if ((*curr_env)->next && \
		!ft_strxcmp(tokens->str, (*curr_env)->next->key_name))
			{
				*tmp_to_delete = (*curr_env)->next;
				(*curr_env)->next = (*tmp_to_delete)->next;
			}
			*curr_env = (*curr_env)->next;
		}
		}
}

int	ft_unset(t_token *tokens, t_env **env)
{
	t_env	*curr_env;
	t_env	*tmp_to_delete;
	int		exit_status;

	exit_status = 0;
	while (tokens)
	{
		curr_env = *env;
		tmp_to_delete = NULL;
		if (!ft_isvalidkey(tokens->str))
			exit_status = ft_error_keyname(tokens->str, 2);
		else
			ft_remove_env_node(tokens, env, &curr_env, &tmp_to_delete);
		if (tmp_to_delete)
		{
			free (tmp_to_delete->key_name);
			free (tmp_to_delete->value);
			free (tmp_to_delete);
		}
		tokens = tokens->next;
	}
	return (exit_status);
}
