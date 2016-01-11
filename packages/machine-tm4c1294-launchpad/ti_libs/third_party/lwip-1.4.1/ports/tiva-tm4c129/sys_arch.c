/**
 * @file - sys_arch.c
 * System Architecture support routines for TI Tiva devices.
 *
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* Copyright (c) 2008 Texas Instruments Incorporated */

/* lwIP includes. */
#include "lwip/opt.h"
#include "lwip/sys.h"

#if NO_SYS

#if SYS_LIGHTWEIGHT_PROT

/* TivaWare header files required for this interface driver. */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

/**
 * This global is defined in lwiplib.c and contains a count of the number of
 * elapsed milliseconds since lwIP started.
 */
extern uint32_t g_ui32LocalTimer;

/**
 * This function returns the system time in milliseconds.
 */
u32_t
sys_now(void)
{
    return(g_ui32LocalTimer);
}


/**
 * This function is used to lock access to critical sections when lwipopt.h
 * defines SYS_LIGHTWEIGHT_PROT. It disables interrupts and returns a value
 * indicating the interrupt enable state when the function entered. This
 * value must be passed back on the matching call to sys_arch_unprotect().
 *
 * @return the interrupt level when the function was entered.
 */
sys_prot_t
sys_arch_protect(void)
{
  return((sys_prot_t)MAP_IntMasterDisable());
}

/**
 * This function is used to unlock access to critical sections when lwipopt.h
 * defines SYS_LIGHTWEIGHT_PROT. It enables interrupts if the value of the lev
 * parameter indicates that they were enabled when the matching call to
 * sys_arch_protect() was made.
 *
 * @param lev is the interrupt level when the matching protect function was
 * called
 */
void
sys_arch_unprotect(sys_prot_t lev)
{
  /* Only turn interrupts back on if they were originally on when the matching
     sys_arch_protect() call was made. */
  if(!(lev & 1)) {
    MAP_IntMasterEnable();
  }
}
#endif /* SYS_LIGHTWEIGHT_PROT */

#else /* NO_SYS */
/* Provide a default maximum number of semaphores. */
#ifndef SYS_SEM_MAX
#define SYS_SEM_MAX             4
#endif /* SYS_SEM_MAX */

/* Provide a default maximum number of mailboxes. */
#ifndef SYS_MBOX_MAX
#define SYS_MBOX_MAX            4
#endif /* SYS_MBOX_MAX */

/* An array to hold the memory for the available semaphores. */
static sem_t sems[SYS_SEM_MAX];

/* An array to hold the memory for the available mailboxes. */
static mbox_t mboxes[SYS_MBOX_MAX];

/**
 * Initializes the system architecture layer.
 *
 */
void
sys_init(void)
{
  u32_t i;

#if RTOS_ECHRONOS
  for( i = 0; i < SYS_MBOX_MAX; ++i ) {
      mboxes[i].isUsed = false;
  }

  mboxes[0].queue = RTOS_MESSAGE_QUEUE_ID_LWIP_AUX_QUEUE_1;
  mboxes[1].queue = RTOS_MESSAGE_QUEUE_ID_LWIP_AUX_QUEUE_2;
  mboxes[2].queue = RTOS_MESSAGE_QUEUE_ID_LWIP_AUX_QUEUE_3;
  mboxes[3].queue = RTOS_MESSAGE_QUEUE_ID_LWIP_AUX_QUEUE_4;


  for(i = 0; i < SYS_SEM_MAX; i++) {
      sems[i].isUsed = false;
  }

  sems[0].sem = RTOS_SEM_ID_LWIP_AUX_SEM_1;
  sems[1].sem = RTOS_SEM_ID_LWIP_AUX_SEM_2;
  sems[2].sem = RTOS_SEM_ID_LWIP_AUX_SEM_3;
  sems[3].sem = RTOS_SEM_ID_LWIP_AUX_SEM_3;
#endif
}

/**
 * Creates a new semaphore.
 *
 * @param count is non-zero if the semaphore should be acquired initially.
 * @return the handle of the created semaphore.
 */
