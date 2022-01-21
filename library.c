/* Name, library.c, CS 24000, Fall 2020
 * Last updated October 12, 2020
 */

/* Add any includes here */

#include "library.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ftw.h>

int find_mid_files(const char *, const struct stat *, int);

tree_node_t *g_song_library = NULL;

/* Define find_parent_pointer here */

/*
 * This function should locate the node in the given tree having the given
 * song_name. If the desired song cannot be found in the tree. return NULL.
 * Otherwise, return the parent node's pointer to the dsired song.
 * If the root node itself is the desire pointer, return the input pointer.
 */

tree_node_t **find_parent_pointer(tree_node_t **tree_ptr, const char *song) {
  tree_node_t *current = NULL;
  current = *tree_ptr;

  while (current != NULL) {
    if (strcmp(current->song_name, song) < 0) {
      current = current->right_child;
    } else if (strcmp(current->song_name, song) > 0) {
      current = current->left_child;
    } else {
      *tree_ptr = current;
      return tree_ptr;
    }
  }
  return NULL;
} /* find_parent_pointer() */

/* Define tree_insert here */

/*
 * The first argument to this function is a double pointer to the root of the
 * tree. The second argument is a node to be inserted into the tree. If the
 * node is already present in the tree, return DUPLICATE_SONG. Otherwise,
 * insert the node at the appropriate location and return INSERT_SUCCESS.
 */

int tree_insert(tree_node_t **tree_ptr, tree_node_t *node) {
  tree_node_t *current = NULL;
  current = *tree_ptr;
  if (current == NULL) {
    current = node;
    return INSERT_SUCCESS;
  }
  while (current != NULL) {
    if (strcmp(current->song_name, node->song_name) < 0) {
      if (current->right_child == NULL) {
        current->right_child = node;
        break;
      }
      current = current->right_child;
    } else if (strcmp(current->song_name, node->song_name) > 0) {
      if (current->left_child == NULL) {
        current->left_child = node;
        break;
      }
      current = current->left_child;
    } else {
      return DUPLICATE_SONG;
    }
  }

  return INSERT_SUCCESS;
} /* tree_insert() */

/* Define remove_song_from_tree here */

/*
 * This function should search the given tree for a song with the given name.
 * If no such song is found, return SONG_NOT_FOUND. Otherwise, remove the
 * tree_node_t associated with that song from the tree and free all memory
 * associated with the node, including the song. Then return, DELETE_SUCCESS.
 */

int remove_song_from_tree(tree_node_t **tree_ptr, const char *song) {
  tree_node_t *current = NULL;
  current = *tree_ptr;

  tree_node_t *parent = NULL;
  parent = *tree_ptr;

  while (current != NULL) {
    if (strcmp(current->song_name, song) > 0) {
      parent = current;
      current = current->left_child;
    } else if (strcmp(current->song_name, song) < 0) {
      parent = current;
      current = current->right_child;
    } else {
      break;
    }
  }

  if (current == NULL) {
    return SONG_NOT_FOUND;
  }

  tree_node_t *var = NULL;
  if ((current->left_child != NULL) && (current->right_child == NULL)) {
    var = current->left_child;
    if (current == parent->left_child) {
      free_node(current);
      parent->left_child = var;
      return DELETE_SUCCESS;
    } else if (current == parent->right_child) {
      free_node(current);
      parent->right_child = var;
      return DELETE_SUCCESS;
    }

  } else if ((current->right_child != NULL) && (current->left_child == NULL)) {
    var = current->right_child;
    if (current == parent->left_child) {
      free_node(current);
      parent->left_child = var;
      return DELETE_SUCCESS;
    } else if (current == parent->right_child) {
      free_node(current);
      parent->right_child = var;
      return DELETE_SUCCESS;
    }

  } else if ((current->left_child != NULL) && (current->right_child != NULL)) {
    tree_node_t *successor = NULL;
    successor = current->right_child;

    tree_node_t *successor_parent = NULL;
    successor_parent = NULL;

    while ((successor != NULL) && (successor->left_child != NULL)) {
      successor_parent = successor;
      successor = successor->left_child;
    }
    strcpy(current->song_name, successor->song_name);
    current->song = successor->song;
    if (successor_parent != NULL) {
      successor_parent->left_child = successor->right_child;
    } else {
      current->right_child = successor->right_child;
    }
    free_node(successor);
    return DELETE_SUCCESS;

  } else {
    if (parent->left_child == current) {
      free_node(current);
      parent->left_child = NULL;
    } else if (parent->right_child == current) {
      free_node(current);
      parent->right_child = NULL;
    }
    return DELETE_SUCCESS;
  }

  return SONG_NOT_FOUND;
} /* remove_song_from_tree() */

