/* Name, parser.c, CS 24000, Fall 2020
 * Last updated October 12, 2020
 */

/* Add any includes here */

#include "parser.h"

#include <assert.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#include "event_tables.h"

uint8_t g_running_status = 0;

/* Define parse_file here */

/*
 * This function takes in the path of MIDI file and returns the parsed
 * representation of that song.
 */

song_data_t *parse_file(const char *file_path) {

  assert(file_path != NULL);
  FILE *file = fopen(file_path, "r");
  assert(file != NULL);
  fseek(file, 0, SEEK_END);
  int file_length = ftell(file);
  song_data_t *song_data = malloc(sizeof(song_data_t));
/*  assert(song_data != NULL);
  song_data->path = malloc(sizeof(strlen(file_path) + 1));
  assert(song_data->path != NULL);

  song_data->division.uses_tpq = malloc(sizeof(bool));
  strcpy(song_data->path, file_path);

  parse_header(file, song_data);*/
  
/*  song_data->track_list = NULL;
  track_node_t *current_track_node;
  track_node_t *prev_track;

  for (int i = 0; i < song_data->num_tracks; i++) {
    printf("hi %d\n", i);
    current_track_node = malloc(sizeof(track_node_t));
    assert(current_track_node != NULL);
    assert(current_track_node->track != NULL);
    printf("before\n");
    parse_track(file, song_data);
    printf("bye\n");
    current_track_node->next_track = NULL;
    if (song_data->track_list == NULL) {
      song_data->track_list = current_track_node;
    } else {
      prev_track->next_track = current_track_node;
    }
    prev_track = current_track_node;
  }

  printf("fileLength: %d\n", file_length);
  printf("Ftell: %ld\n", ftell(file));*/
  assert(file_length == ftell(file));
  fclose(file);
  return song_data;
} /* parse_file() */

/* Define parse_header here */

/*
 * This function should read a MIDI header chunk from the given file pointer
 * and update the given song_data_t pointer with the format, number of tracks,
 * and division information. It needs assertions to check.
 */

void parse_header(FILE *file, song_data_t *song) {

  fseek(file, 0, SEEK_SET);
  char output[14];

  assert(fread(output, 14, 1, file) == 1);
  assert((output[0] == 'M') && (output[1] == 'T') &&
         (output[2] == 'h') && (output[3] == 'd'));
  uint8_t length[4];
  length[0] = output[4];
  length[1] = output[5];
  length[2] = output[6];
  length[3] = output[7];

  assert(end_swap_32(length) == 6);
  uint8_t form[2];
  form[0] = output[8];
  form[1] = output[9];
  assert((end_swap_16(form) <= 2) && (end_swap_16(form) >= 0));

  uint8_t n_tracks[2];
  n_tracks[0] = output[10];
  n_tracks[1] = output[11];

  song->format = (uint8_t)output[9];

  if (song->format == 0) {
    assert(end_swap_16(n_tracks) == 1);
  } else {
    assert(end_swap_16(n_tracks) >= 1);
  }

  song->num_tracks = end_swap_16(n_tracks);
  uint8_t bit15 = (output[12] >> 7) & 1;
  if (bit15 == 1) {
    song->division.uses_tpq = false;
    song->division.ticks_per_frame = (uint8_t)output[13];
    song->division.frames_per_sec = (uint8_t)output[12];
  } else {
    song->division.uses_tpq = true;
    uint8_t tpq[2];
    tpq[0] = output[12];
    tpq[1] = output[13];
    song->division.ticks_per_qtr = end_swap_16(tpq);
  }
  printf("siiiiiiiiiii\n");
} /* parse_header() */

/* Define parse_track here */

/*
 * This function should read a MIDI track chunk from the given file pointer
 * and update the given song_data_t pointer's track_list with the track_t
 * extracted from the file. It needs assertion to check the type.
 */

