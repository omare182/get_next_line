
#include "get_next_line.h"

int main(void)
{
    int fd;
    char *line;
    int i;

    fd = open("file", O_RDONLY);
    if (fd < 0)
        return (1);

    i = 1;
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("line %d = %s", i, line);
        free(line);
        i++;
    }
    close(fd);
    return (0);
}
