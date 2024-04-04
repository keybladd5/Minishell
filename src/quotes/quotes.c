/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 22:06:26 by ade-tole          #+#    #+#             */
/*   Updated: 2024/03/19 22:06:29 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//retorna un 1 si es metachar, sino 0
int	ft_ismetachar(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

//////////////////////////////////////////
///ALL NEW FUNCTIONS NOT FULLLY TESTED////
//////////////////////////////////////////

//Pendiente cambiar exit(1) por mensaje de syntax error cuando no se cierran las quotes
void	handle_quotes(t_token **tmp_token, t_lexer **aux, t_env **env, \
	int exit_status)
{
	char	c;

	c = (*aux)->input[(*aux)->end];
	while ((*aux)->input[++(*aux)->end] != c)
		if (!((*aux)->input[(*aux)->end]))
			exit (1);
	(*aux)->str = ft_substr((*aux)->input, (*aux)->start + 1, \
		(*aux)->end - (*aux)->start - 1);
	if (!(*aux)->str)
		exit (MALLOC_ERROR);
	if (c == '"')
		expansor(&((*aux)->str), env, exit_status);
	if ((*aux)->str)
	{
		(*tmp_token)->str = ft_strjoin_free((*tmp_token)->str, (*aux)->str);
		(*tmp_token)->type = WORD;
	}
	if ((*aux)->input[++(*aux)->end] && \
		ft_ismetachar((*aux)->input[(*aux)->end]))
		(*aux)->flag = 1;
}

void	handle_unquoted(t_lexer **aux)
{
	if (ft_ismetachar((*aux)->input[(*aux)->end]))
	{
		while ((*aux)->input[(*aux)->end] && \
			ft_ismetachar((*aux)->input[(*aux)->end]))
			(*aux)->end++;
		if ((*aux)->input[(*aux)->end])
			(*aux)->flag = 1;
	}
	else
	{
		while ((*aux)->input[(*aux)->end] && !ft_ismetachar((*aux)->input \
	[(*aux)->end]) && !ft_isspace((*aux)->input[(*aux)->end]) && \
	(*aux)->input[(*aux)->end] != '"' && (*aux)->input[(*aux)->end] != '\'')
			(*aux)->end++;
		if ((*aux)->input[(*aux)->end] && (*aux)->input[(*aux)->end] != '"' && \
	(*aux)->input[(*aux)->end] != '\'')
			(*aux)->flag = 1;
	}
	(*aux)->str = ft_substr((*aux)->input, (*aux)->start, \
		(*aux)->end - (*aux)->start);
	if (!(*aux)->str)
		exit (MALLOC_ERROR);
}

void	ft_tokenstr(t_token **tmp_token, t_lexer **aux, t_env **env, \
	int exit_status)
{
	while ((*aux)->input[(*aux)->end] && \
		!ft_isspace((*aux)->input[(*aux)->end]))
	{
		if ((*aux)->input[(*aux)->end] == '"' || \
		(*aux)->input[(*aux)->end] == '\'')
			handle_quotes(tmp_token, aux, env, exit_status);
		else
		{
			handle_unquoted(aux);
			expansor(&(*aux)->str, env, exit_status);
			if ((*aux)->str)
			{
				if (!ft_strxcmp((*aux)->str, "$") && ((*aux)->input \
				[(*aux)->end] == '"' || (*aux)->input[(*aux)->end] == '\''))
					free ((*aux)->str);
				else
					(*tmp_token)->str = ft_strjoin_free((*tmp_token)->str, \
						(*aux)->str);
			}
		}
		(*aux)->start = (*aux)->end;
		if ((*aux)->flag)
			break ;
	}
}

t_token	*ft_createtoken(char *input, int *i, t_env **env, int exit_status)
{
	t_token	*tmp_token;
	t_lexer	*aux;

	aux = (t_lexer *)malloc(sizeof(t_lexer));
	if (!aux)
		exit(MALLOC_ERROR);
	aux->input = input;
	aux->str = NULL;
	aux->start = *i;
	aux->end = aux->start;
	aux->flag = 0;
	tmp_token = (t_token *)malloc(sizeof(t_token));
	if (!tmp_token)
		exit(MALLOC_ERROR);
	tmp_token->str = NULL;
	tmp_token->type = 0;
	tmp_token->next = NULL;
	ft_tokenstr(&tmp_token, &aux, env, exit_status);
	*i = aux->end;
	free(aux);
	return (tmp_token);
}
