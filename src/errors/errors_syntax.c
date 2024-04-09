/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_syntax.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 10:07:08 by polmarti          #+#    #+#             */
/*   Updated: 2024/04/09 10:07:11 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void	ft_err_red_metachar(t_token *t_current)
{
	ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `", 2);
	if (!ft_strncmp(t_current->str, ">>", 2) \
		|| !ft_strncmp(t_current->str, "<<", 2))
	{
		ft_putchar_fd(t_current->str[0], 2);
		ft_putchar_fd(t_current->str[1], 2);
	}
	else
		ft_putchar_fd(t_current->str[0], 2);
	ft_putstr_fd("'\x1b[0m\n", 2);
}

static void	ft_err_red_errorfile(int *exit_status, t_token *t_current)
{
	char	*tmp_abs_path;

	tmp_abs_path = getcwd(NULL, 0);
	if (!tmp_abs_path)
		ft_error_system(MALLOC_ERROR);
	tmp_abs_path = ft_strjoin_s(tmp_abs_path, "/");
	if (!tmp_abs_path)
		ft_error_system(MALLOC_ERROR);
	tmp_abs_path = ft_strjoin_s(tmp_abs_path, t_current->str);
	if (!tmp_abs_path)
		ft_error_system(MALLOC_ERROR);
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
}

int	ft_error_syntax_aux(int *exit_status, t_token *t_current)
{
	if (t_current && (!ft_strncmp(t_current->str, "|", 1) || \
		!ft_strncmp(t_current->str, "<", 1) \
		|| !ft_strncmp(t_current->str, ">", 1) || \
		!ft_strncmp(t_current->str, ">>", 2) || \
		!ft_strncmp(t_current->str, "<<", 1)))
		ft_err_red_metachar(t_current);
	else if (t_current)
	{
		ft_err_red_errorfile(exit_status, t_current);
		t_current->type = ERROR_FILE;
		return (1);
	}
	else
	{
		ft_putstr_fd("\033[31mminishell: syntax error near", 2);
		ft_putstr_fd(" unexpected token `newline'\x1b[0m\n", 2);
		*exit_status = 2 ;
		return (1);
	}
	return (0);
}

//Muestra mensajes de error por el fd 2 y modifica el exit status
void	ft_error_syntax(int *exit_status, int name, t_token *t_current)
{
	if (name == PIPE)
	{
		ft_putstr_fd \
		("\033[31mminishell: syntax error near unexpected \
		token `|'\x1b[0m\n", 2);
		*exit_status = 2 ;
		return ;
	}
	else if (name == RED_IN || name == RED_OUT ||\
	name == HERE_DOC || name == APPEND)
	{
		if (ft_error_syntax_aux(exit_status, t_current))
			return ;
	}
	*exit_status = 258;
	return ;
}
