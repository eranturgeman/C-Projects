#include "SpreaderDetector.h"



#define EMPTY_PEOPLE_TXT "files/empty_people.txt"
#define PEOPLE_TXT "files/people1.txt"
#define EMPTY_MEETING_TXT "files/empty_meeting.txt"
#define MEETING_TXT "files/meeting1.txt"
#define OUTPUT_TXT "files/treatment_output.txt"

enum Failures {Success, Test1Failed, Test2Failed, Test3Failed, Test4Failed,
    Test5Failed, Test6Failed, Test7Failed};

void CheckFailure(int failure);
int Test1(SpreaderDetector **sp);
int Test2(SpreaderDetector *sp, Person*p1,Person*p2,Person*p3,Person*p4,
    Person*p5,Person*p6,Person*p7,Person*p8,Person*p9);

int Test3(SpreaderDetector *sp,Meeting *c1a,Meeting *c1b,Meeting *c1c,
    Meeting *c2a,Meeting *c2b,Meeting *c2c,Meeting *c2d,Meeting *c3a, Person*p);

int Test4( SpreaderDetector **spreader);
int Test5( SpreaderDetector **spreader);
int Test6(SpreaderDetector *spreader_detector);
int Test7( SpreaderDetector *sp);
//-------------------------------------------------------------------
int main()
{
  printf("WARNING: If a test fails the program will not free all memory "
         "allocations! Use Valgrind only when all test passed!\n\n");


  printf("TEST 1: Initializing SpreaderDetector\n");
  SpreaderDetector *sp = NULL;
  int result_test1 = Test1(&sp);
  CheckFailure(result_test1);
  printf("TEST 1 PASSED!\n\n");


  printf("TEST 2: SpreaderDetectorAddPerson\n");
  Person *p1 = PersonAlloc(111111111, "AviGanavi", 100, 0);
  Person *p2 = PersonAlloc(444444444, "YafaLula", 13, 1);
  Person *p3 = PersonAlloc(222222222, "EliCopter", 56, 0);
  Person *p4 = PersonAlloc(555555555, "YafaRunkel", 24, 0);
  Person *p5 = PersonAlloc(333333333, "MotiLuchim", 13, 0);
  Person *p6 = PersonAlloc(666666666, "TikiPur", 56, 0);
  Person *p7 = PersonAlloc(777777777, "MichaFafli", 24, 0);
  Person *p8 = PersonAlloc(999999999, "SimaZgan", 13, 0);
  Person *p9 = PersonAlloc(888888888, "TikiPur", 56, 0);
  int result_test2 = Test2(sp,p1,p2,p3,p4,p5,p6,p7,p8,p9);
  CheckFailure(result_test2);
  printf("TEST 2 PASSED!\n\n");

  printf("TEST 3: SpreaderDetectorAddMeeting\n");
  Meeting *circ1_a = MeetingAlloc(p2, p1, 12, 10);
  Meeting *circ1_b = MeetingAlloc(p2, p5, 7, 15);
  Meeting *circ1_c = MeetingAlloc(p2, p9, 5, 24);
  Meeting *circ2_a = MeetingAlloc(p1, p3, 17, 32);
  Meeting *circ2_b = MeetingAlloc(p5, p4, 12, 19);
  Meeting *circ2_c = MeetingAlloc(p5, p6, 3, 40);
  Meeting *circ2_d = MeetingAlloc(p9, p7, 45, 10);
  Meeting *circ3_a = MeetingAlloc(p7, p8, 42, 6);
  int result_test3 = Test3(sp, circ1_a, circ1_b, circ1_c, circ2_a, circ2_b,
      circ2_c, circ2_d, circ3_a, p5);
  CheckFailure(result_test3);
  printf("TEST 3 PASSED!\n\n");

  MeetingFree(&circ1_a);
  MeetingFree(&circ1_b);
  MeetingFree(&circ1_c);
  MeetingFree(&circ2_a);
  MeetingFree(&circ2_b);
  MeetingFree(&circ2_c);
  MeetingFree(&circ2_d);
  MeetingFree(&circ3_a);
  sp->meeting_size -= 8;
  PersonFree(&p1);
  PersonFree(&p2);
  PersonFree(&p3);
  PersonFree(&p4);
  PersonFree(&p5);
  PersonFree(&p6);
  PersonFree(&p7);
  PersonFree(&p8);
  PersonFree(&p9);
  sp->people_size -= 9;
  SpreaderDetectorFree(&sp);

  printf("TEST 4: SpreaderDetectorReadPeopleFile\n");
  SpreaderDetector *spreader_with_files = NULL;
  int result_test4 = Test4(&spreader_with_files);
  CheckFailure(result_test4);
  printf("TEST 4 PASSED!\n\n");

  printf("TEST 5: SpreaderDetectorReadMeetingFile\n");
  int result_test5 = Test5(&spreader_with_files);
  CheckFailure(result_test5);
  printf("Test 5 PASSED!\n\n");

  printf("TEST 6: SpreaderDetectorCalculateInfectionChances && "
  "SpreaderDetectorGetInfectionRateById\n");
  int result_test6 = Test6(spreader_with_files);
  CheckFailure(result_test6);
  printf("Test 6 PASSED!\n\n");

  printf("TEST 7: SpreaderDetectorPrintRecommendTreatmentToAll\n");
  printf("IMPORTANT! This test works ONLY if you printed the treatments in "
         "the order of your people array! if not DISREGARD this test!\n");
  int result_test7 = Test7(spreader_with_files);
  CheckFailure(result_test7);
  printf("Test 7 PASSED!\n\n");

  printf("TEST 8: SpreaderDetectorGetNumOfPeople && SpreaderDetectorGetNumOfMeetings\n");
  if(SpreaderDetectorGetNumOfMeetings(spreader_with_files) != 7 ||
  SpreaderDetectorGetNumOfPeople(spreader_with_files) != 9)
  {
    fprintf(stderr, "Test 8 FAILED!\n");
    exit(8);
  }
  printf("Test 8 PASSED!\n\n");

  size_t people_array_len = spreader_with_files->people_size;
  size_t meeting_array_len = spreader_with_files->meeting_size;

  for(size_t i = 0; i < people_array_len; i++)
  {
    PersonFree(&(spreader_with_files->people[i]));
    spreader_with_files->people_size--;
  }
  for(size_t i = 0; i < meeting_array_len; i++)
  {
    MeetingFree(&(spreader_with_files->meetings[i]));
    spreader_with_files->meeting_size--;
  }
  SpreaderDetectorFree(&spreader_with_files);
  printf("ALL TESTS PASSED! :)\n");
  return 0;
}


