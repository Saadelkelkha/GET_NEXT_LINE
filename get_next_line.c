/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:28 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/15 13:03:35 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line_helper(int fd)
{
	static char	leftover[BUFFER_SIZE + 1];
	char		*buf;
	ssize_t		ret;
	int			old;

	buf = malloc(BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	buf[0] = '\0';
	if (ft_strlen(leftover))
		ft_strlcpy(buf, leftover, BUFFER_SIZE + 1);
	old = ft_strlen(buf);
	ret = read(fd, buf + old, BUFFER_SIZE - ft_strlen(buf));
	if (ret >= 0)
		buf[old + ret] = '\0';
	if (!ft_strlen(buf) || ret == -1)
	{
		free(buf);
		return (NULL);
	}
	else
	{
		ft_copy_leftover_to_leftover(buf, leftover);
		return (buf);
	}
}

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	j;
	size_t	srclen;

	i = 0;
	while (i < size && dst[i] != '\0')
		i++;
	srclen = ft_strlen(src);
	if (i < size)
	{
		j = 0;
		while ((i + j + 1) < size && src[j] != '\0')
		{
			dst[i + j] = src[j];
			j++;
		}
		dst[i + j] = '\0';
		return (i + srclen);
	}
	return (size + srclen);
}

int	has_new_line(char *new_line)
{
	size_t	i;

	i = 0;
	while (new_line[i])
	{
		if (new_line[i] == '\n')
			return (1);
		i++;
	}
	return (0);
}

char	*get_next_line(int fd)
{
	char		*line;
	char		*older_line;
	char		*new_line;
	int			old;

	new_line = get_next_line_helper(fd);
	line = "\0";
	while (new_line)
	{
		old = ft_strlen(line);
		older_line = line;
		line = malloc(old + BUFFER_SIZE + 1);
		if (!line)
			return (NULL);
		line[0] = '\0';
		ft_strlcat(line, older_line, old + BUFFER_SIZE + 1);
		ft_strlcat(line, new_line, old + BUFFER_SIZE + 1);
		if (old)
			free(older_line);
		if (has_new_line(new_line))
		{
			free(new_line);
			break ;
		}
		free(new_line);
		new_line = get_next_line_helper(fd);
	}
	if (!ft_strlen(line))
		return (NULL);
	return (line);
}
