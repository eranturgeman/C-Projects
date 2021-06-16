/**
 * @file HashMap.c
 * @author  Eran Turgeman <eran.turgeman@mail.huji.ac.il>
 * @version 1.0
 * @date 10/12/2020
 * 
 * @brief implementation for HashMap.h
 *
 * @section LICENSE
 * This program is private and was made for the 2020 67315 course
 */

// ------------------------------ includes ------------------------------
#include <stdlib.h>
#include "HashMap.h"

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
 * @def LOAD_FACTOR
 * @brief calculation of "fake" load factor for the clear function
 */
#define LOAD_FACTOR(size, capacity) (double)size / capacity

/**
 * @def LOAD_FACTOR_FAIL
 * @brief return value of HashMapGetLoadFactor if failed
 */
#define LOAD_FACTOR_FAIL -1

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
 * @def EQUALS
 * @brief return value for compare functions in case the items are equal
 */
#define EQUALS 1

/**
 * @def RESIZED
 * @brief flag symbolizing if a buckets array got resize
 */
#define RESIZED 1

/**
 * @def EMPTY_VECTOR
 * @brief size of empty vector
 */
#define EMPTY_VECTOR 0

// ------------------------------ functions -----------------------------

typedef Vector *mapCellT;

/**
 * the function look for a pair with the given key and returns pointer to it
 * @param hash_map HashMap struct object
 * @param key key of a pair we want to find (void*)
 * @return pointer to the pair if found, NULL otherwise
 */
static Pair *HashMapGetPairByKey(HashMap *hash_map, KeyT key);

/**
 * function allocates a bucket array in given size
 * @param size size of the new dynamiclly allocated array
 * @return pointer to new allocated array or NULL if failed
 */
static mapCellT *BucketsAlloc(size_t size);

/**
 * function gets dest array and src array and rehashes all pairs from src
 * array to dest array
 * @param hash_map HashMap struct object
 * @param dest_buckets array of buckets to rehash to
 * @param src_buckets array of buckets to rehash from
 * @param dest_size number of elements of dest_buckets
 * @param src_size number of elements of src_buckets
 * @return 1 upon success, 0 otherwise
 */
static int ReHashAll(HashMap *hash_map, mapCellT *dest_buckets, mapCellT
*src_buckets, size_t dest_size, size_t src_size);

/**
 * function frees buckets array and everything inside it
 * @param p_buckets pointer to array of buckets to be freed
 * @param arr_size size of the array
 */
static void FreeBuckets(mapCellT **p_buckets, size_t arr_size);

/**
 * function inserts new pair in correct position- allocates new vector if
 * there is none in the spot that the new pair should go to, and replaces
 * pairs if there is a pair with the same key already in map
 * @param hash_map HashMap struct object
 * @param index_to_insert index of a vector in buckets array we want to
 * insert to
 * @param pair_to_insert pointer to the pair we want to insert
 * @return 1 upon success, 0 otherwise
 */
static int VectorInsert(HashMap *hash_map, size_t index_to_insert, Pair
*pair_to_insert);

/**
 * functions vreates new buckets array and rehashes all old array to the new
 * one so at the end of the func we get new buckets array in wanted size with
 * all pairs in it correctly
 * @param hash_map HashMap struct object
 * @param p_new_buckets pointer to the pointer the new array will be kept on
 * @param old_buckets pointer to old buckets array
 * @param new_buckets_size number of elements in new buckets array
 * @param old_buckets_size number of elements in old buckets array
 * @return 1 upon success, 0 otherwise
 */
static int CreateNewBuckets(HashMap *hash_map, mapCellT **p_new_buckets,
    mapCellT *old_buckets, size_t new_buckets_size, size_t old_buckets_size);

/**
 * calculates the decrement in capacity size in the map during clearing it so
 * we wont have to use HashMapErase every time (inefficient)
 * @param vector_size size of the vector being erased
 * @param p_map_size size that the map should have at this point
 * @param p_map_capacity capacity that the map should have at this point
 *
 * what it does? calculates the size the map should have according
 * MIN_LOAD_FACTOR and updates the capacity the map should have after erasing
 * all pairs in the vector
 */
static void UpdateCapacityAndSize(size_t vector_size, size_t *p_map_size,
    size_t *p_map_capacity);


