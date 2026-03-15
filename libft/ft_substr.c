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

static size_t	ft_substr_len(char const *s, unsigned int start, size_t len)
{
	size_t	available;

	available = ft_strlen(s) - start;
	if (len > available)
		return (available);
	return (len);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*newstr;
	size_t	real_len;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	real_len = ft_substr_len(s, start, len);
	newstr = (char *)ft_calloc(real_len + 1, 1);
	if (!newstr)
		return (NULL);
	ft_strlcpy(newstr, s + start, real_len + 1);
	return (newstr);
}
