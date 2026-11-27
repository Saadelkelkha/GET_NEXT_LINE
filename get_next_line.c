/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/26 10:19:56 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/26 10:19:58 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

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
	static char	*s;
	char		*b;
	char		*tmp;
	size_t		n;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	b = malloc((size_t)BUFFER_SIZE + 1);
	if (!b)
		return (free(s), s = NULL, NULL);
	n = 1;
	while (!ft_strchr(s, '\n') && n > 0)
	{
		n = read(fd, b, BUFFER_SIZE);
		if (n < 0)
			return (free(b), free(s), s = NULL, NULL);
		b[n] = '\0';
		tmp = ft_strjoin(s, b);
		free(s);
		s = tmp;
		if (!s)
			return (free(b), NULL);
	}
	return (get_next_line_helper(b, &s));
}
