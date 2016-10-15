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


#ifndef _ALOGG_H_
#define _ALOGG_H_

#define ALOGG_MAJOR 1
#define ALOGG_MINOR 3
#define ALOGG_PATCH 7
#define ALOGG_MAKE_VERSION(x,y,z) (((x)*10000)+((y)*100)+(z))
#define ALOGG_VERSION ALOGG_MAKE_VERSION(ALOGG_MAJOR,ALOGG_MINOR,ALOGG_PATCH)
#define ALOGG_ID "alogg 1.3.7"

#define DAT_OGG_VORBIS DAT_ID('O','G','G','V')

#ifndef ALOGG_USE_TREMOR
#define DAT_OGG_VORBIS_QUALITY DAT_ID('O','G','G','Q')
#endif

#ifdef DEBUG
#include <dmalloc.h>
#endif

#ifdef ALOGG_USE_TREMOR
#include <tremor/ivorbisfile.h>
#include <tremor/ivorbiscodec.h>
#else
#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>
#endif

struct SAMPLE;
struct PACKFILE;
struct DATAFILE;
struct AUDIOSTREAM;
struct OggVorbis_File;

struct alogg_stream;
struct alogg_encoding_data;

extern int alogg_error_code;

#ifdef __cplusplus
extern "C" {
#endif

void alogg_init();
void alogg_exit();

/* Integration with Allegro's SAMPLE management */
struct SAMPLE *alogg_load_ogg(AL_CONST char *filename);
struct SAMPLE *alogg_create_sample(struct DATAFILE *dat);

#ifndef ALOGG_USE_TREMOR
int alogg_save_ogg_param(
  AL_CONST char *filename,struct SAMPLE *sample,float quality,
  size_t ncomments,char **comments
);
int alogg_save_ogg(AL_CONST char *filename,struct SAMPLE *sample);
#endif

/* Streaming support */
struct alogg_stream *alogg_start_streaming(
  AL_CONST char *filename,size_t block_size
);
struct alogg_stream *alogg_start_streaming_datafile(
  AL_CONST struct DATAFILE *dat,size_t block_size
);
struct alogg_stream *alogg_start_streaming_callbacks(
  void *datasource, ov_callbacks *callbacks, size_t block_size,
  int (*update)(struct alogg_stream*,void*)
);
int alogg_read_stream_data( struct alogg_stream*, void *buffer, size_t size );
int alogg_update_streaming( struct alogg_stream* );
int alogg_stop_streaming( struct alogg_stream* );
struct AUDIOSTREAM *alogg_get_audio_stream( struct alogg_stream* );
struct OggVorbis_File *alogg_get_vorbis_file( struct alogg_stream* );

#ifndef ALOGG_USE_TREMOR

/* Encoding */
struct alogg_encoding_data *alogg_start_encoding(
  size_t channels,size_t freq,float quality,size_t ncomments,char **comments,
  void (*write)(void*,size_t,unsigned long),unsigned long write_data
);
int alogg_update_encoding(
  struct alogg_encoding_data*,AL_CONST void *buffer,
  size_t num_samples,size_t channels,size_t bits
);
int alogg_stop_encoding(struct alogg_encoding_data*);

#endif

#ifdef __cplusplus
}
#endif

#endif
