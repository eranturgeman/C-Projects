/**
 * @file Person.c
 * @author  Eran Turgeman <eran.turgeman1@gmail.com>
 * @version 1.0
 * @date 21/11/2020
 * 
 * @brief Implementation for Person.h API.
 *
 * @section LICENSE
 * This program is private and was made for the 2020/2021 67315 course
 */

// ------------------------------ includes ------------------------------

#include "Person.h"
#include <stdlib.h>

// -------------------------- const definitions -------------------------

#define P1_BEFORE_P2 -1
/**
 * P1_BEFORE_P2: return value for comparison functions mean that person1
 * should come before person 2
 */
#define P1_EQUALS_P2 0
/**
 * P1_EQUALS_P2: return value for comparison functions mean that person1
 * and person 2 are equal in value
 */
#define P2_BEFORE_P1 1
/**
 * P1_BEFORE_P2: return value for comparison functions mean that person2
 * should come before person1
 */
#define ID_BOTTOM_LIMIT 1000000000
/**
 * ID_BOTTOM_LIMIT: lower bound for id possible values
 */
#define ID_UPPER_LIMIT 999999999
/**
 * ID_UPPER_LIMIT: upper bound for id possible values
 */
#define FAILED 1
/**
 * FAILED: return value in case function failed
 */
#define SUCCEEDED 0
/**
 * SUCCEEDED: return value in case function succeeded
 */

// ------------------------------ functions -----------------------------

/**
 * checks if the given Id number is a valid number
 * @param id number of id
 * @return 0 if valid, 1 if not valid
 */
static int CheckIdValidity(IdT id);

Person *PersonAlloc(IdT id, char *name, size_t age, int is_sick)
{
  Person *new_person = calloc(1, sizeof(Person));
  if(!new_person)
  {
    return NULL;
  }
  new_person->id = id;
  new_person->age = age;
  new_person->is_sick = is_sick;
  new_person->meetings_capacity = PERSON_INITIAL_SIZE;
  new_person->name = calloc(strlen(name) + 1, sizeof(char));

  if (!new_person->name)
  {
    free(new_person);
    return NULL;
  }
  strcpy(new_person->name, name);

  new_person->meetings = calloc(PERSON_INITIAL_SIZE, sizeof(void*));
  if(new_person->meetings == NULL)
  {
    free(new_person->name);
    free(new_person);
    return NULL;
  }
  return new_person;
}

void PersonFree(Person **p_person)
{
  if (!p_person || !(*p_person))
  {
    return;
  }
  free((*p_person)->meetings);
  free((*p_person)->name);
  (*p_person)->name = NULL;
  free((*p_person));
  (*p_person) = NULL;
}

int PersonCompareById(const Person *person_1, const Person *person_2)
{
  return (person_1->id == person_2->id) ? P1_EQUALS_P2 : \
  (person_1->id > person_2->id) ? P2_BEFORE_P1 : P1_BEFORE_P2;
}

int PersonCompareByName(const Person *person_1, const Person *person_2)
{
  int comparison_parameter = strcmp(person_1->name, person_2->name);
  return (comparison_parameter == 0) ? P1_EQUALS_P2 : \
  (comparison_parameter < 0) ? P1_BEFORE_P2 : P2_BEFORE_P1;
}

int PersonCompareByInfectionRate(const Person *person_1, const Person *person_2)
{
  return (person_1->infection_rate == person_2->infection_rate) ?P1_EQUALS_P2 :\
  (person_1->infection_rate > person_2->infection_rate) ? P1_BEFORE_P2 : \
  P2_BEFORE_P1;
}

int PersonCompareByAge(const Person *person_1, const Person *person_2)
{
  return (person_1->age == person_2->age) ? P1_EQUALS_P2 :\
  (person_1->age > person_2->age) ? P1_BEFORE_P2 : P2_BEFORE_P1;
}


Meeting *PersonGetMeetingById(const Person *const person, IdT id)
{
  if(person == NULL || CheckIdValidity(id))
  {
    return NULL;
  }
  Meeting *cur = NULL;
  for(size_t i = 0; i < person->num_of_meetings; i++)
  {
    cur = person->meetings[i];
    if (cur->person_2->id == id)
    {
      return cur;
    }
  }
  return NULL;
}

static int CheckIdValidity(const IdT id)
{
  if(id >= ID_BOTTOM_LIMIT && id <= ID_UPPER_LIMIT)
  {
    return FAILED;
  }
  return SUCCEEDED;
}

