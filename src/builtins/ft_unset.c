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

static int	ft_isvalidkey(char *str)
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

void	ft_remove_env_node(char *keyname, t_env *tmp_to_delete, t_env **curr_env)
{
	t_env *tmp;

	if (!*curr_env)
		return ;
	if ((*curr_env)->next && \
	!ft_strxcmp((*curr_env)->next->key_name, keyname))
	{
		tmp = *curr_env;
		*curr_env = (*curr_env)->next;
		tmp_to_delete = *curr_env;
		*curr_env = (*curr_env)->next;
		if (*curr_env)
			tmp->next = *curr_env;
		else
			tmp->next = NULL;
	}
	else
	{
		while (*curr_env)
		{
			if ((*curr_env)->next && \
			!ft_strxcmp((*curr_env)->next->key_name, keyname))
			{
				tmp = *curr_env;
				*curr_env = (*curr_env)->next;
				tmp_to_delete = *curr_env;
				*curr_env = (*curr_env)->next;
				if (*curr_env)
					tmp->next = *curr_env;
				else
					tmp->next = NULL;
				break ;
			}
			*curr_env = (*curr_env)->next;
		}
	}
	free (tmp_to_delete->key_name);
	free (tmp_to_delete->value);
	free (tmp_to_delete);
}
int	ft_unset(t_token *tokens, int *exit_status, t_env **env)
{
	t_env	*tmp_to_delete;
	t_env	*curr_env;

	tmp_to_delete = NULL;
	while (tokens)
	{
		curr_env = *env;
		if (!ft_isvalidkey(tokens->str))
		{
			ft_putstr_fd("\033[31mminishell: export: `", 2);
			ft_putstr_fd(tokens->str, 2);
			ft_putstr_fd("': not a valid identifier\x1b[0m\n", 2);
			tokens = tokens->next;
			*exit_status = 1;
			continue ;
		}
		else
		{
			ft_remove_env_node(tokens->str, tmp_to_delete, &curr_env);
			*exit_status = 0;
		}
		tokens = tokens->next;
	}
	return (*exit_status);
}