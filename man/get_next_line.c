#include "get_next_line.h"

static char	*ft_newl(char *s)
{
	char	*rest;
	size_t	i;
	size_t	j;

	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (!s[i] || !s[i + 1])
		return (free(s), NULL);
	rest = malloc(ft_strlen(s + i + 1) + 1);
	if (!rest)
		return (free(s), NULL);
	j = 0;
	i++;
	while (s[i])
		rest[j++] = s[i++];
	rest[j] = '\0';
	free(s);
	return (rest);
}

static char	*ft_read(char *line, int fd)
{
	char	*buf;
	ssize_t	r;

	buf = malloc((size_t)BUFFER_SIZE + 1);
	if (!buf)
		return (free(line), NULL);
	r = 1;
	while (r > 0 && !ft_strchr(line, '\n'))
	{
		r = read(fd, buf, BUFFER_SIZE);
		if (r == 0)
			return (free(buf), line);
        if (r < 0)
            return (free(buf), free(line), NULL);
		buf[r] = '\0';
		line = ft_strjoin(line, buf);
		if (!line)
			return (free(buf), NULL);
	}
	free(buf);
	return (line);
}

static char	*ft_str_return(char *str)
{
	char	*s;
	size_t	len;
	size_t	i;

	if (!str)
		return (NULL);
	len = 0;
	while (str[len] && str[len] != '\n')
		len++;
	if (str[len] == '\n')
		len++;
	s = malloc(len + 1);
	if (!s)
		return (NULL);
	i = -1;
	while (++i < len)
		s[i] = str[i];
	s[len] = '\0';
	return (s);
}

char	*get_next_line(int fd)
{
	static char	*line;
	char		*str;

	if (fd < 0 || BUFFER_SIZE <= 0 || (size_t)BUFFER_SIZE > (SIZE_MAX - 1))
		return (NULL);
	line = ft_read(line, fd);
	if (!line)
		return (NULL);
	str = ft_str_return(line);
	if (!str)
		return (free(line), line = NULL, NULL);
	line = ft_newl(line);
	return (str);
}