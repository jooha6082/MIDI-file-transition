/* Name, alterations.c, CS 24000, Fall 2020
 * Last updated October 12, 2020
 */

/* Add any includes here */

#include "alterations.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

/*Define apply_to_events here */

/*
 * This function takes in a song, a function pointer, and a piece of arbitrary
 * data and applies that function with that piece of data to every event in
 * that given song.
 */

int apply_to_events(song_data_t *song, event_func_t fun, void *data) {
  int result = 0;
  int (*ptr_to_func)(event_t *, void *) = NULL;
  ptr_to_func = fun;
  track_node_t *current_track = NULL;
  current_track = song->track_list;
  while (current_track != NULL) {
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;

    while (current_event != NULL) {
      result += (*ptr_to_func)(current_event->event, data);
      current_event = current_event->next_event;
    }
    current_track = current_track->next_track;
  }
  return result;
} /* apply_to_events() */

/*Define change_event_octave here */

/*
 * This function takes in an event pointer and a pointer to a number of octaves.
 * If the event is a note event, it changes the octave of that note by the given
 * number of octaves, unless doing so would make the note value less than 0 or
 * greater than 127.
 */

int change_event_octave(event_t *event, int *octave_number) {
  if (event->midi_event.name == NULL) {
    return 0;
  }
  if ((event->type >= 128) && (event->type <= 175)) {
    if ((event->midi_event.data[0] + (*octave_number * 12) <= 127) &&
        (event->midi_event.data[0] + (*octave_number * 12) >= 0)) {
      event->midi_event.data[0] += (*octave_number * 12);
      return 1;
    }
  }

  return 0;
} /* change_event_octave() */

/*Define change_event_time here */

/*
 * This function takes in an event pointer and a pointer to a float multiplier.
 * It scales the delta-time of the event by the input multiplier. It returns
 * the difference in bytes associated with the vlq representation of the event's
 * new delta_time relative to its old delta-time.
 */

int change_event_time(event_t *event, float *multiplier) {
  int temp = (event->delta_time);
  uint32_t changed = (*multiplier) * event->delta_time;
  int count_delta = 0;

  /* It calculates the number of bytes of original delta_time. */

  while (temp != 0) {
    temp /= 128;
    count_delta++;
  }

  int count_changed = 0;

  /* It calculates the number of bytes of changed delta_time. */

  while (changed != 0) {
    changed /= 128;
    count_changed++;
  }

  int difference = count_changed - count_delta;
  event->delta_time = (*multiplier) * event->delta_time;

  return difference;
} /* change_event_time() */

/*Define change_event_instrument here */

/*
 * This function takes in an event pointer and a table mapping from current
 * notes to the desired new notes. If the event contains a note, it changes
 * the note to the value mapped to in the remapping table.
 */

int change_event_instrument(event_t *event, remapping_t remap) {
  if ((event->type >= 192) && (event->type <= 207)) {
    event->midi_event.data[0] = remap[event->midi_event.data[0]];
    return 1;
  }
  return 0;
} /* change_event_instrument() */

/*Define change_event_note here */

/*
 * This function takes in an event pointer and a table mapping from current
 * notes to the desired new notes. If the event contains a note, it changes
 * the note to the value mapped to in the remapping table.
 */

int change_event_note(event_t *event, remapping_t remap) {
  if (event->midi_event.name == NULL) {
    return 0;
  }
  if ((event->type >= 128) && (event->type <= 175)) {
    event->midi_event.data[0] = remap[event->midi_event.data[0]];
    return 1;
  }
  return 0;
} /* change_event_note() */

/*Define change_octave here */

/*
 * This function takes in a song and an integer number of octaves. Each note in
 * the song should have its octave shifted by the given number of octaves.
 * It returns the number of events that were modified.
 */

int change_octave(song_data_t *song, int octave_number) {

  /* All functions after this function use same traversing logic. */

  int counter = 0;

  track_node_t *current_track = NULL;
  current_track = song->track_list;

  while (current_track != NULL) {
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;

    while (current_event != NULL) {
      if (change_event_octave(current_event->event, &octave_number) == 1) {
        counter++;
      }

      current_event = current_event->next_event;
    }

    current_track = current_track->next_track;
  }

  return counter;
} /* change_octave() */

/*Define warp_time here */

