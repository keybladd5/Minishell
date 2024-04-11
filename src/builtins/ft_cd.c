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
static char	*ft_cd_noarg(t_env *env)
{
	while (env)
	{
		if (!ft_strncmp(env->key_name, "HOME", 4))
			return (env->value);
		env = env->next;
	}
	return (0);
}

static void	ft_update_pwd(t_env	*env, char **path)
{
	while (env)
	{
		if (!ft_strncmp(env->key_name, "PWD", 3))
		{
			*path = env->value;
			env->value = getcwd(NULL, 0);
			if (!env->value)
				env->value = *path;
			break ;
		}
		env = env->next;
	}
}

static void	ft_update_oldpwd(t_env *env, char *path)
{
	while (env)
	{
		if (!ft_strncmp(env->key_name, "OLDPWD", 6))
		{
			free(env->value);
			env->value = path;
			break ;
		}
		env = env->next;
	}
}

int	ft_cd(t_token *tokens, t_env *env)
{
	char	*path;

	if (!tokens)
		path = ft_cd_noarg(env);
	else
		path = tokens->str;
	if (chdir(path) != 0)
	{
		ft_putstr_fd("Minishell: cd: ", 2);
		perror(path);
		return (1);
	}
	ft_update_pwd(env, &path);
	ft_update_oldpwd(env, path);
	return (0);
}
