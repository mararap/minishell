/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isalpha.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 12:32:31 by marapovi          #+#    #+#             */
/*   Updated: 2025/04/23 12:33:21 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isalpha(int i)
{
	unsigned char	c;

	c = (unsigned char)i;
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}
/*
#include <stdio.h>
#include <ctype.h>

int	main(void)
{
	printf("%d\n", ft_isalpha('M'));
	printf("%d\n", isalpha('M'));
	return (0);
}*/
