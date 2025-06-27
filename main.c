#define F_CPU 3333333UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>

volatile uint16_t tick = 0;
volatile uint16_t pb3LowTime = 0;

typedef enum {
	STATE_SLEEP,
	STATE_2,
	STATE_3
} State;

State currentState = STATE_SLEEP;
bool pb5Pressed = false;
bool pb5Released = false;

// ---------- Clock ----------
void setup_clock(void) {

}

// ---------- GPIO ----------
void setup_gpio(void) {
	PORTA.DIRSET = PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm;
	PORTB.DIRCLR = PIN3_bm | PIN4_bm | PIN5_bm;
	PORTC.DIRCLR = PIN1_bm;

	PORTA.OUTCLR = PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm;
	PORTB.PIN3CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN4CTRL = PORT_PULLUPEN_bm;
	PORTB.PIN5CTRL = PORT_PULLUPEN_bm;
	PORTC.PIN1CTRL = PORT_PULLUPEN_bm;
	PORTA.DIRCLR = PIN7_bm; // Analog in
}

// ---------- Timer ----------
void setup_timer(void) {
	TCA0.SINGLE.PER = 1627;
	TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;
	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc | TCA_SINGLE_ENABLE_bm;
}

void sleep_idle(void) {
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();
	sleep_mode();
	sleep_disable();
}

void enter_sleep_mode(void) {
	PORTA.OUTCLR = PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

float read_voltage(void) {
	ADC0.CTRLC = ADC_PRESC_DIV64_gc;
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc;
	ADC0.COMMAND = ADC_STCONV_bm;
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm));
	uint16_t res = ADC0.RES;
	ADC0.CTRLA = 0;
	return res * 5.0f / 1024.0f;
}

// ---------- Interrupt ----------
ISR(TCA0_OVF_vect) {
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	tick++;

	static bool prevPB5 = true;
	bool currPB5 = (PORTB.IN & PIN5_bm);
	if (!currPB5 && prevPB5) pb5Pressed = true;
	if (currPB5 && !prevPB5) pb5Released = true;
	prevPB5 = currPB5;

	if (!(PORTB.IN & PIN3_bm)) pb3LowTime++;
	else pb3LowTime = 0;
}

// ---------- FSM States ----------
void state1_sleep(void) {
	currentState = STATE_SLEEP;
	enter_sleep_mode();
}

void state2_active(void) {
	currentState = STATE_2;
	tick = 0;
	pb3LowTime = 0;
	PORTA.OUTCLR = PIN3_bm | PIN4_bm | PIN5_bm;
	PORTA.OUTSET = PIN6_bm;

	while (1) {
		if (pb5Pressed) {
			pb5Pressed = false;
			return;
		}

		if (tick == 20) {
			PORTA.OUTSET = PIN3_bm | PIN4_bm;
		}
		if (tick == 30) {
			PORTA.OUTCLR = PIN3_bm | PIN4_bm;
		}

		if (!(PORTB.IN & PIN3_bm)) {
			if (tick % 4 == 0) PORTA.OUTSET = PIN3_bm | PIN4_bm;
			else if (tick % 4 == 2) PORTA.OUTCLR = PIN3_bm | PIN4_bm;
			} else {
			if (tick % 10 == 0) PORTA.OUTSET = PIN3_bm | PIN4_bm;
			else if (tick % 10 == 5) PORTA.OUTCLR = PIN3_bm | PIN4_bm;
		}

		sleep_idle();
	}
}

void state3_loop(void) {
	currentState = STATE_3;
	tick = 0;
	pb3LowTime = 0;
	uint16_t lastBlinkTick = 0;
	bool ledsOn = false;
	PORTA.OUTCLR = PIN3_bm | PIN4_bm;
	PORTA.OUTSET = PIN6_bm;

	while (1) {
		if (pb3LowTime >= 40) {
			state1_sleep();
			return;
		}

		if (tick == 100) PORTA.OUTCLR = PIN6_bm;

		if (tick >= 1200) {
			if (!ledsOn && tick - lastBlinkTick >= 0) {
				PORTA.OUTSET = PIN3_bm | PIN4_bm;
				ledsOn = true;
				lastBlinkTick = tick;
			}
			if (ledsOn && tick - lastBlinkTick >= 20) {
				PORTA.OUTCLR = PIN3_bm | PIN4_bm;
				ledsOn = false;
				lastBlinkTick = tick;
			}
			if (!ledsOn && tick - lastBlinkTick >= 60) {
				lastBlinkTick = tick;
			}
		}

		if (tick == 6000) {
			PORTA.OUTSET = PIN6_bm;
			float v = read_voltage();
			if (v < 3.0f) {
				PORTA.OUTCLR = PIN4_bm;
				PORTA.OUTSET = PIN5_bm;
				} else {
				tick = 5900;
			}
		}

		sleep_idle();
	}
}

// ---------- MAIN ----------
int main(void) {
	setup_clock();
	setup_gpio();
	setup_timer();
	sei();
	set_sleep_mode(SLEEP_MODE_IDLE);
	sleep_enable();

	if (!(PORTB.IN & PIN5_bm)) {
		state1_sleep();
	}

	while (1) {
		if (currentState == STATE_SLEEP && (PORTB.IN & PIN5_bm)) {
			pb5Released = false;
			state2_active();
		}

		if (pb5Pressed && currentState == STATE_2) {
			pb5Pressed = false;
			state3_loop();
		}

		sleep_idle();
	}
}
