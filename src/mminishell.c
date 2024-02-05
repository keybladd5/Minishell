/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mminishell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:41 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/01 11:14:43 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"

#include "../inc/minishell.h"


void ft_catch_env(char **envp, t_env **head)
{
	int	x = 0;
	char	*div;
	t_env	*tmp;
	t_env	*last;
	while (envp[x])
	{
		tmp = (t_env *)malloc(sizeof(t_env) * 1);
		if (!tmp)
			exit(1);
		div = ft_strchr(envp[x], '=');
		tmp->key_name = ft_substr(envp[x], 0, (div - envp[x]));
		if (!tmp->key_name)
			exit (1);
		tmp->value = ft_substr(div+1, 0, ft_strlen(div));
		if (!tmp->value)
			exit (1);
		
		if (x == 0)
			*head = tmp;
		else
			last->next = tmp;
		last = tmp;
		x++;
	}
	last->next = NULL;
}
/*void	try_exec(char *buff, char *envp[])
{
	int i = 0;
	int x = 0;
	char **path = NULL;
	char *cmd = NULL;
	char *filename = NULL;
	char *cmd_exec = NULL;
	int pid = fork();
	dprintf(0, "%d\n", pid);
	if (pid == 0)
	{
		int loop = 1;
		while (loop)
			;
		while (ft_strncmp("PATH=", envp[i], 5) != 0)
		{
			i++;
			if (envp[i] == NULL)
				exit(1);
		}
		path = ft_split(envp[i], ':');
		path[x] = ft_substr(path[x], 5, ft_strlen(path[x]));
		cmd_exec = ft_substr(buff, 0, (ft_strlen(buff)));
		cmd = ft_strjoin("/", cmd_exec);
		while (path[x])
		{
			if (x > 0)
				free(filename);
			filename = ft_strjoin(path[x], cmd);
			if (access(filename, X_OK) == -1)
				x++;
			else
				break ;
			if (path[x] == NULL)
				break;
		}
		execve(filename, &cmd_exec, NULL);
	}
	wait (NULL);
}*/
// This is a test to check how to work with Readline in the input user
/*void input_loop(char *envp[])
{	

	while(42) 
	{
		char *buff = readline("./minishell ");
		
		//try_exec(buff, envp);
		free(buff);
	}
}*/

int main(int argc, char *argv[], char *envp[])
{
	if (argc != 1 || !argv[0] || !envp[0])
		return (1);
	t_env	*head;
	ft_catch_env(envp, &head);
	//input_loop(envp);
	return (0);
}