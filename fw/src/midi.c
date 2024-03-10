#include "midi.h"
#include "serial.h"

void midi_init(void)
{
	midi_all_off();
}

void midi_all_off(void)
{
	uart_write(1, MIDI_CMD_CHANMSG);
	uart_write(1, MIDI_CHANMSG_NOTES_OFF);
}

void midi_note(int chan, int note, int vel)
{
	if(vel > 0) {
		uart_write(1, MIDI_CMD_NOTEON | chan);
	} else {
		uart_write(1, MIDI_CMD_NOTEOFF | chan);
	}
	uart_write(1, note);
	uart_write(1, vel);
}
