/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   gen_compile_flags.c                                :+:    :+:            */
/*                                                     +:+                    */
/*   By: qvan-der <qvan-der@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2025/12/09 01:23:52 by qvan-der      #+#    #+#                 */
/*   Updated: 2025/12/09 01:25:11 by qvan-der      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_INCLUDES 1024
#define MAX_VARS 128

static char	*includes[MAX_INCLUDES];
static int	include_count = 0;
static char	project_root[PATH_MAX];

static char	*var_names[MAX_VARS];
static char	*var_values[MAX_VARS];
static int	var_count = 0;

int	already_added(const char *x)
{
	for (int i = 0; i < include_count; i++)
		if (strcmp(includes[i], x) == 0)
			return (1);
	return (0);
}

const char	*find_var(const char *name)
{
	for (int i = 0; i < var_count; i++)
		if (strcmp(var_names[i], name) == 0)
			return (var_values[i]);
	return (NULL);
}

char	*trim(char *s)
{
	char	*end;

	while (*s && isspace((unsigned char)*s))
		s++;
	if (!*s)
		return (s);
	end = s + strlen(s) - 1;
	while (end > s && isspace((unsigned char)*end))
		end--;
	end[1] = '\0';
	return (s);
}

void	add_var(const char *name, const char *value)
{
	if (!name || !*name || !value || !*value)
		return ;
	if (var_count >= MAX_VARS)
		return ;
	var_names[var_count] = strdup(name);
	var_values[var_count] = strdup(value);
	var_count++;
}

void	expand_make_var(const char *raw, char *out, size_t out_sz)
{
	const char	*p;
	char		tmp[PATH_MAX];
	const char	*end;
	char		var[128];
	size_t		len;
	const char	*val;
	const char	*q;

	p = raw;
	out[0] = '\0';
	if (p[0] == '$' && p[1] == '(')
	{
		end = strchr(p + 2, ')');
		if (end)
		{
			len = (size_t)(end - (p + 2));
			if (len >= sizeof(var))
				len = sizeof(var) - 1;
			memcpy(var, p + 2, len);
			var[len] = '\0';
			val = find_var(var);
			if (!val)
				val = getenv(var);
			if (val)
			{
				snprintf(tmp, sizeof(tmp), "%s%s", val, end + 1);
				p = tmp;
			}
		}
	}
	else if (p[0] == '$')
	{
		q = p + 1;
		while (*q && (isalnum((unsigned char)*q) || *q == '_'))
			q++;
		if (q > p + 1)
		{
			len = (size_t)(q - (p + 1));
			if (len >= sizeof(var))
				len = sizeof(var) - 1;
			memcpy(var, p + 1, len);
			var[len] = '\0';
			val = getenv(var);
			if (val)
			{
				snprintf(tmp, sizeof(tmp), "%s%s", val, q);
				p = tmp;
			}
		}
	}
	snprintf(out, out_sz, "%s", p);
}

void	add_include(const char *raw_path)
{
	char		expanded[PATH_MAX];
	const char	*path;
	char		resolved[PATH_MAX];
	size_t		root_len;
	char		out[PATH_MAX];
	const char	*rel;

	if (!raw_path || !*raw_path)
		return ;
	if (include_count >= MAX_INCLUDES)
		return ;
	expand_make_var(raw_path, expanded, sizeof(expanded));
	path = expanded[0] ? expanded : raw_path;
	if (realpath(path, resolved) == NULL)
		return ;
	root_len = strlen(project_root);
	if (strncmp(resolved, project_root, root_len) == 0 &&
		(resolved[root_len] == '/' || resolved[root_len] == '\0'))
	{
		rel = resolved + root_len;
		if (*rel == '/')
			rel++;
		snprintf(out, sizeof(out), "-I%s", rel);
	}
	else
	{
		snprintf(out, sizeof(out), "-I%s", resolved);
	}
	if (!already_added(out))
		includes[include_count++] = strdup(out);
}

void	parse_assignment(const char *line)
{
	char	buf[4096];
	char	*eq;
	char	*name;
	char	*value;

	strncpy(buf, line, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	if (buf[0] == '\t')
		return ;
	eq = strchr(buf, '=');
	if (!eq)
		return ;
	*eq = '\0';
	name = trim(buf);
	value = trim(eq + 1);
	if (!*name || !*value)
		return ;
	add_var(name, value);
}

void	scan_makefile(const char *path)
{
	FILE	*f;
	char	line[4096];
	char	*p;
	char	buf[PATH_MAX];
	int		i;

	f = fopen(path, "r");
	if (!f)
		return ;
	while (fgets(line, sizeof(line), f))
	{
		parse_assignment(line);
		if (!strstr(line, "-I"))
			continue ;
		p = line;
		while ((p = strstr(p, "-I")) != NULL)
		{
			p += 2;
			i = 0;
			while (*p && !isspace((unsigned char)*p) && i < PATH_MAX - 1)
			{
				buf[i++] = *p;
				p++;
			}
			buf[i] = '\0';
			add_include(buf);
		}
	}
	fclose(f);
}

void	walk(const char *base)
{
	DIR				*dir;
	char			buf[PATH_MAX];
	struct dirent	*ent;
	struct stat		s;

	dir = opendir(base);
	if (!dir)
		return ;
	while ((ent = readdir(dir)) != NULL)
	{
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			continue ;
		snprintf(buf, sizeof(buf), "%s/%s", base, ent->d_name);
		if (stat(buf, &s) != 0)
			continue ;
		if (S_ISDIR(s.st_mode))
		{
			walk(buf);
		}
		else if (strcmp(ent->d_name, "Makefile") == 0)
		{
			scan_makefile(buf);
		}
	}
	closedir(dir);
}

int	cmp_str(const void *a, const void *b)
{
	const char	*sa;
	const char	*sb;

	sa = *(const char *const *)a;
	sb = *(const char *const *)b;
	return (strcmp(sa, sb));
}

int	main(void)
{
	if (!getcwd(project_root, sizeof(project_root)))
		return (1);
	walk(".");
	qsort(includes, include_count, sizeof(char *), cmp_str);
	for (int i = 0; i < include_count; i++)
		printf("%s\n", includes[i]);
	return (0);
}
