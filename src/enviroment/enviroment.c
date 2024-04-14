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

static void	ft_soft_itoa(t_env **tmp, int n)
{
	(*tmp)->value = ft_itoa(n);
	if (!(*tmp)->value)
		ft_error_system(MALLOC_ERROR);
}

static void	ft_aux_catch_env(t_env **tmp, int *shlvl_flag)
{
	int	shlvl;

	shlvl = 0;
	if (ft_strxcmp((*tmp)->key_name, "SHLVL\0") == 0)
	{
		shlvl = ft_atoi((*tmp)->value);
		if (shlvl > 999)
		{
			ft_putstr_fd("minishell: shell level (", 2);
			free((*tmp)->value);
			ft_soft_itoa(tmp, shlvl + 1);
			ft_putstr_fd((*tmp)->value, 2);
			ft_putstr_fd(") too high, resetting to 1", 2);
			shlvl = 1;
		}
		else if (shlvl < 999 && shlvl >= 0)
			shlvl++;
		else
			shlvl = 0;
		free((*tmp)->value);
		ft_soft_itoa(tmp, shlvl);
		*shlvl_flag = 1;
	}
}

static void	ft_aux_shlvl(t_env **tmp, t_env **last, int shlvl_flag)
{
	if (shlvl_flag == 0)
	{
		*tmp = (t_env *)malloc(sizeof(t_env));
		if (!*tmp)
			exit(MALLOC_ERROR);
		(*tmp)->key_name = ft_strdup("SHLVL");
		if (!(*tmp)->key_name)
			exit(MALLOC_ERROR);
		(*tmp)->value = ft_strdup("1");
		if (!(*tmp)->value)
			exit(MALLOC_ERROR);
		(*last)->next = *tmp;
		*last = *tmp;
	}
}

static void	ft_aux_envdup(t_env **tmp, char **envp, int x, char *div)
{
	*tmp = (t_env *)malloc(sizeof(t_env));
	if (!tmp)
		exit(MALLOC_ERROR);
	div = ft_strchr(envp[x], '=');
	(*tmp)->key_name = ft_substr(envp[x], 0, (div - envp[x]));
	if (!(*tmp)->key_name)
		exit (MALLOC_ERROR);
	(*tmp)->value = ft_substr(div + 1, 0, ft_strlen(div));
	if (!(*tmp)->value)
		exit (MALLOC_ERROR);
}

//make a linked list on env, iter shlvl, and ind case of no shlvl,
// creates an add one at the end of the list
void	ft_catch_env(char **envp, t_env **head)
{
	int		x;
	char	*div;
	int		shlvl_flag;
	t_env	*tmp;
	t_env	*last;

	x = 0;
	last = NULL;
	tmp = NULL;
	div = NULL;
	shlvl_flag = 0;
	while (envp[x])
	{
		ft_aux_envdup(&tmp, envp, x, div);
		ft_aux_catch_env(&tmp, &shlvl_flag);
		if (!*head)
			*head = tmp;
		else
			last->next = tmp;
		last = tmp;
		x++;
	}
	ft_aux_shlvl(&tmp, &last, shlvl_flag);
	last->next = NULL;
}
