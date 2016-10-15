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


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <allegro/debug.h>
#include <allegro/file.h>
#include <allegro/datafile.h>
#include <allegro/digi.h>
#include <allegro/stream.h>

#include "aloggcfg.h"
#include "aloggint.h"
#include "alogg.h"

int alogg_error_code=0;

typedef struct alogg_encoding_data {
  void (*write)(void*,size_t,unsigned long);
  unsigned long write_data;
  ogg_stream_state os;
  ogg_page og;
  ogg_packet op;
  vorbis_info info;
  vorbis_dsp_state vd;
  vorbis_block vb;
} alogg_encoding_data;

typedef struct alogg_stream {
  ov_callbacks callbacks;
  void *datasource;
  struct AUDIOSTREAM *audio_stream;
  struct OggVorbis_File *vf;
  size_t block_size;
  int done;
  int (*update)(struct alogg_stream*,void*);
} alogg_stream;

void alogg_init()
{
#ifdef HAS_REGISTER_SAMPLE_FILE_TYPE
  register_sample_file_type(
    "ogg",
    &alogg_load_ogg,
#ifdef ALOGG_USE_TREMOR
    NULL
#else
    &alogg_save_ogg
#endif
  );
#endif
}

void alogg_exit()
{
}



/* Helper to read using libvorbisfile or Tremor */
static int read_ogg_data(
  struct OggVorbis_File *vf,char *buffer,int length,int word,int sgned,
  int *bitstream
)
{
  int ret;
  TRACE("read_ogg_data: read %d bytes\n",length);
  ret=ov_read(
    vf,buffer,length,
#ifndef ALOGG_USE_TREMOR
#ifdef ALLEGRO_BIG_ENDIAN
    1,
#else
    0,
#endif
    word,sgned,
#endif
    bitstream
  );
  TRACE("read_ogg_data: return %d\n",ret);
  ASSERT(ret<=length); /* negative or less or equal than the required size */

#ifdef ALOGG_USE_TREMOR
  /* Tremor reads signed samples, and Allegro needs unsigned samples */
  if (ret>0 && !sgned) {
    if (word) {
      int n;
      ASSERT((ret&1)==0);
      for (n=0;n<ret/2;++n) ((unsigned short*)buffer)[n]^=0x8000;
    }
    else {
      int n;
      for (n=0;n<ret;++n) ((unsigned char*)buffer)[n]^=0x80;
    }
  }
#endif

  return ret;
}

/* Callbacks to read an Ogg/Vorbis file from a FILE */
static size_t file_read(void *ptr,size_t size,size_t nelts,void *datasource)
{
  FILE *f=(FILE*)datasource;
  ASSERT(f);
  return (size_t)fread(ptr,size,nelts,f);
}

static int file_seek(void *datasource,ogg_int64_t offset,int whence)
{
  FILE *f=(FILE*)datasource;
  ASSERT(f);
  return fseek(f,offset,whence);
}

static int file_close(void *datasource)
{
  /* Let the caller do it, in case it doesn't want to close it just yet */
  return 0;
}

static long file_tell(void *datasource)
{
  FILE *f=(FILE*)datasource;
  ASSERT(f);
  return ftell(f);;
}

static ov_callbacks file_callbacks={
  &file_read,
  &file_seek,
  &file_close,
  &file_tell
};

/* Callbacks to read an Ogg/Vorbis file from a PACKFILE */
static size_t packfile_read(void *ptr,size_t size,size_t nelts,void *datasource)
{
  PACKFILE *f=(PACKFILE*)datasource;
  ASSERT(f);
  return (size_t)pack_fread(ptr,size*nelts,f);
}

static int packfile_seek(void *datasource,ogg_int64_t offset,int whence)
{
  return -1;
}

static int packfile_close(void *datasource)
{
  return 0;
}

static long packfile_tell(void *datasource)
{
  return -1;
}

static ov_callbacks packfile_callbacks={
  &packfile_read,
  &packfile_seek,
  &packfile_close,
  &packfile_tell
};

/* Callbacks to read an OGG/Vorbis from memory */
static size_t memory_read(void *ptr,size_t size,size_t nmemb,void *datasource)
{
  int read_size;
  datogg_object *ovd=(datogg_object*)datasource;
  ASSERT(ovd);
  ASSERT(ptr);
  read_size=size*nmemb;
  if (ovd->position+read_size>ovd->size) read_size=ovd->size-ovd->position;
  memcpy(ptr,((char*)ovd->data)+ovd->position,read_size);
  ovd->position+=read_size;
  return read_size;
}

