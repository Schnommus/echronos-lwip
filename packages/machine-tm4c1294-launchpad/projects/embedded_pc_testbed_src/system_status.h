#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

typedef struct _system_status {
    int power_on;
    int current_baud_rate;
} system_status_t;

extern system_status_t system_status;

#endif