void CheckFailure(int failure)
{
  switch (failure)
  {
    case 0:
      return;
    case 1:
      fprintf(stderr, "Test 1 FAILED!\n\n");
      exit(1);
    case 2:
      fprintf(stderr, "Test 2 FAILED!\n\n");
      exit(2);
    case 3:
      fprintf(stderr, "Test 3 FAILED!\n\n");
      exit(3);
    case 4:
      fprintf(stderr, "Test 4 FAILED!\n\n");
      exit(4);
    case 5:
      fprintf(stderr, "Test 5 FAILED!\n\n");
      exit(5);
    case 6:
      fprintf(stderr, "Test 6 FAILED\n\n");
      exit(6);
    case 7:
      fprintf(stderr, "Test 7 FAILED\n\n");
      exit(7);
    default:
      return;
  }
}

int Test1(SpreaderDetector **sp)
{
  SpreaderDetector *spreader_detector = SpreaderDetectorAlloc();
  if(!spreader_detector)
  {
    fprintf(stderr, "Memory allocation Failed: spreader_detector\n");
    return Test1Failed;
  }
  *sp = spreader_detector;
  return Success;
}

int Test2(SpreaderDetector *sp, Person*p1,Person*p2,Person*p3,Person*p4,
    Person*p5,Person*p6,Person*p7,Person*p8,Person*p9)
{
  int add1 = SpreaderDetectorAddPerson(sp, p1);
  int add2 = SpreaderDetectorAddPerson(sp, p2);
  int add3 = SpreaderDetectorAddPerson(sp, p3);
  int add4 = SpreaderDetectorAddPerson(sp, p4);
  int add5 = SpreaderDetectorAddPerson(sp, p5);
  int add6 = SpreaderDetectorAddPerson(sp, p6);
  int add7 = SpreaderDetectorAddPerson(sp, p7);
  int add8 = SpreaderDetectorAddPerson(sp, p8);
  int add9 = SpreaderDetectorAddPerson(sp, p9);
  if(sp->people_cap != SPREADER_DETECTOR_GROWTH_FACTOR * SPREADER_DETECTOR_INITIAL_SIZE)
  {
    return Test2Failed;
  }
  if(sp->people_size != 9)
  {
    return Test2Failed;
  }
  if(!add1 || !add2 || !add3 || !add4 || !add5 || !add6 || !add7 || !add8 ||
  !add9)
  {
    return Test2Failed;
  }
  Person *id_duplicate = PersonAlloc(333333333,"AviRon", 24, 0);
  int add10 = SpreaderDetectorAddPerson(sp, id_duplicate);
  if(add10)
  {
    PersonFree(&id_duplicate);
    return  Test2Failed;
  }
  PersonFree(&id_duplicate);
  return Success;
}

