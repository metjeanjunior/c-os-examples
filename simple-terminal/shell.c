#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/queue.h>

#define KRED  "\x1B[31m"
#define KNRM  "\x1B[0m"

const char delimiters[] = " ,;:!\n";

struct Node
{
	pid_t pid;
	STAILQ_ENTRY(Node) nodes;
};

// typedef struct node Node;
STAILQ_HEAD(stailhead, Node);

int getArguments(char *line, char **args);

int main(int argc, char **argv)
{
	char string[80];
	char *args[10];
	char cwd[1024];

	int status;
	int count = 0;
	pid_t pid;

	// STAILQ_HEAD(stailhead, Node) head = STAILQ_HEAD_INITIALIZER(head);
	// STAILQ_HEAD(stailhead, Node) head;
	struct stailhead head = STAILQ_HEAD_INITIALIZER(head);
	STAILQ_INIT(&head);
	getcwd(cwd, sizeof(cwd));

	while(1)
	{
		printf("%s$", cwd);
		fgets(string, 80, stdin);
		getArguments(string,args);

		// if (strcmp(args[0], "exit\n") == 0)
		if (strcmp(args[0], "exit") == 0)
			exit(0);
		else if(strcmp(args[0], "cd") == 0)
		{
			if (cwd != NULL)
			{
				// inspired by: https://stackoverflow.com/questions/3642050/chdir-not-affecting-environment-variable-pwd
				if(chdir(args[1])==0)
				{
					setenv("OLDPWD", cwd, 1);
					setenv("PWD", getcwd(cwd, sizeof(cwd)), 1);
				}
				else
				{
					perror(KRED "shell: cd: Invalid Directory");
					printf(KNRM "");
					printf("%s\n", args[1]);
				}

			}
			continue;
		}
		else if(strcmp(args[0], "showpid") == 0)
		{
			// This prints the pid of the last 5 forked processes
			struct Node *np = malloc(sizeof(struct Node));
			STAILQ_FOREACH(np, &head, nodes)
				printf("%i\n", np->pid);
			continue;
		}

		switch (pid = fork()) 
		{ 
			case 0:
				execvp(args[0], args);
				perror(KRED "Error: Command could not be executed");
				printf(KNRM "");
				exit(1); 
			default:
				waitpid(pid, &status, 0);

				if (count == 5)
				{
				    struct Node *nps = STAILQ_FIRST(&head);
				    // STAILQ_REMOVE(&head, nps, Node, nodes);
			        STAILQ_REMOVE_HEAD(&head, nodes);
			        free(nps);
				}
				else
					count++;

				struct Node * node = malloc(sizeof(struct Node));
				node->pid=pid;
				STAILQ_INSERT_TAIL(&head, node, nodes);

				
				break;
			case -1:
				perror(KRED "fork"); 
				printf(KNRM "");
				exit(1);
		}
	}
}

int getArguments(char *line, char **args)
{
	int argcount = 0;
	char *cp;
	cp = strdup (line);
	args[argcount] = strtok (cp, delimiters);
	argcount++;
	while ( (args[argcount] = strtok (NULL, delimiters)) )
		argcount++;
}