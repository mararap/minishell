/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 19:49:00 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/14 19:49:03 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t			i;
	unsigned char	uc;

	uc = (unsigned char)c;
	i = 0;
	while (s[i])
	{
		if (s[i] == uc)
		{
			return ((char *)&s[i]);
		}
		i++;
	}
	if (s[i] == uc)
		return ((char *)&s[i]);
	return (0);
}
/*
#include <string.h>
#include <stdio.h>

int	main(void)
{
	const char	s[42] = "you got this!";
	int c = 'X';

	printf("my function: %s\n", ft_strchr(s, c));
	printf("original: %s\n", strchr(s, c));
	return(0);
}*/