void parse_track(FILE *file, song_data_t *song) {
//printf("starting\n");
//  song->track_list = NULL;
  track_t *new_track;
  new_track = malloc(sizeof(track_t));
  assert(new_track != NULL);
/*  track_node_t *prev_track;
  printf("Start Track\n");
  printf("numbertrack %d\n", song->num_tracks);
  for (int i = 0; i < song->num_tracks; i++) {
    current_track_node = malloc(sizeof(track_node_t));
    assert(current_track_node != NULL);
    current_track_node->track = malloc(sizeof(track_t));
    assert(current_track_node->track != NULL);
    printf("Current Track: %d\n", i);*/
    char output [8];
    assert(fread(output, 8, 1, file) == 1);
    assert((output[0] == 'M') && (output[1] == 'T') &&
           (output[2] == 'r') && (output[3] == 'k'));

    uint8_t eight[4];
    eight[0] = output[4];
    eight[1] = output[5];
    eight[2] = output[6];
    eight[3] = output[7];
    new_track->length = end_swap_32(eight);
 //   printf("Length:   %d\n", new_track->length);
    new_track->event_list = NULL;
    event_node_t *current_event;
    uint32_t begin_file_position = (uint32_t)ftell(file);
    uint32_t current_file_position = (uint32_t)ftell(file);
    event_node_t *prev_event;

    while (current_file_position < (begin_file_position +
           new_track->length)) {
//           printf("si\n");
      current_event = malloc(sizeof(event_node_t));
//      assert(current_event != NULL);
      current_event->event = parse_event(file);
      current_file_position = (uint32_t)ftell(file);

      current_event->next_event = NULL;
      if (new_track->event_list == NULL) {
        new_track->event_list = current_event;
      } else {
        prev_event->next_event = current_event;
      }
      prev_event = current_event;
    }
/*    current_track_node->next_track = NULL;
    if (song->track_list == NULL) {
      song->track_list = current_track_node;
    } else {
      prev_track->next_track = current_track_node;
    }
    prev_track = current_track_node;
  }*/

} /* parse_track() */

/* Define parse_event here */

/*
 * This function should read a MIDI track chunk from the given file pointer
 * and update the given song_data_t pointer's track_list with the track_t
 * extracted from the file.
 */

event_t *parse_event(FILE * file) {
//  printf("Start Parse_event\n");
  event_t * new_event;
  new_event = malloc(sizeof(event_t));
  assert(new_event != NULL);
  uint8_t output[1];

  new_event->delta_time = parse_var_len(file);
  assert(fread(output, 1, 1, file) == 1);
//  printf("output: %d\n", output[0]);

  if (output[0] == META_EVENT) {
    new_event->meta_event = parse_meta_event(file);
    new_event->type = (uint8_t)META_EVENT;

  } else if (output[0] == SYS_EVENT_1) {
    new_event->sys_event = parse_sys_event(file);
    new_event->type = (uint8_t)SYS_EVENT_1;

  } else if (output[0] == SYS_EVENT_2) {
    new_event->sys_event = parse_sys_event(file);
    new_event->type = (uint8_t)SYS_EVENT_2;

  } else {
    new_event->midi_event = parse_midi_event(file, output[0]);

  }
//  printf("Type: %d\n", new_event->type);
  return new_event;
} /* parse_event() */

/* Define parse_sys_event here */

/*
 * This function should read and return a sys_event struct from
 * the given MIDI file pointer. It has to allocate enough space in the data
 * pointer to house all of the event data. data_len should encode the size of
 * the data array.
 */

sys_event_t parse_sys_event(FILE *file) {
  sys_event_t sys = { 0 };
  sys.data_len = parse_var_len(file);
  uint8_t *data = 0;

  data = malloc(sizeof(uint8_t) * sys.data_len);
  assert(data != NULL);

  uint8_t output[1];
  for (int i = 0; i < sys.data_len; i++) {
    fread(output, 1, 1, file);
    data[i] = output[0];
  }

  sys.data = data;
  return sys;
} /* parse_sys_event() */

/* Define parse_meta_event here */

/*
 * This function should read and return a meta_event struct from
 * the given MIDI file pointer. It has to allocate appropriate amount of space
 * in data pointer. However, given that event names should not change, leave
 * the name pointer as the default value. It needs following assertions.
 */

meta_event_t parse_meta_event(FILE *file) {
  uint8_t output[1];
  fread(output, 1, 1, file);
  assert(((output[0] >= 0x00) && (output[0] <= 0x07)) || (output[0] == 0x20) || (output[0] == 0x21) || (output[0] == 0x2f) || (output[0] == 0x51) || (output[0] == 0x54) || (output[0] == 0x58) || (output[0] == 0x59) || (output[0] == 0x7f));

  meta_event_t meta = META_TABLE[output[0]];
  printf("Name: %s\n", meta.name);

  fread(output, 1, 1, file);
  if (meta.data_len == 0) {
    meta.data_len = output[0];
  } else {
    if(strcmp(meta.name, "Sequence Number") != 0) {
      assert(meta.data_len == output[0]);
    } else {
      assert(meta.data_len == 2);
    }

  }
  if (strcmp(meta.name, "Sequence Number") != 0) {
    if (meta.data_len != 0) {
      uint8_t *data;

      data = malloc(sizeof(uint8_t) * meta.data_len);
      assert(data != NULL);
      for (int i = 0; i < meta.data_len; i++) {
        fread(output, 1, 1, file);
        data[i] = output[0];
      }

      meta.data = data;
    }
  } else {
    uint8_t size = output[0];
    if (size == 0) {

      return meta;
    } else {
      uint8_t *data;
      data = malloc(sizeof(uint8_t) * size);
      assert(data != NULL);
      for (int i = 0; i < size; i++) {
        fread(output, 1, 1, file);
        data[i] = output[0];
      }
      meta.data = data;
    }
  }
  return meta;
} /* parse_meta_event() */

