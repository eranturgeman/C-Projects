/**
 * @file SpreaderDetector.c
 * @author  Eran Turgeman <eran.turgeman1@gmail.com>
 * @version 1.0
 * @date 24/11/2020
 *
 * @brief implementation for SpreaderDetector.h
 *
 * @section LICENSE
 * This program is private and was made for the 2020 67315 course
 *
 * @section DESCRIPTION
 * This file contains all operations we can preform on a spreader detector
 * Input  : files containing data about people and meetings
 * Process: keeps all data, calculate infection ratio and creating a file
 * containing treatment recommendations
 * Output : A file with treatment recommendations for all people in the system
 */

// ------------------------------ includes ------------------------------
#include <stdio.h>
#include "SpreaderDetector.h"


// -------------------------- const definitions -------------------------
#define PERSON_ARR 'p'
/**
 * PERSON_ARR: flag represents an array of pointers to Person struct
 */
#define MEETING_ARR 'm'
/**
 * PMEETING_ARR: flag represents an array of pointers to Meeting struct
 */
#define SUCCESS 1
/**
 * SUCCESS: return value for function success
 */
#define FAILED 0
/**
 * FAILED: return value for function failure
 */
#define PERSON_LINE_FORMAT "%s %lu %lu %s"
/**
 * PERSON_LINE_FORMAT: parsing format for a line in a people text file
 */
#define MEETING_FILE_FORMAT "%lu %lu %lf %lf"
/**
 * MEETING_LINE_FORMAT: parsing format for a line in a meeting text file
 */
#define GET_INFECTION_RATE_BY_ID_FAILED -1
/**
 * GET_INFECTION_RATE_BY_ID_FAILED: failure return value for
 * SpreaderDetectorGetInfectionRateById function
 */
#define SPREADER_INFECTION_RATE 1
/**
 * SPREADER_INFECTION_RATE: spreader infection rate
 */
#define SICK_STR_LEN 5
/**
 * SICK_STR_LEN: length of the part of the string containing SICK
 */
#define WRITE_MODE "w"
/**
 * WRITE_MODE: file open mode
 */
#define READ_MODE "r"
/**
 * READ_MODE: file open mode
 */
// ------------------------------ functions -----------------------------
typedef Person** person_array;
typedef Meeting** meetings_array;

/**
 *
 * @param measure measure of the meeting
 * @param distance distance between person1 and person2 in the meeting
 * @param age age of the person we calculate the infection rate for
 * @param prev_person_ir the infection rate of person1 in the meeting with
 * the person we are calculating infection rate for
 * @return the infection rate for the person
 */
static double CalculateInfectionRate(double measure, double
distance, int age, double prev_person_ir);

/**
 *
 * @param spreader_detector pointer to a spreader detector struct
 * @param arr_new_size the new size of the array for the reallocation
 * @param arr_type the type of the array we are reallocating
 * @return
 */
static int ResizeArray(SpreaderDetector *spreader_detector,
size_t arr_new_size, char arr_type);

/**
 *
 * @param person person to add the meeting to
 * @param meeting meeting to add
 */
static void AddMeetingToPerson(Person * person, Meeting * meeting);

/**
 *
 * @param sp pointer to spreader detector
 * @param id_to_search the id we want to check if it exist in the spreader
 * detector
 * @return pointer to the person with the given ID or NULL if no such id was
 * found
 */
static Person* IdBinarySearch(SpreaderDetector* sp, size_t id_to_search);

/**
 *
 * @param person_array array of pointers of Person struct
 * @param start starting index of the part of the array we want to check
 * @param end ending index of the part of the array we want to check
 * @param id_to_find id we are trying to find
 * @param array_len length of the whole array
 * @return pointer to the person with the given ID or NULL if no such person
 * was found
 */
static Person *BinarySearchHelper(person_array person_array, size_t start,
    size_t end, size_t id_to_find, size_t array_len);

/**
 *
 * @param sp pointer to the spreader detector we want to add the person to
 * @param person_to_add pointer to the person we want to add
 */
static void AddPersonInPlace(SpreaderDetector *sp, Person *person_to_add);

/**
 *
 * @param person person we want to calculate the infection rate for
 * @param prev_rate the infection rate of the person he met
 * @param meeting pointer to the meeting between the two people
 */
static void CalculateInfectionRateHelper(Person * person, double
prev_rate, const Meeting *meeting);

