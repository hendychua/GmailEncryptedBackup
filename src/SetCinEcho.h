#ifndef SET_CIN_ECHO_H
#define SET_CIN_ECHO_H

#ifdef __linux__
#   include <termios.h>
#   include <unistd.h>
#elif _WIN32
#   include <windows.h>
#else
    // not implemented
#endif

/// <summary> SetCinEcho enables/disables echoing when user types characters into cin. </summary>
/// <param name="enable">
/// If set to true, echoing will be enabled. If set to false, echoing will be disabled.
/// </param>
void SetCinEcho(bool enable = true)
{
#ifdef __linux__

    termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(!enable)
    {
        // don't echo chars back to users when they type something
        tty.c_lflag &= ~ECHO;
    }
    else
    {
        // echo chars back to users when they type something
        tty.c_lflag |= ECHO;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);

#elif _WIN32

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if(!enable)
    {
        // don't echo chars back to users when they type something
        mode &= ~ENABLE_ECHO_INPUT;
    }
    else
    {
        // echo chars back to users when they type something
        mode |= ENABLE_ECHO_INPUT;
    }

    SetConsoleMode(hStdin, mode);


#else

    // not implemented

#endif
}

#endif
