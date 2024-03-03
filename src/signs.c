/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signs.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Meritxu <Meritxu@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/03 15:06:22 by polmarti          #+#    #+#             */
/*   Updated: 2024/03/03 21:03:43 by Meritxu          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//pone exit_status en 1 si se ha recibido la señal SIGINT
void	ctrl_C(int *exit_status)
{
	if (g_signal == SIGINT)
		*exit_status = EXIT_FAILURE;
	//signal(SIGINT, SIG_IGN); //esta linea hace q no funcione CNTL_C para interrumpir comandos (cat)
}

//al recibir la señal SIGINT se ejecuta esta funcion, salta de linea y vuelve a mostrar el prompt
void	sig_handler(int sig)//cambio anadido pendiente analizar🐸
{
	g_signal = sig;
	if (sig == SIGINT)
	{
		write(1, "\n", 1);
		rl_replace_line("", 0);
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
		write(1, "^C\n", 3);//solo aparece asi en el proceso hijo
		g_signal = 130;
	}
	else if (sig == SIGQUIT)
	{
		ft_putstr_fd("Quit: 3\n", 1);
		g_signal = 131;
	}
}

//inicializa variable global y las señales. Variable tc y sus funciones usan la libreria termios.h para no escribir ^C
void	sig_init(int i)//cambio anadido pendiente analizar🐸
{
	struct termios	tc;

	g_signal = 0;
	if (i == 1)//cambio anadido pendiente analizar🐸
	{
		signal(SIGINT, sig_handler); //Init SIGINT (ctrl+C) para ejecutar sig_handler cuando la reciba
		signal(SIGQUIT, SIG_IGN); //Init SIGQUIT (ctrl+\) para ignorarla
	}
	else//cambio anadido pendiente analizar🐸
	{
		signal(SIGINT, process_sig_handler);
		signal(SIGQUIT, process_sig_handler);
	}
	tcgetattr(0, &tc); //Guarda los atributos del FD 0 (STDIN) en la estructura
	tc.c_lflag &= ~ECHOCTL; //Modifica la flag 'local mode' para desactivar el printeo de ctrl+(X) como ^(X)
	tcsetattr(0, TCSANOW, &tc); //Devuelve los atributos modificados al FD 0 (STDIN)
}