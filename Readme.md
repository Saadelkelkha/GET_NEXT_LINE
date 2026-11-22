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

The work is split in two functions.

**`get_next_line_helper(fd)`** performs **one `read`** and returns one piece of text. It uses a `static char leftover[BUFFER_SIZE]` that holds text read earlier but not yet returned.

1. Allocate a buffer of `BUFFER_SIZE + 1` bytes.
2. Copy the leftover into the buffer, then call `read` to fill the rest of the buffer (at most `BUFFER_SIZE` characters in total) and add a `'\0'`.
3. If the buffer is empty, or `read` returned `-1`, free the buffer and return `NULL` (end of file or error).
4. If the buffer contains a `\n`, everything after the first `\n` is saved back into the leftover and cut from the buffer. Otherwise the leftover is emptied.
5. Return the buffer.

**`get_next_line(fd)`** builds the line from these pieces.

1. Call the helper to get a piece.
2. Join it to the text accumulated so far by allocating a new string and freeing the old one.
3. If the piece contains a `\n`, the line is complete and it is returned. Otherwise, call the helper again.
4. If the helper returns `NULL`, the loop stops. The text accumulated so far is returned, or `NULL` if it is empty.
5. If a `malloc` fails, everything allocated so far is freed and `NULL` is returned.

### Why this design

- **One `read` per helper call.** The file is never read further than needed, as the subject requires.
- **The leftover needs only `BUFFER_SIZE` bytes.** It is saved only when the buffer contains a `\n`, and it holds what comes after it. Since the `\n` takes at least one character, the leftover holds at most `BUFFER_SIZE - 1` characters plus the `'\0'`.
- **The line is a separate copy.** The caller can `free` it freely, and it never shares memory with the leftover.
- **It works for every `BUFFER_SIZE`:** with size 1 a line is built one character at a time, and with a huge size one `read` can contain many lines that are handed out one per call.
- **Memory ownership is explicit.** Each piece returned by the helper is freed by `get_next_line` once it has been joined, and the previous accumulated text is freed each time it is replaced.

**Trade-off.** Joining a piece copies the text accumulated so far into a new block. For a very long line read with a very small `BUFFER_SIZE`, the total cost grows with the square of the line length. It is simple, safe and fast enough for normal use, and it avoids the complexity of resizing buffers or keeping a list of chunks.

### Bonus part

The leftover can no longer be a single array, because each file descriptor needs its own. The bonus keeps **one node per file descriptor** in a singly linked list:

```c
typedef struct s_list
{
	char			*leftover;
	int				fd;
	struct s_list	*next;
}	t_list;
```

- The head of the list is the **only `static` variable**: `static t_list *leftover_list;`.
- At the start of each helper call, the node for `fd` is searched in the list. If it does not exist, it is created with a `malloc`ed leftover of `BUFFER_SIZE` bytes, set to an empty string, and added to the list. If `fd < 0` or `BUFFER_SIZE <= 0`, or if an allocation fails, `NULL` is returned.
- The reading algorithm is exactly the one above, but it works on `node->leftover`.
- When the helper returns `NULL` because a file descriptor reached its end (or an error occurred), its leftover and its node are freed and unlinked from the list, so the memory is released and the same fd number can be reused later by another file.

A linked list was chosen over a fixed array indexed by `fd` because it has no maximum number of file descriptors, and it still counts as a single static variable, since the nodes themselves live on the heap.

## Resources

### References

- `man 2 read`, `man 2 open`, `man 3 malloc`, `man 3 free`
- The C standard and POSIX documentation on file descriptors and `static` storage duration
- The 42 Norm (checked with `norminette`)

### Use of AI

AI (Claude) was used as a **learning aid**, not as a replacement for writing the project. It was used for:

- **Explaining concepts:** `static` variables and where they are stored, how `read()` behaves (partial reads, return values), how file descriptors relate to the kernel, `malloc`/`free` ownership and dangling pointers, `BUFFER_SIZE` and the `-D` compiler flag, system calls versus library functions.
