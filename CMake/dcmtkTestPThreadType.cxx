// test to see if pthread_t is a pointer type or not

#include <pthread.h>

int main ()
{
  pthread_t p;
  unsigned long l = p;
  return 0;
}