int Test3(SpreaderDetector *sp,Meeting *c1a,Meeting *c1b,Meeting *c1c,
          Meeting *c2a,Meeting *c2b,Meeting *c2c,Meeting *c2d,Meeting *c3a, Person
          *p) {
  int add1 = SpreaderDetectorAddMeeting(sp, c1a);
  int add2 = SpreaderDetectorAddMeeting(sp, c1b);
  int add3 = SpreaderDetectorAddMeeting(sp, c1c);
  int add4 = SpreaderDetectorAddMeeting(sp, c2a);
  int add5 = SpreaderDetectorAddMeeting(sp, c2b);
  int add6 = SpreaderDetectorAddMeeting(sp, c2c);
  int add7 = SpreaderDetectorAddMeeting(sp, c2d);
  int add8 = SpreaderDetectorAddMeeting(sp, c3a);
  if(sp->meeting_cap != SPREADER_DETECTOR_GROWTH_FACTOR *
      SPREADER_DETECTOR_INITIAL_SIZE)
  {
    return Test3Failed;
  }
  if(sp->meeting_size != 8)
  {
    return Test3Failed;
  }
  if(!add1 || !add2 || !add3 || !add4 || !add5 || !add6 || !add7 || !add8)
  {
    return Test3Failed;
  }
  Person *not_in_detector = PersonAlloc(774837465, "BananaLoti", 13, 0);
  Meeting *not_to_be_added = MeetingAlloc(p, not_in_detector, 8, 9);
  int add9 = SpreaderDetectorAddMeeting(sp, not_to_be_added);
  if(add9)
  {
    PersonFree(&not_in_detector);
    MeetingFree(&not_to_be_added);
    return Test2Failed;
  }
  MeetingFree(&not_to_be_added);
  PersonFree(&not_in_detector);
  return Success;
}

int Test4(SpreaderDetector **spreader) {
  SpreaderDetector *empty_of_people = SpreaderDetectorAlloc();
  if(!empty_of_people)
  {
    fprintf(stderr, "Test 4 memory allocation failed!\n");
    return Test4Failed;
  }
  SpreaderDetectorReadPeopleFile(empty_of_people, EMPTY_PEOPLE_TXT);
  if(empty_of_people->people_size != 0) //|| *empty_of_people->people != NULL
  {
    SpreaderDetectorFree(&empty_of_people);
    return Test4Failed;
  }
  SpreaderDetectorFree(&empty_of_people);

  SpreaderDetector *sp1 = SpreaderDetectorAlloc();
  if(!sp1)
  {
    fprintf(stderr, "Test 4 memory allocation failed!\n");
    return Test4Failed;
  }
  SpreaderDetectorReadPeopleFile(sp1, PEOPLE_TXT);
  if(sp1->people_size != 9 || sp1->people[0] == NULL || sp1->people[8] == NULL)
  {
    for(int i = 0; i < 9; i++)
    {
      PersonFree(&(sp1->people[i]));
      sp1->people_size--;
    }
    SpreaderDetectorFree(&sp1);
    return Test4Failed;
  }
  (*spreader) = sp1;
  return Success;
}

