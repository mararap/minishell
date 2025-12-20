/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putchar_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 15:06:09 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/16 15:06:13 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
/*
#include <fcntl.h>

int	main(void)
{
	char c = 'M';
	int fd = open("ft_memcmp.c", O_RDWR);
	ft_putchar_fd(c, fd);
	return (0);
}*/