/**
 *
 * @param sp pointer to a spreader detector
 * @return pointer to the the sick person or NULL no such person was found
 */
static Person *FindSpreader(const SpreaderDetector * sp);

/**
 *
 * @param person_to_print person we want to print treatment for
 * @param output_file pointer to output file
 */
static void PrintTreatment(Person *person_to_print, FILE *output_file);

//---------------------------------------------------------------------------

static double CalculateInfectionRate(const double measure, const double
distance, const int age, const double prev_person_ir)
{
  double infection_rate = ((measure * MIN_DISTANCE) / (distance *MAX_MEASURE)
      )  * prev_person_ir;
  return (age > AGE_THRESHOLD) ? infection_rate
  +INFECTION_RATE_ADDITION_DUE_TO_AGE : infection_rate;
}

SpreaderDetector *SpreaderDetectorAlloc()
{
  SpreaderDetector *sp = malloc(sizeof(SpreaderDetector));
//  SpreaderDetector *sp = calloc(1, sizeof(SpreaderDetector));
  if(!sp)
  {
    return NULL;
  }
  sp->people_size = 0;
  sp->meeting_size = 0;
  sp->meeting_cap = SPREADER_DETECTOR_INITIAL_SIZE;
  sp->people_cap = SPREADER_DETECTOR_INITIAL_SIZE;
  sp->people = calloc(SPREADER_DETECTOR_INITIAL_SIZE, sizeof(void*));
  if(!sp->people)
  {
    free(sp);
    return NULL;
  }
  sp->meetings = calloc(SPREADER_DETECTOR_INITIAL_SIZE, sizeof(void*));
  if(!sp->meetings)
  {
    free(sp->people);
    free(sp);
    return NULL;
  }
  return sp;
}

void SpreaderDetectorFree(SpreaderDetector **p_spreader_detector)
{
  if(!p_spreader_detector || !(*p_spreader_detector))
  {
    return;
  }
  free((*p_spreader_detector)->meetings);
  free((*p_spreader_detector)->people);
  free(*p_spreader_detector);
  (*p_spreader_detector) = NULL;
}

int SpreaderDetectorAddPerson(SpreaderDetector *spreader_detector, Person
*person){
  if(!spreader_detector || !person)
  {
    return FAILED;
  }

  if(!IdBinarySearch(spreader_detector, person->id))
  {
    if(spreader_detector->people_cap == spreader_detector->people_size)
    {
      if(!ResizeArray(spreader_detector, spreader_detector->people_size
          * SPREADER_DETECTOR_GROWTH_FACTOR * sizeof(void *), PERSON_ARR))
      {
        return FAILED;
      }
      spreader_detector->people_cap *= SPREADER_DETECTOR_GROWTH_FACTOR;
    }
    AddPersonInPlace(spreader_detector, person);
  }else{
    return FAILED;
  }
  return SUCCESS;
}

static void AddPersonInPlace(SpreaderDetector *sp, Person *person_to_add)
{
  if(!sp || !person_to_add)
  {
    return;
  }
  if(sp->people[0] == NULL)
  {
    sp->people[0] = person_to_add;
    sp->people_size++;
    return;
  }
  Person *cur = person_to_add;
  size_t i = 0;
  for(; i < sp->people_size; i++)
  {
    if(PersonCompareById(sp->people[i], person_to_add) < 0)
    {
      continue;
    }
    Person *next = sp->people[i];
    sp->people[i] = cur;
    cur = next;
  }
  sp->people[i] = cur;
  sp->people_size++;
}


static int ResizeArray(SpreaderDetector *spreader_detector, const
size_t arr_new_size, const char arr_type) {
  if(arr_type == PERSON_ARR)
  {
    person_array tmp = realloc(spreader_detector->people, arr_new_size);
    if(!tmp)
    {
      return FAILED;
    }
    spreader_detector->people = tmp;
    return SUCCESS;
  }else{
    meetings_array tmp = realloc(spreader_detector->meetings, arr_new_size);
    if(!tmp)
    {
      return FAILED;
    }
    spreader_detector->meetings = tmp;
    return SUCCESS;
  }
}

