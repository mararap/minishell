/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_isascii.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 10:24:19 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/05 13:38:01 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_isascii(int i)
{
	if (i >= 0 && i <= 127)
		return (1);
	return (0);
}
/*

#include <stdio.h>
#include <ctype.h>

int	main(void)
{
	printf("%d\n", ft_isascii(120));
	printf("%d\n", isascii(120));
	return (0);
}*/
