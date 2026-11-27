/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sel-kelk <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/26 10:20:49 by sel-kelk          #+#    #+#             */
/*   Updated: 2026/11/26 10:20:51 by sel-kelk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s)
{
	char	*c;
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	c = malloc(i + 1);
	if (!c)
		return (0);
	i = 0;
	while (s[i])
	{
		c[i] = s[i];
		i++;
	}
	c[i] = s[i];
	return (c);
}

char	*ft_strchr(const char *s, int c)
{
	char	*sc;

	if (!s)
		return (NULL);
	sc = (char *)s;
	while (*sc)
	{
		if (*sc == (char)c)
			return (sc);
		sc++;
	}
	if (*sc == (char)c)
		return (sc);
	return (0);
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

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*conc;
	size_t	len_s1;
	size_t	len_s2;

	if (!s2)
		return (NULL);
	if (s1)
		len_s1 = ft_strlen(s1);
	else
		len_s1 = 0;
	len_s2 = ft_strlen(s2);
	conc = malloc(len_s1 + len_s2 + 1);
	if (!conc)
		return (NULL);
	conc[0] = '\0';
	if (s1)
		ft_strlcat(conc, s1, len_s1 + 1);
	ft_strlcat(conc, s2, len_s1 + len_s2 + 1);
	return (conc);
}
