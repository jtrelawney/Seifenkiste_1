
// taken from http://www.netzmafia.de/skripten/hardware/RasPi/timer/index.html

/*
 POSIX kennt aber noch weitere Timer-Funktionen. Die klassischen Intervall-Timer leiden ja unter einer Reihe von Einschränkungen:

    Es kann nur ein Timer von jedem der drei o. g. Typen verwendet werden.
    Die einzige Möglichkeit, die über den Timer-Ablauf informiert, ist ein einziges Signal, SIGALRM.
    Auch wenn ein Intervall-Timer mehrmals abläuft, während das entsprechende Signal blockiert ist, dann wird der Signal-Handler nur einmal aufgerufen.
    Der Timer ist auf Mikrosekunden-Auflösung beschränkt. 
*/


/* Compile with:  gcc -Wall -o getres2 -lrt getres2.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(void)
  {
  struct timespec res;

  if (clock_getres(CLOCK_REALTIME, &res) == -1)
    perror("clock_getres: CLOCK_REALTIME");
  else
    printf("CLOCK_REALTIME:           %ld s, %ld ns\n", res.tv_sec, res.tv_nsec);

  if (clock_getres(CLOCK_MONOTONIC, &res) == -1)
    perror("clock_getres: CLOCK_MONOTONIC");
  else
    printf("CLOCK_MONOTONIC:          %ld s, %ld ns\n", res.tv_sec, res.tv_nsec);

  if (clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res) == -1)
    perror("clock_getres: CLOCK_PROCESS_CPUTIME_ID");
  else
    printf("CLOCK_PROCESS_CPUTIME_ID: %ld s, %ld ns\n", res.tv_sec, res.tv_nsec);

  if (clock_getres(CLOCK_THREAD_CPUTIME_ID, &res) == -1)
    perror("clock_getres: CLOCK_THREAD_CPUTIME_ID");
  else
    printf("CLOCK_THREAD_CPUTIME_ID:  %ld s, %ld ns\n", res.tv_sec, res.tv_nsec);

  return 0;
  }
