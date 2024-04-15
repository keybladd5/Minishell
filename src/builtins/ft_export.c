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

static int	ft_export_noargs(t_env	*env)
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

static void	ft_export_new(char *variable, char *div, t_env *env)
{
	t_env	*tmp;
	t_env	*last;

	last = env;
	while (last->next)
		last = last->next;
	if (!env->key_name)
		tmp = env;
	else
	{
		tmp = (t_env *)malloc(sizeof(t_env));
		if (!tmp)
			exit(MALLOC_ERROR);
		ft_memset(tmp, 0, sizeof(t_env));
		last->next = tmp;
	}
	tmp->key_name = ft_substr(variable, 0, (div - variable));
	if (!tmp->key_name)
		exit (MALLOC_ERROR);
	if (div)
	{
		tmp->value = ft_substr(div + 1, 0, ft_strlen(div));
		if (!tmp->value)
			exit (MALLOC_ERROR);
	}
}

static void	ft_update_variable(t_env **curr_env, char *mid)
{
	free((*curr_env)->value);
	(*curr_env)->value = ft_substr(mid + 1, 0, ft_strlen(mid));
	if (!(*curr_env)->value)
		exit (MALLOC_ERROR);
}

static void	ft_export_variable(char *variable, t_env **curr_env, t_env *env)
{
	int		var_flag;
	char	*mid;
	char	*keyname;

	var_flag = 0;
	mid = ft_strchr(variable, '=');
	keyname = ft_substr(variable, 0, (mid - variable));
	if (!keyname)
		exit (MALLOC_ERROR);
	while (*curr_env && (*curr_env)->key_name)
	{
		if (!ft_strxcmp(keyname, (*curr_env)->key_name))
		{
			var_flag = 1;
			break ;
		}
		(*curr_env) = (*curr_env)->next;
	}
	if (var_flag && mid)
		ft_update_variable(curr_env, mid);
	else if (!var_flag)
		ft_export_new(variable, mid, env);
	free(keyname);
}

//esta fallando en casos de NULL
int	ft_export(t_token *tokens, t_env *env)
{
	t_env	*curr_env;
	int		exit_status;

	exit_status = 0;
	if (!tokens)
		return (ft_export_noargs(env));
	if (!env)
	{
		env = (t_env *)malloc(sizeof(t_env));
		if (!env)
			exit(MALLOC_ERROR);
		ft_memset(env, 0, sizeof(t_env));
	}
	while (tokens)
	{
		curr_env = env;
		if (!ft_isvalidkey(tokens->str))
			exit_status = ft_error_keyname(tokens->str, 1);
		else
			ft_export_variable(tokens->str, &curr_env, env);
		tokens = tokens->next;
	}
	return (exit_status);
}
