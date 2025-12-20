/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 13:44:28 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/13 13:44:34 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_bzero(void *s, size_t n)
{
	ft_memset(s, '\0', n);
}
/*
#include <stdio.h>
#include <strings.h>
#include <string.h>

int	main(void)
{
	char	s[42] = "test me if you can";
	char	so[42] = "test me if you can";

	ft_bzero(s, (0));
	bzero(so, (0));

	if (memcmp(s, so, 42) == 0)
		printf("OK - ft_bzero == bezero\n");
	else
		printf("FAIL - ft_bzero != bezero\n");
	return(0);
}*/
