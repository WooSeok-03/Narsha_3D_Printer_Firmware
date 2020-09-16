#define X_DIR 0x20	// PC5
#define X_STEP 0x80	// PD7
#define X_STOP 0x04	// PC2

#define XYEENABLE 0x40	// PD6

#define Z_DIR 0x04	 //PB2
#define Z_STEP 0x08	 //PB3
#define Z_STOP 0x10  //PC4
#define ZENABLE	0x20 //PA5

#define X_LEFT PORTC | X_DIR
#define X_RIGHT PORTC & ~(X_DIR)

#define Z_UP PORTB | Z_DIR
#define Z_DOWN PORTB & ~(Z_DIR)

#define ONE_mm 80	//1mm
#define ONE_cm 800	//1cm

enum {	//상수 리스트(왼쪽부터 0, 1, 2, 3 ... 으로 초기화)
	x_left, x_right, y_up, y_down, z_up, z_down
};

volatile char change_XZ = 0;


volatile char is_x_reset = 1;
int x_distance = 0;

volatile char is_z_reset = 1;
int z_distance = 0;

double current_x = 0;	//mm 단위
double current_z = 0;

void x_move(double x_dis, int DIR, int x_speed) {
	change_XZ = 0;
	//------DIR change------
	if (DIR == x_left) PORTC = X_LEFT;
	if (DIR == x_right) PORTC = X_RIGHT;
	//------Distance change------
	x_distance = x_dis;
	//------Speed------
	TIMSK1 = 0x00;
	OCR1A = x_speed;
	//------Enable Timer X------
	TIMSK1 = 0x02;
}

void z_move(double z_dis, int DIR, int z_speed) {
	change_XZ = 1;
	//------DIR change------
	if (DIR == z_up) PORTB = PORTB | Z_DIR;
	if (DIR == z_down) PORTB = PORTB & ~(Z_DIR);
	//------Distance change------
	z_distance = z_dis;
	//------Speed------
	TIMSK1 = 0x00;
	OCR1A = z_speed;
	//------Enable Timer Z------
	TIMSK1 = 0x02;
}

void setup() {
	//X
	DDRC |= X_DIR;	//RIGHT DIRECTION
	DDRD |= X_STEP | XYEENABLE;
	DDRC &= ~(X_STOP);

	//Z
	DDRB |= Z_DIR | Z_STEP;
	DDRC &= ~(Z_STOP);
	DDRA |= ZENABLE;

	//X Location
	TCCR1A = 0x00;
	TCCR1B = 0x0A;
	TCCR1C = 0x00;
	OCR1A = 400;
	TIMSK1 = 0x00;
}


void loop() {
	x_move(4000, x_right, 400);
	delay(2000);
	z_move(4000, z_up, 400);
	delay(2000);
	x_move(4000, x_left, 400);
	delay(2000);
	z_move(4000, z_down, 400);
	delay(2000);
}

char x_step_toggle = 0;
volatile int x_step_count = 0;

char z_step_toggle = 0;
volatile int z_step_count = 0;

ISR(TIMER1_COMPA_vect) {
	if (change_XZ == 0) {
		if (x_step_toggle == 0) {
			x_step_toggle = 1;
			PORTD |= X_STEP;
		}
		else {
			x_step_toggle = 0;
			PORTD &= ~(X_STEP);

			x_step_count++;
			char x_limit_switch = PINC & X_STOP;

			//5cm 이동
			/*
			if (x_step_count >= (5 * ONE_cm)) {	//X축 5cm 이동
				x_step_count = 0;
				TIMSK1 = 0x00;	//멈춤
			}
			//리밋 스위치를 누르면 동작 멈춤
			if (x_limit_switch) {	//X축 리밋 스위치를 눌렀을 때
				TIMSK1 = 0x00;

			}
			*/

			if (x_step_count >= x_distance) {
				is_x_reset = 0;
				x_step_count = 0;
				TIMSK1 = 0x00;
			}


			if (x_limit_switch && is_x_reset != -1) {
				TIMSK1 = 0x00;
				x_step_count = 0;
				if (is_x_reset == 1) {
					is_x_reset = -1;
					x_move(ONE_cm * 5, x_right, 400);
				}
			}

		}
	}

	else {
		if (z_step_toggle == 0) {
			z_step_toggle = 1;
			PORTB |= Z_STEP;
		}
		else {
			z_step_toggle = 0;
			PORTB &= ~(Z_STEP);

			z_step_count++;
			char z_limit_switch = PINC & Z_STOP;

			if (z_step_count >= z_distance) {
				is_z_reset = 0;
				z_step_count = 0;
				TIMSK1 = 0x00;
			}

			if (z_limit_switch && is_z_reset != -1) {
				TIMSK1 = 0x00;
				z_step_count = 0;
				if (is_z_reset == 1) {
					is_z_reset = -1;
					z_move(200, z_up, 400);
				}
			}
		}
	}
}