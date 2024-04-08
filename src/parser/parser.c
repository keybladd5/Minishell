/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: polmarti <polmarti@student.42barcel>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 11:19:02 by polmarti          #+#    #+#             */
/*   Updated: 2024/02/12 11:19:04 by polmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

//aux function to select the redirs, return 1 if all docs works
//else return 0 to leave the execution cmd line
//flag to close is a flag to say in the las_cmd to execute que no pille 
// el fd de outfile para sacar el output
static int	selector_redirs_child(t_parser *d, t_env **env)
{
	d->flag_input = selector_input(d);
	if (d->flag_input)
	{
		d->data_pipe->flag = NO;
		if (d->flag_input == 2)
			return (l_red_out(d, env), 0);
	}
	d->flag_output = \
	selector_output(d);
	if (d->flag_output)
	{
		if (d->flag_output == 2)
			return (l_red_out(d, env), 0);
		d->data_redir->flag_to_close = 1;
		return (2);
	}
	return (1);
}

//procesa un solo comando, saldra con 1 para acabar el parseo, 
// con 0 si hay pipes.
// en caso de flag_input en 2 
// (que significa que hay error en redir_in con el archivo)
// no ejecuta nada y solo muestra en mensaje de error.
static int	parse_one_cmd(t_parser *d, t_env **env, \
int *exit_status)
{
	if (!d->data_pipe->pipe_counter && d->t_current)
	{
		d->flag_input = selector_input(d);
		if (d->flag_input == 2)
			return (ft_aux_close(d->data_pipe, d->data_redir, \
			d->data_hd_append), 1);
		d->flag_output = selector_output(d);
		if (d->flag_output == 2)
			return (ft_aux_close(d->data_pipe, d->data_redir, \
			d->data_hd_append), 1);
		d->data_pipe->flag = NO;
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		if (ft_is_built_in(&d->t_tmp))
		{
			*exit_status = ft_exec_builtin(&d->t_tmp, env, exit_status);
			ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append);
			return (free_tokens(&d->t_tmp), 1);
		}
		executor(&d->t_tmp, env, d->data_pipe);
		free_tokens(&d->t_tmp);
		ft_wait_child_process(exit_status, 1);
		ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append);
		return (1);
	}
	return (0);
}

// Procesa un solo comando en un proceso hijo, setea la flag
// de pipe para la redireccion en el 
// 	proceso hijo, cancela ejecucion en caso de red< in erronea
static void	parse_child_cmd(t_parser *d, t_env **env)
{
	int	redir_flag;

	redir_flag = -1;
	if (d->t_current && d->data_pipe->pipe_counter)
	{
		redir_flag = selector_redirs_child(d, env);
		if (redir_flag == 0)
			return ;
		else if (redir_flag == 2)
			d->data_pipe->flag = NO;
		else
			d->data_pipe->flag = YES;
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		pipe(d->data_pipe->pipefd);
		d->process++;
		executor(&d->t_tmp, env, d->data_pipe);
		if (dup2(d->data_pipe->pipefd[0], 0) == -1)
			ft_error_system(DUP2_ERROR);
		close(d->data_pipe->pipefd[0]);
		close(d->data_pipe->pipefd[1]);
		d->data_pipe->pipe_counter--;
		return (free_tokens(&d->t_tmp));
	}
	return ;
}

//caso del ultimo comando a ejecutar, donde se recoge el exit status
static int	parse_last_child(t_parser *d, t_env **env, \
int *exit_status)
{
	if (d->t_current && \
	(d->t_current->type == PIPE && !d->data_pipe->pipe_counter))
	{
		d->data_pipe->flag = NO;
		d->t_current = d->t_current->next;
		d->flag_input = selector_input(d);
		if (aux_parse_last_child(d, exit_status))
			return (1);
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		executor(&d->t_tmp, env, d->data_pipe);
		d->process++;
		free_tokens(&d->t_tmp);
		ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append);
		return (ft_wait_child_process(exit_status, d->process), 1);
	}
	return (0);
}

//Procesa los tokens de izquierda a derecha uno por uno. Llama 
//	al typer para setear tipos a los tokens
//	en caso de error pasado por este, pone en NULL la lista de 
//	t_current para evitar la ejecucion
//	Contempla pipeline y no pipes, redireccionesde input i de output,
// y here_doc.
void	parser(t_token **tokens, t_env **env, int *exit_status)
{
	t_parser	*d;

	d = malloc(sizeof(t_parser));
	if (!d)
		ft_error_system(MALLOC_ERROR);
	ft_init_data_parser(d, tokens);
	if (typer_tokens(d, &d->t_current, exit_status))
		d->t_current = NULL;
	else
		d->t_current = *tokens;
	if (parse_one_cmd(d, env, exit_status))
		return (free(d));
	while (d->t_current)
	{
		parse_child_cmd(d, env);
		while (d->t_current && !(d->t_current->type == PIPE))
			d->t_current = d->t_current->next;
		if (parse_last_child(d, env, exit_status))
			return (free(d));
		if (d->t_current)
			d->t_current = d->t_current->next;
	}
	return (ft_aux_close(d->data_pipe, d->data_redir, d->data_hd_append), \
	free(d));
}
