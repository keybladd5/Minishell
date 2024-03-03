/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ade-tole <ade-tole@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/18 19:33:49 by ade-tole          #+#    #+#             */
/*   Updated: 2024/02/18 19:34:03 by ade-tole         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/errno.h>
# include "readline/readline.h"
# include "readline/history.h"
# include <termios.h>

# define MALLOC_ERROR 1
# define WORD 2
# define PIPE 3
# define YES 4
# define NO 5
# define RED_IN 6
# define RED_OUT 7
# define DOC 8

typedef struct s_env
{
	char			*key_name;
	char			*value;
	struct s_env	*next;
}	t_env;

typedef struct s_token
{
	char			*str;
	int				type; 
	struct s_token	*next;
}	t_token;

typedef struct s_pipe
{
	int pipefd[2];
	int pipe_counter;
	int flag;
	int og_stdin;
	int og_stdout;
}	t_pipe;

//STRUC PARA DATOS DE REDIRECCIONES
typedef struct s_redir
{
	int fd_infile;
	int red_in_counter;
	int fd_outfile;
	int red_out_counter;
}	t_redir;

//--------errors.c-----------

int ft_error_syntax(int *exit_status, int name, t_token *t_current);

//--------signs.c-----------

void	ctrl_C(int *exit_status);

void	sig_handler(int sig);

void	process_sig_handler(int sig);

void	sig_init(int i);

int		g_signal;

//--------redirs.c-------------

int		ft_red_in_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe);

int		ft_red_out_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe);

void	ft_aux_close(t_pipe *data_pipe, t_redir *data_redir);

//--------executor.c-------------

int		ft_token_lst_size(t_token *lst);

int		ft_aux_abs(char *str);

void 	ft_wait_child_process(char *cmd, int *exit_status, int process);

void	exec_cmd(t_token **tokens, t_env **env, char **envp, t_pipe *data_pipe);


//--------typer.c-------------

int 	typer_tokens(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe, int *exit_status);

//---------PENDIENTE ORDENAR-----------

int		ft_token_lst_size(t_token *lst);

void	expansor(t_token **tokens, t_env **env, int exit_status);

void	parser(t_token **tokens, t_env **env, char **envp, int *exit_status);

void	print_tokens(t_token **head);

int		ft_token_lst_size(t_token *lst);

void 	free_tokens(t_token **head);

int 	ft_is_built_in(t_token **tokens);

int		ft_exec_builtin(t_token **tokens, t_env **env);

int		ft_echo(t_token *tokens);

int		ft_cd(t_token *tokens, t_env *env);

int		ft_pwd(void);

int		ft_env(t_env *env);


#endif
