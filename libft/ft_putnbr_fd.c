/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:30:52 by marapovi          #+#    #+#             */
/*   Updated: 2025/11/15 16:28:54 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

// original function from circle 0

void	ft_putnbr_fd(int n, int fd)
{
	long int	nbr;

	nbr = (long int) n;
	if (nbr < 0)
	{
		ft_putchar_fd('-', fd);
		nbr = nbr * -1;
	}
	if (nbr <= 9)
		ft_putchar_fd(nbr + 48, fd);
	if (nbr > 9)
	{
		ft_putnbr_fd(nbr / 10, fd);
		ft_putchar_fd((nbr % 10) + 48, fd);
	}
}
/*
int	main(void)
{
	int	n = -2025;
	int	fd = 1;
	ft_putnbr_fd(n, fd);
}*/
