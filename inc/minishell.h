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

# define READLINE_LIBRARY

# include "libft/libft.h"
# include <stdio.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <sys/errno.h>
# include "readline/readline.h"
# include "readline/history.h"
# include <termios.h>
# include <dirent.h>

//--------system_errros_definitions-------------
# define MALLOC_ERROR 1
# define DUP2_ERROR 11
# define FORK_ERROR 12
# define CLOSE_ERROR 13
# define DUP_ERROR 14

//--------tokens_types_definitions-------------
# define WORD 2
# define PIPE 3
# define YES 4
# define NO 5
# define RED_IN 6
# define RED_OUT 7
# define DOC 8
# define ERROR_WORD 9
# define ERROR_FILE 16
# define HERE_DOC 10
# define APPEND 17
# define LIMITER 15

//SIMPLE LINKED LIST PARA ENVIROMENT
typedef struct s_env
{
	char			*key_name;
	char			*value;
	struct s_env	*next;
}	t_env;

//SIMPLE LINKED LIST PARA TOKENS
typedef struct s_token
{
	char			*str;
	int				type;
	struct s_token	*next;
}	t_token;

//STRUC PARA DATOS DE PIPES
typedef struct s_pipe
{
	int	pipefd[2];
	int	pipe_counter;
	int	flag;
	int	og_stdin;
	int	og_stdout;
}	t_pipe;

//STRUC PARA DATOS DE REDIRECCIONES
typedef struct s_redir
{
	int	fd_infile;
	int	red_in_counter;
	int	fd_outfile;
	int	red_out_counter;
	int	flag_to_close;
}	t_redir;

typedef struct s_hd_append
{
	int		heredoc_counter;
	int		append_counter;
	int		fd_append;
	char	*tmp_input;
	char	*limiter;
}	t_hd_append;

//STRUC PARA STRUCS EN EL PARSER
typedef struct s_parser
{
	t_redir		*data_redir;
	t_pipe		*data_pipe;
	t_token		*t_tmp;
	t_token		*t_current;
	t_hd_append	*data_hd_append;
	int			process;
	int			flag_input;
	int			flag_output;
}	t_parser;

//STRUC PARA EL TYPER
typedef struct s_typer
{
	int	first_token;
	int	c_m;
	int	cntrl;
}	t_typer;

//STRUC PARA EL LEXER
typedef struct s_lexer
{
	char	*input;
	char	*str;
	int		start;
	int		end;
	int		flag;
}	t_lexer;

typedef struct s_executor
{
	char	**new_envp;
	char	**cmd_argv;
	char	*absolute_path;
	char	*cmd;
	char	**path;
	int		pid;
}	t_executor;

//BUILTINS
//--------builtins.c----------
int		ft_is_built_in(t_token **tokens);
int		ft_exec_builtin(t_token **tokens, t_env **env, int *exit_status);
int		ft_isvalidkey(char *str);

//--------ft_echo.c-----------
int		ft_echo(t_token *tokens);

//--------ft_cd.c-----------
int		ft_cd(t_token *tokens, t_env *env);

//--------ft_pwd.c-----------
int		ft_pwd(t_env *env);

//--------ft_env.c-----------
int		ft_env(t_env *env);

//--------ft_export.c-----------
int		ft_export(t_token *tokens, t_env *env);

//--------ft_exit.c-----------
int		ft_exit(t_token *tokens, int *exit_status);

//--------ft_unsetc-----------
int		ft_unset(t_token *tokens, t_env **env);

//ENVIROMENT
///---------enviroment.c-----------
void	ft_catch_env(char **envp, t_env **head);

//ERRORS
//--------errors.c-----------
void	ft_error_cmd(char *cmd, int type);
void	ft_error_system(int type);
int		ft_error_keyname(char *keyname, int type);

//--------errors_syntax.c-----------
void	ft_error_syntax(int *exit_status, int name, t_token *t_current);

//EXECUTION
//--------executor.c-------------
void	ft_wait_child_process(int *exit_status, int process);
void	ft_exec_absoluthe_path(t_token **tokens, t_executor *d_exec);
void	executor(t_token **tokens, t_env **env, t_pipe *data_pipe);

//--------executor_utils.c-------------
int		ft_token_lst_size(t_token *lst);
int		ft_aux_abs(char *str);
int		ft_env_lst_size(t_env *lst);
char	**ft_copy_env(t_env **env);

//EXPANSOR
//--------expansor.c-----------
void	expansor(char **str, t_env **env, int exit_status);
//--------expansor_utils.c-----------
void	ft_remove_token(t_token **tokens, t_token **curr_token);

//PARSER
//--------parser_utils.c-------------
t_token	*ft_tokendup(t_token *token);
void	ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens);
int		selector_input(t_parser *d, t_env **env, int *exit_status);
int		selector_output(t_parser *d);
void	l_red_out(t_parser *d, t_env **env);
//--------parser_aux.c-------------
int		aux_parse_last_child(t_parser *d, int *exit_status);
void	ft_init_data_parser(t_parser *d, t_token **tokens);
//--------parser.c-------------
void	parser(t_token **tokens, t_env **env, int *exit_status);

//QUOTES
//--------quotes.c-------------
t_token	*ft_createtoken(char *input, int *i, t_env **env, int exit_status);
int		ft_ismetachar(char c);

//REDIRECTIONS
//--------redirs.c-------------
int		ft_red_in_aux(t_redir *data_redir, \
		t_token *t_current, t_pipe *data_pipe);
int		ft_red_out_aux(t_redir *data_redir, \
		t_token *t_current, t_pipe *data_pipe);
void	ft_aux_close(t_pipe *data_pipe, \
		t_redir *data_redir, t_hd_append *data_hd_append);
//--------append.c-------------
int		ft_append(t_hd_append \
		*data_hd_append, t_token *t_current, t_pipe *data_pipe);
//--------here_doc.c-------------
void	ft_here_doc(t_token *token, \
		t_parser *d, t_env **env, int *exit_status);

//SIGNS
//--------signs.c-----------
void	ctrl_c(int *exit_status);
void	sig_handler(int sig);
void	process_sig_handler(int sig);
void	sig_init(int i);
int		g_signal;

//TYPER
//--------typer.c-------------
int		typer_tokens(t_parser *d, t_token **t_current, int *exit_status);
//--------typer_aux.c-------------
int		typer_red_in(t_token **curr_token, t_parser *d, \
		int *consecutive_metachar, int *exit_status);
int		typer_red_out(t_token **curr_token, t_parser *d, \
		int *consecutive_metachar, int *exit_status);
int		typer_here_doc(t_token **curr_token, t_parser *d, \
		int *consecutive_metachar, int *exit_status);
int		typer_append(t_token **curr_token, t_parser *d, \
		int *consecutive_metachar, int *exit_status);

//UTILS
//--------utils.c-------------
void	ft_close(int fd);
void	ft_close2(int fd1, int fd2);
int		ft_close_v2(int *fd);
void	ft_dup2(int fd1, int fd2);
void	free_tokens(t_token **head);

//MAIN
////--------main.c-------------
void	lexer(t_token **tokens, char *input, t_env **env, int exit_status);
void	input_loop(t_env **env, t_token	*tokens);

#endif
