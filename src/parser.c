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

//itera sin modificar los tokens para comprobar prioridad de input en el comando, en caso de error de archivo
//por falta de permisos o inexistente, devuelve un 2 otorgado por ft_red_in_aux;
int selector_input(t_parser *d)
{
	int i;
	t_token *tmp;

	tmp = d->t_current;
	i = 0;
	while (tmp  && tmp->type != PIPE)
	{
		if (tmp->type == HERE_DOC)
		{
			ft_here_doc(tmp, d->data_heredoc, d->data_pipe);
			i = 0;
		}
		else if (tmp->type == RED_IN)
			i = ft_red_in_aux(d->data_redir, tmp,d->data_pipe);
		if (i == 2)
			return (i);
		tmp = tmp->next;
	}
	return (i);
}

//funcion aux, crea un nuevo token copiando el que recibe por parametro
static t_token *ft_tokendup(t_token *token)
{
	t_token *new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		exit(MALLOC_ERROR);
	new_token->str = ft_strdup(token->str);
	if (!new_token->str)
		exit (MALLOC_ERROR);
	new_token->type = token->type;
	new_token->next = NULL;
	return (new_token);
}
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
//Inicializa los strucs contenidos en el struc del parser, y llama al typer para setear tipos a los tokens
//en caso de error pasado por este, pone en NULL la lista de t_current para evitar la ejecucion
void	ft_init_data_parser(t_parser *d, t_token **tokens,\
		int *exit_status)
{
	d->data_heredoc = malloc(sizeof(t_pipe));
	if (!d->data_heredoc)
		ft_error_system(MALLOC_ERROR);
	d->data_heredoc->heredoc_counter = 0;
	d->process = 0;
	d->t_tmp = NULL;
	d->data_pipe = malloc(sizeof(t_pipe));
	if (!d->data_pipe)
		ft_error_system(MALLOC_ERROR);
	d->data_redir = malloc(sizeof(t_redir));
	if (!d->data_redir)
		ft_error_system(MALLOC_ERROR);
	d->data_redir->red_in_counter = 0;
	d->data_redir->red_out_counter = 0;
	d->data_pipe->og_stdin = dup(0);
	d->data_pipe->og_stdout = dup(1);
	if (d->data_pipe->og_stdin == -1 || d->data_pipe->og_stdout == -1)
		ft_error_system(DUP_ERROR);
	d->data_pipe->flag = 0;
	d->data_pipe->pipe_counter = 0;
	d->data_redir->fd_infile = -1;
	d->data_redir->fd_outfile = -1;
	d->t_current = *tokens;
	if (typer_tokens(d->data_redir, &d->t_current, d->data_pipe,\
	 d->data_heredoc, exit_status))
		d->t_current = NULL;
	else
		d->t_current = *tokens;
}
//Procesa los tokens de izquierda a derecha uno por uno. Contempla pipeline y no pipes, redirecciones
//de input i de output, y here_doc.
void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_parser	*d;
	int			tmp;

	d = malloc(sizeof(t_parser));
	if (!d)
		ft_error_system(MALLOC_ERROR);
	ft_init_data_parser(d, tokens, exit_status);
	if (!d->data_pipe->pipe_counter && d->t_current)//SI SOLO HAY UN COMANDO 
	{	
		selector_input(d);
		ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe);
		d->data_pipe->flag = NO;
		ft_tokens_to_exec(&d->t_current, &d->t_tmp);
		if (ft_is_built_in(&d->t_tmp))
		{
			*exit_status = ft_exec_builtin(&d->t_tmp, env);
			ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
			free_tokens(&d->t_tmp);
			return ;
		}
		executor(&d->t_tmp, env, envp, d->data_pipe);
		free_tokens(&d->t_tmp);
		ft_wait_child_process(exit_status, 1);
		ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
		free(d);
		return ;
	}
	while(d->t_current)//PIPELINE
	{
		if (d->t_current && d->data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
		{
			//el selector puede devolver 2, en ese caso se debe encontrar la forma de saltar al siguiente comando porque
			//no se debe ejecutar nada hasta la siguiente pipe
			//se podria solucionar metiendo todo el bloque en una funcion y haciendo return en caso de 2
			if (selector_input(d))
				d->data_pipe->flag = NO;
			if (ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe))
				d->data_pipe->flag = NO;
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
			free_tokens(&d->t_tmp);
		}
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (d->t_current && !(d->t_current->type == PIPE))
			d->t_current = d->t_current->next;
		//caso del ultimo comando a ejecutar, donde se recoge el exit status
		if (d->t_current && (d->t_current->type == PIPE && !d->data_pipe->pipe_counter))
		{
			d->data_pipe->flag = NO;
			if (dup2(d->data_pipe->og_stdout, 1) == -1) //esto ha hecho que funcione "cat tet1 > newfile | wc newfile" como debe
				ft_error_system(DUP2_ERROR);
			d->t_current = d->t_current->next;
			//ft_red_in_aux(d->data_redir, d->t_current, d->data_pipe);
			selector_input(d);
			ft_red_out_aux(d->data_redir, d->t_current, d->data_pipe);
			ft_tokens_to_exec(&d->t_current, &d->t_tmp);
			executor(&d->t_tmp, env, envp, d->data_pipe);
			d->process++;
			free_tokens(&d->t_tmp);
			if (dup2(d->data_pipe->og_stdin, 0) == -1) //esto ha hecho que funcione "cat tet1 > newfile | wc newfile" como debe
				ft_error_system(DUP2_ERROR);
			ft_aux_close(d->data_pipe, d->data_redir, d->data_heredoc);
			ft_wait_child_process(exit_status, d->process);
			return (free(d));
		}
		if (d->t_current)
			d->t_current = d->t_current->next;
	}
	free(d->data_pipe);
	free(d->data_redir);
	free(d->data_heredoc);
	free(d);
}
