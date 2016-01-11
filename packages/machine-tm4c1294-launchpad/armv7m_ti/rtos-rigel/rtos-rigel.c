
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
#include <stdint.h>
#include "rtos-rigel.h"


#define CONTEXT_SIZE 10
#define CONTEXT_V1_IDX 0
#define CONTEXT_V2_IDX 1
#define CONTEXT_V3_IDX 2
#define CONTEXT_V4_IDX 3
#define CONTEXT_V5_IDX 4
#define CONTEXT_V6_IDX 5
#define CONTEXT_V7_IDX 6
#define CONTEXT_V8_IDX 7
#define CONTEXT_IP_IDX 8
#define CONTEXT_PC_IDX 9




{{#timers.length}}
#define TIMER_ID_ZERO (({{prefix_type}}TimerId) UINT8_C(0))
#define TIMER_ID_MAX (({{prefix_type}}TimerId) UINT8_C({{timers.length}} - 1U))
{{/timers.length}}
#define interrupt_event rtos_internal_interrupt_event
#define interrupt_event_bitband rtos_internal_interrupt_event_bitband


{{#mutexes.length}}
#define MUTEX_ID_NONE ((MutexIdOption) UINT8_MAX)
{{/mutexes.length}}

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
typedef uint8_t MessageQueueIdOption;
typedef {{prefix_type}}TaskId TaskIdOption;




struct sched_task {
    bool runnable;
};

struct sched {
    SchedIndex cur; /* The index of the currently scheduled task */
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



extern void rtos_internal_context_switch(context_t *, context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_context_switch_first(context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_trampoline(void);











extern void {{fatal_error}}({{prefix_type}}ErrorId error_id);
{{#tasks}}
extern void {{function}}(void);
{{/tasks}}



/**
 * Set up the initial execution context of a task.
 * This function is invoked exactly once for each task in the system.
  *
 * @param ctx An output parameter interpreted by the RTOS as the initial context for each task.
 *  After this function returns, the RTOS uses the value of ctx for task/context/stack switching.
 *  The concept of a context and of the context_t type is abstract and may have different implementations on
 *  different platforms.
 *  It can be, e.g., a stack pointer or a data structure for user-level task switching as on POSIX.
 *  This function is expected to set ctx to a value that the RTOS can pass to this platform's implementation of
 *  context_switch() and context_switch_first().
 *  The context must be set up such that the destination task of a task switch executes the code at the address fn
 *  using the memory region defined by stack_base and stack_size as its stack.
 *  For hardware platforms, this typically requires the following set up steps:
 *  - The value of ctx points to either the beginning or the end of the stack area.
 *  - The stack area contains fn so that the context-switch functions can pop it off the stack as a return address to
 *    begin execution at.
 *  - Optionally reserve additional stack space if the context-switch functions depend on it.
 * @param fn Points to a code address at which the given execution context shall start executing.
 *  This is typically a pointer to a parameter-less function that is assumed to never return.
 * @param stack_base Points to the lowest address of the memory area this execution context shall use as a stack.
 * @param stack_size The size in bytes of the stack memory area reserved for this execution context.
 */
static void context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size);

static void sched_set_runnable(const {{prefix_type}}TaskId task_id);
static void sched_set_blocked(const {{prefix_type}}TaskId task_id);
static TaskIdOption sched_get_next(void);
static {{prefix_type}}SignalSet signal_recv({{prefix_type}}SignalSet *pending_signals, {{prefix_type}}SignalSet requested_signals);
static void signal_send_set({{prefix_type}}TaskId task_id, {{prefix_type}}SignalSet signals);
static {{prefix_type}}SignalSet signal_wait_set({{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT;

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




{{#internal_asserts}}
static {{prefix_type}}TaskId get_current_task_check(void);
{{/internal_asserts}}
static void yield_to({{prefix_type}}TaskId to) {{prefix_const}}REENTRANT;
static void block(void) {{prefix_const}}REENTRANT;
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
{{#profiling}}
{{#profiling.task_uptime}}

static uint32_t profiling_task_uptimes[{{tasks.length}} + 1];

{{/profiling.task_uptime}}
{{/profiling}}
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
static {{prefix_type}}TimerId task_timers[{{tasks.length}}] = {
{{#tasks}}
    {{prefix_const}}TIMER_ID_{{timer.name|u}},
{{/tasks}}
};


#define context_switch(from, to) rtos_internal_context_switch(to, from)
#define context_switch_first(to) rtos_internal_context_switch_first(to)
#define preempt_disable()
#define preempt_enable()
#define preempt_clear()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define sched_runnable(task_id) (SCHED_OBJ(task_id).runnable)
#define sched_next_index(cur) (((cur) == sched_max_index()) ? 0 : ((cur) + 1))
#define sched_get_cur_index() (sched_tasks.cur)
#define sched_set_cur_index(idx) sched_tasks.cur = (idx)
#define sched_max_index() (SchedIndex)({{tasks.length}} - 1U)
#define sched_index_to_taskid(sched_index) ({{prefix_type}}TaskId)(sched_index)
#define SCHED_OBJ(task_id) sched_tasks.tasks[task_id]

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
#define yield() {{prefix_func}}yield()
#define interrupt_event_id_to_taskid(interrupt_event_id) (({{prefix_type}}TaskId)(interrupt_event_id))
#define mutex_core_block_on(unused_task) {{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__TASK_TIMER)
#define mutex_core_unblock(task) {{prefix_func}}signal_send(task, {{prefix_const}}SIGNAL_ID__TASK_TIMER)
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
#define message_queue_core_is_unblocked(task) sched_runnable((task))


static void
context_init(context_t *const ctx, void (*const fn)(void), uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *context;
    context = stack_base + stack_size - CONTEXT_SIZE;
    context[CONTEXT_V1_IDX] = (uint32_t) fn;
    context[CONTEXT_PC_IDX] = (uint32_t) rtos_internal_trampoline;
    *ctx = context;
}

static void
sched_set_runnable(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = true;
}

static void
sched_set_blocked(const {{prefix_type}}TaskId task_id)
{
    SCHED_OBJ(task_id).runnable = false;
}

static TaskIdOption
sched_get_next(void)
{
    TaskIdOption task;
    SchedIndex next = sched_get_cur_index();
    bool found = false;

    do
    {
        next = sched_next_index(next);
        found = sched_runnable(sched_index_to_taskid(next));
    } while (
        (!found)
        && (next != (sched_get_cur_index()))
        );

    if (found)
    {
        task = sched_index_to_taskid(next);
    }
    else
    {
        next = sched_max_index();
        task = TASK_ID_NONE;
    }

    sched_set_cur_index(next);

    return task;
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
signal_wait_set(const {{prefix_type}}SignalSet requested_signals) {{prefix_const}}REENTRANT
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
                block();
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
            internal_assert(!message_queue_core_is_unblocked(task),\
                            ERROR_ID_MESSAGE_QUEUE_INTERNAL_VIOLATED_INVARIANT_WAITING_TASK_IS_NOT_BLOCKED);
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
static void
yield_to(const {{prefix_type}}TaskId to) {{prefix_const}}REENTRANT
{
    const {{prefix_type}}TaskId from = get_current_task();

    internal_assert(to < {{tasks.length}}, ERROR_ID_INTERNAL_INVALID_ID);

    current_task = to;
    context_switch(get_task_context(from), get_task_context(to));
}

static void
block(void) {{prefix_const}}REENTRANT
{
    sched_set_blocked(get_current_task());
    {{prefix_func}}yield();
}

static void
unblock(const {{prefix_type}}TaskId task)
{
    sched_set_runnable(task);
}

/* entry point trampolines */
{{#tasks}}
static void
entry_{{name}}(void)
{
    {{^start}}{{prefix_func}}signal_wait({{prefix_const}}SIGNAL_ID__RTOS_UTIL);{{/start}}
    {{function}}();

    api_error(ERROR_ID_TASK_FUNCTION_RETURNS);
}

{{/tasks}}





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

{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_task_set(const {{prefix_type}}InterruptEventId interrupt_event_id, const {{prefix_type}}TaskId task_id)
{
    api_assert(interrupt_event_id < {{interrupt_events.length}}, ERROR_ID_INVALID_ID);
    api_assert(task_id < {{tasks.length}}, ERROR_ID_INVALID_ID);
    interrupt_events[interrupt_event_id].task = task_id;
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
{{#profiling}}

/* MAINTENANCE:
 * When modifying or extending the profiling component, ensure that its code is covered by at least one of the example
 * systems to ensure it builds correctly.
 */

void
{{prefix_func}}profiling_record_sample(void)
{
    {{#profiling.task_uptime}}
    {{prefix_type}}TaskId idx;

    if (!system_is_idle)
    {
        idx = get_current_task();
    }
    else
    {
        idx = {{tasks.length}};
    }

    profiling_task_uptimes[idx] += 1;
    {{/profiling.task_uptime}}
}

{{/profiling}}
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
{{prefix_func}}task_start(const {{prefix_type}}TaskId task)
{
    assert_task_valid(task);
    {{prefix_func}}signal_send(task, {{prefix_const}}SIGNAL_ID__RTOS_UTIL);
}

void
{{prefix_func}}yield(void) {{prefix_const}}REENTRANT
{
    {{prefix_type}}TaskId to = interrupt_event_get_next();
    yield_to(to);
}

void
{{prefix_func}}start(void)
{
    message_queue_init();

    {{#tasks}}
    context_init(get_task_context({{idx}}), entry_{{name}}, stack_{{idx}}, {{stack_size}});
    sched_set_runnable({{idx}});
    {{/tasks}}

    context_switch_first(get_task_context({{prefix_const}}TASK_ID_ZERO));
}
