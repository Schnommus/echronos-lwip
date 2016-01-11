#ifndef SYSTEM_STATUS_H
#define SYSTEM_STATUS_H

typedef struct _system_status {
    int brightness;
    int power_on;
} system_status_t;

extern system_status_t system_status;

#endif
