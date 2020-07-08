#define X_DIR 0x20		// PC5
#define X_STEP 0x80	    // PD7
#define X_STOP 0x04		// PC2
#define XYEENABLE 0x40  // PD6

#define	Y_DIR 0x80	//PC7
#define	Y_STEP 0x40	//PC6
#define	Y_STOP 0x08	//PC3

#define ONE_mm 80	//1mm
#define ONE_cm 800	//1cm

#define X_LEFT PORTC | X_DIR
#define X_RIGHT PORTC & ~(X_DIR)

#define Y_UP PORTC | Y_DIR
#define Y_DOWN PORTC & ~(Y_DIR)

enum {	//상수 리스트(왼쪽부터 0, 1, 2, 3 ... 으로 초기화)
	x_left, x_right, y_up, y_down
};

//각도를 라디안으로 만들어줌
double ANGLE(int x) {
	return PI * (x / 180.0);
}

//volatile : 컴파일러가 자동으로 최적하 하지 않도록 함.
volatile char is_x_reset = 1;
int x_distance = 0;

volatile char is_y_reset = 1;
int y_distance = 0;

int current_x = 0;	//mm 단위
int current_y = 0;	//mm 단위

//속도는 기본 400으로 한다.
void x_move(int x_dis, int DIR, int x_speed) {
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

void y_move(int y_dis, int DIR, int y_speed) {
	//------DIR change------
	if (DIR == y_up) PORTC = Y_UP;
	if (DIR == y_down) PORTC = Y_DOWN;
	//------Distance change------
	y_distance = y_dis;
	//------Speed------
	TIMSK3 = 0x00;
	OCR3A = y_speed;
	//------Enable Timer X------
	TIMSK3 = 0x02;
}

void reset() {
	is_x_reset = 1;
	is_y_reset = 1;
	x_move(32000, x_left, 400);
	y_move(32000, y_up, 400);

	while (is_x_reset != 0 || is_y_reset != 0);

	current_x = 0;
	current_y = 0;
}

void goXLocation(int x, int x_speed) {
	int dir = x_right;
	if (current_x > x) dir = x_left;

	x_move(abs(current_x - x) * ONE_mm, dir, x_speed);	//※abs : 절대값
	current_x = x;
}

void goYLocation(int y, int y_speed) {
	int dir = y_down;
	if (current_y > y) dir = y_up;

	y_move(abs(current_y - y) * ONE_mm, dir, y_speed);	//※abs : 절대값
	current_y = y;
}

void coordinate_shift(int x, int y) {
	int x_speed = 400;
	int y_speed = 400;

	int dis_X = 0;	// X거리
	int dis_Y = 0;	// Y거리

	int dir_X = x_right;
	int dir_Y = y_down;

	if (current_x > x) dir_X = x_left;
	if (current_y > y) dir_Y = y_up;

	dis_X = abs(current_x - x);
	dis_Y = abs(current_y - y);

	if (dis_X > dis_Y) {
		y_speed = (int)(400 * ((double)dis_X / (double)dis_Y));
	}
	else {
		x_speed = (int)(400 * ((double)dis_Y / (double)dis_X));
	}

	x_move(dis_X * ONE_mm, dir_X, x_speed);
	y_move(dis_Y * ONE_mm, dir_Y, y_speed);

	current_x = x;
	current_y = y;

	while (TIMSK1 != 0X00 || TIMSK3 != 0X00);
}

int set_speed(int x, int y) {
	int speed = 0;

	if (x > y) {
		speed = (int)(400.0 * ((double)y / (double)x));
	}
	else {
		speed = (int)(400.0 * ((double)x / (double)y));
	}

	return speed;
}

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

	reset();

	//육각형 시작점 (50, 10)을 기본 좌표로 설정
	coordinate_shift(50, 10);

}


void loop() {

	//육각형 좌표 이동(한변의 길이 : 30mm)

	coordinate_shift(80, 10);
	coordinate_shift(80 + (30 * cos(ANGLE(60))), 10 + (30 * sin(ANGLE(60))));
	coordinate_shift(80, (10 + 2 * (30 * sin(ANGLE(60)))));
	coordinate_shift(50, (10 + 2 * (30 * sin(ANGLE(60)))));
	coordinate_shift(50 - (30 * cos(ANGLE(60))), (10 + (30 * sin(ANGLE(60)))));
	coordinate_shift(50, 10);
}

char x_step_toggle = 0;
volatile int x_step_count = 0;

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

char y_step_toggle = 0;
volatile int y_step_count = 0;

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

		if (y_step_count >= y_distance) {
			is_y_reset = 0;
			y_step_count = 0;
			TIMSK3 = 0x00;
		}

		if (y_limit_switch && is_y_reset != -1) {
			TIMSK3 = 0x00;
			y_step_count = 0;
			if (is_y_reset == 1) {
				is_y_reset = -1;
				y_move(ONE_cm * 5, y_down, 400);
			}
		}
	}
}