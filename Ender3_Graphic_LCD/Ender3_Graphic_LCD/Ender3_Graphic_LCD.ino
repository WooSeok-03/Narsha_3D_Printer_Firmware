//Graphic_LCD_12864
#define LCD_CS  0x08	//PA3
#define LCD_SCK	0x02	//PA1
#define LCD_MOSI 0x02	//PC1

void setup() {
	//LCD
	DDRA |= LCD_CS | LCD_SCK;
	DDRC |= LCD_MOSI;
	PORTA &= ~(LCD_CS | LCD_SCK);
	PORTC &= ~(LCD_MOSI);

	//LCD - Setup
	//-------------------------------------------------------
	LCD_set_inst(0x30);
	LCD_set_inst(0x30);
	LCD_set_inst(0x30);

	LCD_set_inst(0x30);
	LCD_set_inst(0x06);
	LCD_set_inst(0x0C);

	LCD_set_data(0x01);
	delay(10);
	LCD_cls();
	delay(1000);
	//-------------------------------------------------------
}

unsigned char numbers[10][8] = {
  {0x00, 0x38, 0x44, 0x4C, 0x54, 0x64, 0x44, 0x38},		//0
  {0x00, 0x10, 0x30, 0x50, 0x10, 0x10, 0x10, 0x7c},		//1
  {0x00, 0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x7c},		//2
  {0x00, 0x38, 0x44, 0x04, 0x18, 0x04, 0x44, 0x38},		//3
  {0x00, 0x08, 0x18, 0x28, 0x48, 0x7C, 0x08, 0x08},		//4
  {0x00, 0x7C, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38},		//5
  {0x00, 0x38, 0x40, 0x40, 0x78, 0x44, 0x44, 0x38},		//6
  {0x00, 0x7C, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20},		//7
  {0x00, 0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38},		//8
  {0x00, 0x38, 0x44, 0x44, 0x3C, 0x04, 0x44, 0x38},		//9
};

int ten_num = 0;

void loop() {
	//Test
	//set_data(3, 7, 0x5555);

	//두 자리 숫자 count
	for (int j = 0; j <= ten_num; j++) {
		for (int i = 0; i < 8; i++) {
			set_data(0, 8 + i, numbers[j][i]);
		}
	}

	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 8; i++) {
			set_data(1, 8 + i, numbers[j][i]);
		}
		delay(500);
	}
	ten_num++;
	if (ten_num >= 10) ten_num = 0;
}

void LCD_cls() {
	// i == X 
	// j == Y
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 32; j++) {
			set_data(i, j, 0x0000);
		}
	}
}

void set_data(char x, char y, short data) {
	char address_x = 0x80 | (x & 0x0F);
	char address_y = 0x80 | (y & 0x7F);
	char data_high = (data & 0xFF00) >> 8;
	char data_low = data & 0x00FF;

	LCD_set_inst(0x36);
	LCD_set_inst(address_y);
	LCD_set_inst(address_x);

	LCD_set_inst(0x30);
	LCD_set_data(data_high);
	LCD_set_data(data_low);
}

void LCD_set_inst(char inst) {
	PORTA |= LCD_CS; //CS HIGH
	//--------------------------
	shift_out(0xF8);
	shift_out(inst & 0xF0);
	shift_out((inst & 0x0F) << 4);
	//--------------------------
	PORTA &= ~(LCD_CS);

	delayMicroseconds(50);
}

void LCD_set_data(char data) {
	PORTA |= LCD_CS; //CS HIGH
	//--------------------------
	shift_out(0xFA);
	shift_out(data & 0xF0);
	shift_out((data & 0x0F) << 4);
	//--------------------------
	PORTA &= ~(LCD_CS);

	delayMicroseconds(50);
}

//MSB Frist
void shift_out(char data) {
	for (int i = 0; i < 8; i++) {
		if (data & (0x80 >> i)) {
			PORTC |= LCD_MOSI;
		}
		else {
			PORTC &= ~(LCD_MOSI);
		}
		PORTA |= LCD_SCK;
		PORTA &= ~(LCD_SCK);
	}
}