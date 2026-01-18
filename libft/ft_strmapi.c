/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 18:01:43 by marapovi          #+#    #+#             */
/*   Updated: 2025/11/25 17:54:28 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	i;
	char	*newstr;

	i = 0;
	if (!s)
		return (NULL);
	newstr = ft_calloc(ft_strlen(s) + 1, 1);
	if (!newstr)
		return (NULL);
	while (s[i])
	{
		newstr[i] = f(i, s[i]);
		i++;
	}
	if (!newstr)
		return (NULL);
	return (newstr);
}
/*
#include <stdio.h>

char	ft_ttoupper(unsigned int i, char c)
{
	(void) i;
	if (c >= 'a' && c <= 'z')
		return (c - ('a' - 'A'));
	else
		return (c);
}

int	main(void)
{
	char *str = "Hallo";
	char *out;
	out = ft_strmapi(str, ttoupper);
	printf("out: %s\n", out);
	free(out);
	return (0);
}*/
