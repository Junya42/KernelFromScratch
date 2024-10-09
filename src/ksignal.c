#include "../includes/ksignal.h"
#include "../includes/stddef.h"
#include "../includes/vga.h"
#include "../includes/interrupts.h"

signal_handler_t signal_handlers[MAX_SIGNALS];
scheduled_signal_t scheduled_signals[MAX_SCHEDULED_SIGNALS];

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

void schedule_signal(int signal, int interval, signal_handler_t handler) {
    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (scheduled_signals[i].active && scheduled_signals[i].signal == signal) {
            scheduled_signals[i].interval = interval;
            scheduled_signals[i].ticks_until_run = interval;
            if (signal < MAX_SIGNALS) {
                signal_handlers[signal] = handler;
            }
            return ;
        }
    }

    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (!scheduled_signals[i].active) {
            scheduled_signals[i].signal = signal;
            scheduled_signals[i].interval = interval;
            scheduled_signals[i].ticks_until_run = interval;
            scheduled_signals[i].active = 1;
            if (signal < MAX_SIGNALS) {
                signal_handlers[signal] = handler;
            }
            return;
        }
    }
    DEBUG_PRINT("No available slots to schedule the signal %d\n", signal);
}

void scheduler_tick() {
    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (scheduled_signals[i].active && --scheduled_signals[i].ticks_until_run <= 0) {
            trigger_signal(scheduled_signals[i].signal);
            scheduled_signals[i].ticks_until_run = scheduled_signals[i].interval;
        }
    }
}

void init_signals() {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_handlers[i] = NULL;
    }

    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        scheduled_signals[i].active = 0;
    }
	register_interrupt_handler(IRQ0, &scheduler_tick);
}
