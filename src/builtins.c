#include "../inc/minishell.h"

//Depende de como hagamos la funcion que llame a los builtins habra que modificar los parametros y returns
//Por defecto todos los builtins tienen que retornar 0 (= EXIT_SUCCESS) el cual habra q guardar en algun sitio para el $?

//ECHO
//Al llamar a la funcion desde ft_is_built_in ((hacen falta tantas _ ??)) se pasa como argumento el siguiente token al propio comando 'echo'
//Imprime todos los tokens siguientes (habrá que modificarlo para cuando haya pipes o redirects) y \n excepto cuando hay flag -n
int  ft_echo(t_token **tokens)
{
	t_token	*t_current;
	int		flag;

	t_current = *tokens;
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
	return (0)		
}
