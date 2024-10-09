#include "../includes/ksignal.h"
#include "../includes/stddef.h"
#include "../includes/vga.h"
#include "../includes/interrupts.h"
#include "../includes/common.h"
#include "../includes/memory.h"
#include "../includes/io.h"

// signal_handler_t signal_handlers[MAX_SIGNALS];
// scheduled_signal_t scheduled_signals[MAX_SCHEDULED_SIGNALS];

// void trigger_signal(int signal) {
//     if (signal < MAX_SIGNALS && signal_handlers[signal] != NULL) {
//        signal_handlers[signal](signal);
//     }
// }

// void register_signal_handler(int signal, signal_handler_t handler) {
//     if (signal < MAX_SIGNALS) {
//         signal_handlers[signal] = handler;
//     }
// }

// void schedule_signal(int signal, int interval, signal_handler_t handler) {
//     for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
//         if (scheduled_signals[i].active && scheduled_signals[i].signal == signal) {
//             scheduled_signals[i].interval = interval;
//             scheduled_signals[i].ticks_until_run = interval;
//             if (signal < MAX_SIGNALS) {
//                 signal_handlers[signal] = handler;
//             }
//             return ;
//         }
//     }

//     for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
//         if (!scheduled_signals[i].active) {
//             scheduled_signals[i].signal = signal;
//             scheduled_signals[i].interval = interval;
//             scheduled_signals[i].ticks_until_run = interval;
//             scheduled_signals[i].active = 1;
//             if (signal < MAX_SIGNALS) {
//                 signal_handlers[signal] = handler;
//             }
//             return;
//         }
//     }
//     DEBUG_PRINT("No available slots to schedule the signal %d\n", signal);
// }

// void scheduler_tick() {
//     for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
//         if (scheduled_signals[i].active && --scheduled_signals[i].ticks_until_run <= 0) {
//             trigger_signal(scheduled_signals[i].signal);
//             scheduled_signals[i].ticks_until_run = scheduled_signals[i].interval;
//         }
//     }
// }


// void init_signals() {
//     for (int i = 0; i < MAX_SIGNALS; i++) {
//         signal_handlers[i] = NULL;
//     }

//     for (int i = 0; i < MAX_SCHEDULED_SIGNALS; i++) {
//         scheduled_signals[i].active = 0;
//     }
//     initialize_timer(1000);
// 	register_interrupt_handler(IRQ0, &scheduler_tick);
// }


process_node_t *current_process = (void *)0;
process_node_t *process_list = (void *)0;
volatile uint64_t tick_count = 0;

process_t* create_process(pid_t pid) {
    process_t *new_process = (process_t*) malloc(sizeof(process_t));
    if (new_process == NULL) {
        return NULL;
    }
    memset(new_process, 0, sizeof(process_t));
    new_process->pid = pid;
    return new_process;
}

void add_process(process_t *process) {
    process_node_t *new_node = (process_node_t*) malloc(sizeof(process_node_t));
    if (new_node == NULL) {
        return;
    }
    new_node->process = process;
    new_node->next = process_list;
    process_list = new_node;
}

void remove_process(pid_t pid) {
    process_node_t *current = process_list;
    process_node_t *previous = NULL;

    while (current != NULL) {
        if (current->process->pid == pid) {
            if (previous == NULL) {
                process_list = current->next;
            } else {
                previous->next = current->next;
            }
            free(current->process);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void init_pit() {
    uint32_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

uint64_t get_time_in_ms() {
    return tick_count;
}

void pit_interrupt_handler(t_registers reg) {
    (void)reg;
    tick_count++;
    process_node_t *p_list = process_list;
    while (p_list != NULL) {
        process_t *process = p_list->process;
        if (process) {
            for (int i = 0; i < MAX_SIGNALS ; i++) {
                if (process->signals[i].trigger_time <= tick_count && process->signals[i].trigger_time != 0) {
                    process->signals[i].signal_handler(process->signals[i].signum);
                    if (process->signals[i].count == -1) {
                        process->signals[i].trigger_time = get_time_in_ms() + process->signals[i].delay_ms;
                    } else if (process->signals[i].count > 0) {
                        process->signals[i].trigger_time = get_time_in_ms() + process->signals[i].delay_ms;
                        process->signals[i].count--;
                    } else {
                        process->signals[i].trigger_time = 0;
                    }
                }
            }
        }
    }
}

void signal(int signum, void (*signal_handler)(int)) {
    current_process->process->signals[signum].signal_handler = signal_handler;
    current_process->process->signals[signum].signum = signum;
}

void schedule_signal(int signum, uint64_t delay_ms) {
    uint64_t current_time = get_time_in_ms();

    if (signum < 0 || signum >= MAX_SCHEDULED_SIGNALS)
        return;
    current_process->process->signals[signum].trigger_time = current_time + delay_ms;
    current_process->process->signals[signum].delay_ms = delay_ms;
    current_process->process->signals[signum].signum = signum;
}

void schedule_repeat_signal(int signum, uint64_t delay_ms, int count) {
    uint64_t current_time = get_time_in_ms();

    if (signum < 0 || signum >= MAX_SCHEDULED_SIGNALS)
        return;
    current_process->process->signals[signum].trigger_time = current_time + delay_ms;
    current_process->process->signals[signum].delay_ms = delay_ms;
    current_process->process->signals[signum].signum = signum;
    current_process->process->signals[signum].count = count;
}

void init_signals() {
    process_t *kernel_process = create_process(0);
    add_process(kernel_process);
    current_process = process_list;
    init_pit();
    register_interrupt_handler(IRQ0, &pit_interrupt_handler);
}