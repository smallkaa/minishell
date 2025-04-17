#include "minishell.h"

volatile sig_atomic_t g_signal_flag = 0;  // Global flag for signal handling

extern void rl_replace_line(const char *, int);  // Explicitly declare it


void disable_echoctl(void)
{
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) == 0)
    {
        term.c_lflag &= ~ECHOCTL; // Disable printing of control characters
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
    }
}
/* SIGINT (Ctrl-C) Handler
// Tell readline that we are on a new line
rl_on_new_line();
// Clear the current input buffer
rl_replace_line("", 0);
// Redisplay the prompt without printing ^C
rl_redisplay();*/
void handle_sigint(int sig)
{
    (void)sig;
    // write(STDOUT_FILENO, "^C\n", 3);
    write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
    // exit(130);
}

// SIGQUIT (Ctrl-\) Handler - does nothing
void handle_sigquit(int sig)
{
    (void)sig;
}

// Function to setup signal handling
void setup_signal_handlers()
{
    struct sigaction sa;

    disable_echoctl();
    // Handle SIGINT (Ctrl-C)
    sa.sa_handler = handle_sigint;
    sa.sa_flags = SA_RESTART;  // Ensures system calls are restarted
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    // Handle SIGQUIT (Ctrl-\)
    sa.sa_handler = handle_sigquit;
    sigaction(SIGQUIT, &sa, NULL);

    // pipes ???
    // signal(SIGPIPE, SIG_IGN);

}
