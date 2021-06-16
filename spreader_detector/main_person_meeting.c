
#include "Person.h"
#include "Meeting.h"
#include <assert.h>

static void PrintPersonInfo(Person *person);
static void PrintMeetingInfo(Meeting *meeting);
static int Test3(Person* p1, Person *p2, Person *p3, Meeting *m1, Meeting *m2);
static int CheckComparison(Person *p1, Person *p2, int expected_val, int
(*comp_func)(const struct Person *person1, const struct Person *person2));
static int Test4 (Person *person1, struct Person *person2, Person *person3);
static int AddMeetingToPerson(Person *const person, Meeting *const meeting);
static int Test5(const Person *p1,const Person *p2,const Person *p3, const
Meeting *meeting1, const Meeting *meeting2);
int main()
{
  printf("Test 1: PersonAlloc (not from file)\n");
  Person *person1 = PersonAlloc(998767444, "AviGanavi", 100, 0);//TODO free alloc
  Person *person2 = PersonAlloc(998822333, "YafaLula", 13, 1);//TODO free alloc
  Person *person3 = PersonAlloc(443255555, "EliCopter", 56, 0);//TODO free alloc
  assert(person1 && "Person 1 creation failed\n");
  assert(person2 && "Person 2 creation failed\n");
  assert(person3 && "Person 3 creation failed\n");
  person1->infection_rate = 0.5;
  person2->infection_rate = 1;
  person3->infection_rate = 0.3;
  PrintPersonInfo(person1);
  PrintPersonInfo(person2);
  PrintPersonInfo(person3);
  printf("TEST 1 COMPLETED!\n\n");

  printf("Test 2: MeetingAlloc (not from file)\n");
  Meeting *meeting1 = MeetingAlloc(person2, person1, 6.0, 10.7);
  int add_result1 = AddMeetingToPerson(person2, meeting1);
  assert(add_result1 == 0);
  Meeting *meeting2 = MeetingAlloc(person2, person3, 3.3, 15.0);
  int add_result2 = AddMeetingToPerson(person2, meeting2);
  assert(add_result2 == 0);
  assert(meeting1 && "Meeting 1 allocation failed\n");
  assert(meeting2 && "Meeting 2 allocation failed\n");
  PrintMeetingInfo(meeting1);
  PrintMeetingInfo(meeting2);
  printf("TEST 2 COMPLETED!\n\n");

  printf("Test 3: PersonGetMeetingById\n");
  int result_test3 = Test3(person1, person2, person3, meeting1, meeting2);
  if(result_test3)
  {
    fprintf(stderr, "PersonGetMeetingById FAILED!\n");
    return EXIT_FAILURE;
  }
  printf("TEST 3 COMPLETED!\n\n");

  printf("TEST 4: Compare functions\n");
  if(Test4(person1, person2, person3))
  {
    fprintf(stderr, "ERROR: Test 4 FAILED!\n");
    return EXIT_FAILURE;
  }
  printf("TEST 4 COMPLETED!\n\n");

  printf("TEST 5: MeetingGetPerson\n");
  if(Test5(person1, person2, person3, meeting1, meeting2))
  {
    fprintf(stderr, "ERROR: Test 5 FAILED!\n");
    return EXIT_FAILURE;
  }
  printf("TEST 5 COMPLETED!\n\n");

  PersonFree(&person1);
  PersonFree(&person2);
  PersonFree(&person3);
  MeetingFree(&meeting1);
  MeetingFree(&meeting2);

  printf("ALL TESTS PASSED!\n");
}

static int Test5(const Person *p1, const Person *p2, const Person *p3, const
Meeting* meeting1, const Meeting *meeting2) {
  if(p1 != MeetingGetPerson(meeting1, 2))
  {
    return 1;
  }
  if(p2 != MeetingGetPerson(meeting1, 1))
  {
    return 1;
  }
  if(p2 != MeetingGetPerson(meeting2, 1))
  {
    return 1;
  }
  if(p3 != MeetingGetPerson(meeting2, 2))
  {
    return 1;
  }
  return 0;
}

