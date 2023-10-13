/* Copyright (C) 2002, 2003 Vincent Penquerc'h
   This file is part of the alogg library
   Written by Vincent Penquerc'h <lyrian -at- kezako -dot- net>

   The alogg library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version

   The alogg library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details

   You should have received a copy of the GNU Lesser General Public
   License along with the alogg Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA

   As a special exception, if you link this library statically with a
   program, you do not have to distribute the object files for this
   program
   This exception does not however invalidate any other reasons why
   you would have to distribute the object files for this program */


#ifndef _ALOGG_INTERNAL_H_
#define _ALOGG_INTERNAL_H_

#define ALLEGRO_IS_VERSION_OR_NEWER(x,y,z) \
  ALOGG_MAKE_VERSION(ALLEGRO_VERSION,ALLEGRO_SUB_VERSION,ALLEGRO_WIP_VERSION)>=ALOGG_MAKE_VERSION(x,y,z)

typedef struct datogg_object {
  size_t size;
  size_t position;
  size_t allocated;
  void *data;
} datogg_object;

struct alogg_stream;

int aloggint_get_info(
  datogg_object *ovd,int *channels,int *freq,
#ifdef ALOGG_USE_TREMOR
  ogg_int64_t *length
#else
  float *length
#endif
);

struct alogg_stream *aloggint_start_streaming_datogg(
  datogg_object *ovd,size_t block_size
);

int aloggint_basic_streamer_update(
  struct alogg_stream *stream,void *datasource
);

#endif
