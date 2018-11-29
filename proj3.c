/*
Class: CPSC 346-01
Team Member 1: Thomas McDonald
Team Member 2: N/A 
GU Username of project lead: tmcdonald
Pgm Name: proj3.c 
Pgm Desc: exploraiton of the proc file system 
Usage: 1) standard:  ./a.out -s 
Usage: 2) history:  ./a.out -h 
Usage: 3) load:  ./a.out -l 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void standard();
void history();
void load();

int main(int argc, char* argv[])
{
 if (argc != 2)
  {
   fprintf(stderr, "Error: Options required\n"); 
   fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n"); 
   exit(EXIT_FAILURE);
  }
  
 if (!strcmp(argv[1],"-s"))
  standard();
 if (!strcmp(argv[1],"-h"))
  history();
 if (!strcmp(argv[1],"-l"))
  load();
}
/*
pre: none
post: displays CPU vendor_id, model name, and OS version
*/
void standard()
{
 char ch;
 FILE* ifp;
 char str[80];

 /*
 I've deliberately used two different mechanisms for writing to the console.  
 Use whichever suits you.
 strstr locates a substring
 */

 ifp = fopen("/proc/cpuinfo","r");
 while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"vendor_id") || strstr(str,"model name"))
   puts(str); 
 fclose (ifp);

 ifp = fopen("/proc/version","r");
 while ((ch = getc(ifp)) != EOF)
  putchar(ch);
 fclose (ifp);
}

/*
pre: none
post: displays time since the last reboot (DD:HH:MM:SS), time when the system 
      was last booted, number of processes that have been created since the
      last reboot.  
      Hint: strftime could be useful
*/
void history()
{
  FILE* ifp;
  ifp = fopen("/proc/uptime","r");
  int secsPast;
  if(ifp != NULL)
    fscanf(ifp,"%d", &secsPast);
  fclose(ifp);
//convert seconds to days,hourse,mins,seconds
  int days = secsPast/86400;
  secsPast = secsPast%86400;
  int hours = secsPast/3600;
  secsPast = secsPast%3600;
  int mins = secsPast/60;
  secsPast = secsPast%60;
  printf("Time Since Last Reboot: [%d:%d:%d:%d]\n",days,hours,mins,secsPast);
//time booted
  char str[80];
  ifp = fopen("/proc/stat","r");
  int btime;
  while (fgets(str,80,ifp) != NULL)
    if (strstr(str,"btime"))
      btime = atoi(str+6);
//calculate current time
  int bdays = btime/86400;
  btime = btime%86400;
  int bhours = btime/3600+6;
  btime = btime%3600;
  int bmins = btime/60;
  btime = btime%60;
  int curHour= (bhours+hours)%13;
  int curMin = bmins+mins+(secsPast+btime)/60;
  //Print time of last boot
  printf("Last Boot Time: %d:%d\n", bhours%13,bmins);
  //Print current time
  printf("Current Time: %d:%d\n", curHour,curMin);
//finds and prints processes
  
  ifp = fopen("/proc/stat","r");
  while (fgets(str,80,ifp) != NULL)
    if (strstr(str,"processes"))
      puts(str); 
  fclose (ifp);
}

/*
pre: none
post: displays total memory, available memory, load average (avg. number of processes over the last minute) 
*/
void load()
{
  FILE* ifp;
  char str[80];
//memory
  ifp = fopen("/proc/meminfo","r");
  while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"MemTotal")||strstr(str,"MemAvailable"))
   puts(str); 
  fclose (ifp);
  
  //processes (basically history method edited)
  ifp = fopen("/proc/uptime","r");
  int secsPast;
  if(ifp != NULL)
    fscanf(ifp,"%d", &secsPast);
  fclose(ifp);
//amount of mins passed since bootup
  int mins = secsPast/60;
//amount of processes since bootup
  int processes;
  ifp = fopen("/proc/stat","r");
  while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"processes"))
    processes = atoi(str+10); //10 being the offset of processes
  fclose (ifp);
  printf("Average Processes/Min: %d\n",processes/mins);
}
