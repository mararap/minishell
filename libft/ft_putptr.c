/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/10 18:36:56 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/11 21:39:54 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_adrlen(unsigned long long nbr)
{
	int	count;

	count = 1;
	while (nbr >= 16)
	{
		nbr /= 16;
		count++;
	}
	return (count);
}

static void	ft_ptrputhex(unsigned long long nbr)
{
	char	*hex;

	hex = "0123456789abcdef";
	if (nbr >= 16)
		ft_ptrputhex(nbr / 16);
	ft_putchar(hex[nbr % 16]);
}

int	ft_putptr(void *ptr)
{
	unsigned long long	adr;
	int					count;

	if (ptr == 0)
		return (write(1, "(nil)", 5));
	adr = (unsigned long long)ptr;
	write (1, "0x", 2);
	ft_ptrputhex(adr);
	count = ft_adrlen(adr) + 2;
	return (count);
}
