/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 21:00:43 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 21:00:45 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//CD
//INPUT: Siguiente token al comando (puede ser NULL)
//-Guardar valor inicial de PWD en OLDPWD (env list)
//-Cambiar CWD al path especificado (o HOME si es NULL) con chdir
//-Guardar el path en PWD (env list)
int	ft_cd(t_token *tokens, t_env *env)
{
	char	*path;
	t_env	*tmp_env;

	tmp_env = env;
	if (!tokens)
	{
		while (tmp_env)
		{
			if (!ft_strncmp(tmp_env->key_name, "HOME", 4))
			{
				path = tmp_env->value;
				break ;
			}
			tmp_env = tmp_env->next;
		}
		tmp_env = env;
	}
	else
		path = tokens->str;
	if (chdir(path) != 0)
	{
		ft_putstr_fd("Minishell: cd: ", 2);
		perror(path);
		return (1);
	}
	while (tmp_env)
	{
		if (!ft_strncmp(tmp_env->key_name, "PWD", 3))
		{
			path = tmp_env->value;
			tmp_env->value = getcwd(NULL, 0);//aqui puede devolver null la funcion 
			tmp_env = env;
			break ;
		}
		tmp_env = tmp_env->next;
	}
	while (tmp_env)
	{
		if (!ft_strncmp(tmp_env->key_name, "OLDPWD", 6))
		{
			free(tmp_env->value);
			tmp_env->value = path;
			break ;
		}
		tmp_env = tmp_env->next;
	}
	return (0);
}
