/**
 * @file Meeting.c
 * @author  Eran Turgeman <eran.turgeman1@gmail.com>
 * @version 1.0
 * @date 23/11/2020
 * 
 * @brief implementation for Meeting.h API
 *
 * @section LICENSE
 * This program is private and was made for the 2020/2021 67315 course
 */

// ------------------------------ includes ------------------------------
#include "Meeting.h"
// -------------------------- const definitions -------------------------
#define PERSON1 1
/**
 * PERSON1: index of person 1 in a meeting
 */
#define PERSON2 2
/**
 * PERSON2: index of person 2 in a meeting
 */
#define FAILED 0
/**
 * FAILED: return value for function failure
 */
#define SUCCEEDED 1
/**
 * SUCCEEDED: return value for function success
 */



// ------------------------------ functions -----------------------------

/**
 *
 * @param p_meeting pointer to a meeting
 * @return 1 if pointer is valid, 0 otherwise
 */
static int CheckMeetingValidity(const Meeting * p_meeting);


Meeting *MeetingAlloc(Person *person_1, Person *person_2, double measure,
                      double distance)
{
  if (!person_1 || !person_2)
  {
    return NULL;
  }
  Meeting *meeting = malloc(sizeof(Meeting));
  if(!meeting)
  {
    return NULL;
  }
  meeting->person_1 = person_1;
  meeting->person_2 = person_2;
  meeting->distance = distance;
  meeting->measure = measure;
  return meeting;
}

void MeetingFree(Meeting **p_meeting)
{
  if(!p_meeting || !(*p_meeting))
  {
    return;
  }
  free((*p_meeting));
  (*p_meeting) = NULL;
}

Person *MeetingGetPerson(const Meeting * const meeting, size_t person_ind)
{
  if(!CheckMeetingValidity(meeting))
  {
    return NULL;
  }
  if(person_ind == PERSON1)
  {
    return meeting->person_1;
  }else if(person_ind == PERSON2){
    return meeting->person_2;
  }
  return NULL;
}

static int CheckMeetingValidity(const Meeting *const p_meeting)
{
  if(!p_meeting || !p_meeting->person_1 || !p_meeting->person_2)
  {
    return FAILED;
  }
  return SUCCEEDED;
}


