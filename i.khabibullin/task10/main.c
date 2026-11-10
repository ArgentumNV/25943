#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Использование: %s <команда> [аргументы...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }

        // WIFEXITED(pid_t) true если дочерний процесс завершился корректно
        // WIFSIGNALED(pid_t) true если дочерний процесс не смог завершиться сам и был завершен ОС
        if (WIFEXITED(status))
        {
            printf("Код завершения команды: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Процесс убит сигналом: %d\n", WTERMSIG(status));
        }
    }

    return EXIT_SUCCESS;
}
