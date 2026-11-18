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

### Testing

```sh
# Different buffer sizes must give identical output
for n in 1 2 7 42 9999 10000000; do
	cc -Wall -Wextra -Werror -D BUFFER_SIZE=$n get_next_line.c get_next_line_utils.c main.c -o gnl
	./gnl my_file.txt
done

# Memory errors and leaks
valgrind --leak-check=full ./gnl my_file.txt

# Norm
norminette get_next_line.c get_next_line_utils.c get_next_line.h
```

Cases worth testing: an empty file, a file with only newlines, a last line without `\n`, very long lines (much longer than `BUFFER_SIZE`), an invalid file descriptor (`-1`), and standard input.

## Algorithm

### Idea

`read()` returns raw bytes and knows nothing about lines. One call can return half a line, exactly one line, or several lines. The function therefore needs two things:

1. a loop that **keeps reading until a `\n` is found or the end of the file is reached**, and
2. a place to **remember the bytes that were read but not yet returned** between two calls.

The second point is the reason for the `static` variable: it keeps the unread text alive after the function returns.

### Mandatory part

A `static char *` holds the unread text. It is a `malloc`ed string (or `NULL` when nothing is stored), so it can grow to any length and is not limited to `BUFFER_SIZE`.

On each call:

1. **Check the arguments.** An invalid file descriptor (or a non-positive `BUFFER_SIZE`) returns `NULL`.
2. **Read loop.** While the stored text contains no `\n`, call `read(fd, buffer, BUFFER_SIZE)`.
   - If `read` returns `-1`, free everything, reset the stored text to `NULL` and return `NULL`.
   - If `read` returns `0`, the end of the file was reached, so the loop stops.
   - Otherwise a `'\0'` is written right after the bytes that were read, and the chunk is appended to the stored text by building a new string and freeing the old one.
3. **Nothing stored.** If the stored text is empty or `NULL` after the loop, free it, reset it to `NULL` and return `NULL`.
4. **Extract the line.** Copy the stored text up to and including the first `\n` (or the whole text if there is none) into a new `malloc`ed string. This copy is what the caller receives.
5. **Keep the rest.** The stored text becomes what comes after the `\n`, or `NULL` if nothing is left. The old block is freed.

### Why this design

- **One `read` per iteration, never reading more than needed.** The loop stops as soon as a `\n` is available, so the file is never read entirely in advance, as the subject requires.
- **The line is a separate copy.** The caller can `free` it freely, and it never shares memory with the stored text.
- **A dynamically sized stored text** works for every `BUFFER_SIZE`: with size 1 a line is built byte by byte, and with a huge size one read can contain many lines that are handed out one per call.
- **The stored text is always either a valid `'\0'`-terminated string or `NULL`**, so every helper has a clear, safe contract.
- **Memory ownership is explicit.** Each block is freed exactly once: the old stored text when it is replaced, the read buffer at the end of the loop, and the stored text when the file ends or an error occurs.

**Trade-off.** Appending a chunk copies the text accumulated so far into a new block. For a very long line read with a very small `BUFFER_SIZE`, this makes the cost grow with the length of the line divided by `BUFFER_SIZE`. It is simple, safe and fast enough for normal use, and it avoids the complexity of resizing buffers or keeping a list of chunks.

### Bonus part

The unread text can no longer be a single variable, because each file descriptor needs its own. The bonus keeps **one node per file descriptor** in a singly linked list:

```c
typedef struct s_list
{
	int				fd;
	char			*leftover;   /* unread text of this fd, or NULL */
	struct s_list	*next;
}	t_list;
```

- The head of the list is the **only `static` variable**: `static t_list *list;`.
- At the start of a call, the node for `fd` is searched in the list. If it does not exist, it is created with `leftover = NULL` and added to the list.
- The reading algorithm is exactly the one above, but it works on `node->leftover`.
- When a file descriptor reaches its end (or an error occurs), its text and its node are freed and unlinked from the list, so the memory is released and the same fd number can be reused later by another file.

A linked list was chosen over a fixed array indexed by `fd` because it has no maximum number of file descriptors, and it still counts as a single static variable, since the nodes themselves live on the heap.

## Resources

### References

- `man 2 read`, `man 2 open`, `man 3 malloc`, `man 3 free`
- The C standard and POSIX documentation on file descriptors and `static` storage duration
- The 42 Norm (checked with `norminette`)

### Use of AI

AI (Claude) was used as a **learning aid**, not as a replacement for writing the project. It was used for:

- **Explaining concepts:** `static` variables and where they are stored, how `read()` behaves (partial reads, return values), how file descriptors relate to the kernel, `malloc`/`free` ownership and dangling pointers, `BUFFER_SIZE` and the `-D` compiler flag, system calls versus library functions.

The algorithm was designed and the code was written, tested and understood by me, and I can explain every function during the evaluation.
