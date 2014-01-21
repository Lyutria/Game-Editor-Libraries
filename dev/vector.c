// WHY IN-DEV?:
// Currently non-functional due to function pointer
// bug in GE. GE returns a warning message on use
// of a function without use, so trying to set a
// function pointer to a function does return that
// warning. Warnings in GE can break global code
// causing overall error.

// WHEN STOP DEV?:
// When I find a work-around for this function pointer
// bug and finish developing the thing in general
//
// When this is finished, it'll be merged into <data.c>

// CURRENT PRE-REQUISITES:
// > none

// ASSUMED PRE-REQUISITES:
// > main.c

// ____        __ _
//|  _ \  ___ / _(_)_ __   ___
//| | | |/ _ \ |_| | '_ \ / _ \
//| |_| |  __/  _| | | | |  __/
//|____/ \___|_| |_|_| |_|\___|
//

// A couple super-heavy defines are here so the end-user can
// define their own vector types to be compatible with whatever
// typing they want, e.g.:
//
//   typedef struct pair {
//     int p1;
//     int p2;
//   } PAIR;
//
//   NEW_VECTOR_TYPE(PAIR);
//   Vector(PAIR, my_new_vector);
//   my_new_vector.push_back(my_new_vector, [PAIR var]);
//
// Though it's not usuable ATM.
// It also requires a one-word name for your variable, so you'll
// HAVE to typedef those structs and "short unsigned char"'s or whatever.

#define NEW_VECTOR_TYPE(v_type)                                             \
  typedef struct v_type##VectorNode {                                       \
    v_type value;                                                           \
    struct v_type##VectorNode* next;                                        \
  } v_type##VectorNode;                                                     \
                                                                            \
  typedef struct v_type##Vector {                                           \
    int length;                                                             \
    struct v_type##VectorNode* first;                                       \
    struct v_type##VectorNode* last;                                        \
    void (*push_back)(struct v_type##Vector*, v_type);                      \
  } v_type##Vector;                                                         \
                                                                            \
  void v_type##_vector_push_back(v_type##Vector* source, v_type new_value) {\
    v_type##VectorNode* new_node = malloc(sizeof(v_type##VectorNode));      \
    v_type##VectorNode* node_iter;                                          \
    new_node->value = new_value;                                            \
    new_node->next == NULL;                                                 \
    if (source->first == NULL) {                                            \
      source->first = new_node;                                             \
      source->last = new_node;                                              \
    } else {                                                                \
      node_iter = source->first;                                            \
      while (node_iter->next != NULL) {                                     \
        node_iter = node_iter->next;                                        \
      }                                                                     \
      node_iter->next = new_node;                                           \
    }                                                                       \
  }                                                                         \
                                                                            \
  v_type##Vector v_type##_vector_new() {                                    \
    v_type##Vector new_vector;                                              \
    new_vector.length = 0;                                                  \
    new_vector.first = NULL;                                                \
    new_vector.last = NULL;                                                 \
    new_vector.push_back = v_type##_vector_push_back;                       \
    return new_vector;                                                      \
  }

#define Vector(v_type, v_new) \
  v_type##Vector v_new = v_type##_vector_new()


NEW_VECTOR_TYPE(int);