static int memory_seek( void *datasource, ogg_int64_t offset, int whence )
{
  datogg_object *ovd = ( datogg_object* )datasource;
  int position = 0;
  ASSERT( ovd );
  if ( ! ovd ) return -1;
  switch ( whence ) {
    case SEEK_SET: position = offset; break;
    case SEEK_CUR: position = ovd->position+offset; break;
    case SEEK_END: position = ovd->size+offset; break;
    default: ASSERT(0); break;
  }
  ovd->position = position;
  return 0;
}

static int memory_close(void *datasource)
{
  datogg_object *ovd=(datogg_object*)datasource;
  ASSERT(ovd);
  ovd->position=0;
  return 0;
}

static long memory_tell(void *datasource)
{
  datogg_object *ovd=(datogg_object*)datasource;
  ASSERT(ovd);
  return ovd->position;
}

static ov_callbacks memory_callbacks={
  &memory_read,
  &memory_seek,
  &memory_close,
  &memory_tell
};

static int memory_close_free(void *datasource)
{
  datogg_object *ovd=(datogg_object*)datasource;
  ASSERT(ovd);
  free(ovd);
  return 0;
}

static ov_callbacks datafile_callbacks={
  &memory_read,
  &memory_seek,
  &memory_close_free,
  &memory_tell
};

static SAMPLE *load_ov(OggVorbis_File *vf)
{
  int ret;
  vorbis_info *info;
  SAMPLE *sample=NULL;
  ogg_int64_t num_samples;
  size_t offset;
  AL_CONST size_t max_read=4096;
  size_t buffer_size;
  size_t buffer_inc=65536;
  int stereo;

  info=ov_info(vf,-1);
  if (!info) {
    TRACE("ov_info failed\n");
    goto load_error_file;
  }
  stereo=(info->channels>1);

  num_samples=ov_pcm_total(vf,-1);
  TRACE("ov_pcm_total says %lld samples\n",num_samples);
  if (num_samples<0) {
    TRACE("ov_pcm_total failed: %lld - not fatal\n",(ogg_int64_t)num_samples);
  }
  TRACE(
    "ov_load: create sample: %d bits, %d channels (%s), %ld Hz, %lld samples\n",
    16,info->channels,stereo?"stereo":"mono",info->rate,num_samples
  );
  sample=create_sample(16,stereo,info->rate,num_samples>0?num_samples:1);
  if (!sample) {
    TRACE(
      "Failed creating a sample (%d channels, %ld Hz, %u samples",
      info->channels,info->rate,(size_t)num_samples
    );
    goto load_error_file;
  }

  /* decode the encoded data */
  for (offset=0,buffer_size=0;;) {
    if (num_samples<0) {
      /* If we could not determine the size of the sample, allocate as we go */
      if (offset+max_read>buffer_size) {
        TRACE("Buffer too small: adding %u bytes\n",buffer_inc);
        buffer_size+=buffer_inc;
        buffer_inc+=buffer_inc/2;
#ifdef FORTIFY
        {
          /* Fortify won't have seen the allocation which was done in Allegro */
          void *tmpdata=malloc(buffer_size);
          memcpy(tmpdata,sample->data,offset);
          destroy_sample(sample);
          sample=create_sample(16,stereo,info->rate,buffer_size/(stereo?2:1)/2);
          memcpy(sample->data,tmpdata,offset);
          free(tmpdata);
        }
#else
        sample->data=realloc(sample->data,buffer_size);
#endif
      }
      if (!sample->data) {
        TRACE("Failed allocating sample data\n");
        goto load_error_sample;
      }
    }
    ret=read_ogg_data(vf,((char*)sample->data)+offset,max_read,2,0,NULL);
    if (ret==0) break;
    if (ret<0) {
      TRACE("read_ogg_data failed: %d\n",ret);
      alogg_error_code=ret;
      goto load_error_sample;
    }
    offset+=ret;
  }
  TRACE("ov_load: clearing\n");
  ov_clear(vf);
  /* ov_clear will have closed the PACKFILE, so no need to do it there */

  /* If we couldn't allocate up front, update the size related fields */
  if (num_samples<0) {
    ASSERT(offset%(stereo?2:1)/2==0);
    sample->len=offset/(stereo?2:1)/2;
    TRACE("Final size: %lu samples\n",sample->len);
    sample->loop_end=sample->len;
    lock_sample(sample);
  }

  return sample;

load_error_sample:
  destroy_sample(sample);
load_error_file:
  ov_clear(vf);
  return NULL;
}

