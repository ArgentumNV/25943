#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <string.h>

#define WIDTH 40

static char line[WIDTH];
static int len = 0;

static void out(const char *s, int n)
{
    write(STDOUT_FILENO, s, n);
}

// стирает один символ на экране и в буфере
static void erase_char(void)
{
    if (len > 0)
    {
        len--;
        out("\b \b", 3);
    }
}

// стирает все символы в текущей строке
static void kill_line(void)
{
    while (len > 0)
        erase_char();
}

// стирает последнее слово вместе с пробелами после него
static void erase_word(void)
{
    while (len > 0 && line[len - 1] == ' ')
        erase_char();
    while (len > 0 && line[len - 1] != ' ')
        erase_char();
}

// переносит слово, пересекающее 40 столбец, на новую строку
static void wrap(void)
{
    int start = len;
    int wlen;
    int i;

    while (start > 0 && line[start - 1] != ' ')
        start--;
    wlen = len - start;

    // слово занимает всю строку, переносить нечего
    if (wlen == WIDTH)
    {
        out("\r\n", 2);
        len = 0;
        return;
    }

    // убираем слово с конца текущей строки
    for (i = 0; i < wlen; i++)
        out("\b \b", 3);
    out("\r\n", 2);

    // печатаем слово в начале новой строки
    memmove(line, line + start, wlen);
    len = wlen;
    out(line, len);
}

// подаем звуковой сигнал
static void beep(void)
{
    out("\a", 1);
}

// читаем остаток escape-последовательности (стрелки и т.п.), не блокируясь
static void skip_escape(void)
{
    struct termios t;
    char buf[8];
    ssize_t n;

    // временно разрешаем неблокирующее чтение с маленьким таймаутом
    tcgetattr(STDIN_FILENO, &t);
    t.c_cc[VMIN] = 0;
    t.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    n = read(STDIN_FILENO, buf, sizeof(buf));
    (void)n;

    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// добавляет символ в строку и сразу показывает на экране
static void add_char(char c)
{
    if (len == WIDTH)
    {
        // пробел на границе просто начинает новую строку
        if (c == ' ')
        {
            out("\r\n", 2);
            len = 0;
        }
        else
        {
            wrap();
        }
    }
    line[len++] = c;
    out(&c, 1);
}

int main(void)
{
    struct termios oldt, newt;
    char c;

    // запоминаем текущие настройки терминала
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // выключаем эхо, канонический режим и сигналы
    newt.c_lflag &= ~(ICANON | ECHO | ISIG);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == oldt.c_cc[VERASE])
        {
            erase_char();
        }
        else if (c == oldt.c_cc[VKILL])
        {
            kill_line();
        }
        else if (c == 0x17)
        {
            // нажат ctrl w
            erase_word();
        }
        else if (c == 0x04)
        {
            // ctrl d завершает программу только в начале строки
            if (len == 0)
                break;
        }
        else if (c == 0x1B)
        {
            // стрелки и другие escape-последовательности: глотаем и пищим
            skip_escape();
            beep();
        }
        else if (isprint((unsigned char)c))
        {
            add_char(c);
        }
        else
        {
            // непечатаемый символ, подаем звуковой сигнал
            beep();
        }
    }

    // возвращаем старые настройки терминала
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    out("\n", 1);
    return 0;
}
