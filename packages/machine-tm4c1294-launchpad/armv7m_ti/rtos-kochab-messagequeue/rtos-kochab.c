
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


#include <stdbool.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>
{{#interrupt_events.length}}
#include <stdint.h>
#include <stdbool.h>
#include "bitband.h"
{{/interrupt_events.length}}
#include <stdbool.h>




#include <stdint.h>
#include <stdint.h>
#include "rtos-kochab.h"


#define PREEMPTION_SUPPORT

/* To support preemption of tasks by exception handlers, we define a context stack frame that includes the exception
 * frame automatically stored onto the stack by the CPU when it takes an exception.
 *
 * The ARM stack grows downwards, from high to low addresses.
 * Therefore, the context stack frame entries with the lowest index numbers are the last to be pushed to the stack.
 * Note that the Procedure Call Standard for the ARM Architecture (ARM IHI 0042E, ABI release 2.09) says that at a
 * public interface, the stack must be double-word (8-byte) aligned.
 * Please see http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/index.html for more details.
 *
 * The lower-address half of the context stack frame is for register values and task state that the RTOS must manually
 * push onto the stack before it switches context. */

#define CONTEXT_PREEMPT_DISABLED 0
#define CONTEXT_R4_IDX 1
#define CONTEXT_R5_IDX 2
#define CONTEXT_R6_IDX 3
#define CONTEXT_R7_IDX 4
#define CONTEXT_R8_IDX 5
#define CONTEXT_R9_IDX 6
#define CONTEXT_R10_IDX 7
#define CONTEXT_R11_IDX 8
#define CONTEXT_EXCEPTION_RETURN_IDX 9
#define CONTEXT_S16_IDX 10
#define CONTEXT_S17_IDX 11
#define CONTEXT_S18_IDX 12
#define CONTEXT_S19_IDX 13
#define CONTEXT_S20_IDX 14
#define CONTEXT_S21_IDX 15
#define CONTEXT_S22_IDX 16
#define CONTEXT_S23_IDX 17
#define CONTEXT_S24_IDX 18
#define CONTEXT_S25_IDX 19
#define CONTEXT_S26_IDX 20
#define CONTEXT_S27_IDX 21
#define CONTEXT_S28_IDX 22
#define CONTEXT_S29_IDX 23
#define CONTEXT_S30_IDX 24
#define CONTEXT_S31_IDX 25

/* The higher-address half of the context stack frame is the exception frame automatically pushed by the CPU.
 * We generally assume this is pushed and popped correctly by the CPU on exception entry and return, and only really
 * manipulate parts of this region for the setting up of initial task states. */

#define CONTEXT_R0_IDX 26
#define CONTEXT_R1_IDX 27
#define CONTEXT_R2_IDX 28
#define CONTEXT_R3_IDX 29
#define CONTEXT_IP_IDX 30
#define CONTEXT_LR_IDX 31
#define CONTEXT_PC_IDX 32
#define CONTEXT_PSR_IDX 33

#define CONTEXT_NONFP_SIZE 34

/* The very highest portion of the exception frame is only pushed/popped by the CPU if floating-point is enabled.
 * Since we set the initial task state to have floating-point disabled, we exclude these entries when creating the
 * initial context stack frames for each task, and generally don't interfere with this region subsequently.
 * All we must do is ensure that the EXC_RETURN is set correctly for the destination task on exception return. */

#define CONTEXT_S0_IDX 34
#define CONTEXT_S1_IDX 35
#define CONTEXT_S2_IDX 36
#define CONTEXT_S3_IDX 37
#define CONTEXT_S4_IDX 38
#define CONTEXT_S5_IDX 39
#define CONTEXT_S6_IDX 40
#define CONTEXT_S7_IDX 41
#define CONTEXT_S8_IDX 42
#define CONTEXT_S9_IDX 43
#define CONTEXT_S10_IDX 44
#define CONTEXT_S11_IDX 45
#define CONTEXT_S12_IDX 46
#define CONTEXT_S13_IDX 47
#define CONTEXT_S14_IDX 48
#define CONTEXT_S15_IDX 49
#define CONTEXT_FPSCR_IDX 50
#define CONTEXT_ALIGNER_IDX 51

#define CONTEXT_FP_SIZE 52

/* Definitions for ARM-specific initialization.
 *
 * We define the SVCALL priority to be a higher level (i.e. lower numerical value) than PENDSV so that when we disable
 * preemption by setting BASEPRI to PENDSV's priority, PENDSV is disabled but SVCALL remains enabled.
 * This reflects our desire for the RTOS to be able to manually yield (via svc) when preemption is disabled.
 *
 * Some platforms do not implement the low 4 bits of priority.
 * Thus, we choose priority values that are distinct regardless of the lower 4 bits. */

//#define SVCALL_PRIORITY 224u
//#define PENDSV_PRIORITY 240u

// On this TI device, there are only 3 bits for priority

#define SVCALL_PRIORITY 192u
#define PENDSV_PRIORITY 224u


/* System Handler Priority Registers (SHPR) */
#define SHPR2_PHYSADDR 0xE000ED1C
#define SHPR3_PHYSADDR 0xE000ED20

#define SHPR2_SVCALL_PRIO_MASK 0x00ffffff
#define SHPR2_SVCALL_PRIO_OFFSET 24

#define SHPR3_PENDSV_PRIO_MASK 0xff00ffff
#define SHPR3_PENDSV_PRIO_OFFSET 16

/* Execution Program Status Register (EPSR)
 * T-bit is the Thumb state bit, which must be 1 for the Cortex-M4 because it only supports Thumb instructions. */
#define EPSR_THUMB_BIT_OFFSET 24

/* For EXC_RETURN of the first task, we choose this value, which means "Return to Thread mode, exception return uses
 * non-floating-point state from MSP and execution uses MSP after return" according to the Cortex-M4 Devices Generic
 * User Guide (ARM DUI 0553A).
 * Please see http://infocenter.arm.com/help/topic/com.arm.doc.dui0553a/index.html for more details. */
#define EXC_RETURN_INITIAL_TASK 0xfffffff9
#define SCHED_INDEX_ZERO ((SchedIndex) {{prefix_const}}TASK_ID_ZERO)


{{#timers.length}}
#define TIMER_ID_ZERO (({{prefix_type}}TimerId) UINT8_C(0))
#define TIMER_ID_MAX (({{prefix_type}}TimerId) UINT8_C({{timers.length}} - 1U))
{{/timers.length}}
#define interrupt_event rtos_internal_interrupt_event
#define interrupt_event_bitband rtos_internal_interrupt_event_bitband


{{#mutexes.length}}
#define MUTEX_ID_NONE ((MutexIdOption) UINT8_MAX)
{{/mutexes.length}}
#define SEM_ID_NONE ((SemIdOption) UINT8_MAX)
#define SEM_VALUE_ZERO (({{prefix_type}}SemValue) UINT{{semaphore_value_size}}_C(0))
{{#semaphore_enable_max}}
#define SEM_VALUE_MAX (({{prefix_type}}SemValue) UINT{{semaphore_value_size}}_MAX)
{{/semaphore_enable_max}}
#define MESSAGE_QUEUE_ID_NONE ((MessageQueueIdOption)UINT8_C(255))
#define ERROR_ID_NONE (({{prefix_type}}ErrorId) UINT8_C(0))
#define ERROR_ID_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(1))
#define ERROR_ID_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(2))
#define ERROR_ID_NOT_HOLDING_MUTEX (({{prefix_type}}ErrorId) UINT8_C(3))
#define ERROR_ID_DEADLOCK (({{prefix_type}}ErrorId) UINT8_C(4))
#define ERROR_ID_TASK_FUNCTION_RETURNS (({{prefix_type}}ErrorId) UINT8_C(5))
#define ERROR_ID_INTERNAL_CURRENT_TASK_INVALID (({{prefix_type}}ErrorId) UINT8_C(6))
#define ERROR_ID_INTERNAL_INVALID_ID (({{prefix_type}}ErrorId) UINT8_C(7))
#define ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP (({{prefix_type}}ErrorId) UINT8_C(8))
#define ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(9))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT (({{prefix_type}}ErrorId) UINT8_C(10))
#define ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER (({{prefix_type}}ErrorId) UINT8_C(11))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW (({{prefix_type}}ErrorId) UINT8_C(12))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION (({{prefix_type}}ErrorId) UINT8_C(13))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION (({{prefix_type}}ErrorId) UINT8_C(14))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD (({{prefix_type}}ErrorId) UINT8_C(15))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE (({{prefix_type}}ErrorId) UINT8_C(16))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS (({{prefix_type}}ErrorId) UINT8_C(17))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES (({{prefix_type}}ErrorId) UINT8_C(18))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED (({{prefix_type}}ErrorId) UINT8_C(19))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER (({{prefix_type}}ErrorId) UINT8_C(20))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE (({{prefix_type}}ErrorId) UINT8_C(21))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH (({{prefix_type}}ErrorId) UINT8_C(22))
#define ERROR_ID_INTERNAL_PRECONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(23))
#define ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED (({{prefix_type}}ErrorId) UINT8_C(24))
#define ERROR_ID_SEMAPHORE_MAX_INVALID (({{prefix_type}}ErrorId) UINT8_C(25))
#define ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT (({{prefix_type}}ErrorId) UINT8_C(26))
#define ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT (({{prefix_type}}ErrorId) UINT8_C(27))
#define ERROR_ID_SEMAPHORE_MAX_EXCEEDED (({{prefix_type}}ErrorId) UINT8_C(28))
#define ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED (({{prefix_type}}ErrorId) UINT8_C(29))
#define ERROR_ID_SCHED_PRIO_CEILING_TASK_LOCKING_LOWER_PRIORITY_MUTEX (({{prefix_type}}ErrorId) UINT8_C(30))
#define ERROR_ID_SCHED_PRIO_CEILING_MUTEX_ALREADY_LOCKED (({{prefix_type}}ErrorId) UINT8_C(31))
#define TASK_ID_NONE ((TaskIdOption) UINT{{taskid_size}}_MAX)
#define current_task rtos_internal_current_task
#define tasks rtos_internal_tasks

typedef uint32_t* context_t;
typedef {{prefix_type}}TaskId SchedIndex;
typedef uint16_t TicksTimeout;
typedef {{prefix_type}}MutexId MutexIdOption;
typedef {{prefix_type}}SemId SemIdOption;
typedef uint8_t MessageQueueIdOption;
typedef {{prefix_type}}TaskId TaskIdOption;



struct sched_task {
    TaskIdOption blocked_on;
};

/*
 * NOTE: An RTOS variant using the scheduler must ensure that tasks
 * array is sorted by priority.
 */
struct sched {
    struct sched_task tasks[{{tasks.length}}];
};
struct signal_task {
    {{prefix_type}}SignalSet signals;
};

struct signal {
    struct signal_task tasks[{{tasks.length}}];
};

{{#timers.length}}
struct timer
{
    bool enabled;
    bool overflow;
    TicksTimeout expiry;
    {{prefix_type}}TicksRelative reload;

    /*
     * when error_id is not ERROR_ID_NONE, the timer calls
     * the application error function with this error_id.
     */
    {{prefix_type}}ErrorId error_id;

    {{prefix_type}}TaskId task_id;
    {{prefix_type}}SignalSet signal_set;
};
{{/timers.length}}


struct interrupt_event_handler {
    {{prefix_type}}TaskId task;
    {{prefix_type}}SignalSet sig_set;
};

struct mutex {
    TaskIdOption holder;
};
{{#mutex.stats}}
struct mutex_stat {
    uint32_t mutex_lock_counter;
    uint32_t mutex_lock_contended_counter;
    {{prefix_type}}TicksRelative mutex_lock_max_wait_time;
};
{{/mutex.stats}}
struct semaphore {
    {{prefix_type}}SemValue value;
{{#semaphore_enable_max}}
    {{prefix_type}}SemValue max;
{{/semaphore_enable_max}}
};
/* representation of a message queue instance
 * sorted by size of fields */
struct message_queue
{
    /* pointer to the array holding the message data
     * the array contains message_size * queue_length bytes */
    uint8_t *messages;
    /* size of each message in bytes */
    const uint8_t message_size;
    /* maximum number of messages this queue can hold */
    const uint8_t queue_length;
    /* index of the oldest message that has been put into the queue but not yet been retrieved
     * 0 <= head < queue_length */
    uint8_t head;
    /* number of messages that have been put into the queue but not yet been retrieved
     * 0 <= available < queue_length */
    uint8_t available;
};

struct task
{
    context_t ctx;
};



extern void rtos_internal_context_switch_first(context_t *);
extern void rtos_internal_task_entry_trampoline(void);
extern bool rtos_internal_check_preempt_disabled(void);
extern void rtos_internal_yield(void);
extern void rtos_internal_preempt_enable(void);
extern void rtos_internal_preempt_disable(void);
extern void rtos_internal_preempt_pend(void);










extern void {{fatal_error}}({{prefix_type}}ErrorId error_id);
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}


/**
 * Platform-specific initialization for the preemption implementation called once at RTOS start time.
 */
static void preempt_init(void);

/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
 *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void context_init(context_t *ctx, void (*fn)(void), uint32_t *stack_base, size_t stack_size);
static void sched_set_runnable(const {{prefix_type}}TaskId task_id);
static void sched_set_blocked_on(const {{prefix_type}}TaskId task_id, const {{prefix_type}}TaskId blocker);
static TaskIdOption sched_get_next(void);
static {{prefix_type}}SignalSet signal_recv({{prefix_type}}SignalSet *pending_signals, {{prefix_type}}SignalSet requested_signals);
static void signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);
static {{prefix_type}}SignalSet signal_wait_set_blocked_on({{prefix_type}}SignalSet requested_signals,
        {{prefix_type}}TaskId blocker) {{prefix_const}}REENTRANT;

static uint8_t timer_pending_ticks_get_and_clear_atomically(void);
{{#timers.length}}
static void timer_process_one(struct timer *timer);
static void timer_enable({{prefix_type}}TimerId timer_id);
static void timer_oneshot({{prefix_type}}TimerId timer_id, {{prefix_type}}TicksRelative timeout);
{{/timers.length}}
static void timer_tick_process(void);
static void interrupt_event_process(void);
static inline bool interrupt_application_event_check(void);
static inline void interrupt_event_wait(void);
{{prefix_type}}TaskId rtos_internal_interrupt_event_get_next(void);
{{#interrupt_events.length}}
static void interrupt_event_handle({{prefix_type}}InterruptEventId interrupt_event_id);
{{/interrupt_events.length}}

static bool internal_sem_try_wait(const {{prefix_type}}SemId s);


{{#internal_asserts}}
static {{prefix_type}}TaskId get_current_task_check(void);
{{/internal_asserts}}
static void block_on({{prefix_type}}TaskId blocker);
{{#mutexes.length}}
static void mutex_core_block_on_timeout({{prefix_type}}TaskId t, {{prefix_type}}TicksRelative ticks);
{{/mutexes.length}}
static void sem_core_block_timeout({{prefix_type}}TicksRelative ticks);
static void unblock({{prefix_type}}TaskId task);

{{#tasks}}
static uint32_t stack_{{idx}}[{{stack_size}}] __attribute__((aligned(8)));
{{/tasks}}

static struct sched sched_tasks;
static struct signal signal_tasks;
static volatile uint8_t timer_pending_ticks;
{{prefix_type}}TicksAbsolute {{prefix_func}}timer_current_ticks;
{{#timers.length}}
static struct timer timers[{{timers.length}}] = {
{{#timers}}
    {
        {{#enabled}}true{{/enabled}}{{^enabled}}false{{/enabled}},
        false,
        {{#enabled}}{{reload}}{{/enabled}}{{^enabled}}0{{/enabled}},
        {{reload}},
        {{error}},
        {{#task}}{{prefix_const}}TASK_ID_{{name|u}}{{/task}}{{^task}}TASK_ID_NONE{{/task}},
        {{#sig_set}}{{prefix_const}}SIGNAL_SET_{{.|u}}{{/sig_set}}{{^sig_set}}{{prefix_const}}SIGNAL_SET_EMPTY{{/sig_set}}
    },
{{/timers}}
};
{{/timers.length}}
{{#interrupt_events.length}}
VOLATILE_BITBAND_VAR(uint32_t, rtos_internal_interrupt_event);
{{/interrupt_events.length}}
static bool system_is_idle;
{{#interrupt_events.length}}
struct interrupt_event_handler interrupt_events[{{interrupt_events.length}}] = {
{{#interrupt_events}}
    { {{prefix_const}}TASK_ID_{{task.name|u}}, {{prefix_const}}SIGNAL_SET_{{sig_set|u}} },
{{/interrupt_events}}
};
{{/interrupt_events.length}}
{{#mutexes.length}}
static struct mutex mutexes[{{mutexes.length}}] = {
{{#mutexes}}
    {TASK_ID_NONE},
{{/mutexes}}
};
static MutexIdOption mutex_waiters[{{tasks.length}}] = {
{{#tasks}}
    MUTEX_ID_NONE,
{{/tasks}}
};
{{#mutex.stats}}
bool {{prefix_func}}mutex_stats_enabled;
static struct mutex_stat mutex_stats[{{mutexes.length}}];
{{/mutex.stats}}
{{/mutexes.length}}
static struct semaphore semaphores[{{semaphores.length}}];
static SemIdOption sem_waiters[{{tasks.length}}];
{{#message_queues.length}}
{{#message_queues}}
static uint8_t message_queue_{{name}}_messages[{{queue_length}}][{{message_size}}];
{{/message_queues}}
static struct message_queue message_queues[] =
{
{{#message_queues}}
    {
        (uint8_t*)message_queue_{{name}}_messages,
{{#message_size}}
        {{message_size}},
{{/message_size}}
{{#message_type}}
        sizeof({{message_type}}),
{{/message_type}}
        {{queue_length}},
        0,
        0,
    },
{{/message_queues}}
};
static MessageQueueIdOption message_queue_waiters[] =
{
{{#tasks}}
    MESSAGE_QUEUE_ID_NONE,
{{/tasks}}
};

{{/message_queues.length}}

{{prefix_type}}TaskId rtos_internal_current_task;
struct task rtos_internal_tasks[{{tasks.length}}];
{{#timers.length}}
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};
{{/timers.length}}


#define context_switch_first(to) rtos_internal_context_switch_first(get_task_context(to))
#define yield() rtos_internal_yield()
#define preempt_disable() rtos_internal_preempt_disable()
#define preempt_enable() rtos_internal_preempt_enable()
#define preempt_pend() rtos_internal_preempt_pend()
#define precondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_preemption_enabled() internal_assert(!rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define sched_set_blocked(task_id) sched_set_blocked_on(task_id, TASK_ID_NONE)
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(sched_index)
#define sched_taskid_to_index(task_id) (SchedIndex)(task_id)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

#define signal_wait_blocked_on(requested_signals, blocker) signal_wait_set_blocked_on(requested_signals, blocker)
#define signal_wait_set(requested_signals) signal_wait_set_blocked_on(requested_signals, TASK_ID_NONE)
#define signal_wait(requested_signals) signal_wait_set(requested_signals)
#define signal_peek(pending_signals, requested_signals) (((pending_signals) & (requested_signals)) != {{prefix_const}}SIGNAL_SET_EMPTY)
#define signal_pending(task_id, mask) ((PENDING_SIGNALS(task_id) & mask) == mask)
#define PENDING_SIGNALS(task_id) signal_tasks.tasks[task_id].signals
#define timer_pending_ticks_check() ((bool)timer_pending_ticks)
{{#timers.length}}
#define timer_expired(timer, timeout) ((timer)->enabled && (timer)->expiry == timeout)
#define timer_is_periodic(timer) ((timer)->reload > 0)
#define timer_reload_set(timer_id, ticks) timers[timer_id].reload = ticks
#define timer_disable(timer_id) timers[timer_id].enabled = false
#define current_timeout() ((TicksTimeout) {{prefix_func}}timer_current_ticks)
#define TIMER_PTR(timer_id) (&timers[timer_id])
{{/timers.length}}
#define assert_timer_valid(timer) api_assert(timer_id < {{timers.length}}, ERROR_ID_INVALID_ID)

#define interrupt_event_check() (interrupt_application_event_check() || interrupt_system_event_check())
#define interrupt_system_event_check() timer_pending_ticks_check()
#define interrupt_event_get_next() rtos_internal_interrupt_event_get_next()

{{#mutexes.length}}
#define assert_mutex_valid(mutex) api_assert(mutex < {{mutexes.length}}, ERROR_ID_INVALID_ID)
{{/mutexes.length}}
#define assert_sem_valid(sem) api_assert(sem < {{semaphores.length}}, ERROR_ID_INVALID_ID)
{{#message_queues.length}}
#define message_queue_api_assert_valid(message_queue) api_assert(message_queue < {{message_queues.length}},\
                                                                 ERROR_ID_INVALID_ID)
#define message_queue_internal_assert_valid(message_queue) internal_assert(message_queue < {{message_queues.length}},\
                                                                           ERROR_ID_INVALID_ID)
{{^internal_asserts}}
#define message_queue_init() do {} while(0)
#define message_queue_invariants_check() do {} while(0)
{{/internal_asserts}}

{{/message_queues.length}}
{{^message_queues.length}}
#define message_queue_init() do {} while(0)
{{/message_queues.length}}
{{#api_asserts}}
#define api_error(error_id) {{fatal_error}}(error_id)
{{/api_asserts}}
{{^api_asserts}}
#define api_error(error_id) do { } while(0)
{{/api_asserts}}
{{#api_asserts}}
#define api_assert(expression, error_id) do { if (!(expression)) { api_error(error_id); } } while(0)
{{/api_asserts}}
{{^api_asserts}}
#define api_assert(expression, error_id) do { } while(0)
{{/api_asserts}}

{{#internal_asserts}}
#define internal_error(error_id) {{fatal_error}}(error_id)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_error(error_id) do { } while(0)
{{/internal_asserts}}
{{#internal_asserts}}
#define internal_assert(expression, error_id) do { if (!(expression)) { internal_error(error_id); } } while(0)
{{/internal_asserts}}
{{^internal_asserts}}
#define internal_assert(expression, error_id) do { } while(0)
{{/internal_asserts}}
{{#internal_asserts}}
#define get_current_task() get_current_task_check()
{{/internal_asserts}}
{{^internal_asserts}}
#define get_current_task() current_task
{{/internal_asserts}}
#define get_task_context(task_id) &tasks[task_id].ctx
#define internal_assert_task_valid(task) internal_assert(task < {{tasks.length}}, ERROR_ID_INTERNAL_INVALID_ID)
#define assert_task_valid(task) api_assert(task < {{tasks.length}}, ERROR_ID_INVALID_ID)
#define block() block_on(TASK_ID_NONE)
#define mutex_core_block_on(blocker) signal_wait_blocked_on({{prefix_const}}SIGNAL_ID__TASK_TIMER, blocker)
#define mutex_core_unblock(task) signal_send_set(task, {{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define sem_core_block() signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define sem_core_unblock(task) signal_send_set(task, {{prefix_const}}SIGNAL_ID__TASK_TIMER)

#define message_queue_core_block() {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
/* sleep() may return before the timeout occurs because another task may send the timeout signal to indicate that the
 * state of the message queue has changed.
 * Therefore, disable the timer whenever sleep() returns to make sure the timer is no longer active.
 * Note that in the current message-queue implementation, this is not necessary for correctness.
 * The message-queue implementation handles spurious timer signals gracefully.
 * However, disabling the timer avoids confusion and provides a minor benefit in run-time efficiency. */
#define message_queue_core_block_timeout(timeout)\
do\
{\
    {{prefix_func}}sleep((timeout));\
    {{prefix_func}}timer_disable(task_timers[get_current_task()]);\
}\
while (0)
#define message_queue_core_unblock(task) {{prefix_func}}signal_send((task), {{prefix_const}}SIGNAL_ID__TASK_TIMER)
// TODO: Fix the semantics of this!
#define message_queue_core_is_unblocked(task) (task == get_current_task())



/* ARM specific initialization */
static void
preempt_init(void)
{
    /* Set the priority of handlers */
    volatile uint32_t *shpr2 = (uint32_t *) SHPR2_PHYSADDR;
    volatile uint32_t *shpr3 = (uint32_t *) SHPR3_PHYSADDR;
    *shpr2 = (SVCALL_PRIORITY << SHPR2_SVCALL_PRIO_OFFSET) | (SHPR2_SVCALL_PRIO_MASK & *shpr2);
    *shpr3 = (PENDSV_PRIORITY << SHPR3_PENDSV_PRIO_OFFSET) | (SHPR3_PENDSV_PRIO_MASK & *shpr3);

    preempt_disable();
}

static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    /* We use the NONFP size because EXC_RETURN of all tasks is set initially to a non-floating-point state. */
    uint32_t *const context = stack_base + stack_size - CONTEXT_NONFP_SIZE;

    /* Start all tasks with preemption disabled by setting this field to a non-zero value. */
    context[CONTEXT_PREEMPT_DISABLED] = true;
    /* Set R4 to the task entry point, for the task entry trampoline to bounce to. */
    context[CONTEXT_R4_IDX] = (uint32_t) fn;
    context[CONTEXT_PC_IDX] = (uint32_t) rtos_internal_task_entry_trampoline;
    context[CONTEXT_PSR_IDX] = (uint32_t) (1u << EPSR_THUMB_BIT_OFFSET); /* make sure the T-bit is set! */
    context[CONTEXT_EXCEPTION_RETURN_IDX] = (uint32_t) EXC_RETURN_INITIAL_TASK;

    *ctx = context;
}
static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).blocked_on = task_id;
}

static void
sched_set_blocked_on(const {{prefix_type}}TaskId task_id, const {{prefix_type}}TaskId blocker)
{
    SCHED_OBJ(task_id).blocked_on = blocker;
}

static TaskIdOption
sched_get_next(void)
{
    /* NOTE: In the case where assume_runnable is true and no runnable
       tasks are found, then an undefined task will be returned from this
       function.
    */
    TaskIdOption task, next_task;
    SchedIndex idx;

    for (idx = SCHED_INDEX_ZERO; idx <= sched_max_index(); idx++)
    {
        task = sched_index_to_taskid(idx);
        do
        {
            next_task = SCHED_OBJ(task).blocked_on;
            if (next_task == task)
            {
                goto found;
            }
            task = next_task;
        }
        while (task != TASK_ID_NONE);
    }
found:
    return  task;
}
static {{prefix_type}}SignalSet
signal_recv({{prefix_type}}SignalSet *const pending_signals, const {{prefix_type}}SignalSet requested_signals)
{
    const {{prefix_type}}SignalSet received_signals = *pending_signals & requested_signals;

    precondition_preemption_disabled();

    *pending_signals &= ~received_signals;

    postcondition_preemption_disabled();

    return received_signals;
}

static void
signal_send_set(const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signals)
{
    precondition_preemption_disabled();

    PENDING_SIGNALS(task_id) |= signals;
    unblock(task_id);

    postcondition_preemption_disabled();
}

static {{prefix_type}}SignalSet
signal_wait_set_blocked_on(const {{prefix_type}}SignalSet requested_signals, const {{prefix_type}}TaskId blocker)
        {{prefix_const}}REENTRANT
{
    {{prefix_type}}SignalSet received_signals;

    precondition_preemption_disabled();
    {
        {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());

        if (signal_peek(*pending_signals, requested_signals))
        {
            yield();
        }
        else
        {
            do
            {
                block_on(blocker);
            } while (!signal_peek(*pending_signals, requested_signals));
        }

        received_signals = signal_recv(pending_signals, requested_signals);
    }
    postcondition_preemption_disabled();

    return received_signals;
}
static uint8_t
timer_pending_ticks_get_and_clear_atomically(void)
{
    uint8_t pending_ticks;
    asm volatile("cpsid i");
    pending_ticks = timer_pending_ticks;
    timer_pending_ticks = 0;
    asm volatile("cpsie i");
    return pending_ticks;
}
{{#timers.length}}
static void
timer_process_one(struct timer *const timer)
{
    precondition_preemption_disabled();

    if (timer_is_periodic(timer))
    {
        timer->expiry += timer->reload;
    }
    else
    {
        timer->enabled = false;
    }

    if (timer->error_id != ERROR_ID_NONE)
    {
        {{fatal_error}}(timer->error_id);
    }
    else
    {
        if (signal_pending(timer->task_id, timer->signal_set))
        {
            timer->overflow = true;
        }
        signal_send_set(timer->task_id, timer->signal_set);
    }

    postcondition_preemption_disabled();
}

static void
timer_enable(const {{prefix_type}}TimerId timer_id)
{
    precondition_preemption_disabled();

    if (timers[timer_id].reload == 0)
    {
        timer_process_one(&timers[timer_id]);
    }
    else
    {
        timers[timer_id].expiry = current_timeout() + timers[timer_id].reload;
        timers[timer_id].enabled = true;
    }

    postcondition_preemption_disabled();
}

static void
timer_oneshot(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative timeout)
{
    precondition_preemption_disabled();

    timer_reload_set(timer_id, timeout);
    timer_enable(timer_id);
    timer_reload_set(timer_id, 0);

    postcondition_preemption_disabled();
}
{{/timers.length}}

static void
timer_tick_process(void)
{
    precondition_preemption_disabled();
    {
        const uint8_t pending_ticks = timer_pending_ticks_get_and_clear_atomically();

        if (pending_ticks > 1)
        {
            {{fatal_error}}(ERROR_ID_TICK_OVERFLOW);
        }

        if (pending_ticks)
        {
            {{#timers.length}}
            {{prefix_type}}TimerId timer_id;
            struct timer *timer;
            TicksTimeout timeout;
            {{/timers.length}}

            {{prefix_func}}timer_current_ticks++;

            {{#timers.length}}
            timeout = current_timeout();

            for (timer_id = TIMER_ID_ZERO; timer_id <= TIMER_ID_MAX; timer_id++)
            {
                timer = TIMER_PTR(timer_id);
                if (timer_expired(timer, timeout))
                {
                    timer_process_one(timer);
                }
            }
            {{/timers.length}}
        }
    }
    postcondition_preemption_disabled();
}
static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    uint32_t tmp = interrupt_event;
    while (tmp != 0)
    {
        const {{prefix_type}}InterruptEventId i = __builtin_ffs(tmp) - 1;
        interrupt_event_bitband[i] = 0;
        interrupt_event_handle(i);
        tmp &= ~(1U << i);
    }
{{/interrupt_events.length}}
}

static inline bool
interrupt_application_event_check(void)
{
{{#interrupt_events.length}}
    return interrupt_event != 0;
{{/interrupt_events.length}}
{{^interrupt_events.length}}
    return false;
{{/interrupt_events.length}}
}

static inline void
interrupt_event_wait(void)
{
    asm volatile("cpsid i");
    asm volatile("isb");
    if (!interrupt_event_check())
    {
        asm volatile("wfi");
    }
    asm volatile("cpsie i");
}
{{prefix_type}}TaskId
rtos_internal_interrupt_event_get_next(void)
{
    TaskIdOption next;

    for (;;)
    {
        interrupt_event_process();
        timer_tick_process();
        next = sched_get_next();

        if (next == TASK_ID_NONE)
        {
            system_is_idle = true;
            interrupt_event_wait();
        }
        else
        {
            system_is_idle = false;
            break;
        }
    }

    internal_assert_task_valid(next);

    return next;
}
{{#interrupt_events.length}}
static void
interrupt_event_handle(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    precondition_preemption_disabled();

    internal_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INTERNAL_INVALID_ID);

#ifdef PREEMPTION_SUPPORT
    signal_send_set(interrupt_events[interrupt_event_id].task, interrupt_events[interrupt_event_id].sig_set);
#else
    {{prefix_func}}signal_send_set(interrupt_events[interrupt_event_id].task,
            interrupt_events[interrupt_event_id].sig_set);
#endif

    postcondition_preemption_disabled();
}
{{/interrupt_events.length}}
{{#mutexes.length}}
static bool
mutex_try_lock(const {{prefix_type}}MutexId m)
{
    const bool r = mutexes[m].holder == TASK_ID_NONE;

    precondition_preemption_disabled();

    if (r)
    {
        mutexes[m].holder = get_current_task();
    }

    postcondition_preemption_disabled();

    return r;
}
{{#mutex.stats}}

static void
mutex_stats_update(const {{prefix_type}}MutexId m, const bool contended, const {{prefix_type}}TicksAbsolute
        wait_start_ticks)
{
    if ({{prefix_func}}mutex_stats_enabled) {
        mutex_stats[m].mutex_lock_counter += 1;
        if (contended) {
            {{prefix_type}}TicksRelative wait_time = {{prefix_func}}timer_current_ticks - wait_start_ticks;

            mutex_stats[m].mutex_lock_contended_counter += 1;
            if (wait_time > mutex_stats[m].mutex_lock_max_wait_time)
            {
                mutex_stats[m].mutex_lock_max_wait_time = wait_time;
            }
        }
    }
}
{{/mutex.stats}}
{{/mutexes.length}}
static void
sem_init(void)
{
    {{prefix_type}}TaskId t;

    for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
    {
        sem_waiters[t] = SEM_ID_NONE;
    }
}

static bool
internal_sem_try_wait(const {{prefix_type}}SemId s)
{
    precondition_preemption_disabled();

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        return false;
    }
    else
    {
        semaphores[s].value--;
        return true;
    }

    postcondition_preemption_disabled();
}
{{#message_queues.length}}
{{#internal_asserts}}
static void
message_queue_init(void)
{
    {{prefix_type}}MessageQueueId message_queue = {{message_queues.length}} - 1;
    {{prefix_type}}TaskId task;

    /* do not use for loop to work around buggy compiler optimization when there is only one message queue */
    do
    {
        struct message_queue *mq = &message_queues[message_queue];

        internal_assert(mq->messages &&
                        mq->message_size &&
                        mq->queue_length &&
                        !mq->head &&
                        !mq->available, ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    } while (message_queue--);

    for (task = 0; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        internal_assert(message_queue_waiters[task] == MESSAGE_QUEUE_ID_NONE,\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_INCORRECT_INITIALIZATION);
    }
}

static void
message_queue_invariants_check(void)
{
    {{prefix_type}}MessageQueueId message_queue;
    {{prefix_type}}TaskId task;

{{#message_queues}}
    internal_assert(message_queues[{{idx}}].messages == (uint8_t*)message_queue_{{name}}_messages,
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGES_POINTER);
{{#message_size}}
    internal_assert(message_queues[{{idx}}].message_size == {{message_size}},
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE);
{{/message_size}}
{{#message_type}}
    internal_assert(message_queues[{{idx}}].message_size == sizeof({{message_type}}),
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_MESSAGE_SIZE);
{{/message_type}}
    internal_assert(message_queues[{{idx}}].queue_length == {{queue_length}},
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_QUEUE_LENGTH);
{{/message_queues}}

    for (message_queue = 0; message_queue < {{message_queues.length}}; message_queue += 1)
    {
        const struct message_queue *const mq = &message_queues[message_queue];

        internal_assert(mq->messages && mq->message_size && mq->queue_length,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_CONFIGURATION);
        internal_assert(mq->head < mq->queue_length, ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_HEAD);
        internal_assert(mq->available <= mq->queue_length,
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_AVAILABLE);
    }

    for (task = 0; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        message_queue = message_queue_waiters[task];

        internal_assert((message_queue < {{message_queues.length}}) || (message_queue == MESSAGE_QUEUE_ID_NONE),\
                        ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_INVALID_ID_IN_WAITERS);

        if (message_queue != MESSAGE_QUEUE_ID_NONE)
        {
            const struct message_queue *const mq = &message_queues[message_queue];

            internal_assert((mq->available == 0) || (mq->available == mq->queue_length),\
                ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TASKS_BLOCKED_DESPITE_AVAILABLE_MESSAGES);
            /*internal_assert(!message_queue_core_is_unblocked(task),\
                            ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED);*/
        }
    }

    /* The timer of the current task is expected to be disabled.
     * It is expected to be only enabled while the current task is blocked in message_queue_wait_timeout().
     * Unfortunately, we cannot make any assumptions about the relationship between the states of message queues and
     * other timers.
     * The timers of tasks depends not only on the message queue implementation but also on how other components use
     * those task timers. */
    internal_assert(!timers[task_timers[get_current_task()]].enabled,\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_TIMER_IS_ENABLED);
}

{{/internal_asserts}}
static void
message_queue_waiters_wakeup(const {{prefix_type}}MessageQueueId message_queue)
{
    {{prefix_type}}TaskId task;

    message_queue_internal_assert_valid(message_queue);

    for (task = {{prefix_const}}TASK_ID_ZERO; task <= {{prefix_const}}TASK_ID_MAX; task += 1)
    {
        if (message_queue_waiters[task] == message_queue)
        {
            message_queue_core_unblock(task);
            message_queue_waiters[task] = MESSAGE_QUEUE_ID_NONE;
        }
    }
}

static void
message_queue_wait(const {{prefix_type}}MessageQueueId message_queue) {{prefix_const}}REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block();

    message_queue_invariants_check();
}

static void
message_queue_wait_timeout(const {{prefix_type}}MessageQueueId message_queue,
                           const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    message_queue_internal_assert_valid(message_queue);
    internal_assert(timeout, ERROR_ID_MESSAGE_QUEUE_INTERNAL_ZERO_TIMEOUT);
    message_queue_invariants_check();

    message_queue_waiters[get_current_task()] = message_queue;
    message_queue_core_block_timeout(timeout);
    message_queue_waiters[get_current_task()] = MESSAGE_QUEUE_ID_NONE;

    message_queue_invariants_check();
}

/* assumptions: max length 255, no overlap of dst & src */
/* called memcopy instead of memcpy to not conflict with gcc's built-in memcpy declaration on unit test targets */
static void
memcopy(uint8_t *dst, const uint8_t *src, const uint8_t length)
{
    uint8_t *const dst_end = dst + length;

    api_assert((dst < src) || (dst >= (src + length)), ERROR_ID_MESSAGE_QUEUE_BUFFER_OVERLAP);

    while (dst < dst_end)
    {
        *dst++ = *src++;
    }
}

{{/message_queues.length}}

{{#internal_asserts}}
static {{prefix_type}}TaskId
get_current_task_check(void)
{
    internal_assert(current_task < {{tasks.length}}, ERROR_ID_INTERNAL_CURRENT_TASK_INVALID);
    return current_task;
}
{{/internal_asserts}}
{{#tasks}}
static void
entry_{{name}}(void)
{
    precondition_preemption_disabled();

    preempt_enable();
    {{function}}();
}
{{/tasks}}

static void
block_on(const {{prefix_type}}TaskId blocker)
{
    precondition_preemption_disabled();

    sched_set_blocked_on(get_current_task(), blocker);
    yield();

    postcondition_preemption_disabled();
}

{{#mutexes.length}}

static void
mutex_core_block_on_timeout(const {{prefix_type}}TaskId t, const {{prefix_type}}TicksRelative ticks)
{
    precondition_preemption_disabled();

    timer_oneshot(task_timers[get_current_task()], ticks);
    mutex_core_block_on(t);
    timer_disable(task_timers[get_current_task()]);

    postcondition_preemption_disabled();
}
{{/mutexes.length}}

static void
sem_core_block_timeout(const {{prefix_type}}TicksRelative ticks)
{
    precondition_preemption_disabled();

    timer_oneshot(task_timers[get_current_task()], ticks);
    sem_core_block();
    timer_disable(task_timers[get_current_task()]);

    postcondition_preemption_disabled();
}

static void
unblock(const {{prefix_type}}TaskId task)
{
    precondition_preemption_disabled();

    sched_set_runnable(task);

    /* Note: When preemption is enabled a yield should be forced as a higher priority task may have been scheduled. */
    preempt_pend();

    postcondition_preemption_disabled();
}




{{prefix_type}}SignalSet
{{prefix_func}}signal_wait_set(const {{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT
{
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    received_signals = signal_wait_set(requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_poll_set(const {{prefix_type}}SignalSet requested_signals)
{
    {{prefix_type}}SignalSet *const pending_signals = &PENDING_SIGNALS(get_current_task());
    {{prefix_type}}SignalSet received_signals;

    preempt_disable();

    received_signals = signal_recv(pending_signals, requested_signals);

    preempt_enable();

    return received_signals;
}

{{prefix_type}}SignalSet
{{prefix_func}}signal_peek_set(const {{prefix_type}}SignalSet requested_signals)
{
    const {{prefix_type}}SignalSet pending_signals = PENDING_SIGNALS(get_current_task());
    return pending_signals & requested_signals;
}

void
{{prefix_func}}signal_send_set(const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signals)
{
    assert_task_valid(task_id);

    preempt_disable();

    signal_send_set(task_id, signals);

    preempt_enable();
}
void
{{prefix_func}}timer_tick(void)
{
    if (timer_pending_ticks < 2)
    {
        timer_pending_ticks += 1;
    }
}
void
{{prefix_func}}sleep(const {{prefix_type}}TicksRelative ticks) {{prefix_const}}REENTRANT
{
    preempt_disable();

    timer_oneshot(task_timers[get_current_task()], ticks);
    signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER);

    preempt_enable();
}

{{#timers.length}}
void
{{prefix_func}}timer_enable(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_enable(timer_id);

    preempt_enable();
}

void
{{prefix_func}}timer_disable(const {{prefix_type}}TimerId timer_id)
{
    assert_timer_valid(timer_id);

    timer_disable(timer_id);
}

void
{{prefix_func}}timer_oneshot(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative timeout)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timer_oneshot(timer_id, timeout);

    preempt_enable();
}

bool
{{prefix_func}}timer_check_overflow(const {{prefix_type}}TimerId timer_id)
{
    bool r;

    assert_timer_valid(timer_id);

    preempt_disable();

    r = timers[timer_id].overflow;
    timers[timer_id].overflow = false;

    preempt_enable();

    return r;
}

{{prefix_type}}TicksRelative
{{prefix_func}}timer_remaining(const {{prefix_type}}TimerId timer_id)
{
    {{prefix_type}}TicksRelative remaining;

    assert_timer_valid(timer_id);

    preempt_disable();

    remaining = timers[timer_id].enabled ? timers[timer_id].expiry - current_timeout() : 0;

    preempt_enable();

    return remaining;
}

/* Configuration functions */
void
{{prefix_func}}timer_reload_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TicksRelative reload)
{
    assert_timer_valid(timer_id);

    timer_reload_set(timer_id, reload);
}

void
{{prefix_func}}timer_signal_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}TaskId task_id, const {{prefix_type}}SignalSet signal_set)
{
    assert_timer_valid(timer_id);

    preempt_disable();

    timers[timer_id].error_id = ERROR_ID_NONE;
    timers[timer_id].task_id = task_id;
    timers[timer_id].signal_set = signal_set;

    preempt_enable();
}

void
{{prefix_func}}timer_error_set(const {{prefix_type}}TimerId timer_id, const {{prefix_type}}ErrorId error_id)
{
    assert_timer_valid(timer_id);

    timers[timer_id].error_id = error_id;
}
{{/timers.length}}
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_raise(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    interrupt_event_bitband[interrupt_event_id] = 1;
}
{{/interrupt_events.length}}


{{#mutexes.length}}
void
{{prefix_func}}mutex_lock(const {{prefix_type}}MutexId m) {{prefix_const}}REENTRANT
{
{{#mutex.stats}}
    bool contended = false;
    const {{prefix_type}}TicksAbsolute wait_start_ticks = {{prefix_func}}timer_current_ticks;

{{/mutex.stats}}
    assert_mutex_valid(m);
    api_assert(mutexes[m].holder != get_current_task(), ERROR_ID_DEADLOCK);

    preempt_disable();

    while (!mutex_try_lock(m))
    {
{{#mutex.stats}}
        contended = true;
{{/mutex.stats}}
        mutex_waiters[get_current_task()] = m;
        mutex_core_block_on(mutexes[m].holder);
    }

    preempt_enable();

{{#mutex.stats}}
    mutex_stats_update(m, contended, wait_start_ticks);
{{/mutex.stats}}
}

bool
{{prefix_func}}mutex_lock_timeout(const {{prefix_type}}MutexId m, const {{prefix_type}}TicksRelative timeout)
        {{prefix_const}}REENTRANT
{
    bool ret;
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;
{{#mutex.stats}}
    bool contended = false;

{{/mutex.stats}}
    assert_mutex_valid(m);
    api_assert(mutexes[m].holder != get_current_task(), ERROR_ID_DEADLOCK);

    preempt_disable();

    ret = mutex_try_lock(m);
{{#mutex.stats}}
    if (!ret) {
        contended = true;
    }
{{/mutex.stats}}
    while (!ret && absolute_timeout > {{prefix_func}}timer_current_ticks) {
        mutex_waiters[get_current_task()] = m;
        mutex_core_block_on_timeout(mutexes[m].holder, absolute_timeout - {{prefix_func}}timer_current_ticks);
        ret = mutex_try_lock(m);
    }

    preempt_enable();

{{#mutex.stats}}
    mutex_stats_update(m, contended, absolute_timeout - timeout);
{{/mutex.stats}}

    return ret;
}

void
{{prefix_func}}mutex_unlock(const {{prefix_type}}MutexId m)
{
    {{prefix_type}}TaskId t;

    assert_mutex_valid(m);
    api_assert(mutexes[m].holder == get_current_task(), ERROR_ID_NOT_HOLDING_MUTEX);

    preempt_disable();


    for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
    {
        if (mutex_waiters[t] == m)
        {
            mutex_waiters[t] = MUTEX_ID_NONE;
            mutex_core_unblock(t);
        }
    }

    mutexes[m].holder = TASK_ID_NONE;

    preempt_enable();
}

bool
{{prefix_func}}mutex_try_lock(const {{prefix_type}}MutexId m)
{
    bool r;

    assert_mutex_valid(m);

    preempt_disable();

    r = mutex_try_lock(m);

    preempt_enable();

    return r;
}

/* A macro implementation would be preferable to eliminate function call overhead when compilers don't support implicit
 * inlining, but at present this would involve exposing too many implementation internals in the public API header. */
bool
{{prefix_func}}mutex_holder_is_current(const {{prefix_type}}MutexId m)
{
    assert_mutex_valid(m);
    return mutexes[m].holder == get_current_task();
}

{{#mutex.stats}}
void {{prefix_func}}mutex_stats_clear(void)
{
    /* memset would be preferable, but string.h is not available on all platforms */
    uint8_t mutex_index;
    for (mutex_index = 0; mutex_index < {{mutexes.length}}; mutex_index += 1)
    {
        mutex_stats[mutex_index].mutex_lock_counter = 0;
        mutex_stats[mutex_index].mutex_lock_contended_counter = 0;
        mutex_stats[mutex_index].mutex_lock_max_wait_time = 0;
    }
}
{{/mutex.stats}}
{{/mutexes.length}}
void
{{prefix_func}}sem_wait(const {{prefix_type}}SemId s) {{prefix_const}}REENTRANT
{
    assert_sem_valid(s);

    preempt_disable();

    while (!internal_sem_try_wait(s))
    {
        sem_waiters[get_current_task()] = s;
        sem_core_block();
    }

    preempt_enable();
}

bool
{{prefix_func}}sem_wait_timeout(const {{prefix_type}}SemId s, const {{prefix_type}}TicksRelative timeout)
        {{prefix_const}}REENTRANT
{
    bool ret;
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    assert_sem_valid(s);

    preempt_disable();

    while (!(ret = internal_sem_try_wait(s)) && absolute_timeout > {{prefix_func}}timer_current_ticks) {
        sem_waiters[get_current_task()] = s;
        sem_core_block_timeout(absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    preempt_enable();

    return ret;
}

void
{{prefix_func}}sem_post(const {{prefix_type}}SemId s)
{
    {{prefix_type}}TaskId t;

    assert_sem_valid(s);

    preempt_disable();

{{#semaphore_enable_max}}
    api_assert(semaphores[s].max != SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_USE_BEFORE_INIT);

    if (semaphores[s].value >= semaphores[s].max) {
        {{fatal_error}}(ERROR_ID_SEMAPHORE_MAX_EXCEEDED);
    }
{{/semaphore_enable_max}}

    if (semaphores[s].value == SEM_VALUE_ZERO)
    {
        for (t = {{prefix_const}}TASK_ID_ZERO; t <= {{prefix_const}}TASK_ID_MAX; t++)
        {
            if (sem_waiters[t] == s)
            {
                sem_waiters[t] = SEM_ID_NONE;
                sem_core_unblock(t);
            }
        }
    }

    semaphores[s].value++;

    preempt_enable();
}

bool
{{prefix_func}}sem_try_wait(const {{prefix_type}}SemId s)
{
    bool r;

    assert_sem_valid(s);

    preempt_disable();
    r = internal_sem_try_wait(s);
    preempt_enable();

    return r;
}

{{#semaphore_enable_max}}
void
{{prefix_func}}sem_max_init(const {{prefix_type}}SemId s, const {{prefix_type}}SemValue max)
{
    assert_sem_valid(s);

    api_assert(max != SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_INVALID);
    api_assert(semaphores[s].max == SEM_VALUE_ZERO, ERROR_ID_SEMAPHORE_MAX_ALREADY_INIT);

    semaphores[s].max = max;
}
{{/semaphore_enable_max}}
{{#message_queues.length}}
void
{{prefix_func}}message_queue_put(const {{prefix_type}}MessageQueueId message_queue, const void *const message)
        {{prefix_const}}REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!{{prefix_func}}message_queue_try_put(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_put(const {{prefix_type}}MessageQueueId message_queue, const void *message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == mq->queue_length)
        {
            return false;
        }
        else
        {
            const uint8_t buffer_index = (mq->head + mq->available) % mq->queue_length;
            const uint16_t buffer_offset = buffer_index * mq->message_size;
            memcopy(&mq->messages[buffer_offset], message, mq->message_size);
            mq->available += 1;

            if (mq->available == 1)
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
{{prefix_func}}message_queue_put_timeout(const {{prefix_type}}MessageQueueId message_queue, const void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == message_queues[message_queue].queue_length) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_put(message_queue, message);
}

void
{{prefix_func}}message_queue_get(const {{prefix_type}}MessageQueueId message_queue, void *const message)
        {{prefix_const}}REENTRANT
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);

    while (!{{prefix_func}}message_queue_try_get(message_queue, message))
    {
        message_queue_wait(message_queue);
    }
}

bool
{{prefix_func}}message_queue_try_get(const {{prefix_type}}MessageQueueId message_queue, void *message)
{
    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    message_queue_invariants_check();

    {
        struct message_queue *const mq = &message_queues[message_queue];

        if (mq->available == 0)
        {
            return false;
        }
        else
        {
            const uint16_t buffer_offset = mq->head * mq->message_size;
            memcopy((uint8_t*)message, &mq->messages[buffer_offset], mq->message_size);
            mq->head = (mq->head + 1) % mq->queue_length;
            mq->available -= 1;

            if (mq->available == ({{message_queues.length}} - 1))
            {
                message_queue_waiters_wakeup(message_queue);
            }

            message_queue_invariants_check();
            return true;
        }
    }
}

bool
{{prefix_func}}message_queue_get_timeout(const {{prefix_type}}MessageQueueId message_queue, void *const message,
                                         const {{prefix_type}}TicksRelative timeout) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TicksAbsolute absolute_timeout = {{prefix_func}}timer_current_ticks + timeout;

    message_queue_api_assert_valid(message_queue);
    api_assert(message, ERROR_ID_MESSAGE_QUEUE_INVALID_POINTER);
    api_assert(timeout, ERROR_ID_MESSAGE_QUEUE_ZERO_TIMEOUT);
    internal_assert({{prefix_func}}timer_current_ticks < (UINT32_MAX - timeout),\
                    ERROR_ID_MESSAGE_QUEUE_INTERNAL_TICK_OVERFLOW);
    message_queue_invariants_check();

    while ((message_queues[message_queue].available == 0) &&
            (absolute_timeout > {{prefix_func}}timer_current_ticks))
    {
        message_queue_wait_timeout(message_queue, absolute_timeout - {{prefix_func}}timer_current_ticks);
    }

    return {{prefix_func}}message_queue_try_get(message_queue, message);
}

{{/message_queues.length}}

{{prefix_type}}TaskId
{{prefix_func}}task_current(void)
{
    return get_current_task();
}
void
{{prefix_func}}start(void)
{
    sem_init();
    preempt_init();
    message_queue_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first({{prefix_const}}TASK_ID_ZERO);
}