static SAMPLE *load_via_callbacks(void *ptr,ov_callbacks *callbacks)
{
  OggVorbis_File vf;
  int ret=ov_open_callbacks(ptr,&vf,NULL,0,*callbacks);
  if (ret<0) {
    TRACE("ov_open_callbacks failed: %d\n",ret);
    alogg_error_code=ret;
    return NULL;
  }
  return load_ov(&vf);
}

static SAMPLE *load_ogg_from_packfile(PACKFILE *f)
{
  ASSERT(f);
  if (!f) return NULL;

  return load_via_callbacks(f,&packfile_callbacks);
}

static SAMPLE *load_ogg_from_file(FILE *f)
{
  ASSERT(f);
  if (!f) return NULL;

  return load_via_callbacks(f,&file_callbacks);
}

SAMPLE *alogg_create_sample(struct DATAFILE *dat)
{
  SAMPLE *sample;
  datogg_object *ovd;

  ASSERT(dat);
  ASSERT(dat->dat);
  if (!dat || !dat->dat) return NULL;
  if (dat->type!=DAT_OGG_VORBIS) return NULL;

  ovd=malloc(sizeof(datogg_object));
  if (!ovd) return NULL;

  ovd->size=dat->size;
  ovd->position=0;
  ovd->allocated=0;
  ovd->data=dat->dat;
  sample=load_via_callbacks(ovd,&datafile_callbacks);

  return sample;
}

int aloggint_get_info(
  datogg_object *ovd,int *channels,int *freq,
#ifdef ALOGG_USE_TREMOR
  ogg_int64_t *length
#else
  float *length
#endif
)
{
  OggVorbis_File vf;
  int ret;
  vorbis_info *info=NULL;

  ASSERT(ovd);
  if (!ovd) return 0;

  ASSERT(channels);
  ASSERT(freq);
  ASSERT(length);
  ret=ov_open_callbacks(ovd,&vf,NULL,0,memory_callbacks);
  if (ret<0) return -1;
  info=ov_info(&vf,-1);
  if (info) {
    *channels=info->channels;
    *freq=info->rate;
  }
  *length=ov_time_total(&vf,-1);
  ov_clear(&vf);

  return (info==0);
}

SAMPLE *alogg_load_ogg(AL_CONST char *filename)
{
  SAMPLE *sample;
  FILE *f;
  PACKFILE *pf;

  ASSERT(filename);

  /* first try a normal file - allows seeks */
  f=fopen(filename,"rb");
  if (f) {
    sample=load_ogg_from_file(f);
    fclose(f);
    return sample;
  }
  TRACE("Failed to open %s as a normal file, trying as PACKFILE\n",filename);

  /* open the file as a PACKFILE */
  pf=pack_fopen(filename,"r");
  if (pf) {
    sample=load_ogg_from_packfile(pf);
    pack_fclose(pf);
    return sample;
  }

  TRACE("Failed to open %s\n",filename);
  return NULL;
}



int aloggint_basic_streamer_update(alogg_stream *stream,void *datasource)
{
  void *block;

  ASSERT(stream);
  ASSERT(stream->audio_stream);

  block=get_audio_stream_buffer(stream->audio_stream);
  if (block) {
    int ret=alogg_read_stream_data(stream,block,stream->block_size);
    free_audio_stream_buffer(stream->audio_stream);
    if (stream->done) return 0;
    if (ret==0) stream->done=1;
    return 1;
  }
  return 1;
}

alogg_stream *alogg_start_streaming(AL_CONST char *filename,size_t block_size)
{
  alogg_stream *stream=NULL;
  PACKFILE *f=NULL;

  ASSERT(filename);

  /* Open the file */
  f=pack_fopen(filename, F_READ);
  if (!f) return NULL;

  stream=alogg_start_streaming_callbacks(
    f,(ov_callbacks*)&packfile_callbacks,block_size,NULL
  );
  if (!stream) pack_fclose(f);
  return stream;
}

