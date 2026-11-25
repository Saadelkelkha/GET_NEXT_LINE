/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:37 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/25 10:52:49 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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
	char	*temp;

	if (ft_strlen(buf) > ft_strlen_until_terminator(buf))
	{
		temp = leftover;
		leftover = malloc(ft_strlen(buf) - ft_strlen_until_terminator(buf));
		if (!leftover)
		{
			free(temp);
			return ;
		}
		leftover[0] = '\0';
		ft_strlcpy(leftover, temp, ft_strlen(temp) + 1);
		ft_strlcpy(leftover + ft_strlen(temp) + 1,
			buf + ft_strlen_until_terminator(buf) + 1, (size_t)BUFFER_SIZE + 1);
		buf[ft_strlen_until_terminator(buf) + 1] = '\0';
	}
	else
		leftover[0] = '\0';
}
