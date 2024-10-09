#include "../includes/vga.h"

void handle_sigint(int signal) {
    printf("SIGINT: %d\n", signal);
}

void handle_sigalarm(int signal) {
    printf("SIGARLARM: %d\n", signal);
}