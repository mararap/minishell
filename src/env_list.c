#include "minishell.h"

static t_env_var	*ms_env_new_node(char *name, char *value)
{
	t_env_var	*node;

	node = (t_env_var *)ms_xmalloc(sizeof(t_env_var));
	node->name = ms_strdup_safe(name);
	node->value = ms_strdup_safe(value);
	node->next = NULL;
	return (node);
}

t_env_var	*ms_env_from_environ(char **envp)
{
	t_env_var	*head;
	t_env_var	*tail;
	char		*sep;
	char		*name;
	char		*value;
	int			i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (envp && envp[i])
	{
		sep = ft_strchr(envp[i], '=');
		if (sep)
		{
			name = ft_substr(envp[i], 0, sep - envp[i]);
			value = ft_strdup(sep + 1);
			if (!head)
			{
				head = ms_env_new_node(name, value);
				tail = head;
			}
			else
			{
				tail->next = ms_env_new_node(name, value);
				tail = tail->next;
			}
			free(name);
			free(value);
		}
		i++;
	}
	return (head);
}

void	ms_env_free_list(t_env_var *env_list)
{
	t_env_var	*next;

	while (env_list)
	{
		next = env_list->next;
		free(env_list->name);
		free(env_list->value);
		free(env_list);
		env_list = next;
	}
}

char	*ms_env_get_value(t_env_var *env_list, char *name)
{
	size_t	len;

	len = ft_strlen(name);
	while (env_list)
	{
		if (ft_strncmp(env_list->name, name, len + 1) == 0)
			return (env_list->value);
		env_list = env_list->next;
	}
	return (NULL);
}

int	ms_env_set(t_env_var **env_list, char *name, char *value)
{
	t_env_var	*iter;
	t_env_var	*prev;

	iter = *env_list;
	prev = NULL;
	while (iter)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			free(iter->value);
			iter->value = ms_strdup_safe(value);
			return (0);
		}
		prev = iter;
		iter = iter->next;
	}
	if (!prev)
		*env_list = ms_env_new_node(name, value);
	else
		prev->next = ms_env_new_node(name, value);
	return (0);
}

int	ms_env_unset(t_env_var **env_list, char *name)
{
	t_env_var	*iter;
	t_env_var	*prev;

	iter = *env_list;
	prev = NULL;
	while (iter)
	{
		if (ft_strncmp(iter->name, name, ft_strlen(name) + 1) == 0)
		{
			if (prev)
				prev->next = iter->next;
			else
				*env_list = iter->next;
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

char	**ms_env_to_array(t_env_var *env_list)
{
	char	**arr;
	int		len;
	int		i;
	char	*tmp;

	len = 0;
	while (env_list)
	{
		len++;
		env_list = env_list->next;
	}
	arr = (char **)ms_xmalloc(sizeof(char *) * (len + 1));
	i = 0;
	while (i < len)
	{
		arr[i] = NULL;
		i++;
	}
	return (arr);
}
