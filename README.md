*This project has been created as part of the 42 curriculum by omel-kas.*

# get_next_line

## Description

`get_next_line` is a C function that returns **one line at a time** from a file descriptor.
Calling it repeatedly (for example in a loop) reads a whole file, or the standard input,
line by line.

```c
char *get_next_line(int fd);
```

| Situation | Return value |
|---|---|
| A line was read | The line, **including** its terminating `\n` |
| Last line of a file that does not end with `\n` | The line without `\n` |
| Nothing left to read, or an error occurred | `NULL` |

The goal of the project is to learn how `read()` really behaves (it returns fixed-size
chunks, not lines), how to manage heap memory without leaks, and how a **`static` variable**
can keep a state between two calls of the same function.

Allowed external functions: `read`, `malloc`, `free`.
Forbidden: `libft`, `lseek()`, global variables.

### Files

| File | Role |
|---|---|
| `get_next_line.c` | `get_next_line()` and its static helper functions |
| `get_next_line_utils.c` | Small string utilities (`ft_strlen`, `ft_strchr`, `ft_strjoin`, `ft_strdup`) |
| `get_next_line.h` | Prototypes, includes, default `BUFFER_SIZE` |
| `get_next_line_bonus.c` | Bonus: same function, several file descriptors at once |
| `get_next_line_utils_bonus.c` | Bonus: utilities |
| `get_next_line_bonus.h` | Bonus: header |

## Instructions

### Compilation

The size of the buffer given to `read()` is set at compile time with `-D BUFFER_SIZE=n`.
If the flag is omitted, the header defines a default value (`42`).

```bash
# With a chosen buffer size
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c

# Without the flag (uses the default value from the header)
cc -Wall -Wextra -Werror get_next_line.c get_next_line_utils.c main.c

# Bonus
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 \
   get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

`main.c` is your own test program: the project does not ship one.

### Usage example

```c
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int main(void)
{
    int     fd;
    char    *line;

    fd = open("file.txt", O_RDONLY);
    if (fd < 0)
        return (1);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

The caller owns every returned line and must `free()` it.
Reading from the standard input works the same way with `fd = 0`.

### Testing

Check the behaviour with several buffer sizes (`1`, `4`, `42`, `9999`, `10000000`) and
with these inputs: empty file, one line, several lines, empty lines, very long line,
file without a final `\n`, standard input (`./a.out < file` and `cat file | ./a.out`),
invalid file descriptor (`-1`, closed fd), and a call after the end of the file.

Memory checks:

```bash
cc -g -fsanitize=address -D BUFFER_SIZE=4 get_next_line.c get_next_line_utils.c main.c
valgrind --leak-check=full --show-leak-kinds=all ./a.out file.txt
```

Compare the output with the original file to be sure nothing is lost or duplicated:

```bash
./a.out file.txt > out.txt && diff out.txt file.txt && echo SAME
```

## Algorithm

### The problem

`read(fd, buf, BUFFER_SIZE)` returns up to `BUFFER_SIZE` bytes, whatever the content is.
One call may contain half a line, exactly one line, or several lines. Whatever is read
**cannot be given back** to the file descriptor (`lseek()` is forbidden), so the bytes
that belong to the *next* line must be kept somewhere between two calls.

### The chosen approach: a persistent "rest" string

A `static char *line` stores everything that has been read from the file descriptor but
not yet returned. Each call does the following:

1. **Check the input.** If `fd < 0` or `BUFFER_SIZE <= 0`, return `NULL`.
2. **Read and accumulate** (`ft_read`). While the stored text does not contain a `\n`:
   - `read()` up to `BUFFER_SIZE` bytes into a temporary buffer of `BUFFER_SIZE + 1` bytes,
   - add a `'\0'` after the `r` bytes actually read (`read()` never does it),
   - append the buffer to the stored text (`ft_strjoin`).

   The loop stops as soon as a `\n` is found, so the function never reads more than
   needed. If `read()` returns `0` (end of file), the stored text is kept: it is the last
   line of a file that does not end with `\n`. If `read()` returns `-1`, everything is
   freed and `NULL` is returned.
3. **Extract the line** (`ft_str_return`): copy from the beginning of the stored text up to
   and including the first `\n`, or up to the end if there is none.
4. **Update the rest** (`ft_newl`): what follows the `\n` becomes the new stored text in a
   fresh allocation, and the old one is freed. If nothing follows, the stored text is
   freed and set to `NULL`.
5. **Return the extracted line.**

### Trace

File `"ab\ncd\n"`, `BUFFER_SIZE = 4`:

```
Call 1: rest = NULL     read -> "ab\nc"   rest = "ab\nc"   return "ab\n"   rest = "c"
Call 2: rest = "c"      no \n -> read "d\n"  rest = "cd\n"  return "cd\n"   rest = NULL
Call 3: rest = NULL     read -> 0 (EOF)                     return NULL
```

### Why this algorithm?

- **Correct for any `BUFFER_SIZE`.** The reading loop does not depend on the buffer size, so
  `1`, `42`, `9999` and `10000000` all work. The temporary buffer lives on the **heap**,
  because a local array of `10000000` bytes would overflow the stack.
- **Reads as little as possible.** It stops reading at the first `\n`, as the subject
  requires, and never loads the whole file.
- **Works with any file descriptor** (regular file, standard input, pipe), because it does
  not need to know the size of the data in advance.
- **A static pointer, not a counter.** Storing a position or a line number would be
  useless: every `read()` overwrites the buffer, and previously read bytes cannot be read
  again. What must survive between calls is the *text* that has not been returned yet.
- **Small, single-purpose functions** (read, extract, update), which keeps each function
  short enough for the Norm and makes each one testable on its own.

### Trade-offs

- Each concatenation allocates a new string and copies the old content. For a very long
  line with a tiny `BUFFER_SIZE` (for example `1`) the same bytes are copied many times,
  so the cost grows quadratically with the line length. It is acceptable for this project
  and keeps the code simple; a growing buffer with a capacity would avoid it.

### Memory ownership

- The temporary read buffer is allocated and freed inside `ft_read`.
- The returned line is allocated by `get_next_line`; the **caller** frees it.
- The static rest is owned by `get_next_line`: every replacement frees the previous
  allocation, and the rest is set to `NULL` when nothing remains (end of file or error), so
  the next call starts from a clean state.

## Bonus

The bonus handles **several file descriptors at the same time** with **one static variable**:

```c
static char *line[1024];   /* one "rest" string per file descriptor */
```

`line[fd]` holds the unreturned text of the file descriptor `fd`, so reading fd 3, then
fd 4, then fd 3 again never mixes the lines. The same helper functions are used, applied
to `line[fd]` instead of a single pointer.

- The array has a fixed size, so file descriptors outside `0 .. 1023` are rejected
  (`NULL` is returned) to avoid reading or writing outside the array.
- An array is a single `static` variable, and gives direct access to the state of any
  descriptor. A linked list indexed by descriptor would remove the fixed limit at the cost
  of more code.

## Resources

### References

- `man 2 read`, `man 2 open`, `man 2 close`: behaviour and return values of the system calls
- `man 3 malloc`, `man 3 free`: dynamic memory allocation
- Kernighan & Ritchie, *The C Programming Language*: pointers, strings, `static` storage
- The Norm of 42 (`norminette`) for the coding style
- AddressSanitizer and Valgrind documentation: detecting overflows, use-after-free and leaks

### Use of AI

AI was used to help draft this README.md (structure and formatting of the documentation). It was not used to write the library functions themselves: all the C code was written and understood by the autho.
