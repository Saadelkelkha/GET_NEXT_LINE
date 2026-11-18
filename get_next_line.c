/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:28 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/18 11:03:00 by sel-kelk         ###   ########.fr       */
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
	old = ft_strlcpy(buf, leftover, BUFFER_SIZE + 1);
	ret = read(fd, buf + old, BUFFER_SIZE - ft_strlen(buf));
	if (ret >= 0)
		buf[old + ret] = '\0';
	if (!ft_strlen(buf) || ret == -1)
	{
		free(buf);
		return (NULL);
	}
	ft_copy_leftover_to_leftover(buf, leftover);
	return (buf);
}

int	its_line(int old, char *older_line, char *new_line)
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
		ft_strlcpy(line, older_line, old + BUFFER_SIZE + 1);
		ft_strlcpy(line + ft_strlen(line), new_line, old + BUFFER_SIZE + 1);
		if (its_line(old, older_line, new_line))
			break ;
		free(new_line);
		new_line = get_next_line_helper(fd);
	}
	if (!ft_strlen(line))
		return (NULL);
	return (line);
}
