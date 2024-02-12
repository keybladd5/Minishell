/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Meritxu <Meritxu@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:16 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/11 18:54:59 by Meritxu          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../libft/libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/errno.h>
# include <readline/readline.h>
# include <readline/history.h>

# define MALLOC_ERROR 1
# define WORD 2
# define PIPE 3
# define YES 4
# define NO 5

typedef struct s_env
{
	char			*key_name;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_token
{
	char			*str;
	char			*aux_str; //non used at this actual point, wait 4 take it in the PARSER
	int				type; //all init in 0, because not used, wait 4 PARSER
	struct s_token	*next;
}	t_token;

typedef struct s_pipe
{
	int pipefd[2];
	int pipe_counter;
	int flag;
}	t_pipe;

void	exec_cmd(t_token **tokens, t_env **env, char **envp, t_pipe *data_pipe);

int		ft_token_lst_size(t_token *lst);

void	expansor(t_token **tokens, t_env **env);

void parser(t_token **tokens, t_env **env, char **envp);