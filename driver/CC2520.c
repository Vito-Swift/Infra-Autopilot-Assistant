#include "CC2520_HW.h"
#include "CC2520.h"
#include <stdio.h>

void CC2520_Init(void){
	int i;
	
	RPI3_CC2520_HW_Init();  // init SPI, GPIO, reset device

	for(i=0; i<2; i++){
		CC2520_Instruction(i, CC2520_INS_SXOSCON);
	}
	delay(10);
	
	for(i=0; i<2; i++){
		CC2520_Instruction(i, CC2520_INS_SFLUSHRX);   
		CC2520_Instruction(i, CC2520_INS_SFLUSHTX);
		
		CC2520_Frame_Filter_Enable(i, 0);  // off frame filter
		
		CC2520_Instruction(i, CC2520_INS_SRFOFF);  // Force the RF FSM to IDLE state, see datasheet P67
		CC2520_Instruction(i, CC2520_INS_SRXON);  // Force RX calibration
	}
	delay(10);
	
	CC2520_Update_Reg_Datasheet_Recommend();
	
	// outside init:
	// pan cordinater
	// enable frame filter, frame filter address
}

void CC2520_ReInit(void){
	int i;
	
	RPI3_CC2520_HW_ReInit();  // init SPI, GPIO, reset device

	for(i=0; i<2; i++){
		CC2520_Instruction(i, CC2520_INS_SXOSCON);
	}
	delay(10);
	
	for(i=0; i<2; i++){
		CC2520_Instruction(i, CC2520_INS_SFLUSHRX);   
		CC2520_Instruction(i, CC2520_INS_SFLUSHTX);
		
		CC2520_Frame_Filter_Enable(i, 0);  // off frame filter
		
		CC2520_Instruction(i, CC2520_INS_SRFOFF);  // Force the RF FSM to IDLE state, see datasheet P67
		CC2520_Instruction(i, CC2520_INS_SRXON);  // Force RX calibration
	}
	delay(10);
	
	CC2520_Update_Reg_Datasheet_Recommend();
}

// datasheet P103
void CC2520_Update_Reg_Datasheet_Recommend(void){
	int i;
	for(i=0; i<2; i++){
		CC2520_WriteReg(i, CC2520_TXPOWER, 0x32);
		CC2520_WriteReg(i, CC2520_CCACTRL0, 0xF8);
		CC2520_WriteRAM(i, CC2520_MDMCTRL0, 0x85);
		CC2520_WriteRAM(i, CC2520_MDMCTRL1, 0x14);
		CC2520_WriteRAM(i, CC2520_RXCTRL, 0x3F);
		CC2520_WriteRAM(i, CC2520_FSCTRL, 0x5A);
		CC2520_WriteRAM(i, CC2520_FSCAL1, 0x2B);
		CC2520_WriteRAM(i, CC2520_AGCCTRL1, 0x11);
		CC2520_WriteRAM(i, CC2520_ADCTEST0, 0x10);
		CC2520_WriteRAM(i, CC2520_ADCTEST1, 0x0E);
		CC2520_WriteRAM(i, CC2520_ADCTEST2, 0x03);
	}
}

// For the following status decoding, refer to datasheet P53
int CC2520_Decode_Status_RX_active(int status){
	return status & CC2520_STB_RX_ACTIVE_BV;
}
int CC2520_Decode_Status_TX_active(int status){
	return status & CC2520_STB_TX_ACTIVE_BV;
}
int CC2520_Decode_Status_DPUL_active(int status){
	return status & CC2520_STB_DPUL_ACTIVE_BV;
}
int CC2520_Decode_Status_DPUH_active(int status){
	return status & CC2520_STB_DPUH_ACTIVE_BV;
}
int CC2520_Decode_Status_Exception_chB(int status){
	return status & CC2520_STB_EXC_CHB_BV;  // check CC2520_HW.h if it doesn't work
}
int CC2520_Decode_Status_Exception_chA(int status){
	return status & CC2520_STB_EXC_CHA_BV;  // check CC2520_HW.h if it doesn't work
}
int CC2520_Decode_Status_RSSI_valid(int status){
	return status & CC2520_STB_RSSI_VALID_BV;
}
int CC2520_Decode_Status_XOSC_stable_running(int status){
	return status & CC2520_STB_XOSC_STABLE_BV;
}

