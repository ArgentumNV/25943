#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork failed");
        return EXIT_FAILURE;
    }

    if (pid == 0)
    {
        execlp("cat", "cat", "large_file.txt", (char *)NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Родительский процесс: Начало выполнения.\n");

        int status;
        waitpid(pid, &status, 0);

        printf("Родительский процесс: Дочерний процесс cat завершился.\n");
    }

    return EXIT_SUCCESS;
}