HashMap *HashMapAlloc(HashFunc hash_func, HashMapPairCpy pair_cpy,
                      HashMapPairCmp pair_cmp, HashMapPairFree pair_free) {

  CHECK_ERROR(hash_func && pair_cpy && pair_cmp && pair_free, NULL)
  HashMap *hash_map = malloc(sizeof(HashMap));
  CHECK_ERROR(hash_map, NULL)
  mapCellT *bucket_arr = calloc(HASH_MAP_INITIAL_CAP, sizeof(mapCellT));
  if (!bucket_arr) {
    free(hash_map);
    return NULL;
  }
  hash_map->buckets = bucket_arr;
  hash_map->size = 0;
  hash_map->capacity = HASH_MAP_INITIAL_CAP;
  hash_map->hash_func = hash_func;
  hash_map->pair_free = pair_free;
  hash_map->pair_cpy = pair_cpy;
  hash_map->pair_cmp = pair_cmp;
  return hash_map;
}

void HashMapFree(HashMap **p_hash_map) {

  CHECK_ERROR(p_hash_map && (*p_hash_map), NO_RETURN_VALUE)
  if ((*p_hash_map)->size != 0) {
    for (size_t i = 0; i < (*p_hash_map)->capacity; i++) {
      VectorFree(&(*p_hash_map)->buckets[i]);
    }
  }
  free((*p_hash_map)->buckets);
  free(*p_hash_map);
  *p_hash_map = NULL;
}

double HashMapGetLoadFactor(HashMap *hash_map) {

  CHECK_ERROR(hash_map, LOAD_FACTOR_FAIL)
  if (hash_map->capacity != 0) {
    return (double) hash_map->size / hash_map->capacity;
  }
  return LOAD_FACTOR_FAIL;
}

int HashMapInsert(HashMap *hash_map, Pair *pair) {

  CHECK_ERROR(hash_map && pair, FAIL)
  if (!HashMapContainsKey(hash_map, pair->key)) {
    hash_map->size++;
  }
  //get load factor and if fails free pair and decrease size
  double load_factor = HashMapGetLoadFactor(hash_map);
  if (load_factor == LOAD_FACTOR_FAIL) {
    hash_map->size--;
    return FAIL;
  }
  //checks if need new size for buckets
  int resize_flag = 0;
  mapCellT *old_buckets = NULL, *new_buckets = NULL;
  if (load_factor > HASH_MAP_MAX_LOAD_FACTOR) {
    int new_buckets_success = CreateNewBuckets(hash_map, &new_buckets,
        hash_map->buckets, hash_map->capacity * HASH_MAP_GROWTH_FACTOR,
        hash_map->capacity);
    if (new_buckets_success == FAIL) {
      hash_map->size--;
      return FAIL;
    }
    hash_map->capacity *= HASH_MAP_GROWTH_FACTOR;
    old_buckets = hash_map->buckets; // save the original buckets
    hash_map->buckets = new_buckets;
    resize_flag = RESIZED;
  }
  //inserting new pair
  size_t index_to_insert_at = (hash_map->hash_func(pair->key)) &
      (hash_map->capacity - 1);
  int insert_success = VectorInsert(hash_map, index_to_insert_at, pair);
  if (insert_success == FAIL) {
    if (resize_flag) {
      hash_map->buckets = old_buckets;
      hash_map->capacity /= HASH_MAP_GROWTH_FACTOR;
      FreeBuckets(&new_buckets, hash_map->capacity * HASH_MAP_GROWTH_FACTOR);
    }
    hash_map->size--;
    return FAIL;
  }
  if (resize_flag) {
    FreeBuckets(&old_buckets, hash_map->capacity / HASH_MAP_GROWTH_FACTOR);
  }
  return SUCCESS;
}

static int VectorInsert(HashMap *hash_map, size_t index_to_insert, Pair
*pair_to_insert) {

  CHECK_ERROR(hash_map, FAIL)
  CHECK_ERROR(index_to_insert < hash_map->capacity, FAIL)
  CHECK_ERROR(pair_to_insert, FAIL)
  Vector *vec = hash_map->buckets[index_to_insert];
  if (vec == NULL) {
    vec = VectorAlloc(hash_map->pair_cpy, hash_map->pair_cmp,
        hash_map->pair_free);
    if (!vec) {
      return FAIL;
    }
    hash_map->buckets[index_to_insert] = vec;
  }
  for (size_t i = 0; i < vec->size; i++) {
    Pair *cur_pair = (Pair *) (vec->data[i]);
    if (cur_pair->key_cmp(cur_pair->key, pair_to_insert->key) == EQUALS) {
      Pair *new_pair_copy = vec->elem_copy_func((void *) pair_to_insert);
      if (!new_pair_copy) {
        return FAIL;
      }
      vec->elem_free_func(&vec->data[i]);
      vec->data[i] = new_pair_copy;
      return SUCCESS;
    }
  }
  VectorPushBack(vec, (void *) pair_to_insert);
  return SUCCESS;
}

