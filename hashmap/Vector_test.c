#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vector.h"

enum errors{SUCCESS, TEST1FAIL, TEST2FAIL, TEST3FAIL, TEST4FAIL, TEST5FAIL,
    TEST6FAIL};
int IntCompare(const void* val1, const void* val2);
void* IntCopy(const void* val);
void IntFree(void **to_free);

int Test1();
int Test2();
int Test3();

int Test4();
int Test5();
int Test6();
int main()
{
  printf("TEST 1: VectorAlloc\n");
  int result_test1 = Test1();
  if(result_test1 != 0){
    fprintf(stderr, "Test 1 FAILED!\n\n");
    exit(1);
  }
  printf("TEST 1 PASSED!\n\n");

  printf("TEST 2: VectorAt\n");
  int result_test2 = Test2();
  if(result_test2 != 0){
    fprintf(stderr, "Test 2 FAILED!\n\n");
    exit(2);
  }
  printf("TEST 2 PASSED!\n\n");

  printf("TEST 3: VectorFind\n");
  int result_test3 = Test3();
  if(result_test3 != 0){
    fprintf(stderr, "Test 3 FAILED!\n\n");
    exit(3);
  }
  printf("TEST 3 PASSED!\n\n");

  printf("TEST 4: VectorPushBack\n");
  int result_test4 = Test4();
  if(result_test4 != 0){
    fprintf(stderr, "Test 4 FAILED!\n\n");
    exit(4);
  }
  printf("TEST 4 PASSED!\n\n");

  printf("TEST 5: VectorErase\n");
  int result_test5 = Test5();
  if(result_test5 != 0){
    fprintf(stderr, "Test 5 FAILED!\n\n");
    exit(5);
  }
  printf("TEST 5 PASSED!\n\n");

  printf("TEST 6: VectorClear\n");
  printf("NOTICE: I couldn't really check if your vec has been cleared for\n "\
         "values so I am just checking if size and capacity set correctly.\n "\
         "MAKE SURE TO RESIZE YOUR DATA ARRAY!\n\n");
  int result_test6 = Test6();
  if(result_test6 != 0){
    fprintf(stderr, "Test 6 FAILED!\n\n");
    exit(6);
  }
  printf("TEST 6 PASSED!\n\n");
  printf("ALL TESTS PASSED :)\n");
}

int Test6() {
  Vector *vec = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!vec){
    fprintf(stderr, "TEST 6: int_vec allocation failed\n");
    return TEST6FAIL;
  }
  int num_arr[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  void* *num_p_array = malloc(20 * sizeof(void*));
  if(!num_p_array){
    VectorFree(&vec);
    fprintf(stderr, "TEST 6: pointers array allocation failed\n");
    return TEST6FAIL;
  }
  for(int i = 0; i < 20; i++){
    num_p_array[i] = &num_arr[i];
  }

  for(int i = 0; i < 20; i++){
    int result = VectorPushBack(vec, num_p_array[i]);
    if(result == 0)
    {
      VectorFree(&vec);
      free(num_p_array);
      fprintf(stderr, "TEST 6: inserting elem #%d to vector failed\n", i);
      return TEST6FAIL;
    }
  }

  VectorClear(vec);
  if(vec->size != 0 || vec->capacity != 2){
    VectorFree(&vec);
    free(num_p_array);
    fprintf(stderr, "TEST 6: size or capacity haven't been resized\n");
    return TEST6FAIL;
  }
  VectorFree(&vec);
  free(num_p_array);
  return SUCCESS;
}

