/* Copyright (C) 2002, 2003 Vincent Penquerc'h
   This file is part of the alogg library
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>

   The alogg library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version

   The alogg library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details

   You should have received a copy of the GNU Lesser General Public
   License along with the alogg Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   As a special exception, if you link this library statically with a
   program, you do not have to distribute the object files for this
   program
   This exception does not however invalidate any other reasons why
   you would have to distribute the object files for this program.  */


#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <allegro/debug.h>
#include "aloggpth.h"

static void *alogg_threaded_streamer(void *data)
{
  int ret=1;
  struct alogg_thread *thread=data;

#ifndef __WIN32
  struct timespec pause;
  pause.tv_sec = 0;
  pause.tv_nsec = 10000000;
#endif

  ASSERT(thread);
  ASSERT(thread->stream);

  while (1) {


    alogg_lock_thread(thread);
    ret=alogg_update_streaming(thread->stream);
    alogg_unlock_thread(thread);

    if (ret==0 || thread->stop) break;

#ifndef __WIN32
    nanosleep(&pause, 0);
#else
    Sleep(10);
#endif
  }

  TRACE("stopping stream\n");
  alogg_lock_thread(thread);
  thread->alive=0;
  thread->stop=1;
  alogg_stop_streaming(thread->stream);
  alogg_unlock_thread(thread);
  pthread_mutex_destroy(&thread->mutex);
  pthread_exit(NULL);
}

int alogg_lock_thread(struct alogg_thread *thread)
{
  ASSERT(thread);
  return pthread_mutex_lock(&thread->mutex);
}

int alogg_try_lock_thread(struct alogg_thread *thread)
{
  ASSERT(thread);
  return pthread_mutex_trylock(&thread->mutex);
}

int alogg_unlock_thread(struct alogg_thread *thread)
{
  ASSERT(thread);
  return pthread_mutex_unlock(&thread->mutex);
}

int alogg_is_thread_alive(struct alogg_thread *thread)
{
  ASSERT(thread);
  if (!thread) return 0;
  return thread->alive;
}

struct alogg_thread *alogg_create_thread(struct alogg_stream *stream)
{
  alogg_thread *thread=NULL;

  ASSERT(stream);

  /* Create a control structure */
  thread=malloc(sizeof(alogg_thread));
  if (!thread) return NULL;

  thread->stream=stream;
  thread->stop=0;
  thread->alive=1;

  /* Initialize a new thread and its associated mutex */
  pthread_mutex_init(&thread->mutex,NULL);
  if (pthread_create(&thread->thread,NULL,&alogg_threaded_streamer,thread)) {
    pthread_mutex_destroy(&thread->mutex);
    free(thread);
    return NULL;
  }

  return thread;
}

void alogg_destroy_thread(alogg_thread *thread)
{
  ASSERT(thread);

  if (!thread) return;
  ASSERT(!thread->alive);

  free(thread);
}

void alogg_join_thread(alogg_thread *thread)
{
  ASSERT(thread);

  if (!thread) return;
  ASSERT(!thread->alive);

  pthread_join(thread->thread, NULL);
}

void alogg_stop_thread(alogg_thread *thread)
{
  ASSERT(thread);

  if (!thread) return;
  alogg_lock_thread(thread);
  thread->stop=1;
  alogg_unlock_thread(thread);
}

struct alogg_stream *alogg_get_thread_stream(alogg_thread *thread)
{
  ASSERT(thread);

  if (!thread) return NULL;
  if (!thread->alive) return NULL;
  return thread->stream;
}