/*
 * This function takes in a song and a float multiplier. It modify the song
 * that the overall length of the song changes by the multiplier. Additionally,
 * song should be left in a correct state with regard to started track lengths.
 * It returns the difference in the number of bytes between the song's new
 * representation when expressed as a midi file.
 */

int warp_time(song_data_t *song, float multiplier) {
  int counter = 0;

  track_node_t *current_track = NULL;
  current_track = song->track_list;

  while (current_track != NULL) {
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;
    int sum_returned = 0;
    while (current_event != NULL) {
      int returned = change_event_time(current_event->event, &multiplier);
      sum_returned += returned;
      counter += returned;

      current_event = current_event->next_event;
    }
    current_track->track->length += sum_returned;
    current_track = current_track->next_track;
  }

  return counter;
} /* warp_time() */

/*Define remap_instruments here */

/*
 * This function takes in a song and a table mapping from current instruments
 * to desired new instruments. It modifies the song so that all instruments
 * used are remapped according to the table. This function returns the number
 * of events that were modified.
 */

int remap_instruments(song_data_t *song, remapping_t remap) {
  int counter = 0;

  track_node_t *current_track = NULL;
  current_track = song->track_list;

  while (current_track != NULL) {
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;

    while (current_event != NULL) {
      if (change_event_instrument(current_event->event, remap) == 1) {
        counter++;
      }

      current_event = current_event->next_event;
    }

    current_track = current_track->next_track;
  }

  return counter;
} /* remap_instruments() */

/*Define remap_notes here */

/*
 * This function takes in a song and a table mapping from current notes to new
 * notes. It should modify the song so that all notes are remapped according to
 * the table. It returns the number of events that were modified.
 */

int remap_notes(song_data_t *song, remapping_t remap) {
  int counter = 0;

  track_node_t *current_track = NULL;
  current_track = song->track_list;

  while (current_track != NULL) {
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;

    while (current_event != NULL) {
      if (change_event_note(current_event->event, remap) == 1) {
        counter++;
      }

      current_event = current_event->next_event;
    }

    current_track = current_track->next_track;
  }

  return counter;
} /* remap_notes() */

/*Define add_round here */

/*
 * This function duplicates the track at the corresponding index in the song's
 * track list, change its octave by the specified quantity, delay it by the
 * specified instrument, then add it to the end of the song's track list. It
 * also update the song's metadata and change the channel of all MIDI channel
 * events.
 */

