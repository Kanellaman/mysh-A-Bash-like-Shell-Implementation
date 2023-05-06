#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main()
{
    struct termios old, new;
    char c;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &old);
    new = old;

    // Put the terminal in raw mode
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    // Print message that stays on the screen
    printf("Type something: ");
    fflush(stdout);

    // Save cursor position
    printf("\033[s");

    while (1)
    {
        // Read a single character from the terminal
        c = getchar();

        // Check for arrow keys
        if (c == '\033')
        {
            getchar();     // Consume the [
            c = getchar(); // Get the actual arrow key code
            if (c == 'A')
            {
                printf("\033[uYou pressed the up arrow!          ");
                fflush(stdout);
            }
            else if (c == 'B')
            {
                printf("\033[uYou pressed the down arrow!        ");
                fflush(stdout);
            }
            else if (c == 'C')
            {
                printf("\033[uYou pressed the right arrow!       ");
                fflush(stdout);
            }
            else if (c == 'D')
            {
                printf("\033[uYou pressed the left arrow!        ");
                fflush(stdout);
            }
        }
        else if (c == '\n')
        {
            break;
        }
        else
        {
            putchar(c);
            fflush(stdout);
        }
    }

    // Restore the terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    return 0;
}
