/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 15:26:23 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/16 15:26:27 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putstr_fd(char *s, int fd)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		write (fd, &s[i], 1);
		i++;
	}
}
/*
#include <fcntl.h>

int	main(void)
{
	char	s[42] = "testing my function";
	int		fd = open("testing_fd_functions.c", O_RDWR);
	ft_putstr_fd(s, fd);
	return (0);
}*/
