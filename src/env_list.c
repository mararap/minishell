/*
** env_list.c — Environment Variable Management for Minishell
**
** This module implements a linked list of environment variables
** for the minishell project. Environment variables are stored
** internally as nodes with name, value and an export flag. Only
** exported variables are included in the environment array
** passed to child processes (e.g., via execve()).
**
** Functions in this file support:
**   - Importing the initial environment from the parent shell
**   - Validating variable names
**   - Setting, unsetting, getting and exporting variables
**   - Converting the list into a NULL-terminated array suitable
**     for execve
**   - Freeing the list and any arrays created from it
**
** Each function is commented below with its purpose, inputs,
** outputs, and side effects.
*/

#include "minishell.h"

static int	is_valid_name(const char *s)
{
	if (!s)
		return (0);
	if (!ft_isalpha(*s) && *s != '_')
		return (0);
	s++;
	while (*s)
	{
		if (!ft_isalnum(*s) && *s != '_')
			return (0);
		s++;
	}
	return (1);
}

static t_env_var	*ms_env_new_node(char *name, char *value, int exported)
{
	t_env_var	*node;

	if (!name || !is_valid_name(name))
		return (NULL);
	node = ms_xmalloc(sizeof(t_env_var));
	node->name = ms_strdup_safe(name);
	node->value = NULL;
	if (value)
		node->value = ms_strdup_safe(value);
	node->exported = exported;
	node->next = NULL;
	return (node);
}

/*
** ms_env_from_environ
**
** Build the internal environment variable linked list
** from the envp array passed to main(). Each entry in envp
** has format "NAME=value". Only valid identifiers (checked
** by is_valid_name) are imported. Imported variables are
** marked as exported (exported == 1).
**
** Returns head of the new list, or NULL if envp is empty.
*/

static int	ms_env_split_entry(char *entry, char **name, char **value)
{
	char	*sep;

	sep = ft_strchr(entry, '=');
	if (!sep)
	{
		*name = ft_strdup(entry);
		*value = NULL;
		return (!*name);
	}
	*name = ft_substr(entry, 0, sep - entry);
	*value = ft_strdup(sep + 1);
	if (!*name || !*value)
	{
		free(*name);
		free(*value);
		return (1);
	}
	return (0);
}
static void	ms_env_append_node(t_env_var **head, t_env_var **tail,
		t_env_var *node)
{
	if (!node)
		return ;
	if (!*head)
		*head = node;
	else
		(*tail)->next = node;
	*tail = node;
}
static t_env_var	*ms_env_find_var(t_env_var *env_list, char *name)
{
	size_t	len;

	if (!name)
		return (NULL);
	len = ft_strlen(name);
	while (env_list)
	{
		if (ft_strlen(env_list->name) == len && ft_strncmp(env_list->name, name,
				len) == 0)
			return (env_list);
		env_list = env_list->next;
	}
	return (NULL);
}
static int	ms_env_exported_count(t_env_var *env_list)
{
	int	count;

	count = 0;
	while (env_list)
	{
		if (env_list->exported == 1 && env_list->value)
			count++;
		env_list = env_list->next;
	}
	return (count);
}
static char	*ms_env_make_entry(t_env_var *var)
{
	size_t	name_len;
	size_t	val_len;
	char	*str;

	name_len = ft_strlen(var->name);
	val_len = ft_strlen(var->value);
	str = ms_xmalloc(sizeof(char) * (name_len + val_len + 2));
	ft_strlcpy(str, var->name, name_len + 1);
	ft_strlcat(str, "=", name_len + val_len + 2);
	ft_strlcat(str, var->value, name_len + val_len + 2);
	return (str);
}
t_env_var	*ms_env_from_environ(char **envp)
{
	int			i;
	char		*name;
	char		*value;
	t_env_var	*head;
	t_env_var	*tail;

	i = 0;
	head = NULL;
	tail = NULL;
	while (envp && envp[i])
	{
		name = NULL;
		value = NULL;
		if (!ms_env_split_entry(envp[i], &name, &value) && is_valid_name(name))
			ms_env_append_node(&head, &tail, ms_env_new_node(name, value, 1));
		free(name);
		free(value);
		i++;
	}
	return (head);
}

