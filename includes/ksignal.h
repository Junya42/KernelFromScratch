#ifndef KSIGNAL_H
# define KSIGNAL_H

# define MAX_SIGNALS 32

# include "stdint.h"

typedef enum {
    SIG_NONE = 0,
    SIG_HANGUP = 1,
    SIG_INTERRUPT = 2,
    SIG_QUIT = 3,
    SIG_ILLEGAL = 4,
    SIG_TRAP = 5,
    SIG_ABORT = 6,
    SIG_BUS_ERROR = 7,
    SIG_FLOATING_POINT_EXCEPTION = 8,
    SIG_KILL = 9,
    SIG_USER1 = 10,
    SIG_SEGMENTATION_FAULT = 11,
    SIG_USER2 = 12,
    SIG_PIPE = 13,
    SIG_ALARM = 14,
    SIG_TERMINATE = 15,
    SIG_STACK_FAULT = 16,
    SIG_CHILD = 17,
    SIG_CONTINUE = 18,
    SIG_STOP = 19,
    SIG_KEYBOARD_STOP = 20,
    SIG_BACKGROUND_READ = 21,
    SIG_BACKGROUND_WRITE = 22,
    SIG_URGENT = 23,
    SIG_CPU_LIMIT_EXCEEDED = 24,
    SIG_FILE_SIZE_LIMIT_EXCEEDED = 25,
    SIG_VIRTUAL_ALARM = 26,
    SIG_PROFILING = 27,
    SIG_WINDOW_CHANGED = 28,
    SIG_IO = 29,
    SIG_POWER_FAILURE = 30,
    SIG_SYSTEM_CALL = 31,
} signal_enum_t;

typedef int pid_t;

typedef struct signal_s{
    uint64_t trigger_time;
    void (*signal_handler)(int);
    uint64_t delay_ms;
    int signum;
    int count;
} signal_t;

typedef struct process_s {
    pid_t pid;
    signal_t signals[MAX_SIGNALS];
} process_t;

typedef struct process_node_s {
    process_t *process;
    struct process_node_s *next;
} process_node_t;

extern process_node_t *current_process;
extern process_node_t *process_list;
extern volatile uint64_t tick_count;

void trigger_signal(int signum);
void init_signals();
void signal(int signum, void (*signal_handler)(int));
void schedule_repeat_signal(int signum, uint64_t delay_ms, int count);
void schedule_signal(int signum, uint64_t delay_ms);


#endif