alogg_stream *alogg_start_streaming_datafile(
  AL_CONST DATAFILE *dat,size_t block_size
)
{
  alogg_stream *stream=NULL;
  struct datogg_object *ovd;

  ASSERT(dat);
  if (!dat) return NULL;

  ovd=malloc(sizeof(datogg_object));
  if (!ovd) return NULL;

  ovd->size=dat->size;
  ovd->position=0;
  ovd->allocated=0;
  ovd->data=dat->dat;
  stream=alogg_start_streaming_callbacks(
    ovd,(ov_callbacks*)&datafile_callbacks,block_size,NULL
  );
  if (!stream) free(ovd);
  return stream;
}

alogg_stream *aloggint_start_streaming_datogg(
  datogg_object *ovd,size_t block_size
)
{
  if (!ovd) return NULL;
  ovd->position=0;
  return alogg_start_streaming_callbacks(
    ovd,(ov_callbacks*)&memory_callbacks,block_size,NULL
  );
}

alogg_stream *alogg_start_streaming_callbacks(
  void *datasource,ov_callbacks *callbacks,size_t block_size,
  int (*update)(alogg_stream*,void*)
)
{
  alogg_stream *stream=NULL;
  vorbis_info *info;
  int ret;
  int stereo=0;

  /* Allocate resources for the new stream */
  stream=malloc(sizeof(alogg_stream));
  if (!stream) goto error;
  ASSERT(callbacks);
  stream->callbacks=*(ov_callbacks*)callbacks;
  stream->datasource=datasource;
  stream->audio_stream=NULL;
  stream->done=0;
  if (update) {
    stream->update=update;
  }
  else {
    stream->update=&aloggint_basic_streamer_update;
  }
  stream->vf=malloc(sizeof(OggVorbis_File));
  if (!stream->vf) goto error;

  /* Initiate stream decoding */
  ret=ov_open_callbacks(datasource,stream->vf,NULL,0,stream->callbacks);
  if (ret<0) {
    TRACE("ov_open_callbacks failed: %d\n",ret);
    alogg_error_code=ret;
    return NULL;
  }

  /* gather info on the stream */
  info=ov_info(stream->vf,-1);
  if (!info) goto error;
  stereo=(info->channels>1);
  stream->block_size=block_size;
  stream->audio_stream=play_audio_stream(
    stream->block_size/2/(stereo?2:1), /* 16 bits per sample */
    16,stereo,info->rate,255,128
  );

  return stream;

error:
  if (stream) {
    if (stream->vf) free(stream->vf);
    free(stream);
  }
  return NULL;
}

int alogg_read_stream_data(alogg_stream *stream,void *block,size_t size)
{
  size_t read_size=0;

  ASSERT(stream);
  ASSERT(block);

  TRACE("Reading stream data: %u bytes at %p\n",size,block);
  /* decode the encoded data */
  for (;size-read_size>0;) {
    int ret=read_ogg_data(
      stream->vf,((char*)block)+read_size,size-read_size,2,0,NULL
    );
    if (ret==0) {
      TRACE("read_ogg_data returned zero\n");
      break;
    }
    if (ret<0) {
      TRACE("read_ogg_data returned %d, read size %u\n",ret,read_size);
      if (read_size) break;
      TRACE("read_ogg_data failed: %d\n",ret);
      alogg_error_code=ret;
      return ret;
    }
    read_size+=ret;
  }
  TRACE("%u bytes actually read\n",read_size);
  if (size!=read_size) {
    /* fill with silence to make sure old data doesn't get played */
    size_t n;
    if (stream->audio_stream->samp->stereo) {
      unsigned short *sblock=(unsigned short*)((char*)block+read_size);
      unsigned short value=read_size>=2?sblock[-1]:0x8000;
      for (n=0;n<(size-read_size)/2;++n) sblock[n]=value;
    }
    else {
      unsigned char *sblock=(unsigned char*)block+read_size;
      unsigned char value=read_size>=1?sblock[-1]:0x80;
      for (n=0;n<size-read_size;++n) sblock[n]=value;
    }
  }

  return read_size;
}

int alogg_update_streaming( alogg_stream *stream )
{
  ASSERT( stream );
  ASSERT( stream->update );
  return stream->update( stream,stream->datasource );
}

int alogg_stop_streaming( alogg_stream *stream )
{
  ASSERT( stream );
  ASSERT( stream->vf );
  ASSERT( stream->audio_stream );

  /* stop the stream */
  stop_audio_stream( stream->audio_stream );

  /* free all resources */
  ov_clear( stream->vf );
  free( stream->vf );
  free( stream );

  return 0;
}

