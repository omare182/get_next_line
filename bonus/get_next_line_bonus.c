#include "get_next_line.h"

static char	*ft_newl(char *s)
{
	char	*str;
	size_t	i;
	size_t	j;

	i = 0;
	while (s[i] && s[i] != '\n')
		i++;
	if (!s[i] || !s[i + 1])
		return (free(s), NULL);
	str = malloc(ft_strlen(s + i + 1) + 1);
	if (!str)
		return (free(s), NULL);
	j = 0;
	i++;
	while (s[i])
		str[j++] = s[i++];
	str[j] = '\0';
	free(s);
	return (str);
}

static char	*ft_read(char *line, int fd)
{
	char	*str;
	ssize_t	r;

	str = malloc(BUFFER_SIZE + 1);
	if (!str)
		return (free(line), NULL);
	r = 1;
	while (r > 0 && !ft_strchr(line, '\n'))
	{
		r = read(fd, str, BUFFER_SIZE);
		if (r < 0)
			return (free(str), free(line), NULL);
		str[r] = '\0';
		line = ft_strjoin(line, str);
		if (!line)
			return (free(str), free(line), NULL);
	}
	free(str);
	return (line);
}

static char	*ft_str_return(char *str)
{
	char	*s;
	size_t	len;
	size_t	i;

	if (!str[0])
		return (NULL);
	len = 0;
	while (str[len] && str[len] != '\n')
		len++;
	if (str[len] == '\n')
		len++;
	s = malloc(len + 1);
	if (!s)
		return (free(str), NULL);
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

	if (fd < 0 || BUFFER_SIZE <= 0)
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
