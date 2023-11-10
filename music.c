#include "crt0.c"

int count = 0;

void interrupt_handler(){
	count--;
}

int btn_check_3()
{
volatile int *sw_ptr = (int *)0xff04;;
return (*sw_ptr & 0x80) ? 1 : 0;
}


void main()
{
	volatile int *iob_ptr = (int *)0xff10;
	while (1) {
		if (btn_check_3()) {
			*iob_ptr = 1;
			count = 10;
		}

		if (count == 0) {
			*iob_ptr = 0;
		}
	}
	
	
}
