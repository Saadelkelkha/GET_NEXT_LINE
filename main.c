#include "get_next_line.h"
#include <stdio.h>

// int	main()
// {
// 	int fd = open("test.txt", O_RDONLY);
// 	char	*str;
// 	str = get_next_line(fd);
// 	while (str)
// 	{
// 		printf("%s", str);
// 		str = get_next_line(fd);
// 	}
// }


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "get_next_line.h"

static int	g_total = 0;
static int	g_passed = 0;

static void	print_escaped(const char *s)
{
	int	i;

	if (!s)
	{
		printf("(null)");
		return ;
	}
	printf("\"");
	i = 0;
	while (s[i] && i < 60)
	{
		if (s[i] == '\n')
			printf("\\n");
		else
			printf("%c", s[i]);
		i++;
	}
	if (s[i])
		printf("...");
	printf("\"");
}

static int	check_fd(int fd, char **exp, const char *name)
{
	char	*line;
	int		i;

	i = 0;
	while (exp[i])
	{
		line = get_next_line(fd);
		if (!line || strcmp(line, exp[i]) != 0)
		{
			printf("  KO %s, call %d\n     expected: ", name, i + 1);
			print_escaped(exp[i]);
			printf("\n     got     : ");
			print_escaped(line);
			printf("\n");
			free(line);
			return (0);
		}
		free(line);
		i++;
	}
	line = get_next_line(fd);
	if (line)
	{
		printf("  KO %s, call %d: expected NULL, got ", name, i + 1);
		print_escaped(line);
		printf("\n");
		free(line);
		return (0);
	}
	line = get_next_line(fd);
	if (line)
	{
		printf("  KO %s: call after NULL did not return NULL\n", name);
		free(line);
		return (0);
	}
	return (1);
}

static void	report(const char *name, int ok)
{
	g_total++;
	if (ok)
	{
		g_passed++;
		printf("OK  %s\n", name);
	}
}

static void	test_file(const char *name, const char *content, size_t len,
		char **exp)
{
	int		fd;
	int		ok;
	char	path[] = "/tmp/gnl_test_file.txt";

	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		printf("cannot create %s\n", path);
		return ;
	}
	write(fd, content, len);
	close(fd);
	fd = open(path, O_RDONLY);
	ok = check_fd(fd, exp, name);
	close(fd);
	unlink(path);
	report(name, ok);
}

static void	test_pipe(const char *name, const char *content, size_t len,
		char **exp)
{
	int	p[2];
	int	ok;

	if (pipe(p) == -1)
		return ;
	write(p[1], content, len);
	close(p[1]);
	ok = check_fd(p[0], exp, name);
	close(p[0]);
	report(name, ok);
}

static void	test_invalid_fds(void)
{
	char	*line;
	int		fd;
	int		ok;

	ok = 1;
	line = get_next_line(-1);
	if (line)
	{
		printf("  KO fd -1 should return NULL\n");
		free(line);
		ok = 0;
	}
	fd = open("/tmp/gnl_closed.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	write(fd, "abc\n", 4);
	close(fd);
	line = get_next_line(fd);
	if (line)
	{
		printf("  KO closed fd should return NULL\n");
		free(line);
		ok = 0;
	}
	unlink("/tmp/gnl_closed.txt");
	report("invalid fds (-1 and closed)", ok);
}

static void	test_long_lines(void)
{
	char	*content;
	char	*e1;
	char	*e2;
	char	*e3;
	char	*exp[4];

	content = malloc(5000 + 1 + 6 + 20000 + 1);
	e1 = malloc(5000 + 2);
	e2 = strdup("short\n");
	e3 = malloc(20000 + 1);
	if (!content || !e1 || !e2 || !e3)
		return ;
	memset(e1, 'A', 100);
	e1[100] = '\n';
	e1[101] = '\0';
	memset(e3, 'B', 20000);
	e3[20000] = '\0';
	strcpy(content, e1);
	strcat(content, e2);
	strcat(content, e3);
	exp[0] = e1;
	exp[1] = e2;
	exp[2] = e3;
	exp[3] = NULL;
	test_file("long lines (5000, short, 20000 no newline)", content,
		strlen(content), exp);
	free(content);
	free(e1);
	free(e2);
	free(e3);
}

int	main(void)
{
	char	*e1[] = {"Hello world\n", "Second line\n", "Third line\n", NULL};
	char	*e2[] = {"First\n", "Second\n", "Last without newline", NULL};
	char	*e3[] = {NULL};
	char	*e4[] = {"\n", "\n", "\n", NULL};
	char	*e5[] = {"a", NULL};
	char	*e6[] = {"a\n", NULL};
	char	*e7[] = {"L1\n", "\n", "L3\n", "\n", "\n", "L6\n", NULL};
	char	*e8[] = {"ab\n", "cd\n", "ef", NULL};
	char	*e9[] = {"one\n", "two\n", NULL};

	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);
	test_file("normal, ends with newline",
		"Hello world\nSecond line\nThird line\n", 35, e1);
	test_file("no final newline", "First\nSecond\nLast without newline", 33, e2);
	test_file("empty file", "", 0, e3);
	test_file("only newlines", "\n\n\n", 3, e4);
	test_file("one char, no newline", "a", 1, e5);
	test_file("one char + newline", "a\n", 2, e6);
	test_file("empty lines inside", "L1\n\nL3\n\n\nL6\n", 13, e7);
	test_file("trace example ab/cd/ef", "ab\ncd\nef", 8, e8);
	test_long_lines();
	test_pipe("pipe (not a regular file)", "one\ntwo\n", 8, e9);
	test_invalid_fds();
	printf("\nResult: %d/%d tests passed\n", g_passed, g_total);
	return (g_passed != g_total);
}