int SpreaderDetectorAddMeeting(SpreaderDetector *spreader_detector, Meeting *meeting)
{
  if(!spreader_detector || !meeting)
  {
    return FAILED;
  }
  if(IdBinarySearch(spreader_detector, meeting->person_1->id) &&
  IdBinarySearch(spreader_detector, meeting->person_2->id))
  {
    if(spreader_detector->meeting_cap == spreader_detector->meeting_size)
    {
      if(!ResizeArray(spreader_detector, spreader_detector->meeting_size
          * SPREADER_DETECTOR_GROWTH_FACTOR * sizeof(void*), MEETING_ARR))
      {
        return FAILED;
      }
      spreader_detector->meeting_cap *= SPREADER_DETECTOR_GROWTH_FACTOR;
    }
    spreader_detector->meetings[spreader_detector->meeting_size++] = meeting;
    AddMeetingToPerson(meeting->person_1, meeting);
  }else{
    return FAILED;
  }
  return SUCCESS;
}

void SpreaderDetectorReadPeopleFile(SpreaderDetector *spreader_detector, const char *path)
{
  if(!spreader_detector) {
    return;
  }
  FILE *people_file = fopen(path, READ_MODE);
  if(!people_file)
  {
    exit(EXIT_FAILURE);
  }
  char buffer[MAX_LEN_OF_LINE] = {0};
  char name[MAX_LEN_OF_LINE] = {0}, is_sick_str[SICK_STR_LEN] = {0};
  size_t id = 0, age = 0;
  while (fgets(buffer, MAX_LEN_OF_LINE, people_file) != NULL && buffer[0] !=
  '\n' && buffer[0] != '\r')
  {
    memset(is_sick_str, '\0', 4);
    sscanf(buffer, PERSON_LINE_FORMAT, name, &id, &age, is_sick_str);
    int is_sick = (!strcmp(is_sick_str, "SICK")) ? 1 : 0;
    Person *new_person = PersonAlloc(id, name, age, is_sick);
    if(new_person)
    {
      int was_added = SpreaderDetectorAddPerson(spreader_detector, new_person);
      if(!was_added)
      {
        PersonFree(&new_person);
      }
    }
  }
  fclose(people_file);
}

void SpreaderDetectorReadMeetingsFile(SpreaderDetector *spreader_detector,
    const char *path)
{
  if(!spreader_detector)
  {
    return;
  }
  FILE *meeting_file = fopen(path, READ_MODE);
  if(!meeting_file)
  {
    exit(EXIT_FAILURE);
  }
  char buffer[MAX_LEN_OF_LINE] = {0};
  size_t id_p1 = 0, id_p2 = 0;
  double distance = 0, measure = 0;
  while(fgets(buffer, MAX_LEN_OF_LINE, meeting_file) != NULL)
  {
    sscanf(buffer, MEETING_FILE_FORMAT, &id_p1, &id_p2, &distance, &measure);
    Person *person_1 = IdBinarySearch(spreader_detector, id_p1);
    Person *person_2 = IdBinarySearch(spreader_detector, id_p2);
    if(person_1 && person_2)
    {
      Meeting *new_meeting = MeetingAlloc(person_1, person_2, measure,distance);
      if(new_meeting)
      {
        int was_added = SpreaderDetectorAddMeeting(spreader_detector,
            new_meeting);
        if(!was_added)
        {
          MeetingFree(&new_meeting);
          return;
        }
      }
    }
  }
  fclose(meeting_file);
}


static void AddMeetingToPerson(Person *const person, Meeting *const meeting)
{
  if (!person || !meeting)
  {
    return;
  }
  if (person->num_of_meetings != 0 && person->num_of_meetings ==
  person->meetings_capacity)
  {
    meetings_array tmp = realloc(person->meetings, person->num_of_meetings *
    PERSON_GROWTH_FACTOR * sizeof(void*));
    if(tmp == NULL)
    {
      return;
    }
    person->meetings = tmp;
    person->meetings_capacity *= PERSON_GROWTH_FACTOR;
  }
  person->meetings[person->num_of_meetings] = meeting;
  person->num_of_meetings++;
}


static Person* IdBinarySearch(SpreaderDetector* sp, size_t id_to_search)
{
  if(!sp)
  {
    return NULL;
  }
  if(sp->people_size == 0)
  {
    return NULL;
  }
  if(sp->people_size == 1 && sp->people[0]->id == id_to_search)
  {
    return sp->people[0];
  }
  return BinarySearchHelper(sp->people, 0, sp->people_size - 1, id_to_search,
      sp->people_size);
}

