/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/15 11:28:39 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/15 11:28:45 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	uc;
	unsigned char	*ptr;
	size_t			i;

	uc = (unsigned char)c;
	ptr = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		if (ptr[i] == uc)
			return ((void *)(s + i));
		i++;
	}
	return (NULL);
}