err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
  u32_t i;

  /* Find a semaphore that is not in use. */
  for(i = 0; i < SYS_SEM_MAX; i++) {
    if(!sems[i].isUsed) {
      break;
    }
  }
  if(i == SYS_SEM_MAX) {
#if SYS_STATS
    STATS_INC(sys.sem.err);
#endif /* SYS_STATS */
    return ERR_MEM;
  }

#if RTOS_ECHRONOS
  sems[i].isUsed = true;
  *sem = sems[i];

  // Set up initial count
  for( i = 0; i != count; ++i ) {
     rtos_sem_post( sem->sem );
  }
#endif /* RTOS_FREERTOS */

  /* Update the semaphore statistics. */
#if SYS_STATS
  STATS_INC(sys.sem.used);
#if LWIP_STATS
  if(lwip_stats.sys.sem.max < lwip_stats.sys.sem.used) {
    lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
  }
#endif
#endif /* SYS_STATS */

  /* Return this semaphore. */
  return (ERR_OK);
}

/**
 * Signal a semaphore.
 *
 * @param sem is the semaphore to signal.
 */
void
sys_sem_signal(sys_sem_t *sem)
{
  rtos_sem_post( sem->sem );
}

/**
 * Wait for a semaphore to be signalled.
 *
 * @param sem is the semaphore
 * @param timeout is the maximum number of milliseconds to wait for the
 *        semaphore to be signalled
 * @return the number of milliseconds that passed before the semaphore was
 *         acquired, or SYS_ARCH_TIMEOUT if the timeout occurred
 */
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
  RtosTicksAbsolute before_wait = rtos_timer_current_ticks;
  if( rtos_sem_wait_timeout( sem->sem, timeout / SYS_TICK_INTERVAL ) ) {
      return SYS_TICK_INTERVAL * (rtos_timer_current_ticks - before_wait);
  } else {
      return SYS_ARCH_TIMEOUT;
  }
}

/**
 * Destroys a semaphore.
 *
 * @param sem is the semaphore to be destroyed.
 */
void
sys_sem_free(sys_sem_t *sem)
{
    // Push the semaphore down to zero
    while( rtos_sem_try_wait( sem->sem ) );

    // It's no longer used
    sem->isUsed = false;

  /* Update the semaphore statistics. */
#if SYS_STATS
  STATS_DEC(sys.sem.used);
#endif /* SYS_STATS */
}

/**
 * Creates a new mailbox.
 *
 * @param size is the number of entries in the mailbox.
 * @return the handle of the created mailbox.
 */
err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
  u32_t i;

  /* Fail if the mailbox size is too large. */
  if(size > MBOX_MAX) {
#if SYS_STATS
    STATS_INC(sys.mbox.err);
#endif /* SYS_STATS */
    return ERR_MEM;
  }

  /* Find a mailbox that is not in use. */
  for(i = 0; i < SYS_MBOX_MAX; i++) {
    if( !mboxes[i].isUsed ) {
      break;
    }
  }

  if(i == SYS_MBOX_MAX) {
#if SYS_STATS
    STATS_INC(sys.mbox.err);
#endif /* SYS_STATS */
    return ERR_MEM;
  }

#if RTOS_ECHRONOS
  /* Create a queue for this mailbox. */
  mbox->queue = mboxes[i].queue;
  mbox->isUsed = true;
#endif

  /* Update the mailbox statistics. */
#if SYS_STATS
  STATS_INC(sys.mbox.used);
#if LWIP_STATS
  if(lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
    lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
  }
#endif
#endif /* SYS_STATS */

  /* Return this mailbox. */
  return ERR_OK;
}

/**
 * Sends a message to a mailbox.
 *
 * @param mbox is the mailbox
 * @param msg is the message to send
 */
void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
  rtos_message_queue_put( mbox->queue,  &msg );
}

/**
 * Tries to send a message to a mailbox.
 *
 * @param mbox is the mailbox
 * @param msg is the message to send
 * @return ERR_OK if the message was sent and ERR_MEM if there was no space for
 *         the message
 */
err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
  if( rtos_message_queue_try_put( mbox->queue, &msg ) ) {
     return ERR_OK;
  }

  /* Update the mailbox statistics. */