struct AUDIOSTREAM *alogg_get_audio_stream(struct alogg_stream *stream)
{
  ASSERT(stream);
  ASSERT(stream->audio_stream);

  return stream->audio_stream;
}

struct OggVorbis_File *alogg_get_vorbis_file(struct alogg_stream *stream)
{
  ASSERT(stream);
  ASSERT(stream->vf);

  return stream->vf;
}



#ifndef ALOGG_USE_TREMOR

static int write_vorbis_headers(
  alogg_encoding_data *data,size_t ncomments,char **user_comments
)
{
  ogg_packet header,comments,codebooks;
  vorbis_comment vc;
  size_t n;
  char ver[8];

  ASSERT(data);

  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc,"ENCODER",ALOGG_ID " Allegro Ogg/Vorbis library");
  snprintf(ver,sizeof(ver),"%d.%d.%d",ALOGG_MAJOR,ALOGG_MINOR,ALOGG_PATCH);
  ver[sizeof(ver)-1]=0;
  vorbis_comment_add_tag(&vc,"ALOGG VERSION",ver);
  for (n=0;n<ncomments;++n) vorbis_comment_add(&vc,user_comments[n]);
  vorbis_analysis_headerout(&data->vd,&vc,&header,&comments,&codebooks);
  ogg_stream_packetin(&data->os,&header);
  ogg_stream_packetin(&data->os,&comments);
  ogg_stream_packetin(&data->os,&codebooks);

  while (1) {
    int ret=ogg_stream_flush(&data->os,&data->og);
    if (ret==0) break;
    if (data->write) {
      (*data->write)(data->og.header,data->og.header_len,data->write_data);
      (*data->write)(data->og.body,data->og.body_len,data->write_data);
    }
  }

  return 1;
}

static int encode_buffers_8(
  float **buffers,AL_CONST void *data,size_t num_samples,size_t channels
)
{
  size_t n,c;

  ASSERT(data);
  ASSERT(buffers);

  for (n=0;n<num_samples;++n) {
    for (c=0;c<channels;++c) {
      buffers[c][n]=(((unsigned char*)data)[n*channels+c]-128)/128.0f;
    }
  }

  return 1;
}

static int encode_buffers_16(
  float **buffers,AL_CONST void *data,size_t num_samples,size_t channels
)
{
  size_t n,c;

  ASSERT(data);
  ASSERT(buffers);

  for (n=0;n<num_samples;++n) {
    for (c=0;c<channels;++c) {
      buffers[c][n]=(((unsigned short*)data)[n*channels+c]-32768)/32768.0f;
    }
  }

  return 1;
}

static int encode_buffers(
  float **buffers,AL_CONST void *data,
  size_t num_samples,size_t channels,size_t bits
)
{
  int ret=0;
  switch (bits) {
    case 8:
      ret=encode_buffers_8(buffers,data,num_samples,channels);
      break;
    case 16:
      ret=encode_buffers_16(buffers,data,num_samples,channels);
      break;
    default:
      TRACE("Unsupported sample size: %d\n",bits);
      ret=0;
      break;
  }
  return ret;
}

int alogg_save_ogg_param(
  AL_CONST char *filename,struct SAMPLE *sample,float quality,
  size_t ncomments,char **comments
)
{
  alogg_encoding_data *data;
  PACKFILE *f;
  int ret;

  /* assumptions on parameters */
  ASSERT(filename);
  ASSERT(sample);
  ASSERT(quality>=0.0f && quality<=1.0f);

  /* open file */
  f=pack_fopen(filename,F_WRITE);
  if (!f) return 0;

  /* encode stream using pack_fwrite */
  data=alogg_start_encoding(
    sample->stereo?2:1,sample->freq,quality,ncomments,comments,
    (void(*)(void*,size_t,unsigned long))&pack_fwrite,(unsigned long)f
  );
  ret=!pack_ferror(f);
  if (data) {
    ret=alogg_update_encoding(
      data,sample->data,sample->len,sample->stereo?2:1,sample->bits
    ) && ret;
    ret=alogg_stop_encoding(data) && ret;
  }

  /* close file */
  ret=!pack_ferror(f) && ret;
  pack_fclose(f);

  return ret;
}

int alogg_save_ogg(AL_CONST char *filename,struct SAMPLE *sample)
{
  return alogg_save_ogg_param(filename,sample,0.5f,0,NULL);
}

