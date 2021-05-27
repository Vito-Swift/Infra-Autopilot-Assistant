#include "CC2520_HW.h"

// schematic PDF and PCB design files will be available on github
const int CC2520_RESET_PIN = 25;
const int CC2520_IO0_PIN[2] = {27,13};
const int CC2520_IO1_PIN[2] = {24,12};
const int CC2520_IO2_PIN[2] = {17,6};
const int CC2520_IO3_PIN[2] = {23,16};
const int CC2520_IO4_PIN[2] = {4,5};
const int CC2520_IO5_PIN[2] = {22,19};
int CC2520_spi_fd[2];


void RPI3_CC2520_HW_Init(void){  // Init GPIO, SPI and reset
	int i;
	
	wiringPiSetupGpio();
    pinMode(CC2520_RESET_PIN, OUTPUT);
	for(i=0; i<2; i++){
		pinMode(CC2520_IO0_PIN[i], INPUT);
		pinMode(CC2520_IO1_PIN[i], INPUT);
		pinMode(CC2520_IO2_PIN[i], INPUT);
		pinMode(CC2520_IO3_PIN[i], INPUT);
		pinMode(CC2520_IO4_PIN[i], INPUT);
		pinMode(CC2520_IO5_PIN[i], INPUT);
		
		CC2520_spi_fd[i] = wiringPiSPISetup(i, 8000000);  // CC2520 Max 8MHz, RPi3 Max 125MHz
	}
	
	digitalWrite(CC2520_RESET_PIN, LOW);  // reset
	delay(10);
	digitalWrite(CC2520_RESET_PIN, HIGH);
	delay(10);
}

void RPI3_CC2520_HW_ReInit(void){
	digitalWrite(CC2520_RESET_PIN, LOW);  // reset
	delay(10);
	digitalWrite(CC2520_RESET_PIN, HIGH);
	delay(10);
}

// For the usage of GPIO, please refer to datasheet P35-39
int RPI3_CC2520_ReadGPIO(int dev_id, int io_id){  // dev_id: 0-1, io_id: 0-5
	int rpi3_gpio_id;
	
	switch (io_id){
		case 0: rpi3_gpio_id = CC2520_IO0_PIN[dev_id]; break;
		case 1: rpi3_gpio_id = CC2520_IO1_PIN[dev_id]; break;
		case 2: rpi3_gpio_id = CC2520_IO2_PIN[dev_id]; break;
		case 3: rpi3_gpio_id = CC2520_IO3_PIN[dev_id]; break;
		case 4: rpi3_gpio_id = CC2520_IO4_PIN[dev_id]; break;
		case 5: rpi3_gpio_id = CC2520_IO5_PIN[dev_id]; break;
	}

	return digitalRead(rpi3_gpio_id);
}

void RPI3_CC2520_SetGPIODir(int dev_id, int io_id, int dir){  // dev_id: 0-1, io_id: 0-5, dir: 0=OUTPUT, 1=INPUT
	int rpi3_gpio_id;
	
	switch (io_id){
		case 0: rpi3_gpio_id = CC2520_IO0_PIN[dev_id]; break;
		case 1: rpi3_gpio_id = CC2520_IO1_PIN[dev_id]; break;
		case 2: rpi3_gpio_id = CC2520_IO2_PIN[dev_id]; break;
		case 3: rpi3_gpio_id = CC2520_IO3_PIN[dev_id]; break;
		case 4: rpi3_gpio_id = CC2520_IO4_PIN[dev_id]; break;
		case 5: rpi3_gpio_id = CC2520_IO5_PIN[dev_id]; break;
	}
	
	if(dir == 0)
		pinMode(rpi3_gpio_id, OUTPUT);
	else
		pinMode(rpi3_gpio_id, INPUT);
}

// The RPI3 IO must be Set as OUTPUT before using it. Currently All RPI3 GPIO are INPUT
void RPI3_CC2520_WriteGPIO(int dev_id, int io_id, int value){
	int rpi3_gpio_id;
	
	switch (io_id){
		case 0: rpi3_gpio_id = CC2520_IO0_PIN[dev_id]; break;
		case 1: rpi3_gpio_id = CC2520_IO1_PIN[dev_id]; break;
		case 2: rpi3_gpio_id = CC2520_IO2_PIN[dev_id]; break;
		case 3: rpi3_gpio_id = CC2520_IO3_PIN[dev_id]; break;
		case 4: rpi3_gpio_id = CC2520_IO4_PIN[dev_id]; break;
		case 5: rpi3_gpio_id = CC2520_IO5_PIN[dev_id]; break;
	}
	
	if(value == 0){
		digitalWrite(rpi3_gpio_id, LOW);
	}
	else{
		digitalWrite(rpi3_gpio_id, HIGH);
	}
}
 
unsigned char CC2520_ReadReg(int dev_id, unsigned char addr){  // dev_id: 0-1
	unsigned char send_array[2];
	
	send_array[0] = addr | CC2520_INS_REGRD;
	//send_array[1] = don't care;
	
	wiringPiSPIDataRW(dev_id, send_array, 2);
	
	return send_array[1];
}

