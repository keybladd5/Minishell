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
/*void	print_tokens(t_token **head)
{
	t_token	*current = *head;

	while (current)
	{
		dprintf(0, "stack = %s\n", current->str);
		current = current->next;
	}
}*/
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

//split all words by spaces in a linked list
void	lexer(t_token **tokens, char *input, t_env **env, int exit_status)
{
	t_token	*tmp_token;
	t_token	*last;
	int		i;

	i = 0;
	while (input[i])
	{
		while (ft_isspace(input[i]))
			i++;
		if (!input[i])
			break ;
		tmp_token = ft_createtoken(input, &i, env, exit_status);
		if (!tmp_token->str)
			free(tmp_token);
		else
		{
			if (!*tokens)
				*tokens = tmp_token;
			else
				last->next = tmp_token;
			last = tmp_token;
		}
	}
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
			ft_printf("exit\n");
			exit(EXIT_SUCCESS);
		}
		lexer(&tokens, input, env, exit_status);
		parser(&tokens, env, envp , &exit_status);
		add_history(input);
		free_tokens(&tokens);
		free(input);
		free(prompt);
	}
}

int	main(int argc, char *argv[], char **envp)
{
	t_env	*head;

	head = NULL;
	if (argc != 1 || !argv[0] || !envp[0])
		return (1);
	ft_catch_env(envp, &head);
	input_loop(&head, envp);
	return (0);
}
