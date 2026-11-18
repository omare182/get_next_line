
#include "get_next_line_bonus.h"
int main(void)
{
    int fd;
    char *line;
    int i;

    fd = open("file", O_RDONLY);
    if (fd < 0)
        return (1);

    i = 1;
    while ((line = get_next_line_bonus(fd)) != NULL)
    {
        printf("line %d = %s", i, line);
        free(line);
        i++;
    }
    printf("line %d = %s", i, line);printf("line %d = %s", i, line);
    close(fd);
    return (0);
}
