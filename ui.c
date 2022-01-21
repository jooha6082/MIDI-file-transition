/* Name, ui.c, CS 24000, Fall 2020
 * Last updated October 12, 2020
 */

/* Add any includes here */

#include "ui.h"
#include <gtk/gtk.h>
/* Define update_song_list here */
void update_song_list() {

}

/* Define update_drawing_area here */
void update_drawing_area() {

}

/* Define update_info here */
void update_info() {

}

/* Define update_song here */
void update_song() {

}

/* Define range_of_song here */
void range_of_song(song_data_t *song, int *a, int *b, int *c) {

}

/* Define activate here */
void activate(GtkApplication *gtk, gpointer gp){
  GtkWidget *window;
  window = gtk_application_window_new(gtk);
  gtk_window_set_title(GTK_WINDOW(window), "MIDI Library");
  gtk_window_set_default_size(GTK_WINDOW(window), 1000, 1000);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  GtkWidget *add;
  add = gtk_button_new_with_label("Add Song from File");
  g_signal_connect(add, "clicked", G_CALLBACK (add_song_cb), NULL);

  GtkWidget *load;
  load = gtk_button_new_with_label("Load from Directory");
  g_signal_connect(load, "clicked", G_CALLBACK (load_songs_cb), NULL);

  GtkWidget *hbox;
  hbox = gtk_box_new(0,11);
  gtk_box_pack_start(GTK_BOX(hbox),add, 0, 0, 0);
  gtk_box_pack_start(GTK_BOX(hbox), load, 0, 0, 0);
  gtk_container_add(GTK_CONTAINER (window), hbox);

  GtkWidget

//  gtk_grid_attach(GTK_GRID(grid), add, 0, 0, 1, 1);
//  gtk_grid_attach(GTK_GRID(grid), add, 1, 0, 1, 1);

  gtk_widget_show_all(window);
}

/* Define add_song_cb here */
void add_song_cb(GtkButton *button, gpointer gp) {
  g_print("heelo\n");
}

/* Define load_songs_cb here */
void load_songs_cb(GtkButton *button, gpointer gp) {
  g_print("hiilo\n");
}

/* Define song_selected_cb here */
void song_selected_cb(GtkListBox *list, GtkListBoxRow *row) {

}

/* Define search_bar_cb here */
void search_bar_cb(GtkSearchBar *bar, gpointer gp) {

}

/* Define time_scale_cb here */
void time_scale_cb(GtkSpinButton *spin, gpointer gp) {

}

/* Define draw_cb here */
gboolean draw_cb(GtkDrawingArea *draw, cairo_t *cairo, gpointer gp) {

  return 0;
}

/* Define warp_time_cb here */
void warp_time_cb(GtkSpinButton *spin, gpointer gp) {

}

/* Define song_octave_cb here */
void song_octave_cb(GtkSpinButton *spin, gpointer gp) {

}

/* Define instrument_map_cb here */
void song_instrument_map_cb(GtkComboBoxText *combo, gpointer gp) {

}

/* Define note_map_cb here */
void note_map_cb(GtkComboBoxText *combo, gpointer gp) {

}

/* Define save_song_cb here */
void save_song_cb(GtkButton *button, gpointer gp) {

}

/* Define remove_song_cb here */
void remove_song_cb(GtkButton *button, gpointer gp) {

}

/*
 * Function called prior to main that sets up the instrument to color mapping
 */

void build_color_palette()
{
  static GdkRGBA palette[16];	

  memset(COLOR_PALETTE, 0, sizeof(COLOR_PALETTE));
  char* color_specs[] = {
    // Piano, red
    "#ff0000",
    // Chromatic percussion, brown
    "#8b4513",
    // Organ, purple
    "#800080",
    // Guitar, green
    "#00ff00",
    // Bass, blue
    "#0000ff",
    // Strings, cyan
    "#00ffff",
    // Ensemble, teal
    "#008080",
    // Brass, orange
    "#ffa500",
    // Reed, magenta
    "#ff00ff",
    // Pipe, yellow
    "ffff00",
    // Synth lead, indigo
    "#4b0082",
    // Synth pad, dark slate grar
    "#2f4f4f",
    // Synth effects, silver
    "#c0c0c0",
    // Ehtnic, olive
    "#808000",
    // Percussive, silver
    "#c0c0c0",
    // Sound effects, gray
    "#808080",
  };

  for (int i = 0; i < 16; ++i) {
    gdk_rgba_parse(&palette[i], color_specs[i]);
    for (int j = 0; j < 8; ++j) {
      COLOR_PALETTE[i * 8 + j] = &palette[i];
    }
  }
} /* build_color_palette() */
