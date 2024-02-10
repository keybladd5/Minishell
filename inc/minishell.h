/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/01 11:14:16 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/01 11:14:22 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/errno.h>
# include <readline/readline.h>
# include <readline/history.h>

#define MALLOC_ERROR 1

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

void	executor(t_token **tokens, t_env **env, char **envp);

int		ft_token_lst_size(t_token *lst);