#include "../includes/ksignal.h"
#include "../includes/stddef.h"
#include "../includes/vga.h"

signal_handler_t signal_handlers[MAX_SIGNALS];
scheduled_signal_t scheduled_signals[MAX_SCHEDULED_SIGNALS];

void handle_sigint(int signal) {
    printf("SIGINT: %d\n", signal);
}

void handle_sigalarm(int signal) {
    printf("SIGARLARM: %d\n", signal);
}

void trigger_signal(int signal) {
    if (signal < MAX_SIGNALS && signal_handlers[signal] != NULL) {
       signal_handlers[signal](signal);
    }
}

void register_signal_handler(int signal, signal_handler_t handler) {
    if (signal < MAX_SIGNALS) {
        signal_handlers[signal] = handler;
    }
}

void setup_signal_handlers(void) {
    register_signal_handler(SIG_INTERRUPT, handle_sigint);
    register_signal_handler(SIG_ALARM, handle_sigalarm);
}

void setup_signal_scheduling(int signal, int interval) {
    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (!scheduled_signals[i].active) {
            scheduled_signals[i].signal = signal;
            scheduled_signals[i].interval = interval;
            scheduled_signals[i].ticks_until_run = interval;
            scheduled_signals[i].active = 1;
        }
    }
}

void scheduler_tick() {
    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (scheduled_signals[i].active && --scheduled_signals[i].ticks_until_run <= 0) {
            trigger_signal(scheduled_signals[i].signal);
        }
    }
}

void init_scheduled_signals() {
    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        scheduled_signals[i].active = 0;
    }
    setup_signal_scheduling(SIG_ALARM, 10);
}

void init_signal_handlers() {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_handlers[i] = NULL;
    }
    setup_signal_handlers();
}
