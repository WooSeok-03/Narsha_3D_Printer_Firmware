#define X_DIR 0x20		// PC5
#define X_STEP 0x80	    // PD7
#define X_STOP 0x04		// PC2
#define XYEENABLE 0x40  // PD6

#define	Y_DIR 0x80	//PC7
#define	Y_STEP 0x40	//PC6
#define	Y_STOP 0x08	//PC3

#define ONE_mm 80	//1mm
#define ONE_cm 800	//1cm

void setup() {

	DDRC |= X_DIR;	//RIGHT DIRECTION
	DDRD |= X_STEP | XYEENABLE;
	DDRC &= ~(X_STOP);

	DDRC |= (Y_DIR | Y_STEP);
	DDRC &= ~(Y_STOP);

	//X Location
	TCCR1A = 0x00;
	TCCR1B = 0x0A;
	TCCR1C = 0x00;
	OCR1A = 400;
	TIMSK1 = 0x00;

	//Y Location
	TCCR3A = 0x00;
	TCCR3B = 0x0A;
	TCCR3C = 0x00;
	OCR3A = 400;
	TIMSK3 = 0x00;

}

char x_step_dir = 0;
char y_step_dir = 0;

void loop() {

	//사각형 그리기(방향전환)
	
	// X축 Direction
	TIMSK1 = 0x02;
	while (TIMSK1 != 0x00);
	if (x_step_dir == 0) {
		x_step_dir = 1;
		PORTC |= X_DIR;
	}
	else {
		x_step_dir = 0;
		PORTC &= ~(X_DIR);
	}

	// Y축 Direction
	TIMSK3 = 0x02;
	while (TIMSK3 != 0x00);
	if (y_step_dir == 0) {
		y_step_dir = 1;
		PORTC |= Y_DIR;
	}
	else {
		y_step_dir = 0;
		PORTC &= ~(Y_DIR);
	}
	
}

char x_step_toggle = 0;
int x_step_count = 0;

ISR(TIMER1_COMPA_vect) {	//X축만 관여함  / 인터럽트 발생할 때마다 스텝을 껏다켰다 반복
	if (x_step_toggle == 0) {
		x_step_toggle = 1;
		PORTD |= X_STEP;
	}
	else {
		x_step_toggle = 0;
		PORTD &= ~(X_STEP);

		x_step_count++;
		char x_limit_switch = PINC & X_STOP;

		if (x_step_count >= (5 * ONE_cm)) {	//X축 5cm 이동
			x_step_count = 0;
			TIMSK1 = 0x00;	//멈춤
		}

		//리밋 스위치를 누르면 동작 멈춤
		if (x_limit_switch) {	//X축 리밋 스위치를 눌렀을 때
			TIMSK1 = 0x00;

		}

	}

}

char y_step_toggle = 0;
int y_step_count = 0;

ISR(TIMER3_COMPA_vect) {	//Y축만 관여함  / 인터럽트 발생할 때마다 스텝을 껏다켰다 반복
	if (y_step_toggle == 0) {
		y_step_toggle = 1;
		PORTC |= Y_STEP;
	}
	else {
		y_step_toggle = 0;
		PORTC &= ~(Y_STEP);

		y_step_count++;
		char y_limit_switch = PINC & Y_STOP;


		if (y_step_count >= (5 * ONE_cm)) {	//Y축 5cm 이동
			y_step_count = 0;
			TIMSK3 = 0x00;
		}

		//리밋 스위치를 누르면 동작 멈춤
		if (y_limit_switch) {	//Y축 리밋 스위치를 눌렀을때
			TIMSK3 = 0x00;

		}

	}
}