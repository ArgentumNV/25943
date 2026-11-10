#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

// функция печатает права доступа в виде rwxrwxrwx
void print_mode(mode_t m)
{
    printf("%c", (m & S_IRUSR) ? 'r' : '-');
    printf("%c", (m & S_IWUSR) ? 'w' : '-');
    printf("%c", (m & S_IXUSR) ? 'x' : '-');
    printf("%c", (m & S_IRGRP) ? 'r' : '-');
    printf("%c", (m & S_IWGRP) ? 'w' : '-');
    printf("%c", (m & S_IXGRP) ? 'x' : '-');
    printf("%c", (m & S_IROTH) ? 'r' : '-');
    printf("%c", (m & S_IWOTH) ? 'w' : '-');
    printf("%c", (m & S_IXOTH) ? 'x' : '-');
}

int main(int argc, char *argv[])
{
    struct stat st;

    if (argc < 2)
    {
        printf("использование: %s файл...\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        // получаем информацию о файле
        if (lstat(argv[i], &st) == -1)
        {
            perror(argv[i]);
            continue;
        }

        // первый символ - тип файла
        if (S_ISDIR(st.st_mode))
            putchar('d');
        else if (S_ISREG(st.st_mode))
            putchar('-');
        else
            putchar('?');

        // права доступа
        print_mode(st.st_mode);

        // количество связей
        printf(" %2ld", (long)st.st_nlink);

        // имя собственника и группы
        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf(" %-8s", pw ? pw->pw_name : "?");
        printf(" %-8s", gr ? gr->gr_name : "?");

        // размер файла
        printf(" %8ld", (long)st.st_size);

        // дата модификации через mtime
        char buf[32];
        strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M", localtime(&st.st_mtime));
        printf(" %s", buf);

        // выводим только имя файла без пути
        char *name = strrchr(argv[i], '/');
        printf(" %s\n", name ? name + 1 : argv[i]);
    }

    return 0;
}