alogg_encoding_data *alogg_start_encoding(
  size_t channels,size_t freq,float quality,size_t ncomments,char **comments,
  void (*write)(void*,size_t,unsigned long),unsigned long write_data
)
{
  alogg_encoding_data *encoding_data;
  int ret;

  /* create the encoding structure */
  encoding_data=malloc(sizeof(alogg_encoding_data));
  if (!encoding_data) return NULL;

  /* setup the user output routine */
  encoding_data->write=write;
  encoding_data->write_data=write_data;

  /* init encoding */
  vorbis_info_init(&encoding_data->info);
  ret=vorbis_encode_init_vbr(&encoding_data->info,channels,freq,quality);
  if (ret<0) {
    TRACE("vorbis_encode_init failed: %d\n",ret);
    alogg_error_code=ret;
    return 0;
  }
  ret=vorbis_analysis_init(&encoding_data->vd,&encoding_data->info);
  if (ret<0) {
    TRACE("vorbis_analysis_init failed: %d\n",ret);
    alogg_error_code=ret;
    return 0;
  }
  ret=vorbis_block_init(&encoding_data->vd,&encoding_data->vb);
  if (ret<0) {
    TRACE("vorbis_block_init failed: %d\n",ret);
    alogg_error_code=ret;
    return 0;
  }
  ret=ogg_stream_init(&encoding_data->os,rand());
  if (ret<0) {
    TRACE("ogg_stream_init failed: %d\n",ret);
    alogg_error_code=ret;
    return 0;
  }

  /* output Vorbis headers */
  ret=write_vorbis_headers(encoding_data,ncomments,comments);
  if (ret==0) {
    TRACE("write_vorbis_headers failed: %d\n",ret);
    return 0;
  }

  return encoding_data;
}

/* Heavily inspired from libvorbis' encoder_example.c */
int alogg_update_encoding(
  alogg_encoding_data *data,
  AL_CONST void *buffer,size_t num_samples,size_t channels,size_t bits
)
{
  float **buffers;
  int ret;
  int eos=0;
  size_t size;

  ASSERT(data);
  ASSERT(buffer);

  /* encode */
  size=num_samples*channels*bits/8;
  buffers=vorbis_analysis_buffer(&data->vd,num_samples);
  ret=encode_buffers(buffers,buffer,num_samples,channels,bits);
  if (!ret) {
    TRACE("encode_buffers failed: %d\n",ret);
    return 0;
  }
  ret=vorbis_analysis_wrote(&data->vd,num_samples);
  if (ret<0) {
    TRACE("vorbis_analysis_wrote failed: %d\n",ret);
    alogg_error_code=ret;
    return 0;
  }

  while (!eos) {
    while (vorbis_analysis_blockout(&data->vd,&data->vb)==1) {
      ret=vorbis_analysis(&data->vb,NULL);
      if (ret<0) {
        TRACE("vorbis_analysis failed: %d\n",ret);
        alogg_error_code=ret;
        return 0;
      }
      ret=vorbis_bitrate_addblock(&data->vb);
      if (ret<0) {
        TRACE("vorbis_bitrate_addblock failed: %d\n",ret);
        alogg_error_code=ret;
        return 0;
      }
      while (vorbis_bitrate_flushpacket(&data->vd,&data->op)) {
        ret=ogg_stream_packetin(&data->os,&data->op);
        if (ret<0) {
          TRACE("ogg_stream_packetin failed: %d\n",ret);
          alogg_error_code=ret;
          return 0;
        }
        while (!eos) {
          ret=ogg_stream_pageout(&data->os,&data->og);
          if (ret==0) break;
          if (ret<0) {
            TRACE("ogg_stream_pageout failed: %d\n",ret);
            alogg_error_code=ret;
            return 0;
          }
          if (data->write) {
            (*data->write)(
              data->og.header,data->og.header_len,data->write_data
            );
            (*data->write)(
              data->og.body,data->og.body_len,data->write_data
            );
          }
          if (ogg_page_eos(&data->og)) {
            TRACE("alogg_update_encoding: EOS found\n");
            eos=1;
          }
        }
      }
    }
    vorbis_analysis_wrote(&data->vd,0);
  }

  return 1;
}

int alogg_stop_encoding(alogg_encoding_data *data)
{
  ASSERT(data);

  /* clear encoding resources */
  ogg_stream_clear(&data->os);
  vorbis_block_clear(&data->vb);
  vorbis_dsp_clear(&data->vd);
  vorbis_info_clear(&data->info);

  /* free memory */
  free(data);

  return 1;
}

#endif
