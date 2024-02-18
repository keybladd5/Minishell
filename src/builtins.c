#include "../inc/minishell.h"

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
	if (!ft_strncmp("-n\0", t_current->str, 3)) //comprueba si el primer token es la flag "-n" (null terminated para evitar falsos positivos tipo -na)
	{
		t_current = t_current->next;
		flag = 1;
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
		ft_putstr_fd("minishell: cd: ", 2);
		ft_putstr_fd(path, 2);
		ft_putstr_fd(": No such file or directory", 2);
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
		return (1);
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