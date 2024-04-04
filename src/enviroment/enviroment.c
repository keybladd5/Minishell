/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   enviroment.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/04 13:16:36 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/04 13:16:39 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	ft_aux_catch_env(t_env *tmp, int *shlvl_flag)
{
	int	shlvl;

	shlvl = 0;
	if (ft_strxcmp(tmp->key_name, "SHLVL\0") == 0)
	{
		shlvl = ft_atoi(tmp->value);
		if (shlvl > 999)
		{
			ft_putstr_fd("minishell: shell level (", 2);
			free(tmp->value);
			tmp->value = ft_itoa(shlvl + 1);
			if (!tmp->value)
				ft_error_system(MALLOC_ERROR);
			ft_putstr_fd(tmp->value, 2);
			ft_putstr_fd(") too high, resetting to 1", 2);
			shlvl = 1;
		}
		else if (shlvl < 999 && shlvl >= 0)
			shlvl++;
		else
			shlvl = 0;
		free(tmp->value);
		tmp->value = ft_itoa(shlvl);
		if (!tmp->value)
			ft_error_system(MALLOC_ERROR);
		*shlvl_flag = 1;
	}
	return ;
}
//make a linked list on env, iter shlvl, and ind case of no shlvl,
// creates an add one at the end of the list
void ft_catch_env(char **envp, t_env **head)
{
	int	x;
	char	*div;
	int		shlvl_flag;
	t_env	*tmp;
	t_env	*last;

	x = 0;
	last = NULL;
	shlvl_flag = 0;
	while (envp[x])
	{
		tmp = (t_env *)malloc(sizeof(t_env));
		if (!tmp)
			exit(MALLOC_ERROR);
		div = ft_strchr(envp[x], '=');
		tmp->key_name = ft_substr(envp[x], 0, (div - envp[x]));
		if (!tmp->key_name)
			exit (MALLOC_ERROR);
		tmp->value = ft_substr(div+1, 0, ft_strlen(div));
		if (!tmp->value)
			exit (MALLOC_ERROR);
		ft_aux_catch_env(tmp, &shlvl_flag);
		if (!*head)
			*head = tmp;
		else
			last->next = tmp;
		last = tmp;
		x++;
	}
	if (shlvl_flag == 0)
	{
		tmp = (t_env *)malloc(sizeof(t_env));
		if (!tmp)
			exit(MALLOC_ERROR);
		tmp->key_name = ft_strdup("SHLVL");
		if (!tmp->key_name)
			exit(MALLOC_ERROR);
		tmp->value = ft_strdup("1");
		if (!tmp->value)
			exit(MALLOC_ERROR);
		last->next = tmp;
		last = tmp;
	}
	last->next = NULL;
}
