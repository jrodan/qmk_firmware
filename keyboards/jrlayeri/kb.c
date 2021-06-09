#include "kb.h"

void matrix_init_kb(void) {

	matrix_init_user();

	DDRD &= ~(1<<5);
    PORTD &= ~(1<<5);

    DDRB &= ~(1<<0);
    PORTB &= ~(1<<0);
}
