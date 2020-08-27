#define XYEENABLE 0x40  // PD6

#define E_STEP 0x02  //PB1
#define E_DIR 0x01	 //PB0

void setup() {
	//Extruder (E)
	DDRB |= E_STEP;

	//Extruder TIMER
	TCCR2A = 0x02;
	TCCR2B = 0x05;
	TCNT2 = 0x00;
	//OCR2A = 256 - 167; // 334 us per interrupt ( CNT - 167 )
	OCR2A = 245;
	TIMSK2 = 0x02;	//0x02 : TIMER ON
}


void loop() {
	
}

volatile char e_step_toggle = 0;

ISR(TIMER2_COMPA_vect) {
	if (e_step_toggle == 0) {
		e_step_toggle = 1;
		PORTB |= E_STEP;
	}
	else {
		e_step_toggle = 0;
		PORTB &= ~E_STEP;
	}
}