static void FreeBuckets(mapCellT **p_buckets, size_t arr_size) {
  for (size_t i = 0; i < arr_size; i++) {
    if ((*p_buckets)[i] != NULL) {
      VectorFree(&((*p_buckets)[i]));
    }
  }
  free(*p_buckets);
  *p_buckets = NULL;
}

static int ReHashAll(HashMap *hash_map, mapCellT *dest_buckets, mapCellT
*src_buckets, size_t dest_size, size_t src_size) {

  for (size_t i = 0; i < src_size; i++) {
    if (src_buckets[i] == NULL) {
      continue;
    }
    Vector *cur_vec = src_buckets[i];
    for (size_t j = 0; j < cur_vec->size; j++) {
      Pair *cur_pair = (Pair *) cur_vec->data[j];
      size_t where_to = (hash_map->hash_func(cur_pair->key)) & (dest_size - 1);
      if (dest_buckets[where_to] == NULL) {
        Vector *new_vec = VectorAlloc(hash_map->pair_cpy, hash_map->pair_cmp,
                                      hash_map->pair_free);
        if (!new_vec) {
          return FAIL;
        }
        dest_buckets[where_to] = new_vec;
      }
      int push_success = VectorPushBack(dest_buckets[where_to], cur_pair);
      if (push_success == FAIL) {
        return FAIL;
      }
    }
  }
  return SUCCESS;
}

static mapCellT *BucketsAlloc(size_t size) {
  /**
   * creates new buckets list in size of 'size'
   */
  CHECK_ERROR(size != 0, NULL)
  mapCellT *new_buckets = calloc(size, sizeof(mapCellT));
  if (!new_buckets) {
    return NULL;
  }
  return new_buckets;
}

static int CreateNewBuckets(HashMap *hash_map, mapCellT **p_new_buckets,
    mapCellT *old_buckets, size_t new_buckets_size, size_t old_buckets_size) {
  /**
   * creates new dynamiclly allocated buckets array in new size (old_size *
   * GROWTH_FACTOR) and rehashes all from old buckets to new buckets
   * if fails rehash frees new allocated array and return FAIL
   */
  CHECK_ERROR(hash_map, FAIL)
  CHECK_ERROR(p_new_buckets, FAIL)
  CHECK_ERROR((*p_new_buckets) == NULL, FAIL)
  CHECK_ERROR(old_buckets, FAIL)
  CHECK_ERROR(new_buckets_size > 0, FAIL)
  (*p_new_buckets) = BucketsAlloc(new_buckets_size);
  if (!(*p_new_buckets)) {
    return FAIL;
  }

  int rehash_success = ReHashAll(hash_map, (*p_new_buckets), old_buckets,
                                 new_buckets_size, old_buckets_size);
  if (rehash_success == FAIL) {
    FreeBuckets(p_new_buckets, new_buckets_size);
    return FAIL;
  }
  return SUCCESS;
}

static Pair *HashMapGetPairByKey(HashMap *hash_map, KeyT key) {
  /**
   * returns a pointer to the pair with the given key or null if not exists
   */
  CHECK_ERROR(hash_map, NULL)
  CHECK_ERROR(key, NULL)
  size_t index = (hash_map->hash_func(key)) & (hash_map->capacity - 1);
  CHECK_ERROR(index < hash_map->capacity, NULL)
  if (hash_map->buckets[index] == NULL) {
    return NULL;
  }
  Vector *vec_to_look_in = hash_map->buckets[index];
  for (size_t i = 0; i < vec_to_look_in->size; i++) {
    Pair *to_check = (Pair *) vec_to_look_in->data[i];
    if (to_check->key_cmp(to_check->key, key)) {
      return to_check;
    }
  }
  return NULL;
}

ValueT HashMapAt(HashMap *hash_map, KeyT key) {
  CHECK_ERROR(hash_map, NULL)
  CHECK_ERROR(key, NULL)
  Pair *pair = HashMapGetPairByKey(hash_map, key);
  if (!pair) {
    return NULL;
  }
  return pair->value;
}

