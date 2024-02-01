/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mminishell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:41 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/01 11:14:43 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../inc/minishell.h"

int main(int argc, char *argv[], char *envp[])
{
	if (argc != 1 || !argv[0] || !envp[0])
		return (1);
	
	return (0);
}