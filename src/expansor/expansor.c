/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 12:20:52 by ade-tole          #+#    #+#             */
/*   Updated: 2024/03/03 12:20:55 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void	ft_expand_exitstatus(char **str, int exit_status, int *i)
{
	char	*status;

	status = ft_itoa(exit_status);
	if (!status)
		exit (MALLOC_ERROR);
	*str = ft_strjoin_free(*str, status);
	(*i)++;
}

static void	ft_checkvar(char *varname, char **varvalue, t_env *env)
{
	if (!ft_strxcmp(varname, env->key_name))
	{
		if (env->value)
		{
			*varvalue = ft_strdup(env->value);
			if (!*varvalue)
				exit (MALLOC_ERROR);
		}
		else
		{
			*varvalue = ft_strdup("");
			if (!*varvalue)
				exit (MALLOC_ERROR);
		}
	}
}

static void	ft_expand_variable(char *tmp, int *i, t_env *env, \
char **str)
{
	int		j;
	char	*varvalue;
	char	*varname;

	j = *i;
	varvalue = NULL;
	while ((ft_isalnum(tmp[j]) || tmp[j] == '_') && \
		!ft_isspace(tmp[j]) && tmp[j])
		j++;
	varname = ft_substr(tmp, *i, j - *i);
	if (!varname)
		exit (MALLOC_ERROR);
	while (env)
	{
		ft_checkvar(varname, &varvalue, env);
		if (varvalue)
			break ;
		else
			env = env->next;
	}
	free (varname);
	if (varvalue)
		*str = ft_strjoin_free(*str, varvalue);
	*i = j;
}

static void	ft_noexpansion(char *tmp, char **str, int *i)
{
	int		j;
	char	*dst;

	j = *i;
	while (tmp[j] && tmp[j] != '$')
		j++;
	dst = ft_substr(tmp, *i, j - *i);
	if (!dst)
		exit (MALLOC_ERROR);
	*str = ft_strjoin_free(*str, dst);
	*i = j;
}

void	expansor(char **str, t_env **env, int exit_status)
{
	char	*tmp;
	int		i;

	tmp = *str;
	*str = NULL;
	i = 0;
	while (tmp[i])
	{
		if (tmp[i] == '$')
		{
			if (tmp[++i] == '?')
				ft_expand_exitstatus(str, exit_status, &i);
			else if (ft_isalnum(tmp[i]))
				ft_expand_variable(tmp, &i, *env, str);
			else
				*str = ft_strjoin_s(*str, "$");
		}
		else
			ft_noexpansion(tmp, str, &i);
	}
	free (tmp);
}
