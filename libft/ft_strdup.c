/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 17:32:01 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/16 17:32:05 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*s2;
	int		i;

	i = 0;
	if (!s)
		return (NULL);
	s2 = (char *)ft_calloc(ft_strlen(s) + 1, 1);
	if (s2 == NULL)
		return (NULL);
	while (s[i])
	{
		s2[i] = s[i];
		i++;
	}
	s2[i] = '\0';
	return (s2);
}
/*
#include <stdio.h>

int	main(void)
{
	const char s[42] = "testing my function";
	char *result = ft_strdup(s);

	printf("my function: %p\n", result);
	free(result);
	return (0);
}*/