// datasheet P105, default enable
void CC2520_Frame_Filter_Enable(int dev_id, int enable){  // dev_id 0 or 1, enable is 1 = enable 0 = disable
	if(enable == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT0, 0);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT0, 0);
	}
}

// datasheet P105, default not coordinator
void CC2520_PAN_Coordinator(int dev_id, int enable){  // dev_id 0 or 1, enable is 1 = set to coordinator 0 = set to not
	if(enable == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT0, 1);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT0, 1);
	}
}

// datasheet P106, default accept
void CC2520_Accept_MAC_CMD_Frame(int dev_id, int accept){  // dev_id 0 or 1, accept: 1=accept 0=reject
	if(accept == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT1, 6);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT1, 6);
	}
}

// datasheet P106, default accept
void CC2520_Accept_ACK_Frame(int dev_id, int accept){  // dev_id 0 or 1, accept: 1=accept 0=reject
	if(accept == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT1, 5);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT1, 5);
	}
}

// datasheet P106, default accept
void CC2520_Accept_Data_Frame(int dev_id, int accept){  // dev_id 0 or 1, accept: 1=accept 0=reject
	if(accept == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT1, 4);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT1, 4);
	}
}

// datasheet P106, default accept
void CC2520_Accept_Beacon_Frame(int dev_id, int accept){  // dev_id 0 or 1, accept: 1=accept 0=reject
	if(accept == 0){
		CC2520_BCLR(dev_id, CC2520_FRMFILT1, 3);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMFILT1, 3);
	}
}

// datasheet P109, default no auto ack
void CC2520_AutoACK(int dev_id, int enable){  // dev_id 0 or 1, enable: 1=autoACK 0=no autoACK
	if(enable == 0){
		CC2520_BCLR(dev_id, CC2520_FRMCTRL0, 5);
	}
	else{
		CC2520_BSET(dev_id, CC2520_FRMCTRL0, 5);
	}
}

// datasheet P112, 55
unsigned int CC2520_Read_Exception(int dev_id){  // dev_id 0 or 1
	unsigned int ret;
	unsigned int temp;
	
	temp = CC2520_ReadReg(dev_id, CC2520_EXCFLAG0);
	ret = temp & 0xFF;
	temp = CC2520_ReadReg(dev_id, CC2520_EXCFLAG1);
	ret |= (temp << 8) & 0xFF00;
	temp = CC2520_ReadReg(dev_id, CC2520_EXCFLAG2);
	ret |= (temp << 16) & 0xFF0000;
	
	return ret;
}

// datasheet P55
int CC2520_Decode_Exception(unsigned int exc_word, int exc_id){  // example: exc = CC2520_Read_Exception(0);  result = CC2520_decode_Exception(exc, CC2520_EXC_TX_OVERFLOW); it will return 1 if TX overflow, return 0 otherwise. 
	if(exc_word & (((unsigned int)1)<<exc_id))
		return 1;
	else
		return 0;
}

// datasheet P112
void CC2520_Clear_Exception(int dev_id, int exc_id){
	unsigned char exc_reg;
	int id;
	
	if(exc_id >= 0 && exc_id <= 7){
		exc_reg = CC2520_EXCFLAG0;
		id = exc_id;
	}
	else if(exc_id >= 8 && exc_id <= 15){
		exc_reg = CC2520_EXCFLAG1;
		id = exc_id - 8;
	}
	else{
		exc_reg = CC2520_EXCFLAG2;
		id = exc_id - 16;
	}
	
	CC2520_BCLR(dev_id, exc_reg, id);
}

