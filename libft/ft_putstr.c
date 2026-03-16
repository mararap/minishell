/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 12:19:19 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/16 09:24:33 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/* if (!s), return (0) for minishell output stability. otherwise "(null)" */
int	ft_putstr(char *s)
{
	size_t	len;

	if (!s)
		return (0);
	len = ft_strlen(s);
	return (write(1, s, len));
}
