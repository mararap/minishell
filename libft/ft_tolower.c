/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tolower.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 13:12:58 by marapovi          #+#    #+#             */
/*   Updated: 2025/05/14 13:13:02 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_tolower(int c)
{
	unsigned char	uc;

	uc = (unsigned char)c;
	if (uc >= 'A' && uc <= 'Z')
		return (c + ('a' - 'A'));
	else
		return (c);
}