#if SYS_STATS
  STATS_INC(sys.mbox.err);
#endif /* SYS_STATS */

  /* The message could not be sent. */
  return ERR_MEM;
}

/**
 * Retrieve a message from a mailbox.
 *
 * @param mbox is the mailbox
 * @param msg is a pointer to the location to receive the message
 * @param timeout is the maximum number of milliseconds to wait for the message
 * @return the number of milliseconds that passed before the message was
 *         received, or SYS_ARCH_TIMEOUT if the tmieout occurred
 */
u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
  RtosTicksAbsolute before_wait = rtos_timer_current_ticks;
  if( rtos_message_queue_get_timeout( mbox->queue, msg, timeout/SYS_TICK_INTERVAL ) ) {
      return SYS_TICK_INTERVAL * (rtos_timer_current_ticks - before_wait);
  } else {
      return SYS_ARCH_TIMEOUT;
  }
}

/**
 * Try to receive a message from a mailbox, returning immediately if one is not
 * available.
 *
 * @param mbox is the mailbox
 * @param msg is a pointer to the location to receive the message
 * @return ERR_OK if a message was available and SYS_MBOX_EMPTY if one was not
 *         available
 */
u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
  if( rtos_message_queue_try_get( mbox->queue, msg ) ) {
    return ERR_OK;
  } else {
    return SYS_MBOX_EMPTY;
  }
}

/**
 * Destroys a mailbox.
 *
 * @param mbox is the mailbox to be destroyed.
 */
void
sys_mbox_free(sys_mbox_t *mbox)
{
  void *_tmp;
  void **tmp = &_tmp;
  /* There should not be any messages waiting (if there are it is a bug).  If
     any are waiting, increment the mailbox error count. */
#if RTOS_ECHRONOS
  if( rtos_message_queue_try_get( mbox->queue, tmp ) ) {
#endif /* RTOS_FREERTOS */

#if SYS_STATS
    STATS_INC(sys.mbox.err);
#endif /* SYS_STATS */
  }

  // Make sure no messages are left
  while( rtos_message_queue_try_get( mbox->queue, tmp ) );
  mbox->isUsed = false;

  /* Update the mailbox statistics. */
#if SYS_STATS
   STATS_DEC(sys.mbox.used);
#endif /* SYS_STATS */
}

/**
 * Checks the validity of a mailbox.
 *
 * @param mbox is the mailbox whose validity is to be checked.
 */
int
sys_mbox_valid(sys_mbox_t *mbox)
{
  /*Check if a mailbox has been created*/
  if(!mbox->isUsed){
      return 0;
  }
  else{
      return 1;
  }
}

// TIMERS & CRITICAL SECTIONS
// TODO: Test these work as expected
// Currently using bare-metal implementation
// (interrupt enabling is recommended in eChronos manual)

/* TivaWare header files required for this interface driver. */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

//

/**
 * This function returns the system time in milliseconds.
 */
u32_t
sys_now(void)
{
    return rtos_timer_current_ticks * SYS_TICK_INTERVAL;
}

/**
 * This function is used to lock access to critical sections when lwipopt.h
 * defines SYS_LIGHTWEIGHT_PROT. It disables interrupts and returns a value
 * indicating the interrupt enable state when the function entered. This
 * value must be passed back on the matching call to sys_arch_unprotect().
 *
 * @return the interrupt level when the function was entered.
 */
sys_prot_t
sys_arch_protect(void)
{
  return((sys_prot_t)MAP_IntMasterDisable());
}

/**
 * This function is used to unlock access to critical sections when lwipopt.h
 * defines SYS_LIGHTWEIGHT_PROT. It enables interrupts if the value of the lev
 * parameter indicates that they were enabled when the matching call to
 * sys_arch_protect() was made.
 *
 * @param lev is the interrupt level when the matching protect function was
 * called
 */
void
sys_arch_unprotect(sys_prot_t lev)
{
  /* Only turn interrupts back on if they were originally on when the matching
     sys_arch_protect() call was made. */
  if(!(lev & 1)) {
    MAP_IntMasterEnable();
  }
}

#endif /* NO_SYS */
