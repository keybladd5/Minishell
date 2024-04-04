/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/24 16:55:53 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/24 16:55:55 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int		ft_aux_abs(char *str)
{
	int	i;

	i = 0;
	while(str[i])
		i++;
	while (str[i] != '/')
		i--;
	return (i+1);
}

//aux to malloc size on the executor function
int		ft_token_lst_size(t_token *lst)
{
	int	i;

	i = 0;
	while (lst && lst->type == WORD)//PARA QUE SOLO CUENTE HASTA LA PIPE SI HAY
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

int		ft_env_lst_size(t_env *lst)
{
	int	i;

	i = 0;
	while (lst)//PARA QUE SOLO CUENTE HASTA LA PIPE SI HAY
	{
		lst = lst->next;
		i++;
	}
	return (i);
}

//NO FUNCIONA !!!
char **ft_copy_env(t_env **env)
{
	int i;
	char **new_env;
	t_env *ref;

	i = ft_env_lst_size(*env);
	ref = *env;
	new_env = (char **)malloc(sizeof(char *) * (i + 1));
	i = 0;
	while(ref)
	{
		new_env[i] = ft_strjoin(ref->key_name, "=");
		if (!new_env[i])
			ft_error_system(MALLOC_ERROR);
		if (ref->value)//para casos como export AAA= que bash si los toma en el nuevo env
		{
			new_env[i] = ft_strjoin_s(new_env[i], ref->value);
			if (!new_env[i])
				ft_error_system(MALLOC_ERROR);
		}
		ref = ref->next;
		i++;
	}
	new_env[i] = NULL;
	return (new_env);
}
