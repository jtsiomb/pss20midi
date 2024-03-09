#include <avr/io.h>
#include "serial.h"

int main(void)
{
	DDRB = 3;
	DDRC = 0;
	DDRD = 0;

	init_serial(1, 31250, 8, PAR_NONE, 1);
	return 0;
}
