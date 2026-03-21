/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_parse.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 21:30:00 by marapovi          #+#    #+#             */
/*   Updated: 2026/03/20 21:30:00 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*ms_skip_spaces(const char *s)
{
	while (*s && ft_isspace((unsigned char)*s))
		s++;
	return (s);
}

static int	ms_parse_ull(const char **s, unsigned long long *value,
		unsigned long long limit)
{
	int	digit;

	*value = 0;
	if (!ft_isdigit((unsigned char)**s))
		return (0);
	while (ft_isdigit((unsigned char)**s))
	{
		digit = **s - '0';
		if (*value > limit / 10 || (*value == limit / 10
				&& (unsigned long long)digit > limit % 10))
			return (0);
		*value = (*value * 10) + digit;
		(*s)++;
	}
	return (1);
}

int	ms_atoll_strict(const char *s, long long *out)
{
	unsigned long long	value;
	unsigned long long	limit;
	int					sign;

	s = ms_skip_spaces(s);
	sign = 1;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	limit = (unsigned long long)LLONG_MAX + (sign < 0);
	if (!ms_parse_ull(&s, &value, limit))
		return (0);
	s = ms_skip_spaces(s);
	if (*s != '\0')
		return (0);
	if (sign < 0 && value == (unsigned long long)LLONG_MAX + 1)
		*out = LLONG_MIN;
	else
		*out = (long long)value * sign;
	return (1);
}