/* Define free_node here */

/*
 * This function should free all data associated with the given tree_node_t,
 * including the data associated with its song.
 */

void free_node(tree_node_t *node) {

  free_song(node->song);
  free(node);
} /* free_node() */

/* Define print_node here */

/*
 * This function should print the song name associated with the given
 * tree_node_t, including the data associated with its song.
 */

void print_node(tree_node_t *node, FILE *file) {
  assert(fwrite(node->song_name , 1, sizeof(strlen(node->song_name) + 1),
         file) == 1);
  assert(fwrite("\n", 1, 1, file) == 1);
} /* print_node() */

/* Define traverse_pre_order here */

/*
 * This function accepts a tree pointer, a piece of arbitrary data, and a
 * function pointer. This function should perform a pre-order traversal of the
 * tree, where the operation applied to each visited node is call to the
 * function pointer with the node and arbitrary data passed in.
 */

void traverse_pre_order(tree_node_t *node, void *data, traversal_func_t fun) {
  if (node == NULL) {
    return;
  }
  fun(node, data);
  traverse_pre_order(node->left_child, data, fun);
  traverse_pre_order(node->right_child, data, fun);
} /* traverse_pre_order() */

/* Define traverse_in_order here */

/*
 * This function is exactly the same as the prior, but should visit each node
 * in a in-order fashion.
 */

void traverse_in_order(tree_node_t *node, void *data, traversal_func_t fun) {
  if (node == NULL) {
    return;
  }
  traverse_in_order(node->left_child, data, fun);
  fun(node, data);
  traverse_in_order(node->right_child, data, fun);
} /* traverse_in_order() */

/* Define traverse_post_order here */

/*
 * This function is exactly the same as the prior, but should visit each node
 * in a post-order fashion.
 */

void traverse_post_order(tree_node_t *node, void *data, traversal_func_t fun) {
  if (node == NULL) {
    return;
  }
  traverse_post_order(node->left_child, data, fun);
  traverse_post_order(node->right_child, data, fun);
  fun(node, data);
} /* traverse_post_order() */

/* Define free_library here */

/*
 * This function should free all data associated with the input tree.
 */

void free_library(tree_node_t *node) {
  if (node == NULL) {
    return;
  }
  free_library(node->left_child);
  free_library(node->right_child);
  free_node(node);
} /* free_library() */

/* Define write_song_list here */

/*
 * This function should print the names of all songs in the given to tree to
 * the file, separated by newline characters. These songs should be printed in
 * sorted order, where the sorting criteria is based on strcmp.
 */

void write_song_list(FILE *fp, tree_node_t *tree) {

} /* write_song_list() */

/* Define make_library here */

/*
 * This function takes in the name of a directory, and should search through
 * the directory structure, adding every MIDI file found in the structure to
 * the tree held by g_song_library. Assert that the directory structure
 * contains no douplicate songs.
 */

void make_library(const char *directory) {
  printf("in ml: %s\n", directory);
  ftw(directory, find_mid_files, 1);
  g_song_library = NULL;
} /* make_library() */

/*
 * This function?
 */

int find_mid_files(const char *directory, const struct stat * ptr, int flag) {

  printf("flag %d\n", flag);

  return 0;
} /* find_mid_files() */
