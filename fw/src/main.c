#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>
#include "midi.h"
#include "serial.h"

static const int keymap[32] = {
	0, 4, 8,  12, 16, 20, 24, 28,
	1, 5, 9,  13, 17, 21, 25, 29,
	2, 6, 10, 14, 18, 22, 26, 30,
	3, 7, 11, 15, 19, 23, 27, 31
};
static unsigned char key_state[4]; /* 32 bits for 32 note keys */

static int octave_offs = 60;
static int chan = 0;

#ifdef DBGCON
static int con_send_char(char c, FILE *fp);
static int con_get_char(FILE *fp);

static FILE std_stream = FDEV_SETUP_STREAM(con_send_char, con_get_char, _FDEV_SETUP_RW);
#endif


int main(void)
{
	int i, j, key;
	unsigned char new_state[4], diff_state[4];

	DDRB = 3;
	DDRC = 0;
	DDRD = 0;

#ifdef DBGCON
	init_serial(0, 38400, 8, PAR_NONE, 1);
	stdin = stdout = stderr = &std_stream;
#endif
	init_serial(1, 31250, 8, PAR_NONE, 1);

	for(;;) {
		/* read key state and detect changes */
		for(i=0; i<4; i++) {
			PORTB = i;	/* select mux inputs */
			new_state[i] = ~((PINC & 0xf) | (PIND & 0xf0));
			diff_state[i] = new_state[i] ^ key_state[i];
			key_state[i] = new_state[i];
		}

		for(i=0; i<4; i++) {
			if(diff_state[i] == 0) continue;

			for(j=0; j<8; j++) {
				unsigned char keybit = 1 << j;

				if(diff_state[i] & keybit) {
					/* key changed state */
					key = keymap[(i << 3) + j];
					if(new_state[i] & keybit) {
						/* press */
						midi_note(chan, key + octave_offs, 127);
#ifdef DBGCON
						printf("key %d -> note %d ON\n", key, key + octave_offs);
#endif
					} else {
						/* release */
						midi_note(chan, key + octave_offs, 0);
#ifdef DBGCON
						printf("key %d -> note %d OFF\n", key, key + octave_offs);
#endif
					}
				}
			}
		}
	}

	return 0;
}


#ifdef DBGCON
static int con_send_char(char c, FILE *fp)
{
	if(c == '\n') uart_write(0, '\r');
	uart_write(0, c);
	return 0;
}

static int con_get_char(FILE *fp)
{
	return uart_read(0);
}
#endif
