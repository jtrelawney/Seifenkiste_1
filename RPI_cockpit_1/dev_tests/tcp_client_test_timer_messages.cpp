// this is from
// http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html

// it sends a short message to the server and gets one back
// call with the server ip address and the port
// ./tcp.exe 192.168.0.15 8192




#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//added those because of warnings
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <string>
#include <tcp_client.h>
#include <iostream>

// the caller hands over an image pointer, this function creates the memory, the caller is reponsible to release the memory
int read_image( char **image, unsigned long *imagelen, int image_counter=1)
{
        std::string filename = "../ferraricon_"+std::to_string(image_counter)+".jpg";
        std::cout << "filename = " << filename << std::endl;

        char *buffer;
        unsigned long fileLen;

        //Open file
        FILE *file;
        //const char *fn = "../ferraricon_1.jpg";
        const char *fn = filename.c_str();
        
        fprintf(stderr, "opening file %s\n", fn);

        file = fopen(fn, "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s", fn);
                return 2;
        }

        //Get file length
        fseek(file, 0, SEEK_END);
        fileLen=ftell(file);
        fseek(file, 0, SEEK_SET);

        //Allocate memory
        buffer=(char *)malloc(fileLen);
        if (!buffer)
        {
                fprintf(stderr, "Memory error!");
                fclose(file);
                return 1;
        }

	// print some test data
	*imagelen=fileLen;
        fprintf(stderr, "file length %lu\n", fileLen);
        fprintf(stderr, "image len %lu\n", *imagelen);

        fread(buffer,fileLen,sizeof(unsigned char),file);
        fclose(file);
        fprintf(stderr, "done reading file\n");

        *image = buffer;
        fprintf(stderr, "buffer addr %lu\n", (unsigned long)&buffer[0]);
        fprintf(stderr, "img addr %lu\n", (unsigned long)*image);
	
	//print_n(buffer,40);
	//print_n(*image,40);

	return 0;
}

// taken from http://www.netzmafia.de/skripten/hardware/RasPi/timer/index.html


/* Compile with: gcc -Wall -o timer1 -lrt timer1.c */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>

int image_counter;

timer_t Timerid1;
timer_t Timerid2;
int count1 = 0;
int count2 = 0;

bool send_indicator;

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
  if (tidp == Timerid1) {
	  printf(", Count 1: %d\n",count1++);
	  send_indicator=true;
  }
  
  if (tidp == Timerid2)
    printf(", Count 2: %d\n",count2++);
  }

int main(void)
{
	image_counter=1;
	send_indicator=false;	
	tcp_client tcp;
	
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
	// for this test the interrupt routine TCPs the image and increases the image counter
	// the goal is to send each image once to the pc
	while(count1 <= 5){
		if (send_indicator == true) {
		    char *image;
			unsigned long imagelen;			
			int res = read_image(&image,&imagelen, image_counter);

			fprintf(stderr, "return from data fetch\n");
			fprintf(stderr, "image len %lu\n", imagelen);
			fprintf(stderr, "img addr %lu\n", (unsigned long)image);
    
			int result = tcp.send_test_image(image, imagelen);
			if (result<0) std::cout << "error sending TCP image" << std::endl;
			send_indicator=false;
			
			image_counter++;
			if (image_counter>5) image_counter=1;
			
		}
	};

	/* Fertig, Timer stoppen */
	stop_timer(&Timerid2);
	stop_timer(&Timerid1);

	return 0;
}
