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
//aux lexer

int		ft_ismetachar(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (1);
	return (0);
}

///NOT FULLLY TESTED////
void	ft_createtoken(t_token **curr_token, char *input, int *i, t_env **env, int exit_status)
{
	int		j;
	int		flag;
	char	*str;
	
	j = *i;
	flag = 0;
	while (input[j] && !ft_isspace(input[j]))
	{
		if (input[j] == '"')
		{
			j++;
			while (input[j] != '"')
				j++;
			str = ft_substr(input, *i + 1, j - (*i + 1));
			if (!str)
				exit (MALLOC_ERROR);
			expansor(&str, env, exit_status);
			(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
			(*curr_token)->type = WORD;
			j++;
			if (input[j] && ft_ismetachar(input[j]))
				flag = 1;
		}
		else 
		{
			if (ft_ismetachar(input[j]))
			{
				while (input[j] && ft_ismetachar(input[j]))
					j++;
				if (input[j])
					flag = 1;
			}
			else
			{
				while (input[j] && !ft_ismetachar(input[j]) && !ft_isspace(input[j]) && input[j] != '"' /*&& input[j] != '\''*/)
					j++;
				if (input[j] && input[j] != '"' /*&& input[j] != '\''*/)
					flag = 1;
			}
			str = ft_substr(input, *i, j - *i);
			if (!str)
				exit (MALLOC_ERROR);
			expansor(&str, env, exit_status);
			if (str)
			{
				if (!ft_strxcmp(str, "$") && input[j] == '"')
				{
					free (str);
					str = NULL;
				}
				(*curr_token)->str = ft_strjoin_free((*curr_token)->str, str);
			}
		}
		*i = j;
		if (flag)
			break ;
	}
}

//split all words by spaces in a linked list
void	lexer(t_token **tokens, char *input, t_env **env, int exit_status)
{
	t_token	*tmp;
	t_token	*last;
	int i = 0;
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
		ft_createtoken(&tmp, input, &i, env, exit_status);
		if (tmp->str == NULL)
			ft_remove_token(tokens, &tmp);
		if (!*tokens)
			*tokens = tmp;
		else
			last->next = tmp;
		last = tmp;
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

int main(int argc, char *argv[], char **envp)
{
	t_env	*head = NULL;

	if (argc != 1 || !argv[0] || !envp[0])
		return (1);

	ft_catch_env(envp, &head);
	input_loop(&head, envp);
	return (0);
}
