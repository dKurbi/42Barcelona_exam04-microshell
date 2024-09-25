#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

static int	er(char *str, char *str2)
{
	while (*str)
		write(2, str++, 1);
	if (str2)
	{
		write(2, " ", 1);
		while (*str2)
			write(2, str2++, 1);
	}
	write(2, "\n", 1);
	return (1);
}

static int	cd(char **argv, int i)
{
	if (i != 2)
		return (er("error: cd: bad arguments", NULL));
	else if (chdir(argv[1]) == -1 )
		return (er("error cd: cannot change directory to", argv[1]));
	return (0);	
}

static int execut(char **argv, char **env, int i)
{
	int	status;
	int	fd[2];
	int	pipes = (argv[i] && !strcmp(argv[i], "|"));
	
	if (pipes && pipe(fd) == -1)
		return (er("error: fatal", NULL));
	int	pid = fork();

	if (!pid)
	{
		argv[i] = NULL;
		if (pipes && (dup2(fd[1], 1) == -1 || close(fd[1]) == -1 || close(fd[0]) == -1 ))
			return (er("error: fatal", NULL));
		execve(*argv, argv, env);
		return (er("error: cannot execute", *argv));
	}
	waitpid(pid, &status, 0);
	if (pipes && (dup2(fd[0], 0) == -1 || close(fd[1]) == -1 || close(fd[0]) == -1 ))
                        return (er("error: fatal", NULL));
	return WIFEXITED(status) && WEXITSTATUS(status);
}


int main(int argc, char **argv, char **env)
{
	int status;
	int i;

	status = 0;
	if (argc > 1)
	{
		while (*argv && *(argv + 1))
		{
			argv += 1;
			i = 0;
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;
			if (!strcmp(*argv, "cd"))
				status = cd(argv, i);
			else if (i) 
				status = execut(argv, env, i);
			argv+=i;
		}
	}
	return (status);
}
