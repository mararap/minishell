/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 12:58:32 by marapovi          #+#    #+#             */
/*   Updated: 2025/04/24 12:58:37 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlen(const char *str)
{
	size_t	i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}
/*
#include <stdio.h>
#include <bsd/string.h>

int	main(void)
{
	char	str[] = "asdfjklö";
	
	printf("Length of \"%s\" is: %zu\n", str, strlen(str));
	printf("Length of \"%s\" is: %zu\n", str, ft_strlen(str));
	return (0);
}*/