static Person *BinarySearchHelper(person_array person_array, size_t start,
    size_t end, size_t id_to_find, size_t array_len){
  if(end >= start && end != array_len) {
    size_t mid_index = (start + end) / 2;
    if (person_array[mid_index]->id == id_to_find) {
      return person_array[mid_index];
    }
    if (person_array[mid_index]->id > id_to_find) {
      if (mid_index == 0)
      {
        return BinarySearchHelper(person_array,start,array_len,id_to_find,array_len);
      }
      return BinarySearchHelper(person_array, start, mid_index - 1,
          id_to_find, array_len);
    }
    else{
      return BinarySearchHelper(person_array, mid_index + 1, end, id_to_find,
          array_len);
    }
  }
  return NULL;
}

double SpreaderDetectorGetInfectionRateById(SpreaderDetector *spreader_detector, IdT id)
{
  if(!spreader_detector)
  {
    return GET_INFECTION_RATE_BY_ID_FAILED;
  }
  Person *person = IdBinarySearch(spreader_detector, id);
  if(person)
  {
    return person->infection_rate;
  }
  return GET_INFECTION_RATE_BY_ID_FAILED;
}

void SpreaderDetectorCalculateInfectionChances(SpreaderDetector *spreader_detector)
{
  if(!spreader_detector || spreader_detector->people_size == 0)
  {
    return;
  }
  Person *spreader = FindSpreader(spreader_detector);
  if(!spreader)
  {
    return;
  }
  spreader->infection_rate = SPREADER_INFECTION_RATE;
  for(size_t i = 0; i < spreader->num_of_meetings; i++)
  {
    CalculateInfectionRateHelper(MeetingGetPerson(spreader->meetings[i], 2),
                                 spreader->infection_rate,
                                 spreader->meetings[i]);
  }
}

static Person *FindSpreader(const SpreaderDetector *const sp)
{
  for(size_t i = 0; i < sp->people_size; i++)
  {
    if(sp->people[i]->is_sick == 1)
    {
      return sp->people[i];
    }
  }
  return NULL;
}

static void CalculateInfectionRateHelper(Person *const person, const double
prev_rate, const Meeting *meeting)
{
  person->infection_rate = CalculateInfectionRate(meeting->measure,
      meeting->distance, person->age, prev_rate);

  if(person->num_of_meetings == 0)
  {
    return;
  }

  for(size_t i = 0; i < person->num_of_meetings; i++)
  {
    CalculateInfectionRateHelper(MeetingGetPerson(person->meetings[i], 2),
        person->infection_rate, person->meetings[i]);
  }
}

int SpreaderDetectorPrintRecommendTreatmentToAll(SpreaderDetector *spreader_detector, const char *file_path)
{
  if(!spreader_detector)
  {
    return FAILED;
  }
  FILE *output_file = fopen(file_path, WRITE_MODE);
  if(!output_file)
  {
    return FAILED;
  }
  for(size_t i = 0; i < spreader_detector->people_size; i++)
  {
    PrintTreatment(spreader_detector->people[i], output_file);
  }
  fclose(output_file);
  return SUCCESS;
}

static void PrintTreatment(Person *person_to_print, FILE *output_file)
{
  double ratio = person_to_print->infection_rate;
  if(ratio > MEDICAL_SUPERVISION_THRESHOLD)
  {
    fprintf(output_file, MEDICAL_SUPERVISION_THRESHOLD_MSG,
        person_to_print->name, person_to_print->id, person_to_print-> age,
        ratio);
    return;
  }
  if(ratio > REGULAR_QUARANTINE_THRESHOLD)
  {
    fprintf(output_file, REGULAR_QUARANTINE_MSG, person_to_print->name,
        person_to_print->id, person_to_print->age, ratio);
    return;
  }
  fprintf(output_file, CLEAN_MSG, person_to_print->name, person_to_print->id,
      person_to_print->age, ratio);
}

size_t SpreaderDetectorGetNumOfPeople(SpreaderDetector *spreader_detector)
{
  return (!spreader_detector) ? FAILED : spreader_detector->people_size;
}

size_t SpreaderDetectorGetNumOfMeetings(SpreaderDetector *spreader_detector)
{
  return (!spreader_detector) ? FAILED : spreader_detector->meeting_size;
}




