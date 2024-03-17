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

#include "../inc/minishell.h"


//Crea una copia de los tokens a enviar al executor, SOLO DE LAS WORDS encontradas hasta la pipe
static void ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens)
{
	t_token	*tmp;
	t_token	*last;
	t_token	*curr;

	curr = *og_tokens;
	last = NULL;
	tmp = NULL;
	while (curr && curr->type != PIPE)
	{
		if (curr->type == WORD)
		{
			tmp = ft_tokendup(curr);
			if (!*new_tokens)
				*new_tokens = tmp;
			else
			{
				if (last)
					last->next = tmp;
			}	
			last = tmp;
		}
		curr = curr->next;
	}
}
// procesa un solo comando, saldra con 1 para acabar el parseo, con 0 si hay pipes.
//en caso de flag_input en 2 (que significa que hay error en redir_in con el archivo)
//no ejecuta nada y solo muestra en mensaje de error.
static int	parse_one_cmd(t_parser *d, t_env **env, char **envp, int *exit_status)
{
	if (!d->data_pipe->pipe_counter && d->t_current)//SI SOLO HAY UN COMANDO 
	{	
		d->flag_input = selector_input(d);
		if (d->flag_input == 2)
			return (ft_aux_close(d->data_pipe, d->data_redir,\
			 d->data_heredoc), 1);
		if (ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe) == 2)
			return (ft_aux_close(d->data_pipe, d->data_redir,\
			 d->data_heredoc), 1);
		d->data_pipe->flag = NO;
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		if (ft_is_built_in(&d->t_tmp))
		{
			*exit_status = ft_exec_builtin(&d->t_tmp, env);
			ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
			return (free_tokens(&d->t_tmp), 1);
		}
		executor(&d->t_tmp, env, envp, d->data_pipe);
		free_tokens(&d->t_tmp);
		ft_wait_child_process(exit_status, 1);
		ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
		return (1);
	}
	return (0);
}
// Procesa un solo comando en un proceso hijo, setea la flag de pipe para la redireccion en el 
// 		proceso hijo, cancela ejecucion en caso de red< in erronea
int parse_child_cmd(t_parser *d, t_env **env, char **envp)
{
	if (d->t_current && d->data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
	{
		d->flag_input = selector_input(d);
		if (d->flag_input)
		{	
			d->data_pipe->flag = NO;
			if (d->flag_input == 2)
				return (d->data_pipe->pipe_counter--, 0);
		}
		d->flag_output = ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe);
		if (d->flag_output)
		{	
			if (d->flag_output == 2)
				return (d->data_pipe->pipe_counter--, 0);
			d->data_pipe->flag = NO;
		}
		else
			d->data_pipe->flag = YES;
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		pipe(d->data_pipe->pipefd);
		d->process++;
		executor(&d->t_tmp, env, envp, d->data_pipe);
		if (dup2(d->data_pipe->pipefd[0], 0)== -1) //tiene que comunicar la tuberia contenga o no contenido siempre en la pipeline 
			ft_error_system(DUP2_ERROR);
		close(d->data_pipe->pipefd[0]);
		close(d->data_pipe->pipefd[1]);
		d->data_pipe->pipe_counter--;
		return (free_tokens(&d->t_tmp), 0);
	}
	return (0);
}
//caso del ultimo comando a ejecutar, donde se recoge el exit status
int	parse_last_child(t_parser *d, t_env **env, char **envp, int *exit_status)
{
	if (d->t_current && (d->t_current->type == PIPE && !d->data_pipe->pipe_counter))
	{
		d->data_pipe->flag = NO;
		if (dup2(d->data_pipe->og_stdout, 1) == -1) 
			ft_error_system(DUP2_ERROR);
		d->t_current = d->t_current->next;
		d->flag_input = selector_input(d);
		if (d->flag_input == 2)
			return (ft_aux_close(d->data_pipe, d->data_redir,\
			 d->data_heredoc), 1);
		ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe);
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		executor(&d->t_tmp, env, envp, d->data_pipe);
		d->process++;
		free_tokens(&d->t_tmp);
		if (dup2(d->data_pipe->og_stdin, 0) == -1) 
			ft_error_system(DUP2_ERROR);
		ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
		ft_wait_child_process(exit_status, d->process);
		return (1);
	}
	return (0);
}
//Procesa los tokens de izquierda a derecha uno por uno. 
//Contempla pipeline y no pipes, redireccionesde input i de output, y here_doc.
void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_parser	*d;

	d = malloc(sizeof(t_parser));
	if (!d)
		ft_error_system(MALLOC_ERROR);
	ft_init_data_parser(d, tokens, exit_status);
	if (parse_one_cmd(d, env, envp, exit_status))
		return(free(d)) ;
	while(d->t_current)//PIPELINE
	{
		parse_child_cmd(d, env, envp);
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (d->t_current && !(d->t_current->type == PIPE))
			d->t_current = d->t_current->next;
		if (parse_last_child(d, env, envp, exit_status))
			return (free(d));
		if (d->t_current)
			d->t_current = d->t_current->next;
	}
	free(d->data_heredoc);
	free(d->data_pipe);
	free(d->data_redir);
	free(d);
}
