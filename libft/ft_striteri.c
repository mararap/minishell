/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/17 19:49:49 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/17 19:49:52 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_striteri(char *s, void (*f)(unsigned int, char*))
{
	size_t	i;

	i = 0;
	if (!s[i])
		return ;
	while (s[i])
	{
		(*f)(i, &s[i]);
		i++;
	}
}
/*
#include <stdio.h>

void	to_upper_with_index(unsigned int i, char *c)
{
	if ((i % 2 == 0) && *c >= 'a' && *c <= 'z')
		*c = *c - 32;
}

int	main(void)
{
	char	str[] = "hello world";

	ft_striteri(str, to_upper_with_index);

	printf("modified string: %s\n", str); 
	return (0);
}*/