int Test5(SpreaderDetector **spreader)
{
  SpreaderDetector *empty_of_meeting = SpreaderDetectorAlloc();
  if(!empty_of_meeting)
  {
    fprintf(stderr, "Test 5 memory allocation failed!\n");
    return Test4Failed;
  }
  SpreaderDetectorReadMeetingsFile(empty_of_meeting, EMPTY_MEETING_TXT);
  if(empty_of_meeting->meeting_size != 0 || *empty_of_meeting->meetings != NULL)
  {
    for (size_t i = 0; i < empty_of_meeting->meeting_size; i++)
    {
      MeetingFree(&(empty_of_meeting->meetings[i]));
      empty_of_meeting->meeting_size--;
    }
    SpreaderDetectorFree(&empty_of_meeting);
  }
  SpreaderDetectorFree(&empty_of_meeting);

  SpreaderDetectorReadMeetingsFile(*spreader, MEETING_TXT);
  if((*spreader)->meeting_size != 7)
  {
    for(size_t i = 0; i < (*spreader)->meeting_size; i++)
    {
      MeetingFree(&(*spreader)->meetings[i]);
      (*spreader)->meeting_size--;
    }
    SpreaderDetectorFree(spreader);
    return Test5Failed;
  }
  for(size_t i = 0; i < (*spreader)->meeting_size; i++)
  {
    if(MeetingGetPerson((*spreader)->meetings[i], 2)->id == 101010101)
    {
      for(size_t j = 0; j < (*spreader)->meeting_size; j++)
      {
        MeetingFree(&(*spreader)->meetings[j]);
        (*spreader)->meeting_size--;
      }
      SpreaderDetectorFree(spreader);
      return Test5Failed;
    }
  }
  return Success;
}

int Test6(SpreaderDetector *spreader_detector)
{
  if(!spreader_detector)
  {
    return Test6Failed;
  }
  SpreaderDetectorCalculateInfectionChances(spreader_detector);
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 111111111) -
  0.21168 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 222222222) !=0)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 333333333) -
      0.071111 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 444444444) -
      0.015802 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 555555555) -
      0.006048 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 666666666) -
      0.081529 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 777777777) -
      0.80658 >= 0.00001)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 888888888) !=1)
  {
    return Test6Failed;
  }
  if(SpreaderDetectorGetInfectionRateById(spreader_detector, 999999999) -
      0.034074 >= 0.00001)
  {
    return Test6Failed;
  }
  return Success;
}


int Test7(SpreaderDetector *sp)
{
  if(!sp)
  {
    return Test7Failed;
  }
  int printed = SpreaderDetectorPrintRecommendTreatmentToAll(sp, OUTPUT_TXT);
  if(printed == 0)
  {
    return Test7Failed;
  }
  FILE *treatment_file = fopen(OUTPUT_TXT, "r");
  char name[MAX_LEN_OF_LINE] = {0}, buffer[MAX_LEN_OF_LINE] = {0};
  char action[MAX_LEN_OF_LINE] = {0};
  size_t id = 0, age = 0;
  double ratio = 0;
  size_t  i = 0;
  while(fgets(buffer, MAX_LEN_OF_LINE, treatment_file) != NULL)
  {
    sscanf(buffer, "%s %s %lu %lu %lf",action, name, &id, &age, &ratio);
    Person *person = sp->people[i];
    if(strcmp(person->name, name) != 0 || person->id != id || person->age !=age)
    {
      fclose(treatment_file);
      return Test7Failed;
    }
    if(person->infection_rate - ratio >= 0.000001)
    {
      fclose(treatment_file);
      return Test7Failed;
    }
    i++;
  }
  fclose(treatment_file);
  return Success;
}