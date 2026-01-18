/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 14:50:21 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/17 14:50:28 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	j;
	char	*newstr;

	j = 0;
	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	if (len > ft_strlen(s) - start)
		(len = ft_strlen(s) - start);
	newstr = (char *)ft_calloc(len + 1, 1);
	if (newstr == NULL)
		return (NULL);
	i = start;
	while (s[i] && j < len)
	{
		newstr[j] = s[i];
		i++;
		j++;
	}
	newstr[j] = '\0';
	return (newstr);
}
/*
#include <stdio.h>

int	main(void)
{
	char const s[42] = "testing my function";
	unsigned int start = 9;
	size_t len = 3;
	char	*result;

	result = (char *)ft_substr(s, start, len);
	printf("newstr = %s\n", result);
	free(result);
	return (0);
}*/
