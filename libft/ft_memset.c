/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 11:24:51 by marapovi          #+#    #+#             */
/*   Updated: 2025/11/24 23:11:40 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*str;
	size_t			i;

	i = 0;
	str = (unsigned char *) s;
	while (i < n)
	{
		str[i] = c;
		i++;
	}
	return (s);
}
/*
#include <string.h>
#include <stdio.h>
#include <strings.h>

int	main(void)
{
	char s[42] = "lets goooooooo";
	int		c = 'u';
	size_t	n = 8;
	
	char	so[42] = "lets goooooooo";
	int		co = 'u';
	size_t	no = 8;

	printf("mine: %p\n", ft_memset(s, c, n));
	printf("original: %p\n", memset(so, co, no));
	return (0);
}*/
