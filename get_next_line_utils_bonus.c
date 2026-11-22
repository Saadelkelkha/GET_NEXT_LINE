/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils_bonus.c                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:37 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/22 09:41:43 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

ssize_t	ft_strlen(const char *s)
{
	size_t	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

ssize_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size > 0)
	{
		while (src[i] && i < size - 1)
		{
			dst[i] = src[i];
			i++;
		}
		dst[i] = '\0';
	}
	return (ft_strlen(src));
}

ssize_t	ft_strlen_until_terminator(char *buf)
{
	size_t	i;

	i = 0;
	while (buf[i] && buf[i] != '\n')
		i++;
	return (i);
}

void	ft_copy_leftover_to_leftover(char *buf, char *leftover)
{
	if (ft_strlen(buf) > ft_strlen_until_terminator(buf))
	{
		ft_strlcpy(leftover, buf + ft_strlen_until_terminator(buf) + 1,
			(size_t)BUFFER_SIZE + 1);
		buf[ft_strlen_until_terminator(buf) + 1] = '\0';
	}
	else
		leftover[0] = '\0';
}

int	its_line_or_free_lines(int old, char *older_line, char *new_line, int swap)
{
	if (swap == 2)
	{
		if (old)
			free(older_line);
		if (ft_strlen(new_line) > ft_strlen_until_terminator(new_line))
		{
			free(new_line);
			return (1);
		}
		return (0);
	}
	else
	{
		if (old)
			free(older_line);
		free(new_line);
		return (2);
	}
}
