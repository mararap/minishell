#include "minishell.h"

static t_command	*ms_command_new(void)
{
	t_command	*cmd;

	cmd = (t_command *)ms_xmalloc(sizeof(t_command));
	cmd->argv = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	ms_redir_add_back(t_redir **list, t_redir *new_redir)
{
	t_redir	*iter;

	if (!*list)
	{
		*list = new_redir;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_redir;
}

static void	ms_command_add_back(t_command **list, t_command *new_cmd)
{
	t_command	*iter;

	if (!*list)
	{
		*list = new_cmd;
		return ;
	}
	iter = *list;
	while (iter->next)
		iter = iter->next;
	iter->next = new_cmd;
}

static char	**ms_add_word_to_argv(char **argv, char *word)
{
	size_t	len;
	char	**new_argv;
	size_t	i;

	len = ms_str_arr_len(argv);
	new_argv = (char **)ms_xmalloc(sizeof(char *) * (len + 2));
	i = 0;
	while (i < len)
	{
		new_argv[i] = argv[i];
		i++;
	}
	new_argv[len] = word;
	new_argv[len + 1] = NULL;
	free(argv);
	return (new_argv);
}

static t_redir	*ms_create_redir(int type, char *target)
{
	t_redir	*redir;

	redir = (t_redir *)ms_xmalloc(sizeof(t_redir));
	redir->type = type;
	redir->target = target;
	redir->next = NULL;
	return (redir);
}

static int	ms_fill_command(t_command *cmd, t_token **cursor)
{
	t_token	*tok;
	int		type;

	while (*cursor && (*cursor)->type != TOKEN_PIPE)
	{
		tok = *cursor;
		if (tok->type == TOKEN_WORD)
		{
			cmd->argv = ms_add_word_to_argv(cmd->argv, tok->value);
			tok->value = NULL;
			*cursor = tok->next;
		}
		else
		{
			type = tok->type;
			*cursor = tok->next;
			if (!*cursor || (*cursor)->type != TOKEN_WORD)
			{
				write(STDERR_FILENO, SHELL_NAME ": syntax error\n", 26);
				return (-1);
			}
			ms_redir_add_back(&cmd->redirections,
				ms_create_redir(type, (*cursor)->value));
			(*cursor)->value = NULL;
			*cursor = (*cursor)->next;
		}
	}
	return (0);
}

t_command	*ms_parse_tokens(t_token *token_list)
{
	t_command	*cmds;
	t_command	*cmd;
	t_token		*cursor;

	cmds = NULL;
	cursor = token_list;
	while (cursor)
	{
		if (cursor->type == TOKEN_PIPE)
		{
			write(STDERR_FILENO, SHELL_NAME ": syntax error near '|'\n", 38);
			return (NULL);
		}
		cmd = ms_command_new();
		if (ms_fill_command(cmd, &cursor) < 0)
			return (NULL);
		ms_command_add_back(&cmds, cmd);
		if (cursor && cursor->type == TOKEN_PIPE)
			cursor = cursor->next;
	}
	return (cmds);
}

void	ms_free_command_list(t_command *command_list)
{
	t_command	*next_cmd;
	t_redir		*redir;
	t_redir		*next_redir;
	size_t		i;

	while (command_list)
	{
		next_cmd = command_list->next;
		if (command_list->argv)
		{
			i = 0;
			while (command_list->argv[i])
			{
				free(command_list->argv[i]);
				i++;
			}
			free(command_list->argv);
		}
		redir = command_list->redirections;
		while (redir)
		{
			next_redir = redir->next;
			free(redir->target);
			free(redir);
			redir = next_redir;
		}
		free(command_list);
		command_list = next_cmd;
	}
}
