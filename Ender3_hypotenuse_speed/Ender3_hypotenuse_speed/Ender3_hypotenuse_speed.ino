﻿#define X_DIR 0x20		// PC5
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

double current_x = 0;	//mm 단위
double current_y = 0;	//mm 단위

//속도는 기본 400으로 한다.
void x_move(double x_dis, int DIR, int x_speed) {
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

void y_move(double y_dis, int DIR, int y_speed) {
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

void goXLocation(double x, int x_speed) {
	int dir = x_right;
	if (current_x > x) dir = x_left;

	x_move(abs(current_x - x) * ONE_mm, dir, x_speed);	//※abs : 절대값
	current_x = x;
}

void goYLocation(double y, int y_speed) {
	int dir = y_down;
	if (current_y > y) dir = y_up;

	y_move(abs(current_y - y) * ONE_mm, dir, y_speed);	//※abs : 절대값
	current_y = y;
}

void coordinate_shift(double x, double y) {
	//기본 속도 (400)
	int x_speed = 400;
	int y_speed = 400;

	double dis_X = 0;	// X거리
	double dis_Y = 0;	// Y거리

	int dir_X = x_right;  // X 방향
	int dir_Y = y_down;   // Y 방향

	if (current_x > x) dir_X = x_left;
	if (current_y > y) dir_Y = y_up;

	dis_X = abs(current_x - x);
	dis_Y = abs(current_y - y);

	if (dis_X > dis_Y) {
		y_speed = (int)(400 * (dis_X / dis_Y));
	}
	else {
		x_speed = (int)(400 * (dis_Y / dis_X));
	}

	x_move(dis_X * ONE_mm, dir_X, x_speed);
	y_move(dis_Y * ONE_mm, dir_Y, y_speed);

	current_x = x;
	current_y = y;

	while (TIMSK1 != 0X00 || TIMSK3 != 0X00);
}

//빗변을 그릴때, 속도 계산
int set_speed(double x, double y) {
	int speed = 0;

	if (x > y) {
		speed = (int)(400 * (y / x));
	}
	else {
		speed = (int)(400 * (x / y));
	}

	return speed;
}

//대각선 이동할때, X축이 움직일 거리
double X_move_distance(double dis, int dir, int angle) {	//대각선의 이동거리와 삼각비의 기준이 될 각도를 매개변수로 받음
	double x_move_dis = 0;

	if (dir == x_right) x_move_dis = current_x + (dis * cos(ANGLE(angle)));
	if (dir == x_left) x_move_dis = current_x - (dis * cos(ANGLE(angle)));

	return x_move_dis;
}

// 대각선 이동할때, Y축이 움직일 거리
double Y_move_distance(int dis, int dir, int angle) {	//대각선의 이동거리와 삼각비의 기준이 될 각도를 매개변수로 받음
	double y_move_dis = 0;

	if (dir == y_down) y_move_dis = current_y + (dis * sin(ANGLE(angle)));
	if (dir == y_up) y_move_dis = current_y - (dis * sin(ANGLE(angle)));

	return y_move_dis;
}

//치약짜개 배열
const float xy_pos[480][2] = {
66.268 , 82.377,
66.527 , 82.217,
66.796 , 82.073,
67.072 , 81.946,
67.356 , 81.836,
67.647 , 81.743,
67.942 , 81.669,
68.241 , 81.613,
68.544 , 81.576,
68.848 , 81.557,
131.152, 81.557,
131.456, 81.576,
131.759, 81.613,
132.058, 81.669,
132.353, 81.743,
132.644, 81.836,
132.928, 81.946,
133.204, 82.073,
133.473, 82.217,
133.732, 82.377,
133.980, 82.553,
134.218, 82.744,
134.443, 82.950,
134.655, 83.168,
134.853, 83.400,
135.036, 83.643,
135.205, 83.897,
135.357, 84.160,
135.493, 84.433,
135.611, 84.714,
135.713, 85.001,
135.796, 85.294,
135.861, 85.591,
135.908, 85.892,
135.936, 86.196,
135.945, 86.500,
135.936, 86.804,
135.908, 87.108,
135.861, 87.409,
135.796, 87.706,
135.713, 87.999,
135.611, 88.286,
135.493, 88.567,
135.357, 88.840,
135.205, 89.103,
135.036, 89.357,
134.853, 89.600,
134.664, 89.821,
134.225, 90.251,
103.227, 117.248,
102.980, 117.446,
102.732, 117.622,
102.473, 117.783,
102.204, 117.927,
101.928, 118.054,
101.644, 118.164,
101.353, 118.257,
101.058, 118.331,
100.759, 118.387,
100.456, 118.424,
100.152, 118.443,
99.848 , 118.443,
99.544 , 118.424,
99.241 , 118.387,
98.942 , 118.331,
98.647 , 118.257,
98.356 , 118.164,
98.072 , 118.054,
97.796 , 117.927,
97.527 , 117.783,
97.268 , 117.622,
97.021 , 117.447,
96.737 , 117.217,
65.775 , 90.251,
65.336 , 89.821,
65.147 , 89.600,
64.964 , 89.357,
64.795 , 89.103,
64.643 , 88.840,
64.507 , 88.567,
64.389 , 88.286,
64.287 , 87.999,
64.204 , 87.706,
64.139 , 87.409,
64.092 , 87.108,
64.064 , 86.804,
64.055 , 86.500,
64.064 , 86.196,
64.092 , 85.892,
64.139 , 85.591,
64.204 , 85.294,
64.287 , 85.001,
64.389 , 84.714,
64.507 , 84.433,
64.643 , 84.160,
64.795 , 83.897,
64.964 , 83.643,
65.147 , 83.400,
65.345 , 83.168,
65.557 , 82.950,
65.782 , 82.745,
66.020 , 82.553,
65.586 , 81.979,
65.870 , 81.777,
66.167 , 81.593,
66.475 , 81.428,
66.792 , 81.282,
67.117 , 81.156,
67.449 , 81.051,
67.788 , 80.965,
68.131 , 80.901,
68.477 , 80.858,
68.826 , 80.837,
131.174, 80.837,
131.523, 80.858,
131.869, 80.901,
132.212, 80.965,
132.551, 81.051,
132.883, 81.156,
133.208, 81.282,
133.525, 81.428,
133.833, 81.593,
134.130, 81.777,
134.414, 81.979,
134.686, 82.197,
134.944, 82.433,
135.187, 82.683,
135.414, 82.948,
135.624, 83.227,
135.817, 83.517,
135.992, 83.820,
136.147, 84.132,
136.283, 84.453,
136.399, 84.782,
136.495, 85.118,
136.569, 85.459,
136.623, 85.804,
136.655, 86.151,
136.666, 86.500,
136.655, 86.849,
136.623, 87.196,
136.569, 87.541,
136.495, 87.882,
136.399, 88.218,
136.283, 88.547,
136.147, 88.868,
135.992, 89.180,
135.817, 89.483,
135.624, 89.773,
135.414, 90.052,
135.190, 90.313,
134.714, 90.780,
103.689, 117.800,
103.414, 118.021,
103.130, 118.223,
102.833, 118.407,
102.525, 118.572,
102.208, 118.718,
101.883, 118.844,
101.551, 118.949,
101.212, 119.035,
100.869, 119.099,
100.523, 119.142,
100.174, 119.163,
99.826 , 119.163,
99.477 , 119.142,
99.131 , 119.099,
98.788 , 119.035,
98.449 , 118.949,
98.117 , 118.844,
97.792 , 118.718,
97.475 , 118.572,
97.167 , 118.407,
96.870 , 118.223,
96.586 , 118.022,
96.274 , 117.769,
65.286 , 90.780,
64.810 , 90.313,
64.586 , 90.052,
64.376 , 89.773,
64.183 , 89.483,
64.008 , 89.180,
63.853 , 88.868,
63.717 , 88.547,
63.601 , 88.218,
63.505 , 87.882,
63.431 , 87.541,
63.377 , 87.196,
63.345 , 86.849,
63.334 , 86.500,
63.345 , 86.151,
63.377 , 85.804,
63.431 , 85.459,
63.505 , 85.118,
63.601 , 84.782,
63.717 , 84.453,
63.853 , 84.132,
64.008 , 83.820,
64.183 , 83.517,
64.376 , 83.227,
64.586 , 82.948,
64.813 , 82.683,
65.056 , 82.433,
65.314 , 82.197,
65.586 , 81.979,
68.037 , 85.535,
67.847 , 85.916,
67.731 , 86.326,
67.691 , 86.750,
67.731 , 87.174,
67.847 , 87.584,
68.037 , 87.965,
68.304 , 88.319,
68.710 , 88.667,
69.100 , 88.876,
69.506 , 89.005,
69.896 , 89.054,
130.104, 89.054,
130.494, 89.005,
130.900, 88.876,
131.275, 88.675,
131.607, 88.407,
131.884, 88.084,
132.097, 87.715,
132.239, 87.313,
132.304, 86.892,
132.291, 86.466,
132.196, 86.034,
131.973, 85.548,
131.706, 85.195,
131.391, 84.908,
131.029, 84.683,
130.632, 84.529,
130.246, 84.457,
130.071, 84.446,
69.896 , 84.446,
69.506 , 84.495,
69.100 , 84.624,
68.710 , 84.833,
68.304 , 85.181,
68.652 , 85.915,
68.521 , 86.177,
68.441 , 86.459,
68.414 , 86.750,
68.441 , 87.041,
68.521 , 87.323,
68.652 , 87.585,
68.832 , 87.823,
69.119 , 88.069,
69.382 , 88.210,
69.661 , 88.299,
69.940 , 88.334,
130.060, 88.334,
130.339, 88.299,
130.618, 88.210,
130.876, 88.072,
131.104, 87.888,
131.294, 87.666,
131.440, 87.413,
131.538, 87.137,
131.583, 86.848,
131.574, 86.555,
131.510, 86.264,
131.352, 85.920,
131.172, 85.682,
130.956, 85.485,
130.707, 85.331,
130.434, 85.225,
130.157, 85.173,
130.049, 85.166,
69.940 , 85.166,
69.661 , 85.201,
69.382 , 85.290,
69.119 , 85.431,
68.832 , 85.677,
92.904 , 101.189,
92.683 , 101.633,
92.489 , 102.090,
92.324 , 102.558,
92.189 , 103.035,
92.083 , 103.520,
92.006 , 104.010,
91.961 , 104.504,
91.945 , 105.000,
91.961 , 105.496,
92.006 , 105.990,
92.083 , 106.480,
92.189 , 106.965,
92.324 , 107.442,
92.489 , 107.910,
92.683 , 108.367,
92.904 , 108.811,
93.152 , 109.240,
93.426 , 109.654,
93.725 , 110.050,
94.048 , 110.426,
94.393 , 110.783,
94.760 , 111.117,
95.146 , 111.428,
95.551 , 111.714,
95.973 , 111.975,
96.410 , 112.210,
96.860 , 112.418,
97.323 , 112.597,
97.796 , 112.747,
98.277 , 112.868,
98.765 , 112.959,
99.257 , 113.020,
99.752 , 113.051,
100.248, 113.051,
100.743, 113.020,
101.235, 112.959,
101.723, 112.868,
102.204, 112.747,
102.677, 112.597,
103.140, 112.418,
103.590, 112.210,
104.027, 111.975,
104.449, 111.714,
104.854, 111.428,
105.240, 111.117,
105.607, 110.783,
105.952, 110.426,
106.275, 110.050,
106.574, 109.654,
106.848, 109.240,
107.096, 108.811,
107.317, 108.367,
107.511, 107.910,
107.676, 107.442,
107.811, 106.965,
107.917, 106.480,
107.994, 105.990,
108.039, 105.496,
108.055, 105.000,
108.039, 104.504,
107.994, 104.010,
107.917, 103.520,
107.811, 103.035,
107.676, 102.558,
107.511, 102.090,
107.317, 101.633,
107.096, 101.189,
106.848, 100.760,
106.574, 100.346,
106.275, 99.950,
105.952, 99.574,
105.607, 99.217,
105.240, 98.883,
104.854, 98.572,
104.449, 98.286,
104.027, 98.025,
103.590, 97.790,
103.140, 97.582,
102.677, 97.403,
102.204, 97.253,
101.723, 97.132,
101.235, 97.041,
100.743, 96.980,
100.248, 96.949,
99.752 , 96.949,
99.257 , 96.980,
98.765 , 97.041,
98.277 , 97.132,
97.796 , 97.253,
97.323 , 97.403,
96.860 , 97.582,
96.410 , 97.790,
95.973 , 98.025,
95.551 , 98.286,
95.146 , 98.572,
94.760 , 98.883,
94.393 , 99.217,
94.048 , 99.574,
93.725 , 99.950,
93.426 , 100.346,
93.152 , 100.760,
93.538 , 101.530,
93.337 , 101.935,
93.161 , 102.351,
93.011 , 102.777,
92.887 , 103.211,
92.791 , 103.652,
92.721 , 104.099,
92.680 , 104.548,
92.666 , 105.000,
92.680 , 105.451,
92.721 , 105.901,
92.791 , 106.348,
92.887 , 106.789,
93.011 , 107.223,
93.161 , 107.649,
93.337 , 108.065,
93.538 , 108.470,
93.764 , 108.861,
94.014 , 109.238,
94.286 , 109.598,
94.580 , 109.941,
94.894 , 110.265,
95.228 , 110.570,
95.580 , 110.853,
95.949 , 111.114,
96.333 , 111.352,
96.731 , 111.565,
97.141 , 111.754,
97.562 , 111.917,
97.993 , 112.054,
98.431 , 112.164,
98.875 , 112.247,
99.323 , 112.303,
99.774 , 112.331,
100.226, 112.331,
100.677, 112.303,
101.125, 112.247,
101.569, 112.164,
102.007, 112.054,
102.438, 111.917,
102.859, 111.754,
103.269, 111.565,
103.667, 111.352,
104.051, 111.114,
104.420, 110.853,
104.772, 110.570,
105.106, 110.265,
105.420, 109.941,
105.714, 109.598,
105.986, 109.238,
106.236, 108.861,
106.462, 108.470,
106.663, 108.065,
106.839, 107.649,
106.989, 107.223,
107.113, 106.789,
107.209, 106.348,
107.279, 105.901,
107.320, 105.451,
107.334, 105.000,
107.320, 104.548,
107.279, 104.099,
107.209, 103.652,
107.113, 103.211,
106.989, 102.777,
106.839, 102.351,
106.663, 101.935,
106.462, 101.530,
106.236, 101.139,
105.986, 100.762,
105.714, 100.402,
105.420, 100.059,
105.106, 99.735,
104.772, 99.430,
104.420, 99.147,
104.051, 98.886,
103.667, 98.648,
103.269, 98.435,
102.859, 98.246,
102.438, 98.083,
102.007, 97.946,
101.569, 97.836,
101.125, 97.752,
100.677, 97.697,
100.226, 97.669,
99.774 , 97.669,
99.323 , 97.697,
98.875 , 97.752,
98.431 , 97.836,
97.993 , 97.946,
97.562 , 98.083,
97.141 , 98.246,
96.731 , 98.435,
96.333 , 98.648,
95.949 , 98.886,
95.580 , 99.147,
95.228 , 99.430,
94.894 , 99.735,
94.580 , 100.059,
94.286 , 100.402,
94.014 , 100.762,
93.764 , 101.139,
93.764 , 101.139
};


//대각선의 이동 속도를 똑같이 만들기
int X_same_speed(int angle, int speed) {
	int X_hypotenuse_speed;
	X_hypotenuse_speed = speed * (1 / cos(ANGLE(angle)));
	return X_hypotenuse_speed;
}

int Y_same_speed(int angle, int speed) {
	int Y_hypotenuse_speed;
	Y_hypotenuse_speed = speed * (1 / sin(ANGLE(angle)));
	return Y_hypotenuse_speed;
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

	//reset();

}

//char x_step_dir = 0;
//char y_step_dir = 0;

void loop() {

	//대각선의 이동속도도 400으로 구현
	/*
	x_move(50 * cos(ANGLE(30)) * ONE_mm, x_right, 400);
	while (TIMSK1 != 0x00);
	y_move(50 * sin(ANGLE(30)) * ONE_mm, y_down, 400);
	while (TIMSK3 != 0x00);
	x_move(50 * cos(ANGLE(30)) * ONE_mm, x_left, X_same_speed(30, 400));
	y_move(50 * sin(ANGLE(30)) * ONE_mm, y_up, Y_same_speed(30, 400));
	while (TIMSK1 != 0x00 || TIMSK3 != 0x00);
	*/

	//5cm 정삼각형 (대각선의 이동속도를 똑같게 함 - 속도 확인)
	x_move(5 * ONE_cm, x_right, 800);
	while (TIMSK1 != 0x00);
	x_move(50 * cos(ANGLE(60)) * ONE_mm, x_left, X_same_speed(60, 800));
	y_move(50 * sin(ANGLE(60)) * ONE_mm, y_down, Y_same_speed(60, 800));
	while (TIMSK1 != 0x00 || TIMSK3 != 0x00);
	x_move(50 * cos(ANGLE(60)) * ONE_mm, x_left, X_same_speed(60, 800));
	y_move(50 * sin(ANGLE(60)) * ONE_mm, y_up, Y_same_speed(60, 800));
	while (TIMSK1 != 0x00 || TIMSK3 != 0x00);
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