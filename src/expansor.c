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
	free (tmp_current->str);
	free (tmp_current);
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
					if (!ft_strncmp(&(tmp[i]), e_current->key_name, x - i))
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
				{
					t_current->str = ft_strjoin_free(t_current->str, str);
					if (!t_current->str)
						exit(MALLOC_ERROR);
				}
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
