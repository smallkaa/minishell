#include <termios.h>

void setup_signal_handlers(void);
void handle_sigint(int sig);
void handle_sigquit(int sig);