void add_round(song_data_t *song, int track_index, int octave_differential,
               unsigned int time_delay, uint8_t instrument) {
  assert(song->num_tracks > track_index);
  assert(song->format != 2);

  int count_track = 0;

  int channel_count[16] = { 0 };

  track_node_t *current_track = NULL;
  current_track = song->track_list;

  /* new_track is where the target track would be stored and modified. */

  track_node_t *new_track = NULL;
  new_track = malloc(sizeof(track_node_t));

  uint8_t remapper[0x100];
  for (int i = 0; i < 0xFF; i++) {
    remapper[i] = instrument;
  }

  while (current_track != NULL) {
    int count_event = 0;
    event_node_t *current_event = NULL;
    current_event = current_track->track->event_list;

    if (count_track == track_index) {
      new_track->track = malloc(sizeof(track_t));

      new_track->track->length = current_track->track->length;
      new_track->next_track = NULL;
      new_track->track->event_list = NULL;

    }

    /* new_event is storing the traget track's modified events. */

    event_node_t *new_event = NULL;
    event_node_t *new_event_prev = NULL;
    while (current_event != NULL) {

      if ((current_event->event->type == 240) &&
          (current_event->event->type == 247)) {

      } else if (current_event->event->type == 255) {

      } else { /* Check which channel is used. */
        if ((current_event->event->midi_event.status >= 128) &&
            (current_event->event->midi_event.status <= 239)) {
          int channel = current_event->event->midi_event.status & 0xF;
          channel_count[channel] = 1;
        }
      }

      /* Find the target index for allocating memory and give information. */

      if (count_track == track_index) {
        new_event = malloc(sizeof(event_node_t));
        new_event->next_event = NULL;
        new_event->event = malloc(sizeof(event_t));
        new_event->event->delta_time = current_event->event->delta_time;
        new_event->event->type = current_event->event->type;

        if ((current_event->event->type == 240) &&
            (current_event->event->type == 247)) {
          new_event->event->sys_event.data_len =
          current_event->event->sys_event.data_len;

          if (current_event->event->sys_event.data_len != 0) {
            new_event->event->sys_event.data =
            malloc(sizeof(uint8_t) * current_event->event->sys_event.data_len);
            for (int i = 0; i < new_event->event->sys_event.data_len; i++) {
              new_event->event->sys_event.data[i] =
              current_event->event->sys_event.data[i];
            }

          }

        } else if (current_event->event->type == 255) {
          new_event->event->meta_event.name =
          current_event->event->meta_event.name;
          new_event->event->meta_event.data_len =
          current_event->event->meta_event.data_len;

          if (current_event->event->meta_event.data_len != 0) {
            new_event->event->meta_event.data =
            malloc(sizeof(uint8_t) * current_event->event->meta_event.data_len);
            for (int i = 0; i < new_event->event->meta_event.data_len; i++) {
              new_event->event->meta_event.data[i] =
              current_event->event->meta_event.data[i];
            }

          }

        } else {
          new_event->event->midi_event.name =
          current_event->event->midi_event.name;
          new_event->event->midi_event.status =
          current_event->event->midi_event.status;
          new_event->event->midi_event.data_len =
          current_event->event->midi_event.data_len;

          if (current_event->event->midi_event.data_len != 0) {
            new_event->event->midi_event.data =
            malloc(sizeof(uint8_t) * current_event->event->midi_event.data_len);
            for (int i = 0; i < new_event->event->midi_event.data_len; i++) {
              new_event->event->midi_event.data[i] =
              current_event->event->midi_event.data[i];
            }
          }
        }
        change_event_octave(new_event->event, &octave_differential);
        change_event_instrument(new_event->event, remapper);

        if (count_event == 0) {
          new_event->event->delta_time += time_delay;
        }
        if (new_track->track->event_list == NULL){
          new_track->track->event_list = new_event;
        } else {
          new_event_prev->next_event = new_event;
        }
        new_event_prev = new_event;
      }

      count_event++;
      current_event = current_event->next_event;
    }

    if (count_track == song->num_tracks - 1) {
      current_track->next_track = new_track;
      new_track->next_track = NULL;
      break;
    }

    count_track++;
    current_track = current_track->next_track;
  }

  /* Check for which channel is the highest among used ones. */

  int flag = 0;
  int channel_max = 0;
  for (int i = 0; i < 16; i++) {
    if (channel_count[i] < 1) {
      flag = 1;
      channel_max = i;
      break;
    }
  }

  assert(flag == 1);

  song->num_tracks += 1;

  track_node_t *temp_track = NULL;
  temp_track = song->track_list;

  /* Change channel if it is midi_event. */

  while (temp_track->next_track != NULL) {
    temp_track = temp_track->next_track;
  }
  event_node_t *temp_event = NULL;
  temp_event = temp_track->track->event_list;

  while (temp_event != NULL) {
    if ((temp_event->event->type == 240) && (temp_event->event->type == 247)) {
    } else if (temp_event->event->type == 255) {
    } else {
      if ((temp_event->event->midi_event.status >= 128) &&
          (temp_event->event->midi_event.status <= 239)) {
        temp_event->event->midi_event.status =
        (temp_event->event->midi_event.status >> 4) * 16 + channel_max;
        temp_event->event->type = temp_event->event->midi_event.status;
      }
    }

    temp_event = temp_event->next_event;
  }
} /* add_round() */

/*
 * Function called prior to main that sets up random mapping tables
 */

void build_mapping_tables()
{
  for (int i = 0; i <= 0xFF; i++) {
    I_BRASS_BAND[i] = 61;
  }

  for (int i = 0; i <= 0xFF; i++) {
    I_HELICOPTER[i] = 125;
  }

  for (int i = 0; i <= 0xFF; i++) {
    N_LOWER[i] = i;
  }
  //  Swap C# for C
  for (int i = 1; i <= 0xFF; i += 12) {
    N_LOWER[i] = i - 1;
  }
  //  Swap F# for G
  for (int i = 6; i <= 0xFF; i += 12) {
    N_LOWER[i] = i + 1;
  }
} /* build_mapping_tables() */
