
#include "jitterhist.h"


int find_window_type(long int window_num, char *pwindow_type)
{
int error_flag = 0;

char *prectangular = "Rectangular";
char *ptapered_rectangular = "Tapered rectangular";
char *ptriangular = "Triangular";
char *phanning = "Hanning";
char *phamming = "Hamming";
char *pblackman = "Blackman";
char *pblackman_hanning = "Blackman-Hanning";


/* RECTANGULAR\n[2] TAPERED RECTANGULAR\n[3] TRIANGULAR\n");
printf("[4] HANNING\n[5] HAMMING\n[6] BLACKMAN\n[7] BLACKMAN-HANNING\n"); */

switch(window_num)
  {
   case 1:
      strcpy(pwindow_type,prectangular);
      break;
   case 2:
      strcpy(pwindow_type,ptapered_rectangular);
      break;
   case 3:
      strcpy(pwindow_type,ptriangular);
      break;
   case 4:
      strcpy(pwindow_type,phanning);
      break;
   case 5:
      strcpy(pwindow_type,phamming);
      break;
   case 6:
      strcpy(pwindow_type,pblackman);
      break;
   case 7:
      strcpy(pwindow_type,pblackman_hanning);
      break;
   default:
      printf("Did not recognize window type of %ld...exiting\n",window_num);
      error_flag = 1;
      break;
   }

if (error_flag == 0)
	return EXIT_SUCCESS;
else
	return EXIT_FAILURE;
	
}