/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_path.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jatanaso <jatanaso@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/24 15:07:45 by jatanaso          #+#    #+#             */
/*   Updated: 2026/03/24 15:08:23 by jatanaso         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	ms_path_parts_count(const char *s)
{
    size_t	i;
    size_t	count;

    if (!s || s[0] == '\0')
        return (0);
    count = 1;
    i = 0;
    while (s[i])
    {
        if (s[i] == ':')
            count++;
        i++;
    }
    return (count);
}

char	**ms_split_path_keep_empty(const char *s)
{
    char	**out;
    size_t	i;
    size_t	start;
    size_t	j;
    size_t	n;

    n = ms_path_parts_count(s);
    out = (char **)ms_xmalloc(sizeof(char *) * (n + 1));
    i = 0;
    start = 0;
    j = 0;
    while (1)
    {
        if (s[i] == ':' || s[i] == '\0')
        {
            out[j++] = ft_substr(s, start, i - start);
            if (s[i] == '\0')
                break ;
            start = i + 1;
        }
        i++;
    }
    out[j] = NULL;
    return (out);
}