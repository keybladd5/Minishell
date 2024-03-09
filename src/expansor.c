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

#include "../inc/minishell.h"

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
	free (tmp_current);
}

void	ft_expand_exitstatus(t_token **curr_token, int exit_status, int *i)
{
	char	*str;

	str = ft_itoa(exit_status);
	if (!str)
		exit (MALLOC_ERROR);
	(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
	(*i)++;
}

void	ft_checkvar(char *varname, char **str, t_env *env)
{
	if (!ft_strxcmp(varname, env->key_name))
	{
		*str = ft_strdup(env->value);
		if (!*str)
			exit (MALLOC_ERROR);
	}
}

void	ft_expand_variable(char **tmp, int *i, t_env *env, \
t_token **curr_token)
{
	int		x;
	char	*str;
	char	*varname;

	x = *i;
	str = NULL;
	while (ft_isalnum((*tmp)[x]) && !ft_isspace((*tmp)[x]) && (*tmp)[x])
		x++;
	varname = ft_substr(*tmp, *i, x - *i);
	if (!varname)
		exit (MALLOC_ERROR);
	while (env)
	{
		ft_checkvar(varname, &str, env);
		if (str)
			break ;
		else
			env = env->next;
	}
	free (varname);
	if (str)
		(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
	*i = x;
}

void	ft_noexpansion(char **tmp, t_token **curr_token, int *i)
{
	int		x;
	char	*str;

	x = *i;
	while ((*tmp)[x] && (*tmp)[x] != '$')
		x++;
	str = ft_substr(*tmp, *i, x - *i);
	if (!str)
		exit (MALLOC_ERROR);
	(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
	*i = x;
}

void	ft_expand_token(t_token **curr_token, char **tmp, t_env **env, \
int exit_status)
{
	int		i;

	i = 0;
	while ((*tmp)[i])
	{
		if ((*tmp)[i] == '$')
		{
			if ((*tmp)[++i] == '?')
				ft_expand_exitstatus(curr_token, exit_status, &i);
			else if (ft_isalnum((*tmp)[i]))
				ft_expand_variable(tmp, &i, *env, curr_token);
			else
				(*curr_token)->str = ft_strjoin_s((*curr_token)->str, "$");
		}
		else
			ft_noexpansion(tmp, curr_token, &i);
	}
}

void	expansor(t_token **tokens, t_env **env, int exit_status)
{
	t_token	*curr_token;
	char	*tmp;

	curr_token = *tokens;
	while (curr_token)
	{
		tmp = curr_token->str;
		curr_token->str = NULL;
		ft_expand_token(&curr_token, &tmp, env, exit_status);
		if (!curr_token->str && tmp[0] == '$' && tmp[1])
		{
			ft_remove_token(tokens, &curr_token);
			free (tmp);
		}
		else
		{
			if (!curr_token->str)
				curr_token->str = tmp;
			else
				free (tmp);
			curr_token = curr_token->next;
		}
	}
}
