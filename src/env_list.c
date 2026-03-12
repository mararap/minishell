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

// this should be moved to utils.c
/*
** is_valid_name
**
** Check that a string is a valid shell variable name:
**   - Must not be NULL
**   - Must start with letter or underscore
**   - Remaining chars must be alphanumeric or underscore
**
** Returns 1 if valid, 0 otherwise. Used by setter and
** import functions to enforce shell naming rules.
*/

int	is_valid_name(const char *s)
{
	if (s == NULL)
		return (0);
	if (!ft_isalpha(*s) && *s != '_')
		return (0);
	s++;
	while (*s != '\0')
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

	if (name == NULL || is_valid_name(name) == 0)
		return (NULL);
	node = ms_xmalloc(sizeof(t_env_var));
	node->name = ms_strdup_safe(name);
	if (value != NULL)
		node->value = ms_strdup_safe(value);
	else
		node->value = ms_strdup_safe("");
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

t_env_var	*ms_env_from_environ(char **envp)
{
	int			i;
	char		*sep;
	char		*name;
	char		*value;
	t_env_var	*head;
	t_env_var	*tail;
	t_env_var	*node;

	i = 0;
	head = NULL;
	tail = NULL;
	while (envp != NULL && envp[i] != NULL)
	{
		sep = ft_strchr(envp[i], '=');
		if (sep != NULL)
		{
			name = ft_substr(envp[i], 0, sep - envp[i]);
			value = ft_strdup(sep + 1);
		}
		else
		{
			name = ft_strdup(envp[i]);
			value = NULL;
		}
		if (is_valid_name(name) == 1)
		{
			node = ms_env_new_node(name, value, 1);
			if (node != NULL)
			{
				if (head == NULL)
					head = node;
				else
					tail->next = node;
				tail = node;
			}
		}
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

	if (env_list == NULL)
		return ;
	while (*env_list != NULL)
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
	size_t	len;

	if (name == NULL)
		return (NULL);
	len = ft_strlen(name);
	while (env_list != NULL)
	{
		if (ft_strlen(env_list->name) == len && ft_strncmp(env_list->name, name,
				len) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	return (NULL);
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
	t_env_var	*iter;
	t_env_var	*prev;
	t_env_var	*new_node;

	if (name == NULL || is_valid_name(name) == 0)
		return (1);
	iter = *env_list;
	prev = NULL;
	while (iter != NULL)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			free(iter->value);
			if (value != NULL)
				iter->value = ms_strdup_safe(value);
			else
				iter->value = ms_strdup_safe("");
			iter->exported = exported;
			return (0);
		}
		prev = iter;
		iter = iter->next;
	}
	new_node = ms_env_new_node(name, value, exported);
	if (new_node == NULL)
		return (1);
	if (prev == NULL)
		*env_list = new_node;
	else
		prev->next = new_node;
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

int	ms_env_unset(t_env_var **env_list, char *name)
{
	t_env_var	*iter;
	t_env_var	*prev;

	if (name == NULL || is_valid_name(name) == 0)
		return (1);
	iter = *env_list;
	prev = NULL;
	while (iter != NULL)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			if (prev == NULL)
				*env_list = iter->next;
			else
				prev->next = iter->next;
			free(iter->name);
			free(iter->value);
			free(iter);
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
	char		**envp;
	t_env_var	*iter;
	int			count;
	int			i;
	char		*str;
	size_t		name_len;
	size_t		val_len;

	iter = env_list;
	count = 0;
	while (iter != NULL)
	{
		if (iter->exported == 1 && iter->value != NULL)
			count++;
		iter = iter->next;
	}
	envp = ms_xmalloc(sizeof(char *) * (count + 1));
	i = 0;
	iter = env_list;
	while (iter != NULL)
	{
		if (iter->exported == 1 && iter->value != NULL)
		{
			name_len = ft_strlen(iter->name);
			val_len = ft_strlen(iter->value);
			str = ms_xmalloc(sizeof(char) * (name_len + val_len + 2));
			ft_strlcpy(str, iter->name, name_len + 1);
			ft_strlcat(str, "=", name_len + val_len + 2);
			ft_strlcat(str, iter->value, name_len + val_len + 2);
			envp[i] = str;
			i++;
		}
		iter = iter->next;
	}
	envp[i] = NULL;
	return (envp);
}