void CC2520_WriteReg(int dev_id, unsigned char addr, unsigned char value){
	unsigned char send_array[2];
	
	send_array[0] = addr | CC2520_INS_REGWR;
	send_array[1] = value;
	
	write(CC2520_spi_fd[dev_id], send_array, 2);
}

unsigned char CC2520_ReadRAM(int dev_id, unsigned short addr){
	unsigned char send_array[3];
	unsigned char addrH, addrL;
	
	addrH = (addr >> 8) & 0xFF;
	addrL = addr & 0xFF;
	send_array[0] = addrH | CC2520_INS_MEMRD;
	send_array[1] = addrL;
	//send_array[2] = don't care;
	
	wiringPiSPIDataRW(dev_id, send_array, 3);
	
	return send_array[2];
}

void CC2520_WriteRAM(int dev_id, unsigned short addr, unsigned char value){
	unsigned char send_array[3];
	unsigned char addrH, addrL;
	
	addrH = (addr >> 8) & 0xFF;
	addrL = addr & 0xFF;
	send_array[0] = addrH | CC2520_INS_MEMWR;
	send_array[1] = addrL;
	send_array[2] = value;
	
	write(CC2520_spi_fd[dev_id], send_array, 3);
}

unsigned char CC2520_ReadRXFIFO(int dev_id, unsigned char *buf){  // declare: unsigned char buf[129], do nothing and call function. buf[0] is not relavent. buf[1] to buf[return_value] are FIFO content.
	unsigned char count;
	
	count = CC2520_ReadReg(dev_id, CC2520_RXFIFOCNT);  // datasheet P121
	buf[0] = CC2520_INS_RXBUF;
	wiringPiSPIDataRW(dev_id, buf, count+1);
	
	return count;
}

// the count is simply how many bytes in the buf. It is not related to Frame Length field in PHY header
void CC2520_WriteTXFIFO(int dev_id, unsigned char *buf, unsigned char count){  // declare: unsigned char buf[129]. Don't touch buf[0]. Fill your data(max 128byte) from buf[1] to (max)buf[128]. Range of count is 1-128
	buf[0] = CC2520_INS_TXBUF;
	write(CC2520_spi_fd[dev_id], buf, count+1);
}

/*
 * This function is only suitable for 1 byte instruction including:
 * SNOP
 * SIBUFEX
 * SSAMPLECCA
 * SXOSCON
 * STXCAL
 * SRXON
 * STXON
 * STXONCCA
 * SRFOFF
 * SXOSCOFF
 * SFLUSHRX
 * SFLUSHTX
 * SACK
 * SACKPEND
 * SNACK
 * SRXMASKBITSET
 * SRXMASKBITCLR
 * For their usage, please refer to datasheet p43-51
 * For other N bytes instruction, please call other function in this file, or implement one if it is not here
 */
void CC2520_Instruction(int dev_id, unsigned char ins){
	write(CC2520_spi_fd[dev_id], &ins, 1);
}

unsigned char CC2520_Status(int dev_id){  // refer to datasheet p53
	unsigned char data;
	
	data = CC2520_INS_SNOP;
	wiringPiSPIDataRW(dev_id, &data, 1);  // write NOP instruction and read status byte;
	
	return data;  // now data is status byte
}

// For the following instruction function, please refer to datasheet p43-51
void CC2520_SRES(int dev_id){  // reset device except SPI interface
	unsigned char data[2];
	
	data[0] = CC2520_INS_SRES;
	//data[1] = don't care
	
	write(CC2520_spi_fd[dev_id], data, 2);
}

void CC2520_IBUFLD(int dev_id, unsigned char ins){
	unsigned char data[2];
	
	data[0] = CC2520_INS_IBUFLD;
	data[1] = ins;
	
	write(CC2520_spi_fd[dev_id], data, 2);
}

unsigned char CC2520_RANDOM(int dev_id){
	unsigned char data[3];
	
	data[0] = CC2520_INS_RANDOM;
	// data[1-2] don't care
	
	write(CC2520_spi_fd[dev_id], data, 3);
	
	return data[2];
}

// Warning: only 5bit address register can be access by this function, please refer to datasheet P45
unsigned char CC2520_BCLR(int dev_id, unsigned char addr, unsigned char bit_loc){  // REG bit clear, addr: 0-31, bit_loc: 0-7
	unsigned char data[2];
	
	data[0] = CC2520_INS_BCLR;
	data[1] = (addr << 3) & 0xF8;
	data[1] |= (bit_loc & 0x7);
	
	write(CC2520_spi_fd[dev_id], data, 2);
}

// Warning: Same as above
unsigned char CC2520_BSET(int dev_id, unsigned char addr, unsigned char bit_loc){  // REG bit set, addr: 0-31, bit_loc: 0-7
	unsigned char data[2];
	
	data[0] = CC2520_INS_BSET;
	data[1] = (addr << 3) & 0xF8;
	data[1] |= (bit_loc & 0x7);
	
	write(CC2520_spi_fd[dev_id], data, 2);
}

unsigned char CC2520_ABORT(int dev_id, unsigned char abort_code){
	unsigned char data[2];
	
	data[0] = CC2520_INS_ABORT;
	data[1] = abort_code & 0x3;
	
	write(CC2520_spi_fd[dev_id], data, 2);
}
