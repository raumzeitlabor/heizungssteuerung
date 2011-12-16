#ifndef _NUMTOSTR_H
#define _NUMTOSTR_H

/* uncomment to use simpler routines - no negative numbers,
number of decimals and type of fill character are hard coded */

//#define SIMPLE

#ifdef SIMPLE

void ftoa(char * buf, double f, char pad);
//char * ltoa(char * buf, unsigned long val, char pad);

#else

void ftoa(char * buf, double f, char d, char pad, char padchar);
//char * ltoa(char * buf, long val, char pad, char padchar);

#endif

void ctob(char * buf, unsigned char val);
void itob(char * buf, unsigned int val);
void ctoh(char * buf, unsigned char val);
void itoh(char * buf, unsigned int val);
void ltoh(char * buf, unsigned long val);

#endif
