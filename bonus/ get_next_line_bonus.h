#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 2

#endif

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


char	*ft_strdup(const char *s);
char	*ft_strchr(const char *s, int c);
size_t ft_strlen(const char * s);
char	*ft_strjoin(char *s1, char *s2);



char *get_next_lineS(int fd);
#endif