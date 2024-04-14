/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signs.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:06:22 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 21:03:43 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//pone exit_status en 1 si se ha recibido la señal SIGINT
void	ctrl_c(int *exit_status)
{
	if (g_signal == SIGINT)
		*exit_status = EXIT_FAILURE;
	signal(SIGINT, SIG_IGN);
}

//al recibir la señal SIGINT se ejecuta esta funcion, 
//salta de linea y vuelve a mostrar el prompt
void	sig_handler(int sig)//cambio anadido pendiente analizar🐸
{
	g_signal = sig;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_replace_line("", 1);
		rl_on_new_line();
		rl_redisplay();
	}
}

//cambio anadido pendiente analizar🐸
void	process_sig_handler(int sig)
{
	g_signal = sig;
	if (sig == SIGINT)
	{
		write(2, "\n", 1);
		g_signal = 130;
	}
	else if (sig == SIGQUIT)
	{
		ft_putstr_fd("Quit: 3\n", 1);
		g_signal = 131;
	}
}

//inicializa variable global y las señales. 
//Variable tc y sus funciones usan la libreria termios.h para no escribir ^C
void	sig_init(int i)
{
	struct termios	tc;

	if (i == 1)
	{
		g_signal = 0;
		signal(SIGINT, sig_handler);
		signal(SIGQUIT, SIG_IGN);
	}
	else
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
	}
	tcgetattr(0, &tc);
	tc.c_lflag &= ~ECHOCTL;
	if (i == 0)
		tc.c_lflag |= ECHOCTL;
	tcsetattr(0, TCSANOW, &tc);
}