void CC2520_Print_All_Exception(int dev_id){
	unsigned int exc_word;
	
	exc_word = CC2520_Read_Exception(dev_id);
	
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RF_IDLE)){
		printf("CC2520_EXC_RF_IDLE\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_TX_FRM_DONE)){
		printf("CC2520_EXC_TX_FRM_DONE\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_TX_ACK_DONE)){
		printf("CC2520_EXC_TX_ACK_DONE\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_TX_UNDERFLOW)){
		printf("CC2520_EXC_TX_UNDERFLOW\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_TX_OVERFLOW)){
		printf("CC2520_EXC_TX_OVERFLOW\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_UNDERFLOW)){
		printf("CC2520_EXC_RX_UNDERFLOW\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_OVERFLOW)){
		printf("CC2520_EXC_RX_OVERFLOW\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RXENABLE_ZERO)){
		printf("CC2520_EXC_RXENABLE_ZERO\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_FRM_DONE)){
		printf("CC2520_EXC_RX_FRM_DONE\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_FRM_ACCEPTED)){
		printf("CC2520_EXC_RX_FRM_ACCEPTED\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_SRC_MATCH_DONE)){
		printf("CC2520_EXC_SRC_MATCH_DONE\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_SRC_MATCH_FOUND)){
		printf("CC2520_EXC_SRC_MATCH_FOUND\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_FIFOP)){
		printf("CC2520_EXC_FIFOP\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_SFD)){
		printf("CC2520_EXC_SFD\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_DPU_DONE_L)){
		printf("CC2520_EXC_DPU_DONE_L\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_DPU_DONE_H)){
		printf("CC2520_EXC_DPU_DONE_H\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_MEMADDR_ERROR)){
		printf("CC2520_EXC_MEMADDR_ERROR\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_USAGE_ERROR)){
		printf("CC2520_EXC_USAGE_ERROR\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_OPERAND_ERROR)){
		printf("CC2520_EXC_OPERAND_ERROR\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_SPI_ERROR)){
		printf("CC2520_EXC_SPI_ERROR\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RF_NO_LOCK)){
		printf("CC2520_EXC_RF_NO_LOCK\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_FRM_ABORTED)){
		printf("CC2520_EXC_RX_FRM_ABORTED\n");
	}
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RXBUFMOV_TIMEOUT)){
		printf("CC2520_EXC_RXBUFMOV_TIMEOUT\n");
	}
}

// refer to datasheet P62, 118, default 0x0B=2405MHz=ch11
int CC2520_Set_Channel(int dev_id, int ch){  // 11 <= ch <= 26, dev_id is 0 or 1
	if(ch < 11 || ch > 26)
		return -1;  // channel invalid
	
	CC2520_WriteReg(dev_id, CC2520_FREQCTRL, 11 + 5*(ch-11));
	
	CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
	CC2520_Instruction(dev_id, CC2520_INS_SRXON);
	
	return 1; // channel valid and set
}

int CC2520_Set_TX_Power(int dev_id, int power){  // datasheet P72, 118, default=0x06
	unsigned char reg_value;
	
	switch(power){
		case -18: reg_value = CC2520_TXPOWER_MIN_18_DBM; break;
		case -7: reg_value = CC2520_TXPOWER_MIN_7_DBM; break;
		case -4: reg_value = CC2520_TXPOWER_MIN_4_DBM; break;
		case -2: reg_value = CC2520_TXPOWER_MIN_2_DBM; break;
		case 0: reg_value = CC2520_TXPOWER_0_DBM; break;
		case 1: reg_value = CC2520_TXPOWER_1_DBM; break;
		case 2: reg_value = CC2520_TXPOWER_2_DBM; break;
		case 3: reg_value = CC2520_TXPOWER_3_DBM; break;
		case 5: reg_value = CC2520_TXPOWER_5_DBM; break;
		default: return -1;  // setting not exist
	}
	
	CC2520_WriteReg(dev_id, CC2520_TXPOWER, reg_value);
	
	return 1;  // setting exist and set
}

//reg in P119-120 are pending to be implemented, they are CCS/RSSI related

// datasheet P121
int CC2520_Verify_ChipID(int dev_id){
	unsigned char ID;
	
	ID = CC2520_ReadRAM(dev_id, CC2520_CHIPID);
	if(ID == CHIP_ID_CC2520)
		return 1;
	else
		return 0;  // false instead of error, so return 0 instead of -1
}

// datasheet P121
unsigned char CC2520_Chip_Version(int dev_id){
	unsigned char version;
	
	version = CC2520_ReadRAM(dev_id, CC2520_VERSION);
	
	return version;  // if it is RevA, you may need to update correlation threshold. see datasheet P123, 74. However, function CC2520_update_REG_datasheet_recommend has already updated the correlation threshold
}

int CC2520_Empty_TX_FIFO(int dev_id){
	CC2520_Instruction(dev_id, CC2520_INS_SFLUSHTX);
}

int CC2520_Empty_RX_FIFO(int dev_id){
	CC2520_Instruction(dev_id, CC2520_INS_SFLUSHRX);
}

// datasheet P60, 105
void CC2520_Set_Frame_Filter_PAN_ID(int dev_id, unsigned short pan_id){
	CC2520_WriteRAM(dev_id, CC2520_RAM_PANID, (unsigned char)(pan_id&0xFF));
	CC2520_WriteRAM(dev_id, CC2520_RAM_PANID+1, (unsigned char)((pan_id>>8)&0xFF));
}

void CC2520_Set_Frame_Filter_Short_Addr(int dev_id, unsigned short addr){
	CC2520_WriteRAM(dev_id, CC2520_RAM_SHORTADDR, (unsigned char)(addr&0xFF));
	CC2520_WriteRAM(dev_id, CC2520_RAM_SHORTADDR+1, (unsigned char)((addr>>8)&0xFF));
}

void CC2520_Set_Frame_Filter_Ext_Addr(int dev_id, unsigned char *addr){
	int i;
	for(i=0; i<8; i++){
		CC2520_WriteRAM(dev_id, CC2520_RAM_EXTADDR+i, addr[i]);
	}
}

// datasheet P83, 120
char CC2520_receive_signal_strength(int dev_id){
	return CC2520_ReadReg(dev_id, CC2520_RSSI);
}

int CC2520_receive_signal_strength_valid(int dev_id){
	int temp;

	temp = CC2520_ReadReg(dev_id, CC2520_RSSISTAT);
	if(temp)
		return 1;
	else
		return 0;
}

// datasheet P35, P109, 118-120
int CC2520_Get_CCA(int dev_id){
	return RPI3_CC2520_ReadGPIO(dev_id, 3);
}

// it just set the timer, but it will not block.
struct timeval cca_start_time[2];
int cca_wait_time[2];
void IEEE802154_CCA_Set_Backoff(int dev_id, int us){
	gettimeofday(&(cca_start_time[dev_id]), NULL);
	cca_wait_time[dev_id] = us;
}

// return 0 if the backoff period have not pass yet, return 1 if pass already
int IEEE802154_CCA_Backoff_pass(int dev_id){
	struct timeval current_time;
	int delta_time;  // warning: it will overflow after ~2000 second
	
	gettimeofday(&current_time, NULL);
	delta_time = (current_time.tv_sec - cca_start_time[dev_id].tv_sec)*1000*1000 + (current_time.tv_usec - cca_start_time[dev_id].tv_usec);
	
	return (delta_time > cca_wait_time[dev_id]);
}

void IEEE802154_CCA_Backoff_Block(int dev_id){
	while(IEEE802154_CCA_Backoff_pass(dev_id) == 0);
}

// SPEC 2006 P172
int IEEE802154_CSMACA_Algo(int dev_id){  // 0=fail 1=success
	int NB = 0;
	int BE = IEEE802154_MAC_MIN_BE;
	int r;
	
	srand(time(NULL));
	while(NB <= IEEE802154_MAC_MAX_CSMA_BACKOFFS){
		r = rand() % (1<<BE);
		usleep(r * IEEE802154_A_UNIT_BACKOFF_PERIOD * IEEE802154_2450MHZ_SYMBOL_PERIOD);
		
		if(CC2520_Get_CCA(dev_id))
			return 1;  // if clear channel, return success
		
		NB++;
		BE++;
		if(BE > IEEE802154_MAC_MAX_BE)
			BE = IEEE802154_MAC_MAX_BE;
	}
	return 0;
}

// datasheet P65 P68,70
// only for data frame
void IEEE802154_Fill_Data_Frame_Control(unsigned char *buf, int frame_pending, int request_ack, int dst_addr_mode, int src_addr_mode){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	(*buf_ptr) = 0;
	(*buf_ptr) |= IEEE802154_DATA_FRAME;
	(*buf_ptr) |= IEEE802154_SECURITY_DISABLE; // security is not supported
	(*buf_ptr) |= frame_pending;
	(*buf_ptr) |= request_ack;
	(*buf_ptr) |= IEEE802154_PAN_ID_NO_COMRESSION;
	(*buf_ptr) |= (dst_addr_mode << IEEE802154_DST_ADDR_MODE_SHIFT);
	(*buf_ptr) |= IEEE802154_FRAME_VERSION_2003;
	(*buf_ptr) |= (src_addr_mode << IEEE802154_SRC_ADDR_MODE_SHIFT);
}

// for any frame
void IEEE802154_Fill_Sequence_Num(unsigned char *buf, unsigned char seq){
	buf[4] = seq;
}

// only for data frame because other frame may not have both src and dst addr
void IEEE802154_Fill_Data_Frame_Short_Addr(unsigned char *buf, unsigned short dst_pan_id, unsigned short dst_addr, unsigned short src_pan_id, unsigned short src_addr){
	if( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) != IEEE802154_ADDR_MODE_SHORT_ADDR )
		return;
	if( ((buf[3]>>(IEEE802154_SRC_ADDR_MODE_SHIFT-8))&0x03) != IEEE802154_ADDR_MODE_SHORT_ADDR )
		return;
		
	buf[5] = dst_pan_id&0xFF;
	buf[6] = (dst_pan_id>>8)&0xFF;
	
	buf[7] = dst_addr&0xFF;
	buf[8] = (dst_addr>>8)&0xFF;
	
	buf[9] = src_pan_id&0xFF;
	buf[10] = (src_pan_id>>8)&0xFF;
	
	buf[11] = src_addr&0xFF;
	buf[12] = (src_addr>>8)&0xFF;
}

// it assume security bit is 0, and both src and dst addr mode are the same
unsigned char *IEEE802154_Data_Frame_Get_Payload_Pointer(unsigned char *buf){
	if( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_SHORT_ADDR ){
		return buf+1+1+2+1+(2+2+2+2);  // instruction, phy_frame_length_field, frame_control, seq, 4 X addr_field
	}
	else if ( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_EXTEN_ADDR ){
		return buf+1+1+2+1+(2+8+2+8);
	}
}

/*
 * CC2520 SPI format of sending packet: 
 * [Write FIFO instruction(1byte)]
 * [PHY Layer field: len of MAC frame: header+payload+ACK(1byte)]
 * [MAC header(xbyte)]
 * [MAC payload(ybyte)]
 * The 2 byte ACK is auto generated. x+y = len-2.
 */

// it assume security bit is 0, and both src and dst addr mode are the same
// it fill the "PHY Layer field"
void IEEE802154_Fill_Data_Frame_PHY_Frame_Len_Field(unsigned char *buf, int mac_payload_len){  // fill mac frame len in phy layer field
	// 2+1+addr+n+2
	if( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_SHORT_ADDR )
		buf[1] = 2+1+(2+2+2+2)+mac_payload_len+2;
	else if ( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_EXTEN_ADDR )
		buf[1] = 2+1+(2+8+2+8)+mac_payload_len+2;
}

// it return the entire SPI cmd len, excluding "Write FIFO instruction" which will be added in the Write_FIFO function.
int CC2520_IEEE802154_Data_Frame_Get_SPI_Payload_Len(unsigned char *buf, int mac_payload_len){
	// 1+2+1+addr+n
	if( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_SHORT_ADDR )
		return 1+2+1+(2+2+2+2)+mac_payload_len;
	else if ( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_EXTEN_ADDR )
		return 1+2+1+(2+8+2+8)+mac_payload_len;
}

void CC2520_Send_Packet_Non_Blocking(int dev_id, unsigned char *buf, unsigned char count){
	CC2520_Instruction(dev_id, CC2520_INS_SFLUSHTX);
	CC2520_WriteTXFIFO(dev_id, buf, count);
	
	CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
	CC2520_Instruction(dev_id, CC2520_INS_STXON);
}

int CC2520_Send_Packet_Finish(int dev_id){
	unsigned int exc_word;
	
	exc_word = CC2520_Read_Exception(dev_id);
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_TX_FRM_DONE)){
		CC2520_Clear_Exception(dev_id, CC2520_EXC_TX_FRM_DONE);
		CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
		CC2520_Instruction(dev_id, CC2520_INS_SRXON);
		return 1;
	}
	else{
		return 0;
	}
}

// should work for any frame
void CC2520_Send_Packet_Blocking(int dev_id, unsigned char *buf, unsigned char count){
	/*CC2520_Instruction(dev_id, CC2520_INS_SFLUSHTX);
	CC2520_WriteTXFIFO(dev_id, buf, count);
	
	CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
	CC2520_Instruction(dev_id, CC2520_INS_STXON);*/
	CC2520_Send_Packet_Non_Blocking(dev_id, buf, count);
	
	/*while( !RPI3_CC2520_ReadGPIO(dev_id, 4) );
	while( RPI3_CC2520_ReadGPIO(dev_id, 4) );
	
	CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
	CC2520_Instruction(dev_id, CC2520_INS_SRXON);*/
	while(CC2520_Send_Packet_Finish(dev_id) == 0);
}

// work for any frame
int CC2520_Receive_Packet_Pending(int dev_id){
	/*if( (!RPI3_CC2520_ReadGPIO(dev_id, 4)) && RPI3_CC2520_ReadGPIO(dev_id, 1) ){
		return 1;
	}
	return 0;*/
	
	unsigned int exc_word;
	
	exc_word = CC2520_Read_Exception(dev_id);
	if(CC2520_Decode_Exception(exc_word, CC2520_EXC_RX_FRM_DONE)){
		CC2520_Clear_Exception(dev_id, CC2520_EXC_RX_FRM_DONE);
		CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);  // is that necessary?
		CC2520_Instruction(dev_id, CC2520_INS_SRXON);
		return 1;
	}
	else{
		return 0;
	}
}

// should work for any frame
unsigned char CC2520_Receive_Packet_Non_Blocking(int dev_id, unsigned char *buf){
	unsigned char count;
	
	count = CC2520_ReadRXFIFO(dev_id, buf);
	CC2520_Instruction(dev_id, CC2520_INS_SFLUSHRX);
	
	CC2520_Instruction(dev_id, CC2520_INS_SRFOFF);
	CC2520_Instruction(dev_id, CC2520_INS_SRXON);

	return count;
}

unsigned char CC2520_Receive_Packet_Blocking(int dev_id, unsigned char *buf){
	unsigned char count;
	
	while(CC2520_Receive_Packet_Pending(dev_id) == 0);
	count = CC2520_Receive_Packet_Non_Blocking(dev_id, buf);
	
	return count;
}

unsigned char IEEE802154_Get_PHY_Frame_Len_Field(unsigned char *buf){
	return buf[1];
}

int IEEE802154_Decode_Frame_Control_Frame_Type(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	return (*buf_ptr)&0x0007;
}

int IEEE802154_Decode_Frame_Control_Security_Enable(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	if( (*buf_ptr)&IEEE802154_SECURITY_ENABLE )
		return 1;
	else
		return 0;
}

int IEEE802154_Decode_Frame_Control_Frame_Pending(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	if( (*buf_ptr)&IEEE802154_FRAME_PENDING )
		return 1;
	else
		return 0;
}

int IEEE802154_Decode_Frame_Control_Request_ACK(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	if( (*buf_ptr)&IEEE802154_REQUEST_ACK )
		return 1;
	else
		return 0;
}

int IEEE802154_Decode_Frame_Control_PAN_ID_Compression(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	if( (*buf_ptr)&IEEE802154_PAN_ID_COMPRESSION )
		return 1;
	else
		return 0;
}

int IEEE802154_Decode_Frame_Control_DST_Addr_Mode(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	return ((*buf_ptr)>>IEEE802154_DST_ADDR_MODE_SHIFT) & 0x0003;
}

int IEEE802154_Decode_Frame_Control_Frame_Version(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	if( (*buf_ptr)&IEEE802154_FRAME_VERSION_2006 )
		return 2006;
	else
		return 2003;
}

int IEEE802154_Decode_Frame_Control_SRC_Addr_Mode(unsigned char *buf){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	
	return ((*buf_ptr)>>IEEE802154_SRC_ADDR_MODE_SHIFT) & 0x0003;
}

unsigned char IEEE802154_Get_Sequence_Num(unsigned char *buf){
	return buf[4];
}

unsigned short IEEE802154_Get_DST_PAN_ID(unsigned char *buf){
	unsigned short pan_id;
	
	if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR )
		return 0;
	
	// either short or ext addr
	pan_id = 0;
	pan_id = buf[6];
	pan_id <<= 8;
	pan_id |= buf[5];
	return pan_id;
}

unsigned short IEEE802154_Get_DST_Short_Addr(unsigned char *buf){
	unsigned short addr;
	
	if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) != IEEE802154_ADDR_MODE_SHORT_ADDR )
		return 0;
		
	// if it is short addr mode, offset must be 2
	addr = 0;
	addr = buf[8];
	addr <<= 8;
	addr |= buf[7];
	return addr;
}

unsigned short IEEE802154_Get_SRC_PAN_ID(unsigned char *buf){
	unsigned short pan_id;
	int offset;

	offset = 0;
	if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR )
		offset = 5+0;
	else if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_SHORT_ADDR )
		offset = 5+2+2;
	else if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_EXTEN_ADDR )
		offset = 5+2+8;
	
	if( IEEE802154_Decode_Frame_Control_SRC_Addr_Mode(buf) == IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR )
		return 0;
	
	// either short or ext addr
	pan_id = 0;
	pan_id = buf[offset+1];
	pan_id <<= 8;
	pan_id |= buf[offset];
	return pan_id;
}

unsigned short IEEE802154_Get_SRC_Short_Addr(unsigned char *buf){
	unsigned short addr;
	int offset;

	offset = 0;
	if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR )
		offset = 5+0;
	else if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_SHORT_ADDR )
		offset = 5+2+2;
	else if( IEEE802154_Decode_Frame_Control_DST_Addr_Mode(buf) == IEEE802154_ADDR_MODE_EXTEN_ADDR )
		offset = 5+2+8;
		
	if( IEEE802154_Decode_Frame_Control_SRC_Addr_Mode(buf) != IEEE802154_ADDR_MODE_SHORT_ADDR )
		return 0;
		
	offset += 2;  // if it is short addr mode, offset must be 2
	addr = 0;
	addr = buf[offset+1];
	addr <<= 8;
	addr |= buf[offset];
	return addr;
}

// it assume security bit is 0, and both src and dst addr mode are the same
unsigned char IEEE802154_Get_Data_Frame_Payload_Len(unsigned char *buf){
	if( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_SHORT_ADDR )
		return buf[1] -2 -(2+2+2+2)-1-2;  // -ACK -addr-seq-frame_ctrl
	else if ( ((buf[3]>>(IEEE802154_DST_ADDR_MODE_SHIFT-8))&0x03) == IEEE802154_ADDR_MODE_EXTEN_ADDR )
		return buf[1] -2 -(2+8+2+8)-1-2;
}

// SPEC 2006 P147
void IEEE802154_Fill_ACK_Frame_Control(unsigned char *buf, int frame_pending){
	unsigned short *buf_ptr;
	buf_ptr = (unsigned short*)(buf+2);
	(*buf_ptr) = 0;
	(*buf_ptr) |= IEEE802154_ACK_FRAME;
	(*buf_ptr) |= IEEE802154_SECURITY_DISABLE; // security is not supported
	(*buf_ptr) |= frame_pending;
	(*buf_ptr) |= IEEE802154_NO_ACK;
	(*buf_ptr) |= IEEE802154_PAN_ID_NO_COMRESSION;
	(*buf_ptr) |= (IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR << IEEE802154_DST_ADDR_MODE_SHIFT);
	(*buf_ptr) |= IEEE802154_FRAME_VERSION_2003;
	(*buf_ptr) |= (IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR << IEEE802154_SRC_ADDR_MODE_SHIFT);
}

void IEEE802154_Fill_ACK_Frame_PHY_Frame_Len_Field(unsigned char *buf){
	buf[1] = 2+1+2;  // frame_ctrl + seq_num + FCS
}

int CC2520_IEEE802154_ACK_Frame_Get_SPI_Payload_Len(unsigned char *buf){
	return 1+2+1;  // PHY_frame_len_field + frame_ctrl + seq_num
}

// datasheet P80
int CC2520_CRC_Correct(unsigned char *buf, unsigned char count){  // return 1 if CRC correct
	unsigned char *data_buf;
	
	data_buf = buf+1;
	
	if(data_buf[count-1] & 0x80)
		return 1;
	else
		return 0;
}