/*
** ms_env_free_list
**
** Free all memory used by the environment list, including
** names, values, and nodes. env_list is a pointer to the
** head pointer; this function sets *env_list to NULL
** on completion to avoid dangling pointers.
*/

void	ms_env_free_list(t_env_var **env_list)
{
	t_env_var	*next;

	if (!env_list)
		return ;
	while (*env_list)
	{
		next = (*env_list)->next;
		free((*env_list)->name);
		free((*env_list)->value);
		free(*env_list);
		*env_list = next;
	}
}

/*
** ms_env_get_value
**
** Look up a variable value by name in the linked list.
** Returns pointer to the value string if found,
** or NULL if not present. This does not allocate new memory;
** the return pointer belongs to the list node.
*/

char	*ms_env_get_value(t_env_var *env_list, char *name)
{
	t_env_var	*var;

	var = ms_env_find_var(env_list, name);
	if (!var)
		return (NULL);
	return (var->value);
}

/*
** ms_env_set
**
** Set or update a shell environment variable:
**   - If a node with name exists, update its value and
**     set export flag accordingly.
**   - If not, create a new node and attach it to list.
**
** Returns 0 on success, 1 on failure (invalid name or
** allocation failure). exported controls whether the
** variable should be included in child environments.
*/

int	ms_env_set(t_env_var **env_list, char *name, char *value, int exported)
{
	t_env_var	*var;
	t_env_var	*tail;

	if (!name || !is_valid_name(name))
		return (1);
	var = ms_env_find_var(*env_list, name);
	if (var)
	{
		free(var->value);
		var->value = NULL;
		if (value)
			var->value = ms_strdup_safe(value);
		var->exported = exported;
		return (0);
	}
	tail = *env_list;
	while (tail && tail->next)
		tail = tail->next;
	ms_env_append_node(env_list, &tail, ms_env_new_node(name, value, exported));
	return (0);
}

/*
** ms_env_unset
**
** Remove a variable by name from the environment list.
** Only valid names are considered; invalid names return
** failure (1). Removing a non-existent variable returns
** success (0) per POSIX unset semantics. Frees all memory
** for the removed node.
*/

static void	ms_env_remove_node(t_env_var **env_list, t_env_var *prev,
		t_env_var *iter)
{
	if (!prev)
		*env_list = iter->next;
	else
		prev->next = iter->next;
	free(iter->name);
	free(iter->value);
	free(iter);
}

int	ms_env_unset(t_env_var **env_list, char *name)
{
	t_env_var	*iter;
	t_env_var	*prev;

	if (!name || !is_valid_name(name))
		return (1);
	iter = *env_list;
	prev = NULL;
	while (iter)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			ms_env_remove_node(env_list, prev, iter);
			return (0);
		}
		prev = iter;
		iter = iter->next;
	}
	return (0);
}

// function to free envp array. New not yet used
/*
** free_envp_array
**
** Free a NULL-terminated environment array created by
** ms_env_to_array(). Frees each string and then the array
** itself.
*/

/*
void	free_envp_array(char **envp)
{
	int	i;

	if (envp == NULL)
		return ;
	i = 0;
	while (envp[i] != NULL)
	{
		free(envp[i]);
		i++;
	}
	free(envp);
}*/

/*
** ms_env_to_array
**
** Build a NULL-terminated array of strings suitable for
** passing as envp to execve():
**   ["NAME=value", "OTHER=stuff", NULL]
**
** Only nodes with exported == 1 are included.
** Memory is allocated for each "NAME=value" string
** and the array itself; caller must free them using
** free_envp_array().
*/

char	**ms_env_to_array(t_env_var *env_list)
{
	char **envp;
	int count;
	int i;

	count = ms_env_exported_count(env_list);
	envp = ms_xmalloc(sizeof(char *) * (count + 1));
	i = 0;
	while (env_list)
	{
		if (env_list->exported == 1 && env_list->value)
			envp[i++] = ms_env_make_entry(env_list);
		env_list = env_list->next;
	}
	envp[i] = NULL;
	return (envp);
}