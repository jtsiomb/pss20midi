#include <avr/io.h>
#include <stdint.h>
#include "midi.h"
#include "serial.h"

union key_state {
	uint32_t s32;
	uint8_t s8[4];
};

const int keymap[32] = {
	0, 4, 8,  12, 16, 20, 24, 28,
	1, 5, 9,  13, 17, 21, 25, 29,
	2, 6, 10, 14, 18, 22, 26, 30,
	3, 7, 11, 15, 19, 23, 27, 31
};
union key_state key_state; /* 32 bits for 32 note keys */

int octave_offs = 60;
int chan = 0;

void select_mux(int sel);

int main(void)
{
	union key_state new_state;
	union key_state diff_state;
	int i, j;
	int key;

	DDRB = 3;
	DDRC = 0;
	DDRD = 0;

	init_serial(1, 31250, 8, PAR_NONE, 1);

	/* reading keys */
	for(;;) {
		for(i = 0; i < 4; i++) {
			select_mux(i);
			new_state.s8[i] = ~((PINC & 0xf) | (PIND & 0xf0));
		}
		diff_state.s32 = new_state.s32 ^ key_state.s32;

		for(i = 0; i < 4; i++) {
			if(diff_state.s8[i] == 0) continue;
			for(j = 0; j < 8; j++) {
				if(diff_state.s8[i] & (1 << j)) {
					/* key changed state */
					key = keymap[(i << 3) + j];
					if(new_state.s8[i] & (1 << j)) {
						midi_note(chan, key + octave_offs, 127);
					} else {
						midi_note(chan, key + octave_offs, 0);
					}
				}
			}
		}
		key_state = new_state;
	}

	return 0;
}

void select_mux(int sel)
{
	PORTB = sel;
}
