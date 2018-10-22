// example taken from http://www.netzmafia.de/skripten/hardware/RasPi/timer/index.html

#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring> //memset

/* globale Variablen fuer den itimer */
struct sigaction old_handler;

 typedef void(*timer_handler_ptr)(void);

/* Funktion, die beim Timer-Interrupt aufgerufen wird */
int var = 0;
void timer_handler(void)
  {
  printf("timer: var is %i\n", var++);
  }


/* Setzt den itimer auf die angegebene Zeit
 * mSec: Timer-Zeit in Millisekunden
 * timer_handler: Adresse der Timmer-Handler-Funktion
 * Die urspruengliche Adresse des Handlers fuer SIGALRM
 * wird in old_handler gespeichert.
 */

//int start_timer(int mSec, void *timer_handler, struct sigaction *old_handler)
int start_timer(int mSec, timer_handler_ptr timer_handler, struct sigaction *old_handler)
  {
  struct itimerval timervalue;
  struct sigaction new_handler;

  timervalue.it_interval.tv_sec = mSec / 1000;
  timervalue.it_interval.tv_usec = (mSec % 1000) * 1000;
  timervalue.it_value.tv_sec = mSec / 1000;
  timervalue.it_value.tv_usec = (mSec % 1000) * 1000;
  if(setitimer(ITIMER_REAL, &timervalue, NULL))
    {
    /* setitimer() error */
    return(1);
    }

  memset(&new_handler, 0, sizeof (new_handler));
  sigemptyset(&new_handler.sa_mask);
  new_handler.sa_handler = (void (*)(int))timer_handler;
  new_handler.sa_flags = SA_NOMASK;
  if(sigaction(SIGALRM, &new_handler, old_handler))
    {
    /* sigaction() error */
    return(2);
    }
  return(0);
  }

/* Anhalten des Timers, restaurieren des urspruenglichen
 * Handlers fuer SIGALRM
 */
void stop_timer(struct sigaction *old_handler)
  {
  struct itimerval timervalue;

  timervalue.it_interval.tv_sec = 0;
  timervalue.it_interval.tv_usec = 0;
  timervalue.it_value.tv_sec = 0;
  timervalue.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timervalue, NULL);
  sigaction(SIGALRM, old_handler, NULL);
  }

int main(void)
{

   
//timer_handler_ptr test = &timer_handler;

  if(start_timer(1000, &timer_handler, &old_handler) )
    {
    printf("\n timer error\n");
    return(1);
    }

  while(var <= 10)
    {
    /* Mach was ganz Wichtiges */
    }

  stop_timer(&old_handler);
  return(0);
  }
