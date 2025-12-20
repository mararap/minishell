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
	int	amount;

	amount = 0;
	if (type_specifier == 'c')
		amount += ft_putchar(va_arg(argp, int));
	else if (type_specifier == 's')
		amount += ft_putstr(va_arg(argp, char *));
	else if (type_specifier == 'p')
		amount += ft_putptr(va_arg(argp, void *));
	else if (type_specifier == 'd' || type_specifier == 'i')
		amount += ft_putint(va_arg(argp, int));
	else if (type_specifier == 'u')
		amount += ft_putusint(va_arg(argp, unsigned int));
	else if (type_specifier == 'x')
		amount += ft_puthex(va_arg(argp, unsigned int), type_specifier);
	else if (type_specifier == 'X')
		amount += ft_puthex(va_arg(argp, unsigned int), type_specifier);
	else if (type_specifier == '%')
		amount += write(1, "%", 1);
	else
		amount = (write(1, "%", 1) + write(1, &type_specifier, 1));
	return (amount);
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
	amount = 0;
	i = 0;
	while (format[i] != '\0')
	{
		if (format[i] == '%' && format[i + 1])
		{
			amount = print_format(format[++i], argp);
			count += amount;
		}
		else if (format[i] == '%' && !format[i + 1])
			return (va_end(argp), -1);
		else
			count += write(1, &format[i], 1);
		i++;
	}
	return (va_end(argp), count);
}