int Test5() {
  Vector *vec = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!vec){
    fprintf(stderr, "TEST 5: int_vec allocation failed\n");
    return TEST5FAIL;
  }
  int num_arr[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  void* *num_p_array = malloc(20 * sizeof(void*));
  if(!num_p_array){
    VectorFree(&vec);
    fprintf(stderr, "TEST 5: pointers array allocation failed\n");
    return TEST5FAIL;
  }
  for(int i = 0; i < 20; i++){
    num_p_array[i] = &num_arr[i];
  }

  for(int i = 0; i < 20; i++){
    int result = VectorPushBack(vec, num_p_array[i]);
    if(result == 0)
    {
      VectorFree(&vec);
      free(num_p_array);
      fprintf(stderr, "TEST 5: inserting elem #%d to vector failed\n", i);
      return TEST5FAIL;
    }
  }

  int erase_invalid = VectorErase(vec,20);
  if(erase_invalid != 0 || vec->size != 20){
    fprintf(stderr, "TEST 5: erasing invalid index failed\n");
    free(num_p_array);
    VectorFree(&vec);
    return TEST5FAIL;
  }

  int erase_ind_0 = VectorErase(vec, 0);
  int erase_ind_10 = VectorErase(vec,10);
  int erase_ind_17 = VectorErase(vec,17);
  if(vec->size != 17 || erase_ind_0 != 1 || erase_ind_17 != 1 || erase_ind_10
  != 1){
    fprintf(stderr, "TEST 5: erasing indexes 0, 17, 10 failed\n");
    free(num_p_array);
    VectorFree(&vec);
    return TEST5FAIL;
  }
  if(*((int*)vec->data[0]) != 2 || *((int*)vec->data[10]) != 13 || *((int*)
  vec->data[16]) != 19){
    fprintf(stderr, "TEST 5: data array values didnt move back after "\
                    "erasing\n");
    free(num_p_array);
    VectorFree(&vec);
    return TEST5FAIL;
  }

  for(int i = 0; i < 10; i++){
    int erase = VectorErase(vec, 0);
    if(erase != 1){
      fprintf(stderr, "TEST 5: erasing indexes failed\n");
      free(num_p_array);
      VectorFree(&vec);
      return TEST5FAIL;
    }
  }

  if(vec->size != 7 || vec->capacity != 16){
    fprintf(stderr, "TEST 5: vector have not been resized correctly after "
                    "deleting elements\n");
    free(num_p_array);
    VectorFree(&vec);
    return TEST5FAIL;
  }
  free(num_p_array);
  VectorFree(&vec);
  return SUCCESS;
}

int Test4() {
  Vector *vec = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!vec){
    fprintf(stderr, "TEST 4: int_vec allocation failed\n");
    return TEST4FAIL;
  }
  int num_arr[20] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
  void* *num_p_array = malloc(20 * sizeof(void*));
  if(!num_p_array){
    VectorFree(&vec);
    fprintf(stderr, "TEST 4: pointers array allocation failed\n");
    return TEST4FAIL;
  }
  for(int i = 0; i < 20; i++){
    num_p_array[i] = &num_arr[i];
  }

  for(int i = 0; i < 20; i++){
    int result = VectorPushBack(vec, num_p_array[i]);
    if(result == 0)
    {
      VectorFree(&vec);
      free(num_p_array);
      fprintf(stderr, "TEST 4: inserting elem #%d to vector failed\n", i);
      return TEST4FAIL;
    }
  }

  //check if all inserted correctly
  for(int i = 0; i < 20; i++)
  {
    void* elem = VectorAt(vec, i);
    if(*((int*)elem) != i + 1)
    {
      VectorFree(&vec);
      free(num_p_array);
      fprintf(stderr, "TEST 4: element returned from vector in iteration "\
                      "#%d incorrect (before release)\n", i);
      return TEST4FAIL;
    }
  }

  free(num_p_array);

  for(int i = 0; i < 20; i++)
  {
    void* elem = VectorAt(vec, i);
    if(*((int*)elem) != i + 1)
    {
      VectorFree(&vec);
      fprintf(stderr, "TEST 4: element returned from vector in iteration "\
                      "#%d incorrect (after release)\n", i);
      return TEST4FAIL;
    }
  }
  VectorFree(&vec);
  return SUCCESS;
}

int Test3() {
  Vector *int_vec = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!int_vec){
    fprintf(stderr, "TEST 3: int_vec allocation failed\n");
    return TEST3FAIL;
  }
  //create values and insert manually
  int num1 = 1;
  int num2 = 2;
  int num3 = 3;
  void* p_num1 = &num1;
  void* p_num2 = &num2;
  void* p_num3 = &num3;
  int_vec->data[0] = p_num1;
  int_vec->data[1] = p_num2;
  int_vec->data[2] = p_num3;
  int_vec->size +=3;
  int index1 = VectorFind(int_vec, p_num1);
  int index2 = VectorFind(int_vec, p_num2);
  int index3 = VectorFind(int_vec, p_num3);
  if(index1 != 0 || index2 != 1 || index3 != 2){
    //freeing vector manually because elements were not allocated!
    free(int_vec->data);
    free(int_vec);
    fprintf(stderr, "TEST 3: returned wrong index\n");
    return TEST3FAIL;
  }

  int num4 = 4;
  void *p_num4 = &num4;
  int index_not_found1 = VectorFind(int_vec, p_num4);
  int index_not_found2 = VectorFind(NULL, p_num4);
  int index_not_found3 = VectorFind(int_vec, NULL);
  if(index_not_found1 != -1 || index_not_found2 != -1 || index_not_found3 !=-1){
    //freeing vector manually because elements were not allocated!
    free(int_vec->data);
    free(int_vec);
    fprintf(stderr, "TEST 3: returned wrong value when should return -1\n");
    return TEST3FAIL;
  }
  //freeing vector manually because elements were not allocated!
  free(int_vec->data);
  free(int_vec);
  return SUCCESS;
}

