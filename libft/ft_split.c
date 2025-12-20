/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 09:40:52 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/28 20:33:54 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_free(char **arr, size_t ia)
{
	size_t	i;

	i = 0;
	while (i < ia)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

static size_t	ft_wcount(char const *s, char c)
{
	size_t	i;
	size_t	wcount;

	wcount = 0;
	i = 0;
	while (s[i])
	{
		if ((s[i] != c) && (s[i + 1] == c || s[i + 1] == '\0'))
			wcount++;
		i++;
	}
	return (wcount);
}

static char	*ft_fillsub(const char *s, size_t start, size_t end)
{
	size_t	i;
	char	*sub;

	i = 0;
	sub = ft_calloc((end - start + 1), sizeof(char));
	if (!sub)
		return (NULL);
	while (start < end)
	{
		sub[i] = s[start];
		start++;
		i++;
	}
	return (sub);
}

static char	**ft_arr(const char *s, char c, char **arr, size_t wcount)
{
	size_t	i;
	size_t	ia;
	size_t	start;

	i = 0;
	ia = 0;
	while (s[i] && ia < wcount)
	{
		while (s[i] == c && s[i] != '\0')
			i++;
		start = i;
		while (s[i] != c && s[i] != '\0')
			i++;
		arr[ia] = ft_fillsub(s, start, i);
		if (!arr[ia])
			return (ft_free(arr, ia), NULL);
		ia++;
	}
	return (arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	size_t	wcount;

	if (!s)
		return (NULL);
	wcount = ft_wcount(s, c);
	arr = (char **)malloc((wcount + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	arr = ft_arr(s, c, arr, wcount);
	if (!arr)
		return (NULL);
	arr[wcount] = NULL;
	return (arr);
}
/*
#include <stdio.h>

int	main(void)
{
	char **arr = ft_split("   tripuille 42 ", ' ');
	if (!arr)
		return (-1);
	int i = 0;

	while (arr[i])
	{
		printf("arr[%d] = %s\n", i, arr[i]);
		free (arr[i]);
		i++;
	}
	free (arr);
	return (0);
}*/
