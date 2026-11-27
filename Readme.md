*This project has been created as part of the 42 curriculum by sel-kelk.*

# Get Next Line

## Description

**Get Next Line** is a C function, `get_next_line(int fd)`, that returns the next line of text read from a file descriptor each time it is called. Calling it repeatedly in a loop reads a whole file (or any readable file descriptor, such as standard input) one line at a time.

```c
char *get_next_line(int fd);
```

**Goal.** The project teaches how to read data in chunks with `read()`, how to keep state between calls with a `static` variable, and how to manage dynamic memory without leaks.

**Behavior.**
- Each call returns one line, **including its terminating `\n`**. The last line of a file that does not end with `\n` is returned without it.
- It returns `NULL` when there is nothing left to read, or when an error occurs.
- It works with any `BUFFER_SIZE` value (1, 42, 9999, 10000000, ...), defined at compile time.
- It works on regular files, standard input and pipes.
- Only `read`, `malloc` and `free` are used. No `libft`, no `lseek`, no global variables.

**Files.**

| Part | Files |
|---|---|
| Mandatory | `get_next_line.c`, `get_next_line_utils.c`, `get_next_line.h` |
| Bonus | `get_next_line_bonus.c`, `get_next_line_utils_bonus.c`, `get_next_line_bonus.h` |

**Bonus.** The bonus version keeps the reading state of **several file descriptors at the same time**, so calls can alternate between fd 3, 4, 5 and so on without mixing their lines. It uses a single `static` variable.

## Instructions

### Compilation

The buffer size is chosen at compile time with `-D BUFFER_SIZE=n`. The code also compiles without the flag, in which case a default value defined in the header is used.

```sh
# Mandatory part
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl

# Without the flag (uses the default BUFFER_SIZE)
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c -o gnl

# Bonus part
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl_bonus
```

`main.c` is your own test program and is not part of the submitted files.

### Usage example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "get_next_line.h"

int	main(int argc, char **argv)
{
	int		fd;
	char	*line;

	fd = 0;
	if (argc > 1)
		fd = open(argv[1], O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("[%s]", line);
		free(line);
		line = get_next_line(fd);
	}
	if (fd > 0)
		close(fd);
	return (0);
}
```

```sh
./gnl my_file.txt          # read a file
echo "hello" | ./gnl       # read from a pipe / standard input
```

The caller owns each returned line and must `free` it.

## Algorithm

### Idea

`read()` returns raw bytes and knows nothing about lines. One call can return half a line, exactly one line, or several lines. The function therefore needs two things:

1. a way to **read the file piece by piece**, one `read` of at most `BUFFER_SIZE` bytes at a time, until a `\n` is found or the end of the file is reached, and
2. a place to **remember the bytes that were read but not yet returned** between two calls.

The second point is the reason for the `static` variable: it keeps the unread text alive after the function returns.

### Mandatory part

State is kept in one `static char *s`, a heap string holding text that was read but not yet returned (the "stash").

**`get_next_line(fd)`**
1. Allocate a buffer of `BUFFER_SIZE + 1` bytes.
2. While the stash contains no `\n` and `read` has not reached EOF: `read` up to `BUFFER_SIZE` bytes, add a `'\0'`, and append them to the stash with `ft_strjoin` (the old stash is freed).
3. Hand over to `get_next_line_helper`.
4. On a `read` error or a failed `malloc`, free the buffer and the stash, reset the stash to `NULL`, and return `NULL`.

**`get_next_line_helper(b, &s)`**
1. Free the read buffer.
2. `line(s)` copies the stash up to and including the first `\n` (or to the end) into a new string.
3. If there is nothing to return (EOF with an empty stash), or the allocation fails, free the stash, set it to `NULL`, and return `NULL`.
4. `next(s)` replaces the stash with the text after the first `\n` (`ft_strdup`) and frees the old stash. If no `\n` is left, the stash is freed and set to `NULL`.

### Why this design

- **Reads only as needed.** The loop stops as soon as the stash contains a `\n`, so the file is never read further than required.
- **The returned line is a separate copy.** The caller can `free` it without affecting the stash.
- **Works for any `BUFFER_SIZE`.** With 1, the line is built one character at a time. With a huge value, one `read` can hold many lines, which are handed out one per call.
- **Explicit ownership.** Every failure path frees the buffer and the stash and leaves no dangling pointer.

**Trade-off.** `ft_strjoin` copies the whole stash on every read, and `ft_strchr` rescans it each iteration. For a very long line with a very small `BUFFER_SIZE`, the cost grows with the square of the line length. This is simple and safe, and fast enough for normal use.

### Bonus part

Each file descriptor needs its own stash, so the single static variable becomes an array of stashes indexed by `fd`:

    static char *s[OPEN_MAX];   /* OPEN_MAX = 1024 */

- It is still **one static variable**, and the algorithm is identical to the mandatory part but works on `s[fd]`.
- `fd < 0`, `fd >= OPEN_MAX`, or `BUFFER_SIZE <= 0` returns `NULL`, so an out-of-range fd can never index outside the array.
- When an fd reaches EOF or hits an error, `s[fd]` is freed and set to `NULL`, so the same fd number can be reused by another file.

**Trade-off.** An array is simpler and faster than a list (direct access, no search), but it limits the function to 1024 file descriptors.

## Resources

### References

- `man 2 read`, `man 2 open`, `man 3 malloc`, `man 3 free`
- The C standard and POSIX documentation on file descriptors and `static` storage duration
- The 42 Norm (checked with `norminette`)

### Use of AI

AI (Claude) was used as a **learning aid**, not as a replacement for writing the project. It was used for:

- **Explaining concepts:** `static` variables and where they are stored, how `read()` behaves (partial reads, return values), how file descriptors relate to the kernel, `malloc`/`free` ownership and dangling pointers, `BUFFER_SIZE` and the `-D` compiler flag, system calls versus library functions.
