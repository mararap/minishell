/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/28 17:10:30 by marapovi          #+#    #+#             */
/*   Updated: 2025/10/11 21:37:46 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	print_format(char type_specifier, va_list argp)
{
	if (type_specifier == 'c')
		return (ft_putchar(va_arg(argp, int)));
	if (type_specifier == 's')
		return (ft_putstr(va_arg(argp, char *)));
	if (type_specifier == 'p')
		return (ft_putptr(va_arg(argp, void *)));
	if (type_specifier == 'd' || type_specifier == 'i')
		return (ft_putint(va_arg(argp, int)));
	if (type_specifier == 'u')
		return (ft_putusint(va_arg(argp, unsigned int)));
	if (type_specifier == 'x' || type_specifier == 'X')
		return (ft_puthex(va_arg(argp, unsigned int), type_specifier));
	if (type_specifier == '%')
		return (write(1, "%", 1));
	return (write(1, "%", 1) + write(1, &type_specifier, 1));
}

static int	ft_printf_step(const char *format, size_t *i, va_list argp)
{
	if (format[*i] == '%' && !format[*i + 1])
		return (-1);
	if (format[*i] == '%')
		return (print_format(format[++(*i)], argp));
	return (write(1, &format[*i], 1));
}

int	ft_printf(const char *format, ...)
{
	va_list	argp;
	int		count;
	int		amount;
	size_t	i;

	if (!format)
		return (-1);
	va_start(argp, format);
	count = 0;
	i = 0;
	while (format[i])
	{
		amount = ft_printf_step(format, &i, argp);
		if (amount < 0)
			return (va_end(argp), -1);
		count += amount;
		i++;
	}
	return (va_end(argp), count);
}
