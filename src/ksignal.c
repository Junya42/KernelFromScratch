#include "../includes/ksignal.h"
#include "../includes/stddef.h"
#include "../includes/vga.h"

signal_handler_t signal_handlers[MAX_SIGNALS];

void handle_sigint(int signal) {
    printf("SIGINT: %d\n", signal);
}

void trigger_signal(int signal) {
    if (signal < MAX_SIGNALS && signal_handlers[signal] != NULL) {
       signal_handlers[signal](signal);
    }
}

void handle_timer_interrupt(int signal) {
    static int tick_count = 0;
    tick_count++;
    if (tick_count % 1000 == 0) {
        trigger_signal(SIGNAL_ONE_SECOND);
    }
}

void setup_signal_handlers(void) {
    DEBUG_PRINT("%d\n", SIG_INTERRUPT);
    register_signal_handler(SIG_INTERRUPT, handle_sigint);
}

void register_signal_handler(int signal, signal_handler_t handler) {
    DEBUG_PRINT("%d\n", signal);
    DEBUG_PRINT("%d\n", MAX_SIGNALS);
    if (signal < MAX_SIGNALS) {
        signal_handlers[signal] = handler;
    }
}

void init_signal_handlers() {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_handlers[i] = NULL;
    }
    setup_signal_handlers();
}
