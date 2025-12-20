/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putendl_fd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 16:05:04 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/16 16:05:10 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putendl_fd(char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i])
	{
		write (fd, &s[i], 1);
		i++;
	}
	write (fd, "\n", 1);
}
/*
#include <fcntl.h>

int	main(void)
{
	char	s[42] = "testing ft_pudendl_fd once again";
//	int		fd = open("testing_fd_functions.c", O_RDWR);
	int	fd = 1;
	ft_putendl_fd(s, fd);
	return (0);
}*/
