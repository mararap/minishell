/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 13:30:10 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/14 13:30:15 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while ((s1[i] || s2[i]) && i < n)
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
/*
#include <string.h>
#include <stdio.h>

int	main(void)
{
	//const char s1[42] = "if you're happy and you know it...";
	//const char s2[42] = "if you know it and you're happy...";
	//size_t n = 15;
	
	//printf("mine: %d\n", ft_strncmp("test\200", "test\0", 6));
	printf("Myversion: %d\n", ft_strncmp("hallo", "abcdwxyz", 4));
	printf("original: %d\n", strncmp("hallo", "abcdwxyz", 4));
}*/
