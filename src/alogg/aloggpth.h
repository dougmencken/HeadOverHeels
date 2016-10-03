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


#ifndef _ALOGG_PTHREAD_H_
#define _ALOGG_PTHREAD_H_

#ifdef __WIN32
#include <allegro.h>
#include <winalleg.h>
#endif
#include <pthread.h>
#include "alogg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct alogg_thread {
  pthread_t thread;
  pthread_mutex_t mutex;
  struct alogg_stream *stream;
  int stop;
  int alive;
} alogg_thread;

struct alogg_thread *alogg_create_thread(struct alogg_stream *stream);
void alogg_destroy_thread(struct alogg_thread *thread);

int alogg_lock_thread(struct alogg_thread *thread);
int alogg_try_lock_thread(struct alogg_thread *thread);
int alogg_unlock_thread(struct alogg_thread *thread);

int alogg_is_thread_alive(struct alogg_thread *thread);
void alogg_join_thread(struct alogg_thread *thread);
void alogg_stop_thread(struct alogg_thread *thread);

struct alogg_stream *alogg_get_thread_stream();

#ifdef __cplusplus
}
#endif

#endif
