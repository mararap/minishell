/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 10:54:26 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/19 10:54:29 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_intsize(int n)
{
	size_t	size;
	long	ln;

	ln = n;
	if (n <= 0)
		size = 1;
	else
		size = 0;
	if (ln < 0)
		ln = ln * -1;
	while (ln > 0)
	{
		ln = ln / 10;
		size++;
	}
	return (size);
}

char	*ft_itoa(int n)
{
	char	*newstr;
	long	ln;
	size_t	size;

	ln = n;
	size = ft_intsize(n);
	if (n < 0)
		ln = ln * -1;
	newstr = ft_calloc(size + 1, sizeof(char));
	if (!newstr)
		return (NULL);
	while (size > 0)
	{
		size--;
		newstr[size] = ((ln % 10) + 48);
		ln = (ln / 10);
		if (size == 0 && n < 0)
			newstr[0] = '-';
	}
	return (newstr);
}
/*
#include <stdio.h>

int	main(void)
{
	int	n = -42;
	char * result = ft_itoa(n);

	printf("newstr: %s\n", result);
	free(result);
	return (0);
}*/