int Test2() {
  Vector *int_vec = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!int_vec){
    fprintf(stderr, "TEST 2: int_vec allocation failed\n");
    return TEST2FAIL;
  }
  //create values and insert manually
  int num1 = 1;
  int num2 = 2;
  int num3 = 3;
  void* p_num1 = &num1;
  void* p_num2 = &num2;
  void* p_num3 = &num3;
  int_vec->data[0] = p_num1;
  int_vec->data[1] = p_num2;
  int_vec->data[2] = p_num3;
  int_vec->size +=3;
  void* ret_num1 = VectorAt(int_vec, 0);
  void* ret_num2 = VectorAt(int_vec, 1);
  void* ret_num3 = VectorAt(int_vec, 2);
  void* ret_null1 = VectorAt(int_vec, -3);
  void* ret_null2 = VectorAt(int_vec, 3);

  if(ret_null1 != NULL || ret_null2 != NULL){
    //freeing vector manually bacause elements were not allocated!
    free(int_vec->data);
    free(int_vec);
    fprintf(stderr, "TEST 2: ret_null1 or ret_null2 != NULL\n");
    return TEST2FAIL;
  }
  if(*(int*)ret_num1 != 1 || *(int*)ret_num2 != 2 || *(int*)ret_num3 != 3)
  {
    //freeing vector manually bacause elements were not allocated!
    free(int_vec->data);
    free(int_vec);
    fprintf(stderr, "TEST 2: some value didn't return as expected\n");
    return TEST2FAIL;
  }
  if(p_num1 != ret_num1 || p_num2 != ret_num2 || p_num3 != ret_num3){
    //freeing vector manually bacause elements were not allocated!
    free(int_vec->data);
    free(int_vec);
    fprintf(stderr, "TEST 2: pointers are not matching\n");
    return TEST2FAIL;
  }
  //freeing vector manually bacause elements were not allocated!
  free(int_vec->data);
  free(int_vec);
  return SUCCESS;
}

int Test1() {
  Vector *vector1 = VectorAlloc(IntCopy, IntCompare, IntFree);
  if(!vector1 || vector1->size != 0 || vector1->capacity != VECTOR_INITIAL_CAP){
    fprintf(stderr, "TEST 1: vector1 allocation failed or incorrect values\n");
    return TEST1FAIL;
  }
  VectorFree(&vector1);

  Vector *vector_missing_func = VectorAlloc(IntCopy, IntCompare, NULL);
  if(vector_missing_func){
    fprintf(stderr, "TEST 1: vector_missing_func allocation returned allocated"\
                    "vector\n");
    VectorFree(&vector_missing_func);
    return TEST1FAIL;
  }
  return SUCCESS;

}


int IntCompare(const void* val1, const void* val2){
  if(!val1 || !val2)
  {
    fprintf(stderr, "IntCompare got NULL data\n");
    exit(0);
  }
  int int1 = *((int*)val1);
  int int2 = *((int*)val2);
  return int1 == int2 ? 1 : 0;
}

void* IntCopy(const void* val){
  if(!val){
    fprintf(stderr, "IntCopy got NULL data\n");
    exit(0);
  }
  void* copy = malloc(sizeof(void*));
  if(!copy){
    fprintf(stderr, "IntCopy memory allocation failed\n");
    exit(0);
  }
  memcpy(copy, val, sizeof(int));
  return copy;
}

void IntFree(void **to_free){
  if(!to_free ||!(*to_free)){
    fprintf(stderr, "IntFree got NULL pointer\n");
    exit(0);
  }
  free(*to_free);
  *to_free = NULL;
}
