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


//Funcion que abre los fds en caso de redireccion input, el retorno se usa para iterar en el parser en base a los tokens
int	ft_red_in_aux(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe)
{
	t_token	*dir_doc;

	dir_doc = t_current;
	if (!data_redir->red_in_counter)
		return (0);
	while (dir_doc && dir_doc->type != RED_IN) //busca el token '<'. Puede salir con la direccion del token o NULL si no lo ha encontrado
		dir_doc = dir_doc->next;
	if (!dir_doc)
		return (0);
	data_redir->fd_infile = open(dir_doc->next->str, O_RDONLY);
	data_redir->red_in_counter--;
	dup2(data_redir->fd_infile, 0);
	if (data_pipe->pipe_counter)
	{
		close(data_redir->fd_infile);
		data_redir->fd_infile = -1;
		return (1);
	}
	return (0);
}
//Reestablece los fd originales, cierra el resto de fd y libera estructuras de pipes y redirs
void	ft_aux_close(t_pipe *data_pipe, t_redir *data_redir)
{
	if (!data_pipe->pipe_counter)//pedniente modificar ya se hace dup2 de mas
	{
		dup2(data_pipe->og_stdin, 0);
		dup2(data_pipe->og_stdout, 1);
	}
	close(data_redir->fd_infile);
	close(data_redir->fd_outfile);
	close(data_pipe->og_stdin);
	close(data_pipe->og_stdout);
	free(data_pipe);
	free(data_redir);
}
//Muestra mensajes de error por el fd 2 y modifica el exit status
int ft_error_syntax(int *exit_status, int name, t_token *t_current)
{
	if (name == PIPE)
		ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `|'\x1b[0m\n", 2);
	else if (name == RED_IN || name == RED_OUT)
	{
		if (t_current && !ft_strncmp(t_current->str, "|", 1))
			ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `|'\x1b[0m\n", 2);
		else if (t_current && (!t_current->next || t_current->next->type != PIPE))
		{
			ft_putstr_fd("\033[31mminishell: ", 2);
			ft_putstr_fd(t_current->str, 2);
			ft_putstr_fd(": No such file or directory\x1b[0m\n", 2);
			*exit_status = 1;
			return (*exit_status);
		}
		else
			ft_putstr_fd("\033[31mminishell: syntax error near unexpected token `newline'\x1b[0m\n", 2);
	}
	*exit_status = 258;
	return (*exit_status);
}
//funcion que setea los tipos a todos los nodos de la lista
int typer_tokens(t_redir *data_redir, t_token *t_current, t_pipe *data_pipe, int *exit_status)
{
	while(t_current)
	{	
		if (!ft_strncmp(t_current->str, "|\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			t_current->type = PIPE; //seteo type
			data_pipe->pipe_counter++;
			t_current = t_current->next;
			if (!t_current)
				return (ft_error_syntax(exit_status, PIPE, NULL));
		}
		else if (!ft_strncmp(t_current->str, "<\0", 2)) //si encuentras pipe //!!!que pasa si el string tiene mas caracteres???
		{
			t_current->type = RED_IN; //seteo type
			t_current = t_current->next;
			if (!t_current || (data_redir->fd_infile = open(t_current->str, O_RDONLY)) == -1)
				return (ft_error_syntax(exit_status, RED_IN, t_current));
			close(data_redir->fd_infile);
			data_redir->fd_infile = -1;
			t_current->type = DOC;
			data_redir->red_in_counter++;
			t_current = t_current->next;	
		}
		else if (!ft_strncmp(t_current->str, ">\0", 2))
		{
			t_current->type = RED_OUT; //seteo type
			t_current = t_current->next;
			//para checkear que esto funcione, en la segunda opcion del if, n
			if (!t_current) 
				return (ft_error_syntax(exit_status, RED_OUT, t_current));
			t_current->type = DOC;
			data_redir->red_out_counter++;
			t_current = t_current->next;	
		}
		else
		{
			t_current->type = WORD; //seteo type default
			t_current = t_current->next;
		}
	}
	return (0);
}
//Espera los procesos hijo y recoge los exit status, ademas muestra el error de comando no encontrado
void ft_wait_child_process(char *cmd, int *exit_status)
{
	int status;

	wait(&status);
	if (WIFEXITED(status))
		*exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status)) //esta opcion no sera funcional hasta implementar signals en procesos hijos
	{
		if (WTERMSIG(status) == SIGINT)
			*exit_status = 130;
		else if (WTERMSIG(status) == SIGQUIT)
			*exit_status = 131;
	}
	if (*exit_status == 127)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putendl_fd(": command not found", 2);
	}
}

