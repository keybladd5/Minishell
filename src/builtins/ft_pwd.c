/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 21:04:00 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 21:04:05 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//PWD
//Con getcwd se hace malloc del string y guarda el path actual
//Imprime el path seguido de \n y libera el string
int	ft_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		exit (MALLOC_ERROR);
	ft_putstr_fd(cwd, 1);
	write(1, "\n", 1);
	free (cwd);
	return (0);
}
