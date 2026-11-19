
#include "get_next_line_bonus.h"
int main(void)
{
    int fd;
    char *line;
    int i;

    fd = open("file", O_RDONLY);
    int fd1 = open("t1", O_RDONLY);
    int fd2 = open("t2", O_RDONLY);
    if (fd < 0)
        return (1);

    i = 1;
    // while ((line = get_next_line_bonus(fd)) != NULL)
    // {
    //     printf("line %d = %s", i, line);
    //     free(line);
    //     i++;
    // }
    printf("line %d = %s", i, get_next_line_bonus(fd));
    printf("line %d = %s", i, get_next_line_bonus(fd1));
    printf("line %d = %s", i, get_next_line_bonus(fd2));
    printf("line %d = %s", i, get_next_line_bonus(fd));
    printf("line %d = %s", i, get_next_line_bonus(fd1));
    printf("line %d = %s", i, get_next_line_bonus(fd2));
    
    close(fd);
    return (0);
}
