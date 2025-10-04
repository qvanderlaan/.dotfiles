#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define TEXT_SAMPLE 4096

int	is_text(const char *path)
{
	int				fd;
	unsigned char	buf[TEXT_SAMPLE];
	ssize_t			n;
	unsigned char	c;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (0);
	n = read(fd, buf, sizeof(buf));
	close(fd);
	if (n <= 0)
		return (0);
	for (ssize_t i = 0; i < n; i++)
	{
		c = buf[i];
		if (c == 0 || (c < 0x09 && c != '\n' && c != '\t') || c == 0x7F)
			return (0);
	}
	return (1);
}

void	output_file(const char *path)
{
	int		fd;
	char	buf[8192];
	ssize_t	n;

	printf("===== %s =====\n", path);
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return ;
	while ((n = read(fd, buf, sizeof(buf))) > 0)
		fwrite(buf, 1, n, stdout);
	close(fd);
	printf("\n\n");
}

int	main(void)
{
	FILE		*tree;
	char		path[4096];
	size_t		len;
	struct stat	st;

	tree = popen("tree -if --gitignore", "r");
	if (!tree)
	{
		fprintf(stderr, "tree command not found\n");
		return (1);
	}
	while (fgets(path, sizeof(path), tree))
	{
		len = strlen(path);
		if (len > 0 && path[len - 1] == '\n')
			path[len - 1] = '\0';
		if (stat(path, &st) == -1 || !S_ISREG(st.st_mode))
			continue ;
		if (is_text(path))
			output_file(path);
	}
	pclose(tree);
	return (0);
}
