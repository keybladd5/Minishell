/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/02 20:57:44 by ade-tole          #+#    #+#             */
/*   Updated: 2024/04/02 20:57:46 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	ft_atoi(const char *str)
{
	int	output;
	int	sign;

	output = 0;
	sign = 1;
	while (*str && (ft_strchr("\n\t\v\f\r ", *str)) != NULL)
		str++;
	if (*str == '+')
		str++;
	else if (*str == '-')
	{
		str++;
		sign = -1;
	}
	while (*str && (ft_strchr("0123456789", *str)) != NULL)
	{
		output = (output * 10) + (*str - '0');
		str++;
	}
	return (output * sign);
}


int	ft_print_exit_argerr(char *arg)
{
	ft_putstr_fd("Minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putendl_fd(": numeric argument required", 2);
	return (255);
}

void	ft_exit_argchecker(char *arg, int *i, char *sign, int *len)
{
	while (arg[*i] == ' ')
	{
		(*i)++;
		if (!arg[*i])
			exit(ft_print_exit_argerr(arg));
	}
	if (arg[*i] == '-' || arg[*i] == '+')
	{
		*sign = arg[*i];
		(*i)++;
		if (!arg[*i] || !ft_isdigit(arg[*i]))
			exit(ft_print_exit_argerr(arg));
	}
	while (arg[*i] == '0')
		(*i)++;
	while (arg[*i])
	{
		if (!ft_isdigit(arg[*i]) && arg[*i] != ' ')
			exit(ft_print_exit_argerr(arg));
		(*len)++;
		(*i)++;
	}
}

int	ft_exit(t_token *tokens, int *exit_status)
{
	int		i;
	char	sign;
	int		len;

	i = 0;
	sign = '+';
	len = 0;
	if (tokens)
	{
		ft_exit_argchecker(tokens->str, &i, &sign, &len);
		if (len > 19)
			exit(ft_print_exit_argerr(tokens->str));
		if (len == 19 && ((sign == '+' && (ft_strncmp(tokens->str + (i - len), \
		"9223372036854775807", 19) > 0)) || (sign == '-' && \
		(ft_strncmp(tokens->str + (i - len), "9223372036854775808", 19) > 0))))
			exit(ft_print_exit_argerr(tokens->str));
		if (!exit_status)
			exit (0);
		*exit_status = ft_atoi(tokens->str);
		if (tokens->next)
			return (ft_putendl_fd("Minishell: exit: too many arguments", 2), 1);
	}
	//ft_putendl_fd("exit", 1);
	exit (*exit_status);
}
