# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/19 16:42:16 by marapovi          #+#    #+#              #
#    Updated: 2025/12/20 21:18:45 by marapovi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #



NAME		:=	minishell
CC			:=	cc

CPPFLAGS	:=	-I include -I libft

CFLAGS		:=	-Wall -Wextra -Werror

LDFLAGS		:=	-L libft

LDLIBS		:=	-lft

RM			:=	rm -rf

