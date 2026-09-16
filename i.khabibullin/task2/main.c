#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern char *tzname[];

int main()
{
    time_t now;
    struct tm *sp;

    if (putenv("TZ=America/Los_Angeles") != 0)
    {
        perror("Error changing TZ");
        exit(1);
    }

    tzset();

    (void)time(&now);
    printf("%s", ctime(&now));

    sp = localtime(&now);

    // PST - зимнее время
    // PDT - летнее время
    printf("%d/%d/%02d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday,
           sp->tm_year, sp->tm_hour,
           sp->tm_min, tzname[sp->tm_isdst]);

    exit(0);
}