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

void	ft_remove_token(t_token **tokens, t_token **t_current)
{
	t_token	*tmp_current;

	tmp_current = NULL;
	if (*tokens == *t_current)
	{
		*tokens = (*t_current)->next;
		tmp_current = *t_current;
		*t_current = (*t_current)->next;
	}
	else
	{
		while (*tokens)
		{
			if ((*tokens)->next == *t_current)
			{
				tmp_current = *t_current;
				(*tokens)->next = (*t_current)->next;
				*t_current = (*t_current)->next;
				break ;
			}
			*tokens = (*tokens)->next;
		}
	}
	free (tmp_current);
}

void	ft_expand_variable(char **tmp, int *i, t_env **env, t_token **curr_token)
{
	int		x;
	t_env	*curr_env;
	char	*str;

	x = *i;
	curr_env = *env;
	str = NULL;
	while(*tmp[x] != '$' && !ft_isspace(*tmp[x]) && *tmp[x])
		x++;
	while (curr_env)
	{
		if (!ft_strncmp(&(tmp[*i]), curr_env->key_name, x - *i + 1))
			;
		else if (!ft_strncmp(&(tmp[*i]), "?", x - *i))
			;
		else
			curr_env = curr_env->next;
	}
	if (str)
		(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
	else
		*i = x;
}

void	ft_expand_token(t_token **curr_token, char **tmp, t_env **env)
{
	int		i;

	i = 0;
	while (*tmp[i])
	{
		if (*tmp[i] == '$')
		{
			i++;
			if (*tmp[i])
				ft_expand_variable(tmp, &i, env, curr_token);
		}
		else
		{
			i++;
			if (tmp[i] == '$')
				{
					(*curr_token)->str = ft_substr(tmp, 0, i);
					if (!(*curr_token)->str)
						exit(MALLOC_ERROR);
				}
		}
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
		ft_expand_token(&curr_token, &tmp, env);
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

void	expansor(t_token **tokens, t_env **env, int exit_status)
{
	t_token *t_current; //token current
	t_env	*e_current; //enviroment current
	char	*tmp;
	char	*str = NULL;
	int		i;
	int		x;

	t_current = *tokens;
	e_current = *env;
	x = 0;

	//hay que comprobar cuando hay caracteres de mas en la variable y los del principio del string
	while (t_current)
	{
		i = 0;
		tmp = t_current->str;
		t_current->str = NULL;
		while (tmp[i])
		{
			if (!ft_strncmp(tmp + i, "$", 1)) //si el primer caracter es un $ recorre el env hasta encontrar el contenido y sustituirlo
			{
				i++;
				if (!tmp[i])
					break;
				x = i;
				while(tmp[x] != '$' && !ft_isspace(tmp[x]) && tmp[x])
					x++;
				while (e_current)
				{
					if (!ft_strncmp(&(tmp[i]), e_current->key_name, x - i + 1))
					{
						str = ft_strdup(e_current->value);
						if (!str)
							exit (MALLOC_ERROR);
						i = x;
						break ;
					}
					else if (!ft_strncmp(&(tmp[i]), "?", x - i))
					{
						str = ft_itoa(exit_status);
						if (!str)
							exit (MALLOC_ERROR);
						i = x;
						break ;
					}
					else
						 e_current = e_current->next;
				}
				if (str && e_current)
					t_current->str = ft_strjoin_free(t_current->str, str);
				else
					i = x;
				e_current = *env;
			}
			else
			{
				i++;
				if (tmp[i] == '$')
				{
					t_current->str = ft_substr(tmp, 0, i);
					if (!t_current->str)
						exit(MALLOC_ERROR);
				}
			}
		}
		if (!t_current->str && tmp[0] == '$' && tmp[1])
		{
			ft_remove_token(tokens, &t_current);
			free (tmp);
		}
		else
		{
			if (!t_current->str)
				t_current->str = tmp;
			else
				free (tmp);
			t_current = t_current->next;
		}
	}
}
