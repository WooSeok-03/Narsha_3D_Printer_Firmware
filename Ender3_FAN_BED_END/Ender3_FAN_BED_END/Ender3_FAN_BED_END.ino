#define FAN	0x10	//PB4

#define BED_TEMP A6		//PA6 (Analog_Pin 6)
#define HOT_BED	0x10	//PD4

#define END_TEMP A7		//PA7 (Analog_Pin 7)
#define HOT_END	0x20	//PD5


void setup() {
	//FAN
	DDRB |= FAN;
	PORTB |= FAN;

	//BED
	DDRD |= BED_TEMP;
	//PORTD |= BEDTEMP;

	//END (Nozzle)
	DDRD |= END_TEMP;
	PORTD |= END_TEMP;
	DDRD |= HOT_END;
	PORTD |= HOT_END;
}


void loop() {
	//FAN
	PORTB |= FAN;
	delay(2000);
	PORTB & ~FAN;
	delay(2000);

	//BED
	int bed_analog_value = analogRead(BED_TEMP);
	Serial.println(bed_analog_value);

	if (bed_analog_value < 920) PORTD &= ~HOT_BED;
	delay(300);

	//END (Nozzle)
	int end_analog_value = analogRead(END_TEMP);
	Serial.println(end_analog_value);

	if (end_analog_value <= 100) PORTD &= ~HOT_END;
	else if (end_analog_value > 105) PORTD |= HOT_END;
	delay(300);
}
