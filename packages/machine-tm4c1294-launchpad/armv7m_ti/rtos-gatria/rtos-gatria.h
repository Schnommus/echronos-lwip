#ifndef RTOS_GATRIA_H
#define RTOS_GATRIA_H





#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdint.h>
#include <stdint.h>






{{#mutexes.length}}
typedef uint8_t {{prefix_type}}MutexId;
{{/mutexes.length}}
typedef uint8_t {{prefix_type}}ErrorId;
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;










#define {{prefix_const}}REENTRANT {{reentrant}}




{{#mutexes}}
#define {{prefix_const}}MUTEX_ID_{{name|u}} (({{prefix_type}}MutexId) UINT8_C({{idx}}))
{{/mutexes}}

#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}



















#ifdef __cplusplus
extern "C" {
#endif





{{#mutexes.length}}
void {{prefix_func}}mutex_lock({{prefix_type}}MutexId) {{prefix_const}}REENTRANT;
bool {{prefix_func}}mutex_try_lock({{prefix_type}}MutexId);
void {{prefix_func}}mutex_unlock({{prefix_type}}MutexId);
{{/mutexes.length}}

{{prefix_type}}TaskId {{prefix_func}}task_current(void);
void {{prefix_func}}yield_to({{prefix_type}}TaskId) {{prefix_const}}REENTRANT;
void {{prefix_func}}yield(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}block(void) {{prefix_const}}REENTRANT;
void {{prefix_func}}unblock({{prefix_type}}TaskId);
void {{prefix_func}}start(void);
#ifdef __cplusplus
}
#endif

#endif /* RTOS_GATRIA_H */