/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isprint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 12:50:43 by marapovi          #+#    #+#             */
/*   Updated: 2025/04/24 12:50:52 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isprint(int i)
{
	unsigned char	c;

	c = (unsigned char)i;
	if (c >= 32 && c <= 126)
		return (1);
	return (0);
}
/*
#include <ctype.h>
#include <stdio.h>

int	main(void)
{
	unsigned char c;
	c = 50;
	printf("%d\n", ft_isprint(c));
	printf("%d\n", isprint(c));
	return (0);
}*/
