/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 21:04:40 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 21:04:42 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//EXPORT
//no args -> printea env list en formato {declare -x env->key_name="env->value"}
//!!!{export A} -> segfault
//!!!{export A=}
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

//esta fallando en casos de NULL
int	ft_export(t_token *tokens, t_env *env)
{
	t_env	*tmp;
	t_env	*last;
	t_env	*curr_env;
	char	*div;
	char	*keyname;
	int		var_flag;
	int		exit_status;

	exit_status = 0;
	curr_env = env;
	if (!tokens)
	{
		while (env)
		{
			ft_putstr_fd("declare -x ", 1);
			ft_putstr_fd(env->key_name, 1);
			if (env->value)
			{
				write(1, "=\"", 2);
				ft_putstr_fd(env->value, 1);
				ft_putstr_fd("\"", 1);
			}
			write(1, "\n", 1);
			env = env->next;
		}
		return (0);
	}
	while (curr_env->next)
	{
		last = curr_env->next;
		curr_env = last;
	}
	while (tokens)
	{
		curr_env = env;
		if (!ft_isvalidkey(tokens->str))
		{
			ft_putstr_fd("\033[31mminishell: export: `", 2);
			ft_putstr_fd(tokens->str, 2);
			ft_putstr_fd("': not a valid identifier\x1b[0m\n", 2);
			tokens = tokens->next;
			exit_status = 1;
			continue ;
		}
		var_flag = 0;
		div = ft_strchr(tokens->str, '=');
		keyname = ft_substr(tokens->str, 0, (div - tokens->str));
		if (!keyname)
			exit (MALLOC_ERROR);
		while (curr_env)
		{
			if (!ft_strxcmp(keyname, curr_env->key_name))
			{
				var_flag = 1;
				break ;
			}
			curr_env = curr_env->next;
		}
		if (var_flag)
		{
			if (div)
			{
				free(curr_env->value);
				curr_env->value = ft_substr(div + 1, 0, ft_strlen(div));
				if (!curr_env->value)
					exit (MALLOC_ERROR);
			}
		}
		else
		{
			tmp = (t_env *)malloc(sizeof(t_env));
			if (!tmp)
				exit(MALLOC_ERROR);
			tmp->key_name = NULL;
			tmp->value = NULL;
			tmp->next = NULL;
			tmp->key_name = ft_substr(tokens->str, 0, (div - tokens->str));
			if (!tmp->key_name)
				exit (MALLOC_ERROR);
			if (div)
			{
				tmp->value = ft_substr(div + 1, 0, ft_strlen(div));
				if (!tmp->value)
					exit (MALLOC_ERROR);
			}
			last->next = tmp;
			last = tmp;
		}
		free(keyname);
		tokens = tokens->next;
	}
	return (exit_status);
}