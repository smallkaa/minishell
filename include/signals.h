#include <termios.h>

void setup_signal_handlers(void);
void handle_sigint(int sig);
void handle_sigquit(int sig);
extern volatile sig_atomic_t	g_signal_flag;
