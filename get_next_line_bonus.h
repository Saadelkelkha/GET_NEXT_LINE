/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.h                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:46 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/16 11:19:37 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

typedef struct s_list{
	char	*leftover;
	int		fd;
	struct s_list	*next;
}	t_list;

ssize_t	ft_strlen(const char *s);
ssize_t	ft_strlcpy(char *dst, const char *src, size_t size);
ssize_t	ft_strlen_until_terminator(char *buf);
void	ft_copy_leftover_to_leftover(char *buf, char *leftover);
char	*get_next_line(int fd);

#endif
