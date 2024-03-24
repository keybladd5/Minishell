#include "../inc/minishell.h"

//check if the word on the token is built in comand
//!!!ft_strncmp puede dar falsos positivos si hay mas caracteres despues del comando
int ft_is_built_in(t_token **tokens)
{
	if (!*tokens)
		return (0);
	if (!ft_strncmp("echo\0", (*tokens)->str, 5) || \
	!ft_strncmp("cd\0", (*tokens)->str, 3) || \
	!ft_strncmp("pwd\0", (*tokens)->str, 4) || \
	!ft_strncmp("env\0", (*tokens)->str, 4) || \
	!ft_strncmp("export\0", (*tokens)->str, 7) || \
	!ft_strncmp("exit\0", (*tokens)->str, 5))
		return (1);
	return (0);
}

int	ft_exec_builtin(t_token **tokens, t_env **env, int *exit_status)
{
	if (!ft_strncmp("echo\0", (*tokens)->str, 5))
		return (ft_echo((*tokens)->next));
	else if (!ft_strncmp("cd\0", (*tokens)->str, 3))
		return (ft_cd((*tokens)->next, *env));
	else if (!ft_strncmp("pwd\0", (*tokens)->str, 4))
		return (ft_pwd());
	else if (!ft_strncmp("env\0", (*tokens)->str, 4))
		return (ft_env(*env));
	else if (!ft_strncmp("export\0", (*tokens)->str, 7))
		return (ft_export((*tokens)->next, *env));
	else if (!ft_strncmp("exit\0", (*tokens)->str, 5))
	{
			ft_printf("exit\n");
			exit(*exit_status);
	}
	return (0);
}

//Depende de como hagamos la funcion que llame a los builtins habra que modificar los parametros y returns
//Por defecto todos los builtins tienen que retornar 0 (= EXIT_SUCCESS) el cual habra q guardar en algun sitio para el $?

//ECHO
//Al llamar a la funcion desde ft_is_built_in ((hacen falta tantas _ ??)) se pasa como argumento el siguiente token al propio comando 'echo'
//Imprime todos los tokens siguientes (habrá que modificarlo para cuando haya pipes o redirects) y \n excepto cuando hay flag -n
int	ft_echo(t_token *tokens)
{
	t_token	*t_current;
	int		flag;

	t_current = tokens;
	flag = 0;
	if (!tokens)
		return(ft_putstr_fd("\n", 1), 0);
	while (t_current && !ft_strncmp("-n", t_current->str, 2) && !ft_strchr(t_current->str, ' ')) //comprueba si el primer token es la flag "-n" (null terminated para evitar falsos positivos tipo -na)
	{
		flag = 1;
		t_current = t_current->next;
	}
	while (t_current) //para cada token imprime su contenido y un espacio excepto para el ultimo (next = NULL)
	{
		ft_putstr_fd(t_current->str, 1);
		if (t_current->next)
			ft_putstr_fd(" ", 1);
		t_current = t_current->next;
	}
	if (!flag) //si no habia flag imprime newline al final
		ft_putstr_fd("\n", 1);
	return (0);
}

//CD
//INPUT: Siguiente token al comando (puede ser NULL)
//-Guardar valor inicial de PWD en OLDPWD (env list)
//-Cambiar CWD al path especificado (o HOME si es NULL) con chdir
//-Guardar el path en PWD (env list)
int	ft_cd(t_token *tokens, t_env *env)
{
	char	*path;
	t_env	*tmp_env;

	tmp_env = env;
	if (!tokens) //si no hay token despues del comando debe mandar a HOME
	{
		while (tmp_env)
		{
			if (!ft_strncmp(tmp_env->key_name, "HOME", 4))
			{
				path = tmp_env->value;
				break ;
			}
			tmp_env = tmp_env->next;
		}
		tmp_env = env;
	}
	else //guarda el path especificado
		path = tokens->str;
	if (chdir(path) != 0) //cambia el directrio actual al que le pasa y si falla (return -1) printea el error y sale
	{
		// ft_putstr_fd("minishell: cd: ", 2);
		// ft_putstr_fd(path, 2);
		// ft_putendl_fd(": No such file or directory", 2);
		perror(path);
		return (1); //!!!Este 1 lo pasaria a ft_is_built_in, que lo pasaria a executor y petaria. REVISAR
	}
	while (tmp_env) //Busca en la env list el dato guardado en PWD, lo guarda en path y lo actualiza
	{
		if (!ft_strncmp(tmp_env->key_name, "PWD", 3))
		{
			path = tmp_env->value;
			tmp_env->value = getcwd(NULL, 0);
			tmp_env = env;
			break;
		}
		tmp_env = tmp_env->next;
	}
	while (tmp_env) //Ahora busca OLDPWD y lo cambia por el que habia antes en PWD
	{
		if (!ft_strncmp(tmp_env->key_name, "OLDPWD", 6))
		{
			free(tmp_env->value);
			tmp_env->value = path;
			break;
		}
		tmp_env = tmp_env->next;
	}
	return (0);
}

