#define Encoder_A 0x04	//PD2
#define Encoder_B 0x08	//PD3
#define Encoder_button 0x01 //PD0

void setup() {
	//Encoder
	DDRD &= ~(Encoder_A);
	DDRD &= ~(Encoder_B);
	PORTD |= Encoder_A | Encoder_B;

	//Encoder_button
	DDRC &= ~(Encoder_button);
	PORTC |= Encoder_button;

	Serial.begin(9600);
}

int c_millis = 0;
int Encoder_p_millis = 0;

int previous_state = 0;
int current_state = 0;

int button_toggle = 0;
int right_count = 0;
int left_count = 0;

void loop() {
	//Encoder
	c_millis = millis();
	if (c_millis - Encoder_p_millis > 1) {
		Encoder_p_millis = c_millis;

		previous_state = current_state;

		char Encoder_A_val = PIND & Encoder_A;
		char Encoder_B_val = PIND & Encoder_B;
		int button_state = PINC & Encoder_button;

		//Encoder Button State
		if (button_state == 1) {
			if (button_toggle == 1) {
				Serial.println("UP");
			}
			button_toggle = 0;
		}
		else {
			if (button_toggle == 0) {
				Serial.println("DOWN");
			}
			button_toggle = 1;
		}

		//Encoder State
		if (Encoder_A_val == 0 && Encoder_B_val == 4) {
			current_state = 1;
		}
		else if (Encoder_A_val == 0 && Encoder_A_val == 0) {
			current_state = 2;
		}
		else if (Encoder_A_val == 4 && Encoder_B_val == 0) {
			current_state = 3;
		}
		else if (Encoder_A_val == 4 && Encoder_A_val == 4) {
			current_state = 4;
		}

		if (current_state == 3) {
			if (previous_state == 2) {
				right_count++;
				Serial.print("R : ");
				Serial.println(right_count);
			}
		}

		if (current_state == 2) {
			if (previous_state == 3) {
				left_count++;
				Serial.print("L : ");
				Serial.println(left_count);
			}
		}
	}
}
