/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mminishell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:41 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 18:55:58 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

//debug function


void	print_tokens(t_token **head)
{
	t_token	*current = *head;

	while (current)
	{
		dprintf(0, "stack = %s\n", current->str);
		current = current->next;
	}
}
//free the list do it by lexer
void free_tokens(t_token **head)
{
	t_token	*current;

	while (*head)
	{
		current = (*head)->next;
		free((*head)->str);
		free(*head);
		*head = current;
	}
	*head = NULL;
}
//get env on a list
void ft_catch_env(char **envp, t_env **head)
{
	int	x;
	char	*div;
	t_env	*tmp;
	t_env	*last;

	x = 0;
	while (envp[x])
	{
		tmp = (t_env *)malloc(sizeof(t_env));
		if (!tmp)
			exit(MALLOC_ERROR);
		div = ft_strchr(envp[x], '=');
		tmp->key_name = ft_substr(envp[x], 0, (div - envp[x]));
		if (!tmp->key_name)
			exit (MALLOC_ERROR);
		tmp->value = ft_substr(div+1, 0, ft_strlen(div));
		if (!tmp->value)
			exit (MALLOC_ERROR);
		if (!*head)
			*head = tmp;
		else
			last->next = tmp;
		last = tmp;
		x++;
	}
	last->next = NULL;
}
//aux lexer
int		ft_tokenlen(char *input)
{
	int	len;

	len = 0;
	while (!ft_isspace(input[len]) && input[len]) // CAMBIO! ahora acepta cualquier tipo de caracter hasta espacio o  NULL como token
		len++;
	return (len);
}

//split all words by spaces in a linked list
void	lexer(t_token **tokens, char *input)
{
	int	i = 0;
	t_token	*tmp;
	t_token	*last;

	while (input[i])
	{
		while (ft_isspace(input[i]))
			i++;
		if (!input[i]) //para no tomar los espacios como argumentos para crear un nodo
			break ;
		tmp = (t_token *)malloc(sizeof(t_token));
		if (!tmp)
			exit(MALLOC_ERROR);
		tmp->str = NULL;
		tmp->type = 0;
		tmp->next = NULL; 
		tmp->str = ft_substr(input, i, ft_tokenlen(input + i));
		if (!tmp->str)
			exit (MALLOC_ERROR);
		i += ft_tokenlen(input + i);
		if (!*tokens)
				*tokens = tmp;
		else
			last->next = tmp;
		last = tmp;
	}
	
}

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
		write(1, "\n", 1);
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
char	*prompt_builder(void)
{
	char	*cwd;
	char	*aux;

	cwd = getcwd(NULL, 0); //direccion
	if (!cwd)
		exit (MALLOC_ERROR);
	aux = ft_strdup("\x1b[94m"); //aux 1
	if (!aux)
		exit (MALLOC_ERROR);
	cwd = ft_strjoin_free(aux, cwd); //unir aux+dir
	if (!cwd)
		exit (MALLOC_ERROR);
	aux = ft_strdup("\x1b[0m "); //aux normal
	if (!aux)
		exit (MALLOC_ERROR);
	cwd = ft_strjoin_free(cwd, aux); //unir dir+aux normal
	if (!cwd)
		exit (MALLOC_ERROR);
	aux = ft_strdup("\x1b[92m⌁./MiniShell→\x1b[0m ");
	if (!aux)
		exit(MALLOC_ERROR);
	cwd = ft_strjoin_free(cwd, aux);
	if (!cwd)
		exit(MALLOC_ERROR);
	return (cwd);
}

//infinte loop to get the user_input and parse it
void 	input_loop(t_env **env, char **envp)
{
	char	*input = NULL;
	t_token	*tokens = NULL;
	int		exit_status = 0;
	char	*prompt;
	while(42)
	{
		sig_init(1);
		prompt = prompt_builder();
		input = readline(prompt);
		ctrl_C(&exit_status);
		if (!input)//cambio anadido pendiente analizar🐸
		{
			printf("exit\n");
			exit(EXIT_SUCCESS);
		}
		lexer(&tokens, input);
		expansor(&tokens, env, exit_status);
		//print_tokens(&tokens);//debug
		//printf("%d\n%d\n", exit_status, g_signal);
		parser(&tokens, env, envp , &exit_status);
		//dprintf(1, "%i", exit_status);
		add_history(input);
		free_tokens(&tokens);
		free(input);
		free(prompt);

	}
}

int main(int argc, char *argv[], char **envp)
{
	t_env	*head = NULL;

	if (argc != 1 || !argv[0] || !envp[0])
		return (1);

	ft_catch_env(envp, &head);
	input_loop(&head, envp);
	return (0);
}
