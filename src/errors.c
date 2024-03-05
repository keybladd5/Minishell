/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:13:38 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 15:13:39 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//Muestra mensajes de error por el fd 2 y modifica el exit status
int ft_error_syntax(int *exit_status, int name, t_token *t_current)
{
	char *tmp_abs_path;

	if (name == PIPE)
		ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `|'\x1b[0m\n", 2);
	else if (name == RED_IN || name == RED_OUT)
	{
		if (t_current && (!ft_strncmp(t_current->str, "|", 1) || \
		 !ft_strncmp(t_current->str, "<", 1) || !ft_strncmp(t_current->str, ">", 1) || \
		 !ft_strncmp(t_current->str, ">>", 2) || !ft_strncmp(t_current->str, "<<", 1))) //si es algun meta caracter
		{
			ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `", 2);
			if (!ft_strncmp(t_current->str, ">>", 2) || !ft_strncmp(t_current->str, "<<", 2))
			{
				ft_putchar_fd(t_current->str[0], 2);
				ft_putchar_fd(t_current->str[1], 2);
			}
			else
				ft_putchar_fd(t_current->str[0], 2);
			ft_putstr_fd("'\x1b[0m\n", 2);
		}
		else if (t_current /* && (!t_current->next || t_current->next->type != PIPE)*/) //porque la ultima condicion??, suele entrar por error de apertura o < 
		{
			tmp_abs_path = getcwd(NULL, 0);
			if (!tmp_abs_path)
				exit (MALLOC_ERROR);
			tmp_abs_path = ft_strjoin(tmp_abs_path, "/");
			if (!tmp_abs_path)
				exit(MALLOC_ERROR);
			tmp_abs_path = ft_strjoin_s(tmp_abs_path, t_current->str);
			if (!tmp_abs_path)
				exit(MALLOC_ERROR);
			//ft_putstr_fd(tmp_abs_path, 2); 4 debugg
			if (access(tmp_abs_path, F_OK) != 0)
			{
				ft_putstr_fd("\033[31mminishell: ", 2);
				ft_putstr_fd(t_current->str, 2);
				ft_putstr_fd(": No such file or directory\x1b[0m\n", 2);
			}
			else
			{
				ft_putstr_fd("\033[31mminishell: ", 2);
				ft_putstr_fd(t_current->str, 2);
				ft_putstr_fd(": Permission denied\x1b[0m\n", 2);
			}
			free(tmp_abs_path);
			*exit_status = 1;
			return (*exit_status);
		}
		else
			ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `newline'\x1b[0m\n", 2);
	}
	*exit_status = 258;
	return (*exit_status);
}
