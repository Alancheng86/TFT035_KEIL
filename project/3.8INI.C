
{//SHARP 3.8 d Initial_38  

SSD1963GPIOOUTCOM(0xB9);
SSD1963GPIOOUTDATA(0xFF);
SSD1963GPIOOUTDATA(0x83);
SSD1963GPIOOUTDATA(0x63);
//SPI_3W_SET_POWER
SSD1963GPIOOUTCOM(0xB1);
SSD1963GPIOOUTDATA(0x81);
SSD1963GPIOOUTDATA(0x30);
SSD1963GPIOOUTDATA(0x03);
SSD1963GPIOOUTDATA(0x34);
SSD1963GPIOOUTDATA(0x02);
SSD1963GPIOOUTDATA(0x13);
SSD1963GPIOOUTDATA(0x11);
SSD1963GPIOOUTDATA(0x00);
SSD1963GPIOOUTDATA(0x35);
SSD1963GPIOOUTDATA(0x3E);
SSD1963GPIOOUTDATA(0x16);
SSD1963GPIOOUTDATA(0x16);
//Sleep Out
SSD1963GPIOOUTCOM(0x11);
DelayX1ms(150);
//set RGB ORDER
SSD1963GPIOOUTCOM(0x36);
SSD1963GPIOOUTDATA(0x0B);
//Set COLMOD
SSD1963GPIOOUTCOM(0x3A);
SSD1963GPIOOUTDATA(0x50);
//SPI_3W_SET_RGBIF
SSD1963GPIOOUTCOM(0xB3);
SSD1963GPIOOUTDATA(0x01);
//SPI_3W_SET_CYC
SSD1963GPIOOUTCOM(0xB4);
SSD1963GPIOOUTDATA(0x00);
SSD1963GPIOOUTDATA(0x03);
SSD1963GPIOOUTDATA(0x7E);
SSD1963GPIOOUTDATA(0x02);
SSD1963GPIOOUTDATA(0x01);
SSD1963GPIOOUTDATA(0x12);
SSD1963GPIOOUTDATA(0x64);
SSD1963GPIOOUTDATA(0x01);
SSD1963GPIOOUTDATA(0x60);
//SPI_3W_SET_VCOM
SSD1963GPIOOUTCOM(0xB6);
SSD1963GPIOOUTDATA(0x38);
//SPI_3W_SET_PANEL
SSD1963GPIOOUTCOM(0xCC);
SSD1963GPIOOUTDATA(0x07);
DelayX1ms(5);
//Set Gamma 2.2
SSD1963GPIOOUTCOM(0xE0);
SSD1963GPIOOUTDATA(0x00);
SSD1963GPIOOUTDATA(0x15);
SSD1963GPIOOUTDATA(0x1C);
SSD1963GPIOOUTDATA(0x32);
SSD1963GPIOOUTDATA(0x36);
SSD1963GPIOOUTDATA(0x3F);
SSD1963GPIOOUTDATA(0x08);
SSD1963GPIOOUTDATA(0xCE);
SSD1963GPIOOUTDATA(0x0F);
SSD1963GPIOOUTDATA(0x56);
SSD1963GPIOOUTDATA(0x58);
SSD1963GPIOOUTDATA(0x16);
SSD1963GPIOOUTDATA(0x18);
SSD1963GPIOOUTDATA(0x8F);
SSD1963GPIOOUTDATA(0x19);
SSD1963GPIOOUTDATA(0x00);
SSD1963GPIOOUTDATA(0x15);
SSD1963GPIOOUTDATA(0x1C);
SSD1963GPIOOUTDATA(0x32);
SSD1963GPIOOUTDATA(0x36);
SSD1963GPIOOUTDATA(0x3F);
SSD1963GPIOOUTDATA(0x08);
SSD1963GPIOOUTDATA(0xCE);
SSD1963GPIOOUTDATA(0x0F);
SSD1963GPIOOUTDATA(0x56);
SSD1963GPIOOUTDATA(0x58);
SSD1963GPIOOUTDATA(0x16);
SSD1963GPIOOUTDATA(0x18);
SSD1963GPIOOUTDATA(0x8F);
SSD1963GPIOOUTDATA(0x19);
DelayX1ms(5);
//Display On
SSD1963GPIOOUTCOM(0x29);
}