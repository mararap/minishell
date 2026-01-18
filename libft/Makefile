# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: marapovi <marapovi@student.42vienna.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/25 12:39:13 by marapovi          #+#    #+#              #
#    Updated: 2025/11/25 17:51:38 by marapovi         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

HEADER	=	libft.h

CFILES	=	ft_isalpha.c\
			ft_isalnum.c\
			ft_isascii.c\
			ft_isdigit.c\
			ft_isprint.c\
			ft_strlen.c\
			ft_strlcpy.c\
			ft_strlcat.c\
			ft_strncmp.c\
			ft_memset.c\
			ft_bzero.c\
			ft_toupper.c\
			ft_tolower.c\
			ft_strnstr.c\
			ft_strchr.c\
			ft_strrchr.c\
			ft_memchr.c\
			ft_memcmp.c\
			ft_memcpy.c\
			ft_memmove.c\
			ft_calloc.c\
			ft_putchar_fd.c\
			ft_putstr_fd.c\
			ft_putendl_fd.c\
			ft_putnbr_fd.c\
			ft_strdup.c\
			ft_atoi.c\
			ft_substr.c\
			ft_strmapi.c\
			ft_striteri.c\
			ft_strjoin.c\
			ft_strtrim.c\
			ft_itoa.c\
			ft_split.c\
			ft_error_exit.c\
			ft_printf.c\
			ft_putchar.c\
			ft_putstr.c\
			ft_putint.c\
			ft_putusint.c\
			ft_puthex.c\
			ft_putptr.c

OFILES	=	$(CFILES:.c=.o)

CC	=	cc

CFLAGS	=	-Wall -Wextra -Werror

NAME	:=	libft.a

%.o: %.c $(HEADER)
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OFILES)
	@ar rcs $(NAME) $(OFILES)
		@echo "     ✅ LIBFT executable successfully created."

clean:
	@rm -f $(OFILES)

fclean: clean
	@rm -f $(NAME)

re: fclean $(NAME)

.PHONY: all clean fclean re
