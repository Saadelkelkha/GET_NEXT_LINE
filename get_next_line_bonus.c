/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/26 10:19:43 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/26 10:19:49 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

static char	*next(char *s)
{
	char	*p;
	int		i;

	if (!s)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (!s[i])
	{
		free(s);
		return (NULL);
	}
	p = ft_strdup(s + i + 1);
	free(s);
	return (p);
}

static char	*line(char *s)
{
	char	*p;
	size_t	i;

	if (!s || !*s)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (s[i] == '\n')
		p = malloc(i + 1 + 1);
	else
		p = malloc(i + 1);
	if (!p)
		return (NULL);
	i = 0;
	while (s[i] != '\n' && s[i])
	{
		p[i] = s[i];
		i++;
	}
	if (s[i] == '\n')
		p[i++] = '\n';
	p[i] = '\0';
	return (p);
}

static char	*get_next_line_helper(char *b, char **s)
{
	char	*l;

	free(b);
	l = line(*s);
	if (!l)
		return (free(*s), *s = NULL, NULL);
	*s = next(*s);
	return (l);
}

char	*get_next_line(int fd)
{
	static char	*s[MAX_FD];
	char		*b;
	char		*tmp;
	size_t		n;

	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	b = malloc((size_t)BUFFER_SIZE + 1);
	if (!b)
		return (free(s[fd]), s[fd] = NULL, NULL);
	n = 1;
	while (!ft_strchr(s[fd], '\n') && n > 0)
	{
		n = read(fd, b, BUFFER_SIZE);
		if (n < 0)
			return (free(b), free(s[fd]), s[fd] = NULL, NULL);
		b[n] = '\0';
		tmp = ft_strjoin(s[fd], b);
		free(s[fd]);
		s[fd] = tmp;
		if (!(s[fd]))
			return (free(b), NULL);
	}
	return (get_next_line_helper(b, &(s[fd])));
}
