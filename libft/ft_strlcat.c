/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 10:46:53 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/07 10:47:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	j;
	size_t	dl;
	size_t	sl;

	i = 0;
	j = 0;
	sl = ft_strlen(src);
	dl = ft_strlen(dest);
	if (size <= dl)
		return (sl + size);
	while (dest[j] != '\0')
		j++;
	while (src[i] && j < (size - 1))
	{
		dest[j] = src[i];
		i++;
		j++;
	}
	dest[j] = '\0';
	return (dl + sl);
}
/*
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>

int main(void)
{
	char	desto[30]; memset(desto, 0, 30); memset(desto, 'B', 4);
	char	*srco = "AAAAAAAAA";
	size_t	sizeo = 6;
	
	char	destm[30]; memset(destm, 0, 30); memset(destm, 'B', 4);

	char	*srcm = "AAAAAAAAA";
	size_t	sizem = 6;

	printf("original: %zu\n", strlcat(desto, srco, sizeo));
	printf("original: %s\n", desto);
	printf("mine: %zu\n", ft_strlcat(destm, srcm, sizem));
	printf("mine: %s\n", destm);
	return (0);
}*/
