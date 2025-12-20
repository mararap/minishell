/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 15:06:02 by marapovi          #+#    #+#             */
/*   Updated: 2025/11/15 16:28:37 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char	*d;
	unsigned char	*s;

	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	if (d > s)
	{
		while (len != 0)
		{
			len--;
			d[len] = s[len];
		}
		return (d);
	}
	else
	{
		ft_memcpy(dst, src, len);
		return (d);
	}
}
/*
#include <stdio.h>
#include <string.h>

int	main(void)
{
	char	dst[42] = "does it work?";
	char	dsto[42] = "does it work?";
	char	src[] = "yeah it does!";
	char	srco[] = "yeah it does!";
	size_t	len = 14;

	printf("my function: %s\n", (char *)ft_memmove(dest, src, len));
	printf("original function: %s\n", (char *)memmove(desto, srco, len));
}*/