//funcion aux, crea un nuevo token copiando el que recibe por parametro
t_token *ft_tokendup(t_token *token)
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
//Crea una copia de los tokens a enviar al executor 
void ft_tokens_to_exec(t_token **og_tokens, t_token **new_tokens)
{
	t_token	*tmp = NULL;
	t_token	*last = NULL;
	t_token *curr = NULL;
	curr = *og_tokens;
	while (curr && curr->type != WORD)//para saltar casos como "< test cat" hasta la palabra necesaria para ejecutar como es el cat
		curr = curr->next;
	while (curr && curr->type == WORD)
	{
		tmp = ft_tokendup(curr);
		if (!*new_tokens)
			*new_tokens = tmp;
		else
			last->next = tmp;	
		last = tmp;
		curr = curr->next;
	}
}

//Procesa los tokens de izquierda a derecha uno por uno.
void parser(t_token **tokens, t_env **env, char **envp, int *exit_status)
{
	t_token		*t_current;
	t_token		*t_tmp = NULL;
	t_pipe		*data_pipe;
	t_redir		*data_redir;

	data_pipe = malloc(sizeof(t_pipe));
	if (!data_pipe)
		exit (MALLOC_ERROR);
	data_redir = malloc(sizeof(t_redir));
	if (!data_redir)
		exit (MALLOC_ERROR);
	data_redir->red_in_counter = 0;
	data_pipe->og_stdin = dup(0);
	data_pipe->og_stdout = dup(1);
	data_pipe->pipe_counter = 0;
	data_redir->fd_infile = -1;
	data_redir->fd_outfile = -1;
	t_current = *tokens;
	if (typer_tokens(data_redir, t_current, data_pipe, exit_status))
		return (ft_aux_close(data_pipe, data_redir));
	t_current = *tokens;
	if (!data_pipe->pipe_counter && t_current)//SI SOLO HAY UN COMANDO 
	{
		ft_red_in_aux(data_redir, t_current, data_pipe);
		data_pipe->flag = NO;
		ft_tokens_to_exec(&t_current, &t_tmp);
		if (ft_is_built_in(&t_tmp))
		{
			*exit_status = ft_exec_builtin(&t_tmp, env);
			return ;
		}
		exec_cmd(&t_tmp, env, envp, data_pipe);
		free_tokens(&t_tmp);
		ft_wait_child_process(t_current->str, exit_status);
		ft_aux_close(data_pipe, data_redir);
		return ;
	}
	while(t_current)//PIPELINE
	{
		while ((t_current && t_current->type != WORD) || (t_current->next && t_current->next->type == RED_IN ))//en caso de orden de redireccion necesito colocarme en la palabra a ejecutar
		{
			if (ft_red_in_aux(data_redir, t_current, data_pipe))
				break ;
			t_current = t_current->next;
		}
		//entra aqui si hay pipes leidas, y ejecuta hasta nodos hasta la pipe(en el exec)
		if (t_current && data_pipe->pipe_counter)//si hay minimo 1 pipe leido PUEDE SER UN ELSE
		{
			ft_tokens_to_exec(&t_current, &t_tmp);
			data_pipe->flag = YES;
			pipe(data_pipe->pipefd);
			exec_cmd(&t_tmp, env, envp, data_pipe);
			dup2(data_pipe->pipefd[0], 0); 
			close(data_pipe->pipefd[0]);
			close(data_pipe->pipefd[1]);
			data_pipe->pipe_counter--;
			free_tokens(&t_tmp);
			ft_wait_child_process(t_current->str, exit_status);
		}
		//ahora quiero iterar hasta que los nodos sean de otro comando, los diferencia el nodo pipe
		while (t_current && !(t_current->type == PIPE))
			t_current = t_current->next;
		//caso del ultimo comando a ejecutar, donde se recoge el exit status
		if (t_current && (t_current->type == PIPE && !data_pipe->pipe_counter))
		{
			data_pipe->flag = NO;
			t_current = t_current->next;
			ft_red_in_aux(data_redir, t_current, data_pipe);
			ft_tokens_to_exec(&t_current, &t_tmp);	
			exec_cmd(&t_tmp, env, envp, data_pipe);
			free_tokens(&t_tmp);
			ft_wait_child_process(t_current->str, exit_status);
			ft_aux_close(data_pipe, data_redir);
			return ;
		}
		if (t_current)
			t_current = t_current->next;
	}
}

//El problema actual es que no se puede ejecutar el REDIR_IN en cualquier parte de la pipeline substituyendo los fds
