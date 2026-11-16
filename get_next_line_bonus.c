/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <sel-kelk@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/14 10:36:28 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/16 11:20:08 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

t_list	*ft_lstnew(int fd)
{
	t_list	*lst;

	lst = malloc(sizeof(t_list));
	if (!lst)
		return (NULL);
	lst->fd = fd;
	lst->leftover = malloc(BUFFER_SIZE + 1);
	if (!lst->leftover)
		return (NULL);
	lst->leftover[0] = '\0';
	lst->next = NULL;
	return (lst);
}

char	*get_leftover(t_list **leftover_list, int fd)
{
	t_list	*leftover_list_c;
	char	*ret_leftover;
	int	i;

	i = 0;
	leftover_list_c = *leftover_list;
	if (*leftover_list)
	{
		while ((*leftover_list)->next)
		{
			if ((*leftover_list)->fd == fd)
			{
				ret_leftover = (*leftover_list)->leftover;
				*leftover_list = leftover_list_c;
				return (ret_leftover);
			}
			*leftover_list = (*leftover_list)->next;
			i++;
		}
		if ((*leftover_list)->fd == fd)
		{
			ret_leftover = (*leftover_list)->leftover;
			*leftover_list = leftover_list_c;
			return (ret_leftover);
		}
		(*leftover_list)->next = ft_lstnew(fd);
		ret_leftover = (*leftover_list)->next->leftover;
		*leftover_list = leftover_list_c;
		return (ret_leftover);
	}
	*leftover_list = ft_lstnew(fd);
	ret_leftover = (*leftover_list)->leftover;
	return (ret_leftover);
}
void	ft_lstdelone(t_list **lst, int fd)
{
	t_list	*next;
	t_list	*leftover_list_c;

	if (lst)
	{
		leftover_list_c = *lst;
		if ((*lst)->fd == fd)
		{
			free((*lst)->leftover);
			next = (*lst)->next;
			free(*lst);
			*lst = next;
			leftover_list_c = *lst;
		}
		else
		{
			while ((*lst)->next)
			{
				if ((*lst)->next->fd == fd)
				{
					free((*lst)->next->leftover);
					next = (*lst)->next->next;
					free((*lst)->next);
					(*lst)->next = next;
					break ;
				}
				else
					*lst = (*lst)->next;
			}
		}
		*lst = leftover_list_c;
	}
}

char	*get_next_line_helper(int fd)
{
	static t_list	*leftover_list;
	char			*leftover;
	char			*buf;
	ssize_t			ret;
	int				old;

	leftover = get_leftover(&leftover_list, fd);
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
		ft_lstdelone(&leftover_list, fd);
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
