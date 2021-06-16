/**
 * @file Vector.c
 * @author  Eran Turgeman <eran.turgeman@mail.huji.ac.il>
 * @version 1.0
 * @date 07/12/2020
 * 
 * @brief Implementation for Vector.h.
 *
 * @section LICENSE
 * This program is private and was made for the 2020 67315 course
 */

// ------------------------------ includes ------------------------------
#include <string.h>
#include "Vector.h"

// -------------------------- const definitions -------------------------

/**
 * @def NO_RETURN_VALUE
 * @brief return value used in void functions for the CHECK_ERROR constant
 */
#define NO_RETURN_VALUE ((void)(0))

/**
 * @def CHECK_ERROR
 * @brief constant reecives a boolean expression and checks if it is true- if
 * not return the retern value ret_val
 */
#define CHECK_ERROR(expression, ret_val) if(!(expression)){\
return ret_val;\
}

/**
 * @def VAL_NOT_IN_VEC
 * @brief return value of VectorFind in case the value haven't been found
 */
#define VAL_NOT_IN_VEC -1

/**
 * @def SUCCESS
 * @brief return value for int type functions in case of succession
 */
#define SUCCESS 1

/**
 * @def FAIL
 * @brief return value for int type functions in case of failure
 */
#define FAIL 0

/**
 * @def LOAD_FACTOR_FAIL
 * @brief return value of HashMapGetLoadFactor if failed
 */
#define LOAD_FACTOR_FAIL -1

// ------------------------------ functions -----------------------------

typedef void* vectorElemT;

/**
 * resizes the given array to the given new_size
 * @param p_array_to_resize pointer to an array of data needed to be resized
 * @param new_size array new size
 * @return  upon success, 0 otherwise
 */
static int ResizeArray(void ***p_array_to_resize, size_t new_size);

/**
 * closes a hole in index ind in the vector after removing an element
 * @param vector Vector struct object
 * @param ind the empty index we need to close the hole upon
 */
static void VectorCloseHole(Vector *vector, size_t ind);

/**
 * reopens a hole in the vector in the given index
 * @param vector Vector struct object
 * @param ind the index we need to open the hole at
 */
static void VectorOpenHole(Vector *vector, size_t ind);


Vector *VectorAlloc(VectorElemCpy elem_copy_func, VectorElemCmp
elem_cmp_func, VectorElemFree elem_free_func){

  CHECK_ERROR(elem_cmp_func, NULL)
  CHECK_ERROR(elem_copy_func, NULL)
  CHECK_ERROR(elem_free_func, NULL)
  Vector* vector = malloc(sizeof(Vector));
  CHECK_ERROR(vector, NULL)
  vector->capacity = VECTOR_INITIAL_CAP;
  vector->size = 0;
  vector->elem_cmp_func = elem_cmp_func;
  vector->elem_copy_func = elem_copy_func;
  vector->elem_free_func = elem_free_func;
  vectorElemT *vec_data = malloc(VECTOR_INITIAL_CAP * sizeof(vectorElemT));
  if(!vec_data)
  {
    free(vector);
    return NULL;
  }
  vector->data = vec_data;
  return vector;
}

void VectorFree(Vector **p_vector){

  CHECK_ERROR(p_vector, NO_RETURN_VALUE)
  CHECK_ERROR(*p_vector, NO_RETURN_VALUE)
  Vector *to_free = *p_vector;
  if(to_free->size != 0){
    for(size_t i = 0; i < to_free->size; i++){
      to_free->elem_free_func(&to_free->data[i]);
    }
  }
  free((*p_vector)->data);
  free((*p_vector));
  *p_vector = NULL;
}

void *VectorAt(Vector *vector, size_t ind){

  CHECK_ERROR(vector, NULL)
  CHECK_ERROR( ind < vector->size , NULL)
  return vector->data[ind];
}

int VectorFind(Vector *vector, void *value){

  CHECK_ERROR(vector, VAL_NOT_IN_VEC)
  CHECK_ERROR(value, VAL_NOT_IN_VEC)
  if(vector->size != 0){
    for(size_t i = 0; i < vector->size; i++){
      if(vector->elem_cmp_func(vector->data[i], value)){
        return i;
      }
    }
  }
  return VAL_NOT_IN_VEC;
}

