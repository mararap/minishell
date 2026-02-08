/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 13:20:13 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/02 13:20:19 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcpy(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	slen;

	i = 0;
	slen = ft_strlen(src);
	if (size == 0)
		return (ft_strlen(src));
	while (i < (size - 1) && src[i])
	{
		(dest[i]) = (src[i]);
		i++;
	}
	dest[i] = '\0';
	return (slen);
}