int HashMapContainsKey(HashMap *hash_map, KeyT key) {
  CHECK_ERROR(hash_map, FAIL)
  CHECK_ERROR(key, FAIL)
  if (HashMapGetPairByKey(hash_map, key)) {
    return SUCCESS;
  }
  return FAIL;
}

int HashMapContainsValue(HashMap *hash_map, ValueT value) {

  CHECK_ERROR(hash_map, FAIL)
  CHECK_ERROR(value, FAIL)
  for (size_t i = 0; i < hash_map->capacity; i++) {
    if (hash_map->buckets[i] == NULL) {
      continue;
    }
    Vector *cur_vec = hash_map->buckets[i];
    for (size_t j = 0; j < cur_vec->size; j++) {
      Pair *cur_pair = (Pair *) cur_vec->data[j];
      if (cur_pair->value_cmp(cur_pair->value, value)) {
        return SUCCESS;
      }
    }
  }
  return FAIL;
}

int HashMapErase(HashMap *hash_map, KeyT key) {

  CHECK_ERROR(hash_map, FAIL)
  CHECK_ERROR(key, FAIL)
  Pair *to_remove = HashMapGetPairByKey(hash_map, key);
  CHECK_ERROR(to_remove, FAIL)

  hash_map->size--;
  double load_factor = HashMapGetLoadFactor(hash_map);
  if (load_factor == LOAD_FACTOR_FAIL) {
    hash_map->size++;
    return FAIL;
  }
  int resize_flag = 0;
  mapCellT *old_buckets = NULL, *new_buckets = NULL;
  if (load_factor < HASH_MAP_MIN_LOAD_FACTOR) {
    int new_buckets_success = CreateNewBuckets(hash_map, &new_buckets,
        hash_map->buckets, hash_map->capacity / HASH_MAP_GROWTH_FACTOR,
        hash_map->capacity);
    if (new_buckets_success == FAIL) {
      hash_map->size++;
      return FAIL;
    }
    hash_map->capacity /= HASH_MAP_GROWTH_FACTOR;
    old_buckets = hash_map->buckets;
    hash_map->buckets = new_buckets;
    resize_flag = RESIZED;
  }

  //deleting new pair
  size_t vec_index = (hash_map->hash_func(key)) & (hash_map->capacity - 1);
  Vector *vec_contains_pair = hash_map->buckets[vec_index];
  int pair_index = VectorFind(vec_contains_pair, to_remove);

  int erase_success = VectorErase(vec_contains_pair, pair_index);
  if (erase_success == FAIL) {
    if (resize_flag) {
      hash_map->buckets = old_buckets;
      hash_map->capacity *= HASH_MAP_GROWTH_FACTOR;
      FreeBuckets(&new_buckets, hash_map->capacity / HASH_MAP_GROWTH_FACTOR);
    }
    hash_map->size++;
    return FAIL;
  }
  if (vec_contains_pair->size == EMPTY_VECTOR) {
    VectorFree(&vec_contains_pair);
    hash_map->buckets[vec_index] = NULL;
  }
  if (resize_flag) {
    FreeBuckets(&old_buckets, hash_map->capacity * HASH_MAP_GROWTH_FACTOR);
  }
  return SUCCESS;
}

void HashMapClear(HashMap *hash_map) {
  CHECK_ERROR(hash_map, NO_RETURN_VALUE)
  size_t size_like = hash_map->size, capacity_like = hash_map->capacity;
  for (size_t i = 0; i < hash_map->capacity; i++) {
    if(hash_map->buckets[i] != NULL){
      UpdateCapacityAndSize(hash_map->buckets[i]->size, &size_like,
          &capacity_like);
      VectorFree(&(hash_map->buckets[i]));
      hash_map->buckets[i] = NULL;
    }
  }
  mapCellT *tmp = realloc(hash_map->buckets, capacity_like * sizeof(mapCellT));
  if (!tmp) {
    return;
    //this case cannot happen but just in case
  }
  hash_map->buckets = tmp;
  hash_map->size = size_like;
  hash_map->capacity = capacity_like;
}

static void UpdateCapacityAndSize(size_t vector_size, size_t *p_map_size,size_t
*p_map_capacity) {
  for(size_t i = 0; i < vector_size; i++){
    (*p_map_size)--;
    if(LOAD_FACTOR((*p_map_size), (*p_map_capacity)) <
    HASH_MAP_MIN_LOAD_FACTOR){
      (*p_map_capacity) /= HASH_MAP_GROWTH_FACTOR;
    }
  }
}
