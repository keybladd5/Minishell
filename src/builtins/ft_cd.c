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
		if (!ft_strxcmp(env->key_name, "HOME"))
			return (env->value);
		env = env->next;
	}
	return (0);
}

static void	ft_update_pwd(t_env	*env, char *path)
{
	while (env)
	{
		if (!ft_strxcmp(env->key_name, "PWD"))
		{
			free(env->value);
			env->value = getcwd(NULL, 0);
			if (!env->value)
				env->value = ft_strdup(path);
			break ;
		}
		env = env->next;
	}
}

static void	ft_update_oldpwd(t_env *env, char *path)
{
	while (env)
	{
		if (!ft_strxcmp(env->key_name, "OLDPWD"))
		{
			free(env->value);
			env->value = path;
			return ;
		}
		env = env->next;
	}
	free (path);
}

int	ft_cd(t_token *tokens, t_env *env)
{
	char	*origin;
	char	*dest;

	origin = getcwd(NULL, 0);
	if (!origin)
		ft_error_system(MALLOC_ERROR);
	if (!tokens)
		dest = ft_cd_noarg(env);
	else
		dest = tokens->str;
	if (chdir(dest) != 0)
	{
		ft_putstr_fd("Minishell: cd: ", 2);
		if (!dest)
			ft_putstr_fd("HOME not set\n", 2);
		else
			perror(dest);
		free(origin);
		return (1);
	}
	ft_update_pwd(env, dest);
	ft_update_oldpwd(env, origin);
	return (0);
}