double VectorGetLoadFactor(Vector *vector){

  CHECK_ERROR(vector, LOAD_FACTOR_FAIL)
  if(vector->capacity != 0){
    return (double)vector->size / vector->capacity;
  }
  return LOAD_FACTOR_FAIL;
}

int VectorPushBack(Vector *vector, void *value){

  CHECK_ERROR(vector, FAIL)
  CHECK_ERROR(value, FAIL)
  void *val_copy = vector->elem_copy_func((const void*)value);
  CHECK_ERROR(val_copy, FAIL)
  vector->size++; //increase to get the load factor "after" insert
  double cur_load_factor = VectorGetLoadFactor(vector);
  if(cur_load_factor == LOAD_FACTOR_FAIL){
    vector->elem_free_func(val_copy);
    vector->size--;
    return FAIL;
  }
  if(cur_load_factor > VECTOR_MAX_LOAD_FACTOR){
    int resize_succeeded = ResizeArray(&(vector->data), vector->capacity *
    VECTOR_GROWTH_FACTOR * sizeof(vectorElemT));
    if(!resize_succeeded){
      vector->elem_free_func(val_copy);
      vector->size--;
      return FAIL;
    }
    vector->capacity = vector->capacity * VECTOR_GROWTH_FACTOR;
  }
  vector->data[vector->size - 1] = val_copy;
  return SUCCESS;
}
static int ResizeArray(void ***p_array_to_resize, const size_t new_size) {

  CHECK_ERROR(p_array_to_resize, FAIL)
  CHECK_ERROR(*p_array_to_resize, FAIL)
  vectorElemT *tmp = realloc((*p_array_to_resize), new_size);
  if(!tmp){
    return FAIL;
  }
  *p_array_to_resize = tmp;
  return SUCCESS;
}

int VectorErase(Vector *vector, size_t ind){

  CHECK_ERROR(vector, FAIL)
  CHECK_ERROR(ind < vector->size, FAIL)
  vector->size--;
  double cur_load_factor = VectorGetLoadFactor(vector);
  if(cur_load_factor == LOAD_FACTOR_FAIL){
    vector->size++;
    return FAIL;
  }
  vectorElemT tmp = vector->data[ind];
  vector->data[ind] = NULL;
  VectorCloseHole(vector, ind);

  if(cur_load_factor < VECTOR_MIN_LOAD_FACTOR){
    int resize_succeeded = ResizeArray(&(vector->data),(vector->capacity /
    VECTOR_GROWTH_FACTOR) * sizeof(vectorElemT));
    if(!resize_succeeded){
      VectorOpenHole(vector, ind);
      vector->data[ind] = tmp;
      vector->size++;
      return FAIL;
    }
    vector->capacity = vector->capacity / VECTOR_GROWTH_FACTOR;
  }
  vector->elem_free_func(&tmp);
  return SUCCESS;

}
static void VectorOpenHole(Vector *vector, size_t ind) {

  CHECK_ERROR(vector, NO_RETURN_VALUE)
  CHECK_ERROR(ind < vector->size, NO_RETURN_VALUE)
  size_t i = vector->size;
  for(; i > ind; i--){
    vector->data[i] = vector->data[i - 1];
  }
  vector->data[ind] = NULL;
}
static void VectorCloseHole(Vector *vector, size_t ind) {

  CHECK_ERROR(vector, NO_RETURN_VALUE)
  CHECK_ERROR(ind < vector->size, NO_RETURN_VALUE)
  if(ind == vector->size - 1){
    return;
  }
  size_t i = ind;
  for(; i < vector->size; i++){
    vector->data[i] = vector->data[i + 1];
  }
  vector->data[i] = NULL;
}

void VectorClear(Vector *vector){

  CHECK_ERROR(vector, NO_RETURN_VALUE)
  while(vector->size != 0){
    VectorErase(vector, vector->size - 1);
  }
}
