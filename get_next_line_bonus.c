/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:28 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/22 09:42:50 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*get_leftover(t_list **head, int fd)
{
	t_list	*node;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	node = *head;
	while (node && node->fd != fd)
		node = node->next;
	if (node)
		return (node->leftover);
	node = malloc(sizeof(t_list));
	if (!node)
		return (NULL);
	node->fd = fd;
	node->leftover = malloc(BUFFER_SIZE);
	if (!node->leftover)
	{
		free(node);
		return (NULL);
	}
	node->leftover[0] = '\0';
	node->next = *head;
	*head = node;
	return (node->leftover);
}

void	ft_lstdelone_helper(t_list **head, t_list *prev, t_list *node)
{
	t_list	*next;

	free(node->leftover);
	next = node->next;
	free(node);
	node = next;
	if (prev)
		prev->next = next;
	else
		*head = next;
}

void	ft_lstdelone(t_list **head, int fd)
{
	t_list	*node;
	t_list	*prev;

	if (head)
	{
		node = *head;
		prev = NULL;
		while (node)
		{
			if (node->fd == fd)
			{
				ft_lstdelone_helper(head, prev, node);
				break ;
			}
			else
			{
				prev = node;
				node = node->next;
			}
		}
	}
}

char	*get_next_line_helper(int fd)
{
	static t_list	*leftover_list;
	char			*leftover;
	char			*buf;
	ssize_t			ret;
	size_t			old;

	leftover = get_leftover(&leftover_list, fd);
	if (!leftover)
		return (NULL);
	buf = malloc((size_t)BUFFER_SIZE + 1);
	if (!buf)
		return (NULL);
	buf[0] = '\0';
	old = ft_strlcpy(buf, leftover, (size_t)BUFFER_SIZE + 1);
	ret = read(fd, buf + old, BUFFER_SIZE - old);
	if (ret >= 0)
		buf[old + ret] = '\0';
	if (!ft_strlen(buf) || ret == -1)
	{
		ft_lstdelone(&leftover_list, fd);
		free(buf);
		return (NULL);
	}
	ft_copy_leftover_to_leftover(buf, leftover);
	return (buf);
}

char	*get_next_line(int fd)
{
	char		*line;
	char		*older_line;
	char		*new_line;
	size_t		old;

	new_line = get_next_line_helper(fd);
	line = "\0";
	while (new_line)
	{
		old = ft_strlen(line);
		older_line = line;
		line = malloc(old + BUFFER_SIZE + 1);
		if (!line && its_line_or_free_lines(old, older_line, new_line, 1) == 2)
			return (NULL);
		line[0] = '\0';
		ft_strlcpy(line, older_line, old + BUFFER_SIZE + 1);
		ft_strlcpy(line + ft_strlen(line), new_line, old + BUFFER_SIZE + 1);
		if (its_line_or_free_lines(old, older_line, new_line, 2))
			break ;
		free(new_line);
		new_line = get_next_line_helper(fd);
	}
	if (!ft_strlen(line))
		return (NULL);
	return (line);
}
