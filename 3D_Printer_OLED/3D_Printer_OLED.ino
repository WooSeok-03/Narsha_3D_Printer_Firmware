#define OLED_CS   6
#define OLED_RST  4
#define OLED_DC   5

#define OLED_SCK  52
#define OLED_DATA 51

void setup() {
  pinMode(53, OUTPUT); // Mega
  
  pinMode(OLED_RST, OUTPUT);
  pinMode(OLED_CS, OUTPUT);
  pinMode(OLED_DC, OUTPUT);
  pinMode(OLED_SCK, OUTPUT);
  pinMode(OLED_DATA, OUTPUT);

  digitalWrite(OLED_SCK, LOW);

  spi_init();
  oled_init();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void spi_init()
{
  SPCR = 0x50;
  SPSR = 0x01;
}

void spi_write(char data)
{
  SPDR = data;
  while(!(SPSR & 0x80)); 
}

void Write_Command(char command)
{
  digitalWrite(OLED_CS, LOW);   // CS LOW
  digitalWrite(OLED_DC, LOW);   // DC LOW

  //shift_out(command);
  spi_write(command);
  
  digitalWrite(OLED_CS, HIGH);  // CS HIGH
  digitalWrite(OLED_DC, HIGH);  // DC HIGH
}


void Write_Data(char data)
{
  digitalWrite(OLED_CS, LOW);   // CS LOW
  //shift_out(data);
  spi_write(data);
  digitalWrite(OLED_CS, HIGH);  // CS HIGH
}

void oled_init()
{
  digitalWrite(OLED_CS, LOW);
  digitalWrite(OLED_RST, LOW);
  delay(100);
  digitalWrite(OLED_RST, HIGH);
  delay(100);

  Write_Command(0xfd);  // command lock
  Write_Data(0x12);
  Write_Command(0xfd);  // command lock
  Write_Data(0xB1);

  Write_Command(0xae);  // display off
  
  Write_Command(0xB3);
  Write_Data(0xF1);

  Write_Command(0xa0);  //set re-map & data format
  Write_Data(0x74); //Horizontal address increment
  
  Write_Command(0xCA);  
  Write_Data(0x7F);

  Write_Command(0xa2);  //set display offset
  Write_Data(0x00);

  Write_Command(0xB5);  
  Write_Data(0x00);

  Write_Command(0xAB);
  Write_Data(0x01);

  Write_Command(0xB1);
  Write_Data(0x32);

  Write_Command(0xBE);
  Write_Data(0x05);

  Write_Command(0xA6);

  Write_Command(0xC1);
  Write_Data(0xC8);
  Write_Data(0x80);
  Write_Data(0xC8);  

  Write_Command(0xC7);
  Write_Data(0x0F);

  Write_Command(0xB4);
  Write_Data(0xA0);
  Write_Data(0xB5);
  Write_Data(0x55);  
  
  Write_Command(0xB6);
  Write_Data(0x01);

  Write_Command(0xAF);  
}