static void PrintPersonInfo(Person *person)
{
  if(!person)
  {
    printf("Invalid Pointer or Person does not exists\n");
    return;
  }
  printf("PERSON NAME: %s\n", person->name);
  printf("PERSON ID: %lu\n", person->id);
  printf("PERSON AGE: %lu\n", person->age);
  printf("IS PERSON SICK: ");
  if(person->is_sick)
  {
    printf("YES\n");
  }
  else{
    printf("NO\n");
  }
  printf("INFECTION RATE: %f\n", person->infection_rate);
  printf("NUMBER OF MEETINGS: %lu\n", person->num_of_meetings);
  printf("CURRENT MAX MEETINGS CAPACITY: %lu\n", person->meetings_capacity);
  for(size_t i = 0; i < person->num_of_meetings; i++)
  {
    PrintMeetingInfo(person->meetings[i]);
  }
  printf("\n");
}

static void PrintMeetingInfo(Meeting *meeting)
{
  printf("Person 1 in meeting: %s %lu\n", meeting->person_1->name,
         meeting->person_1->id);
  printf("Person 2 in meeting: %s %lu\n", meeting->person_2->name,
         meeting->person_2->id);
  printf("Distance: %f\n", meeting->distance);
  printf("Measure: %f\n\n", meeting->measure);
}

static int Test3(Person* p1, Person *p2, Person *p3, Meeting *m1, Meeting *m2)
{
  Meeting *meeting_exists1 = PersonGetMeetingById(p2, 998767444);
  if(meeting_exists1 != m1)
  {
    return 1;
  }
  Meeting *meeting_exists2 = PersonGetMeetingById(p2, 443255555);
  if(meeting_exists2 != m2)
  {
    return 1;
  }
  Meeting *meeting_not_exist1 = PersonGetMeetingById(p1, 337485767);
  if(meeting_not_exist1)
  {
    return 1;
  }
  Meeting *meeting_not_exist2 = PersonGetMeetingById(p3, 888877776);
  if(meeting_not_exist2)
  {
    return 1;
  }
  return 0;
}

static int Test4 (Person *person1, Person *person2, Person *person3)
{
  // this test checks all compares
  int (*comp_by_id)(const Person *p1, const Person *p2) = PersonCompareById;
  if(CheckComparison(person1, person2, -1, comp_by_id) || CheckComparison
      (person2, person3, 1, comp_by_id) || CheckComparison(person3, person1,
                                                           -1,comp_by_id))
  {
    fprintf(stderr, "ERROR: comparison by ID failed!\n");
    return 1;
  }

  int (*comp_by_name)(const Person *p1, const Person *p2) = PersonCompareByName;
  if(CheckComparison(person1, person2, -1, comp_by_name) || CheckComparison
      (person2, person3, 1, comp_by_name) || CheckComparison(person3,
                                                             person1,1,comp_by_name))
  {
    fprintf(stderr, "ERROR: comparison by NAME failed!\n");
    return 1;
  }

  int(*comp_by_infection)(const Person *p1, const Person *p2) =
  PersonCompareByInfectionRate;
  if(CheckComparison(person1, person2, 1, comp_by_infection) ||
      CheckComparison(person1, person3, -1, comp_by_infection) || CheckComparison
      (person3, person2, 1, comp_by_infection))
  {
    fprintf(stderr, "ERROR: comparison by INFECTION failed!\n");
    return 1;
  }

  int(*comp_by_age)(const Person *p1, const Person *p2) = PersonCompareByAge;
  if(CheckComparison(person1, person2, -1, comp_by_age) ||
      CheckComparison(person1, person3, -1, comp_by_age) || CheckComparison
      (person3, person2, -1, comp_by_age))
  {
    fprintf(stderr, "ERROR: comparison by AGE failed!\n");
    return 1;
  }

  return 0;
}

static int CheckComparison(Person *p1, Person *p2, int expected_val, int
(*comp_func) (const Person *person1, const Person *person2))
{
  int result = comp_func(p1, p2);
  if (result == expected_val)
  {
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}

static int AddMeetingToPerson(Person *const person, Meeting *const
meeting)
{
  if (!person || !meeting)
  {
    return 1;
  }
  if (person->num_of_meetings % PERSON_INITIAL_SIZE == 0)
  {
    Meeting **tmp = realloc(person->meetings, person->num_of_meetings *
    PERSON_GROWTH_FACTOR * sizeof(void*));
    if(tmp == NULL)
    {
      assert(tmp != NULL && "Reallocation of person meeting array failed\n");
    }
    person->meetings = tmp;
  }
  person->meetings[person->num_of_meetings] = meeting;
  person->num_of_meetings++;
  return 0;
}
