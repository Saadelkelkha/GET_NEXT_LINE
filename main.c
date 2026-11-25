#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"      /* use "get_next_line_bonus.h" for the bonus */

ssize_t			__real_read(int fd, void *buf, size_t n);

static int		g_calls;
static int		g_bad;
static size_t	g_last_bad;

ssize_t	__wrap_read(int fd, void *buf, size_t n)
{
	g_calls++;
	if (n != (size_t)BUFFER_SIZE)
	{
		g_bad++;
		g_last_bad = n;
	}
	return (__real_read(fd, buf, n));
}

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	if (argc != 2)
		return (fprintf(stderr, "usage: %s file\n", argv[0]), 1);
	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		return (fprintf(stderr, "cannot open %s\n", argv[1]), 1);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	fprintf(stderr, "--- %d read() call(s) ---\n", g_calls);
	if (g_bad)
		fprintf(stderr, "KO: %d call(s) with a size != BUFFER_SIZE (%d), "
			"last one: %zu\n", g_bad, BUFFER_SIZE, g_last_bad);
	else
		fprintf(stderr, "OK: every read() used BUFFER_SIZE (%d)\n",
			BUFFER_SIZE);
	return (g_bad != 0);
}
