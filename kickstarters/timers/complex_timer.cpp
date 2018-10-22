// taken from http://www.netzmafia.de/skripten/hardware/RasPi/timer/index.html


/* Compile with: gcc -Wall -o timer1 -lrt timer1.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>

timer_t Timerid1;
timer_t Timerid2;
int count1 = 0;
int count2 = 0;

/* Timer erzeugen und starten
 * Timerid: die zurueckgegebene ID des Timers
 * sek: Wartezeit Sekundenanteil
 * msek: Wartezeit Millisekundenanteil
 */
void start_timer(timer_t *Timerid, int sek, int msek)
  {
  struct itimerspec timer;

  timer.it_value.tv_sec = sek;
  timer.it_value.tv_nsec = msek * 1000000;
  timer.it_interval.tv_sec = sek;
  timer.it_interval.tv_nsec = msek * 1000000;

  timer_create (CLOCK_REALTIME, NULL, Timerid);
  timer_settime (*Timerid, 0, &timer, NULL);
  printf("Timer gestartet, ID: 0x%lx\n", (long) *Timerid);
  }

/* Anhalten eines durch Timerid identifizierten Timers
 * durch Setzen aller Zeiten auf 0
 */
void stop_timer(timer_t *Timerid)
  {
  struct itimerspec timer;

  timer.it_value.tv_sec = 0;
  timer.it_value.tv_nsec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_nsec = 0;

  timer_settime (*Timerid, 0, &timer, NULL);
  }

/* Signalhandler für alle Timer
 * Unterscheidung der Timer anhand von tidp
 */
void timer_callback(int sig, siginfo_t *si, void *uc)
  {
  timer_t tidp;

  tidp = si->si_value.sival_ptr;
  printf("Signal: %d, ID: %p ", sig, tidp);
  if (tidp == Timerid1)
    printf(", Count 1: %d\n",count1++);
  if (tidp == Timerid2)
    printf(", Count 2: %d\n",count2++);
  }

int main(void)
  {
  struct sigaction sa;

  /* cllback-Handler installieren */
  memset(&sa, 0, sizeof (sa));
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = timer_callback;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGALRM, &sa, NULL) == -1)
    perror("sigaction");

  /* timer starten */
  start_timer(&Timerid1,1,0);
  start_timer(&Timerid2,0,500);

  /* Programm macht irgendwas */
  while(count1 <= 5);

  /* Fertig, Timer stoppen */
  stop_timer(&Timerid2);
  stop_timer(&Timerid1);
  return 0;
  }
