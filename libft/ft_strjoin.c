/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 13:59:56 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/25 18:04:56 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newstr;
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	if (!(s1 || s2))
		return (NULL);
	newstr = (char *)ft_calloc(ft_strlen(s1) + ft_strlen(s2) + 1, 1);
	if (newstr == NULL)
		return (NULL);
	while (s1[i])
	{
		newstr[i] = s1[i];
		i++;
	}
	while (s2[j])
	{
		newstr[i + j] = s2[j];
		j++;
	}
	return (newstr);
}
/*
#include <stdio.h>

int	main(void)
{
	const char *s1 = "testing";
	const char *s2 = " function";
	char * result = ft_strjoin(s1, s2);

	printf("newstr: %s\n", result);
	free(result);
	return (0);
}*/
