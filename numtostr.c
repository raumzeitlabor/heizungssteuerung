#include "numtostr.h"

#ifdef SIMPLE

void ftoa(char * buf, double f, char pad) 
{ 
	char * p; 

	f = (f*100.0 + 0.5)/100.0;        // round 
	p = ltoa(buf,f,pad);
	*p++ = '.';

	f -= (unsigned int) f;
	f = f * 100.0;
	ltoa(p,f,2);
}
/*
char * ltoa(char * buf, unsigned long val, char pad)
{
	char i;
	char tmp_buf[10];

	for(i=0; val>0; val/=10)
		tmp_buf[i++] = (val % 10) + '0';
	if(i==0)
		tmp_buf[i++] = '0';

	while(i<pad)
		tmp_buf[i++] = '0';

	while(i>0)
		*buf++ = tmp_buf[--i];

	*buf = 0;

	return buf;
}*/

#else

void ftoa(char * buf, double f, char d, char pad, char padchar)
{
	char * p;
	const double powers[] = {1.0,10.0,100.0,1000.0,10000.0,100000.0};

	/* round */
	f = f>=0.0 ? (f*powers[d]+0.5)/powers[d] : (f*powers[d]-0.5)/powers[d];

	/* special case */
	if (f<0.0 && f > -1.0) {
		p = ltoa(buf,-1,pad,padchar);
		*(p-1) = '0';
	}
	else
		p = ltoa(buf,f,pad,padchar);

	*p++ = '.';

	if (f<0.0)
		f = -f;
	
	f -= (long) f;
	f *= powers[d];
	ltoa(p,(long)f,d,0);
}


/*
char * ltoa(char * buf, long val, char pad, char padchar)
{
	char i;
	char neg = 0;
	char tmp_buf[21];

	if(val<0) {
		val = -val;
		neg = 1;
	}

	for(i=0; val>0; val/=10)
		tmp_buf[i++] = (val % 10) + '0';

	if(i==0)
		tmp_buf[i++] = '0';

	if(neg) {
		if(padchar) {
			tmp_buf[i++] = '-';
			while(i<pad)
				tmp_buf[i++] = ' ';
		}
		else {
			while(i<pad-1)
				tmp_buf[i++] = '0';
			tmp_buf[i++] = '-';
		}
	}
	else
		while(i<pad)
			tmp_buf[i++] = padchar ? ' ' : '0';

	while(i>0)
		*buf++ = tmp_buf[--i];

	*buf = 0;

	return buf;
}*/

#endif

/*
** convert char to binary string
*/
void ctob(char * buf, unsigned char val)
{
	signed char i;

	buf+=8;
	*buf-- = 0;
	
    for (i=7; i>=0; i--) {
        *buf-- = val%2 + '0';
        val >>= 1;
    }
}

/*
** convert int to binary string
*/
void itob(char * buf, unsigned int val)
{
	signed char i;

	buf+=16;
	*buf-- = 0;
	
    for (i=15; i>=0; i--) {
        *buf-- = val%2 + '0';
        val >>= 1;
    }
}

/*
** convert char to hex string
*/
void ctoh(char * buf, unsigned char val)
{
	buf+=2;
	*buf-- = 0;

	*buf-- = val%16 < 10 ? val%16 + '0' : val%16 + 'a'-10;
	val >>= 4;
	*buf = val%16 < 10 ? val%16 + '0' : val%16 + 'a'-10;
}

/*
** convert int to hex string
*/
void itoh(char * buf, unsigned int val)
{
	char i;

	buf+=4;
	*buf-- = 0;

	for (i=0;i<4;i++) {
		*buf-- = val%16 < 10 ? val%16 + '0' : val%16 + 'a'-10;
		val >>= 4;
	}
}

/*
** convert long to hex string
*/
void ltoh(char * buf, unsigned long val)
{
	char i;

	buf+=8;
	*buf-- = 0;

	for (i=0;i<8;i++) {
		*buf-- = val%16 < 10 ? val%16 + '0' : val%16 + 'a'-10;
		val >>= 4;
	}
}
