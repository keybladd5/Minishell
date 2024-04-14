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

static char	*prompt_builder_aux(t_env *env)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		while (env)
		{
			if (!ft_strncmp(env->key_name, "PWD", 3))
			{
				cwd = ft_strdup(env->value);
				break ;
			}
			env = env->next;
		}
	}
	return (cwd);
}

static char	*prompt_builder(t_env *env)
{
	char	*cwd;
	char	*aux;

	cwd = prompt_builder_aux(env);
	aux = ft_strdup("\x1b[94m");
	if (!aux)
		exit (MALLOC_ERROR);
	cwd = ft_strjoin_free(aux, cwd);
	if (!cwd)
		exit (MALLOC_ERROR);
	aux = ft_strdup("\x1b[0m ");
	if (!aux)
		exit (MALLOC_ERROR);
	cwd = ft_strjoin_free(cwd, aux);
	if (!cwd)
		exit (MALLOC_ERROR);
	aux = ft_strdup("\001\x1b[92m⌁./MiniShell→\x1b[0m\002 ");
	if (!aux)
		exit(MALLOC_ERROR);
	cwd = ft_strjoin_free(cwd, aux);
	if (!cwd)
		exit(MALLOC_ERROR);
	return (cwd);
}

//infinte loop to get the user_input and parse it
void	input_loop(t_env **env, t_token	*tokens)
{
	char	*input;
	int		exit_status;
	char	*prompt;

	input = NULL;
	exit_status = 0;
	while (42)
	{
		sig_init(1);
		prompt = prompt_builder(*env);
		input = readline("minishell$ ");
		ctrl_c(&exit_status);
		if (!input)
		{
			ft_printf("exit\n");
			exit(exit_status);
		}
		lexer(&tokens, input, env, exit_status);
		parser(&tokens, env, &exit_status);
		add_history(input);
		free_tokens(&tokens);
		free(input);
		free(prompt);
	}
}

int	main(int argc, char *argv[], char **envp)
{
	t_env	*head;
	t_token	*tokens;

	head = NULL;
	tokens = NULL;
	if (argc != 1 || !argv[0])
	{
		ft_putendl_fd("minishell: no input arguments authorized ❌", 2);
		return (1);
	}
	if (!*envp)
	{
		ft_putendl_fd("minishell: enviroment required ❌", 2);
		return (1);
	}
	ft_catch_env(envp, &head);
	input_loop(&head, tokens);
	return (0);
}