//PWD
//Con getcwd se hace malloc del string y guarda el path actual
//Imprime el path seguido de \n y libera el string
int	ft_pwd(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
		exit (MALLOC_ERROR);
	ft_putstr_fd(cwd, 1);
	write(1, "\n", 1);
	free (cwd);
	return (0);
}

//ENV
//Recore la env list y va printeando los valores
int	ft_env(t_env *env)
{
	while (env)
	{
		ft_putstr_fd(env->key_name, 1);
		write(1, "=", 1);
		ft_putendl_fd(env->value, 1);
		env = env->next;
	}
	return (0);
}

//EXPORT
//no args -> printea env list en formato {declare -x env->key_name="env->value"}
//!!!{export A} -> segfault
//!!!{export A=}
static int	ft_isvalidkey(char *str)
{
	if (!(ft_isalpha(str[0]) || str[0] == '_'))
		return 0;
	while (*(++str) && *str != '=')
	{
		if (!(ft_isalnum(*str) || *str == '_'))
			return 0;
	}
	return 1;
}

//esta fallando en casos de NULL
int	ft_export(t_token *tokens, t_env *env)
{
	t_env	*tmp;
	t_env	*last;
	t_env	*curr_env;
	char	*div;
	char	*keyname;
	int		var_flag;

	curr_env = env;
	if (!tokens)
	{
		while (env)
		{
			ft_putstr_fd("declare -x ", 1);
			ft_putstr_fd(env->key_name, 1); //aqui peta
			if (env->value)
			{
				write(1, "=\"", 2);
				ft_putstr_fd(env->value, 1);
				ft_putstr_fd("\"", 1);
			}
			write(1, "\n", 1);
			env = env->next;
		}
		return (0);
	}
	while (curr_env->next)
	{
		last = curr_env->next;
		curr_env = last;
	}
	while (tokens)
	{
		if (!ft_isvalidkey(tokens->str))
		{
			ft_putstr_fd("\033[31mminishell: export: `", 2);
			ft_putstr_fd(tokens->str, 2);
			ft_putstr_fd("': not a valid identifier\x1b[0m\n", 2);
			return (1);
		}
		var_flag = 0;
		div = ft_strchr(tokens->str, '=');
		keyname = ft_substr(tokens->str, 0, (div - tokens->str));
		if (!keyname)
			exit (MALLOC_ERROR);
		while (env)
		{
			if (!ft_strxcmp(keyname, env->key_name))
			{
				var_flag = 1;
				break ;
			}
			env = env->next;
		}
		if (var_flag)
		{
			if (div)
			{
				env->value = ft_substr(div+1, 0, ft_strlen(div));
				if (!env->value)
					exit (MALLOC_ERROR);
			}
		}
		else
		{
			tmp = (t_env *)malloc(sizeof(t_env));
			if (!tmp)
				exit(MALLOC_ERROR);
			tmp->key_name = NULL;
			tmp->value = NULL;
			tmp->next = NULL;
			div = ft_strchr(tokens->str, '=');
			tmp->key_name = ft_substr(tokens->str, 0, (div - tokens->str));
			if (!tmp->key_name)
				exit (MALLOC_ERROR);
			if (div)
			{
				tmp->value = ft_substr(div+1, 0, ft_strlen(div));
				if (!tmp->value)
					exit (MALLOC_ERROR);
			}
			last->next = tmp;
			last = tmp;
		}
		tokens = tokens->next;
	}
	return (0);
}
