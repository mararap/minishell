/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isdigit.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 16:51:55 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/24 21:28:27 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isdigit(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}
/*
#include <ctype.h>
#include <stdio.h>

int	main(void)
{
 	printf("my function: %d\n", ft_isdigit(-2147483649));
	printf("original function: %d\n", isdigit(-2147483649));
	return (0);
}*/
