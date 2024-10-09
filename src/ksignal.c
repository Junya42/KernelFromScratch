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
    static uint32_t tick_count = 0;
    tick_count++;

    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        if (scheduled_signals[i].active && --scheduled_signals[i].ticks_until_run <= 0) {
            trigger_signal(scheduled_signals[i].signal);
            scheduled_signals[i].ticks_until_run = scheduled_signals[i].interval;
        }
    }

    if (tick_count % 1000 == 0) {
        printf("Tick count: %u\n", tick_count);
    }
}

void initialize_timer(uint32_t frequency) {
    uint32_t divisor = 1193182 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void init_signals() {
    for (int i = 0; i < MAX_SIGNALS; i++) {
        signal_handlers[i] = NULL;
    }

    for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
        scheduled_signals[i].active = 0;
    }
    initialize_timer(100);
	register_interrupt_handler(IRQ0, &scheduler_tick);
}
