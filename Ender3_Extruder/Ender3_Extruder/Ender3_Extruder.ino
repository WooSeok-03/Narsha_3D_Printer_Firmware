#define XYEENABLE 0x40  // PD6

#define FAN	0x10	//PB4

#define BED_TEMP A6		//PA6 (Analog_Pin 6)
#define HOT_BED	0x10	//PD4

#define END_TEMP A7		//PA7 (Analog_Pin 7)
#define HOT_END	0x20	//PD5

#define E_STEP 0x02  //PB1
#define E_DIR 0x01	 //PB0

void setup() {
	//Extruder (E)
	DDRB |= E_STEP;

	//FAN
	DDRB |= FAN;

	//BED
	DDRD |= HOT_BED;
	PORTD |= HOT_BED;

	//END (Nozzle)
	DDRD |= HOT_END;
	PORTD |= HOT_END;

	//Extruder TIMER
	TCCR2A = 0x02;
	TCCR2B = 0x05;
	TCNT2 = 0x00;
	//OCR2A = 256 - 167; // 334 us per interrupt ( CNT - 167 )
	OCR2A = 245;
	TIMSK2 = 0x02;	//0x02 : TIMER ON

	Serial.begin(9600);
}


void loop() {
	//Nozzle이 필라멘트를 녹일만큼 온도가 높아지면 Extruder 동작
	int end_value = analogRead(A7);
	int bed_value = analogRead(A6);
	Serial.print("END : ");
	Serial.print(end_value);
	Serial.print("	BED : ");
	Serial.println(bed_value);
	if (end_value < 95) {
		PORTD &= ~HOT_END;
		PORTB |= FAN;
		TIMSK2 = 0x02;
	}
	else if (end_value > 100) {
		PORTD |= HOT_END;
	}
	if (bed_value < 920) PORTD &= ~HOT_BED;
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
