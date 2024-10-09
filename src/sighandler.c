#include "../includes/vga.h"
#include "../includes/kernel.h"

void handle_sigint(int signum) {
    kernel_colored_log(MAGENTA, "SIGINT: %d\n", signum);
}

void handle_sigalarm(int signal) {
    kernel_colored_log(GREEN, "SIGARLARM: %d\n", signal);
}
