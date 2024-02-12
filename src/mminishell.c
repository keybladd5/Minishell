/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mminishell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Meritxu <Meritxu@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:41 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 18:55:58 by Meritxu          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../inc/minishell.h"

#define MALLOC_ERROR 1

#include <stdio.h>
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
	int	x = 0;
	char	*div;
	t_env	*tmp;
	t_env	*last;
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
		tmp->aux_str = NULL;
		tmp->str = NULL;
		tmp->type = 0;
		tmp->next = NULL; 
		//leer caracteres del input hasta espacio/tab
		//guardar en tmp->str

		tmp->str = ft_substr(input, i, ft_tokenlen(input + i));
		if (!tmp->str)
			exit (MALLOC_ERROR);
		i += ft_tokenlen(input + i);


		/*  ------------⬇️ESTA PARTE SE POSPONE AL PARSER⬇️-----------------------

		//checkear 1er caracter siguiente token. '-' -> flag
		if (input[i+1] == '-' &&  ft_isalpha(input[i+2])) //si es el caso anterior
		{
			//debe buscar  "-a" o "-ab" o "-ab -a -abc" o "-asdasdasdasdadasdasda"
			//vale la pena plantearlo asi o simplemente hacer un split hasta el punto donde despues de un espacio no haya '-'?
			//tambien podria ser un argumento de comando lo que haya despues de los flags.
			//tambien esta la opcion e hacer un acces y checkear todo lo que venga despues como argumento
			//hasta el siguiente acces valido
			int j = i;
			int i2 = 0;
			j++;
			i++;

			while (input[i] == '-' && ft_isalpha(input[i+j]))
			{
				tmp->aux_str[i2] = input[i+j];
				i++;
				j++;
			}
			
		} 
		
		----------- ⬆️ESTA PARTE SE POSPONE AL PARSER⬆️-----------------*/


		//leer caracteres del input hasta espacio/tab
		//guardar en tmp->aux_str
		//set type
		
	if (!*tokens)
			*tokens = tmp;
	else
		last->next = tmp;
	last = tmp;
	}
	
}

void input_loop(t_env **env, char **envp)
{
	char	*input = NULL;
	t_token	 *tokens = NULL;

	while(42)
	{
		//signals
		input = readline("\x1b[92m⌁./MiniShell→\x1b[0m ");
		
		/*if (!ft_strncmp("exit", input, 4))
		{
			ft_printf("exit\n");
			exit(0);
		}*/
		lexer(&tokens, input);//separa input en tokens
		expansor(&tokens, env);
		print_tokens(&tokens);//debug
		executor(&tokens, env, envp);
		add_history(input);
		free_tokens(&tokens);
		free(input);
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