/* Define parse_midi_event here */

/*
 * This function should read and return a midi_event struct from
 * the given MIDI file pointer. It has to allocate appropriate amount of space
 * in data pointer. However, given that event names should not change, leave
 * the name pointer as the default value. It needs following assertions.
 */

midi_event_t parse_midi_event(FILE *file, uint8_t status) {
  midi_event_t midi = { 0 };
  if (MIDI_TABLE[status].name == NULL) {
    midi = MIDI_TABLE[g_running_status];
    uint8_t output[1];
    uint8_t *data = 0;
    data = malloc(sizeof(uint8_t) * midi.data_len);
    assert(data != NULL);
    data[0] = status;
    for (int i = 1; i < midi.data_len; i++) {
      fread(output, 1, 1, file);
      data[i] = output[0];
    }
    midi.data = data;

  } else {
    midi = MIDI_TABLE[status];
    uint8_t output[1];
    uint8_t *data = 0;
    data = malloc(sizeof(uint8_t) * midi.data_len);
    assert(data != NULL);
    for (int i = 0; i < midi.data_len; i++) {
      fread(output, 1, 1, file);
      data[i] = output[0];
      midi.data = data;

    }

    g_running_status = status;
  }
  return midi;
} /* parse_midi_event() */

/* Define parse_var_len here */

/*
 * This function should read a variable length integer from the given MIDI
 * file pointer and return it as a fixed-size uint32_t.
 */

uint32_t parse_var_len(FILE * file) {
  uint32_t vlq_to_int = 0;
  uint8_t output[1];
  fread(output, 1, 1, file);
  while (output[0] >= 128) {
    vlq_to_int += (output[0] - 128);
    vlq_to_int = vlq_to_int << 7;
    fread(output, 1, 1, file);

  }
  vlq_to_int += output[0];
  return vlq_to_int;
} /* parse_var_len() */

/* Define event_type here */

/* This function takes in a pointer to an event, and should return either
 * SYS_EVENT_T, META_EVENT_T, or MIDI_EVENT_T, based on the type of the
 * event.
 */

uint8_t event_type(event_t *input_event) {
  if ((input_event->type == SYS_EVENT_1) ||
      (input_event->type == SYS_EVENT_2)) {
    return SYS_EVENT_T;
  } else if (input_event->type == META_EVENT) {
    return META_EVENT_T;
  } else {
    return MIDI_EVENT_T;
  }
} /* event_type() */

/* Define free_song here */

/*
 * This function should free all memory associated with the given song.
 */

void free_song(song_data_t *song) {
  free(song->path);
  track_node_t *current = NULL;
  while (song->track_list != NULL) {
    current = song->track_list;
    song->track_list = song->track_list->next_track;
    free_track_node(current);
  }
  free(song);
} /* free_song() */

/* Define free_track_node here */

/*
 * This function should free all memory associated with the given track node.
 */

void free_track_node(track_node_t *tracks) {
  event_node_t *current = NULL;
  while (tracks->track->event_list != NULL) {
    current = tracks->track->event_list;
    tracks->track->event_list = tracks->track->event_list->next_event;
    free_event_node(current);
  }
  free(tracks->track);
  free(tracks);
} /* free_track_node() */

/* Define free_event_node here */

/*
 * This function should free all memory associated with the given event node.
 */

void free_event_node(event_node_t *events) {
  if (events->event->type == META_EVENT) {
    free(events->event->meta_event.data);
  } else if (events->event->type == SYS_EVENT_1) {
    free(events->event->sys_event.data);

  } else if (events->event->type == SYS_EVENT_2) {
    free(events->event->sys_event.data);
  } else {
    free(events->event->midi_event.data);

  }
  free(events->event);
  //free(events->next_event);
  free(events);
} /* free_event_node() */

/* Define end_swap_16 here */

/*
 * This function takes in a buffer of two uint8_ts representing a uint16_t,
 * and should return a uint16_t with opposite endianness.
 */

uint16_t end_swap_16(uint8_t u8[2]) {
  uint16_t u16 = (u8[0] << 8) | u8[1];

  return u16;
} /* end_swap_16() */

/* Define end_swap_32 here */

/*
 * This function takes in a buffer of four uint8_ts representing a uint32_t.
 * and should return a uint32_t with opposite endianness.
 */

uint32_t end_swap_32(uint8_t u8[4]) {
  uint32_t u32 = (u8[0] << 24) | (u8[1] << 16) | (u8[2] << 8) | u8[3];
  return u32;
} /* end_swap_32() */
