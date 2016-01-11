#ifndef RTOS_ACAMAR_H
#define RTOS_ACAMAR_H




#include <stdint.h>
#include <stdint.h>




typedef uint8_t {{prefix_type}}ErrorId;
typedef uint{{taskid_size}}_t {{prefix_type}}TaskId;






#define {{prefix_const}}REENTRANT {{reentrant}}




#define {{prefix_const}}TASK_ID_ZERO (({{prefix_type}}TaskId) UINT{{taskid_size}}_C(0))
#define {{prefix_const}}TASK_ID_MAX (({{prefix_type}}TaskId)UINT{{taskid_size}}_C({{tasks.length}} - 1))
{{#tasks}}
#define {{prefix_const}}TASK_ID_{{name|u}} (({{prefix_type}}TaskId) UINT{{taskid_size}}_C({{idx}}))
{{/tasks}}












#ifdef __cplusplus
extern "C" {
#endif

void {{prefix_func}}yield_to({{prefix_type}}TaskId) {{prefix_const}}REENTRANT;
void {{prefix_func}}start(void);



{{prefix_type}}TaskId {{prefix_func}}task_current(void);
#ifdef __cplusplus
}
#endif

#endif /* RTOS_ACAMAR_H */