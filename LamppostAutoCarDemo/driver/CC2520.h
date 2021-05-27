#ifndef __CC2520_H
#define __CC2520_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "CC2520_HW.h"

// TXPOWER values, datasheet P72
#define CC2520_TXPOWER_MIN_18_DBM 0x03
#define CC2520_TXPOWER_MIN_7_DBM 0x2C
#define CC2520_TXPOWER_MIN_4_DBM 0x88  // Gives ~ -3.1 dBm TX output power
#define CC2520_TXPOWER_MIN_2_DBM 0x81
#define CC2520_TXPOWER_0_DBM 0x32  // Gives ~ 0.3 dBm TX output power
#define CC2520_TXPOWER_1_DBM 0x13
#define CC2520_TXPOWER_2_DBM 0xAB
#define CC2520_TXPOWER_3_DBM 0xF2
#define CC2520_TXPOWER_5_DBM 0xF7  // Gives ~ 4.6 dBm TX output power

// datasheet P121
#define CHIP_ID_CC2520               0x84

// IEEE802.15.4-2006 P137
// Frame control field 16bit
#define IEEE802154_BEACON_FRAME 0U
#define IEEE802154_DATA_FRAME 1U
#define IEEE802154_ACK_FRAME 2U
#define IEEE802154_MACCMD_FRAME 3U
#define IEEE802154_SECURITY_ENABLE (1U<<3)
#define IEEE802154_SECURITY_DISABLE 0
#define IEEE802154_FRAME_PENDING (1U<<4)
#define IEEE802154_NO_FRAME_PENDING 0
#define IEEE802154_REQUEST_ACK (1U<<5)
#define IEEE802154_NO_ACK 0
#define IEEE802154_PAN_ID_COMPRESSION (1U<<6)
#define IEEE802154_PAN_ID_NO_COMRESSION 0
#define IEEE802154_ADDR_MODE_NO_PAN_NO_ADDR 0
#define IEEE802154_ADDR_MODE_SHORT_ADDR 2U
#define IEEE802154_ADDR_MODE_EXTEN_ADDR 3U
#define IEEE802154_DST_ADDR_MODE_SHIFT 10
#define IEEE802154_SRC_ADDR_MODE_SHIFT 14
#define IEEE802154_FRAME_VERSION_2003 0
#define IEEE802154_FRAME_VERSION_2006 (1U<<12)  // SPEC 2006 P143 said it should be zero in beacon?

// IEEE802.15.4-2006
#define IEEE802154_MAC_MIN_BE 3  // P164
#define IEEE802154_MAC_MAX_BE 5  // P163
#define IEEE802154_MAC_MAX_CSMA_BACKOFFS 4  // P163
#define IEEE802154_A_UNIT_BACKOFF_PERIOD 20  // P159, 20 symbol
#define IEEE802154_2450MHZ_SYMBOL_PERIOD 16  // us, P47, 250kbps = 4us, 4bit = 1symbol -> 16us


void CC2520_Init(void);
void CC2520_ReInit(void);
void CC2520_Update_Reg_Datasheet_Recommend(void);
int CC2520_Decode_Status_RX_active(int status);
int CC2520_Decode_Status_TX_active(int status);
int CC2520_Decode_Status_DPUL_active(int status);
int CC2520_Decode_Status_DPUH_active(int status);
int CC2520_Decode_Status_Exception_chB(int status);
int CC2520_Decode_Status_Exception_chA(int status);
int CC2520_Decode_Status_RSSI_valid(int status);
int CC2520_Decode_Status_XOSC_stable_running(int status);
void CC2520_Frame_Filter_Enable(int dev_id, int enable);
void CC2520_PAN_Coordinator(int dev_id, int enable);
void CC2520_Accept_MAC_CMD_Frame(int dev_id, int accept);
void CC2520_Accept_ACK_Frame(int dev_id, int accept);
void CC2520_Accept_Data_Frame(int dev_id, int accept);
void CC2520_Accept_Beacon_Frame(int dev_id, int accept);
void CC2520_AutoACK(int dev_id, int enable);
unsigned int CC2520_Read_Exception(int dev_id);
int CC2520_Decode_Exception(unsigned int exc_word, int exc_id);
void CC2520_Clear_Exception(int dev_id, int exc_id);
void CC2520_Print_All_Exception(int dev_id);
int CC2520_Set_Channel(int dev_id, int ch);
int CC2520_Set_TX_Power(int dev_id, int power);
int CC2520_Verify_ChipID(int dev_id);
unsigned char CC2520_Chip_Version(int dev_id);
int CC2520_Empty_TX_FIFO(int dev_id);
int CC2520_Empty_RX_FIFO(int dev_id);
void CC2520_Set_Frame_Filter_PAN_ID(int dev_id, unsigned short pan_id);
void CC2520_Set_Frame_Filter_Short_Addr(int dev_id, unsigned short addr);
void CC2520_Set_Frame_Filter_Ext_Addr(int dev_id, unsigned char *addr);
char CC2520_receive_signal_strength(int dev_id);
int CC2520_receive_signal_strength_valid(int dev_id);
int CC2520_Get_CCA(int dev_id);
void IEEE802154_CCA_Set_Backoff(int dev_id, int us);
int IEEE802154_CCA_Backoff_pass(int dev_id);
void IEEE802154_CCA_Backoff_Block(int dev_id);
int IEEE802154_CSMACA_Algo(int dev_id);

void IEEE802154_Fill_Data_Frame_Control(unsigned char *buf, int frame_pending, int request_ack, int dst_addr_mode, int src_addr_mode);
void IEEE802154_Fill_Sequence_Num(unsigned char *buf, unsigned char seq);
void IEEE802154_Fill_Data_Frame_Short_Addr(unsigned char *buf, unsigned short dst_pan_id, unsigned short dst_addr, unsigned short src_pan_id, unsigned short src_addr);
unsigned char *IEEE802154_Data_Frame_Get_Payload_Pointer(unsigned char *buf);
void IEEE802154_Fill_Data_Frame_PHY_Frame_Len_Field(unsigned char *buf, int mac_payload_len);
int CC2520_IEEE802154_Data_Frame_Get_SPI_Payload_Len(unsigned char *buf, int mac_payload_len);
void CC2520_Send_Packet_Non_Blocking(int dev_id, unsigned char *buf, unsigned char count);
int CC2520_Send_Packet_Finish(int dev_id);
void CC2520_Send_Packet_Blocking(int dev_id, unsigned char *buf, unsigned char count);

int CC2520_Receive_Packet_Pending(int dev_id);
unsigned char CC2520_Receive_Packet_Non_Blocking(int dev_id, unsigned char *buf);
unsigned char CC2520_Receive_Packet_Blocking(int dev_id, unsigned char *buf);
unsigned char IEEE802154_Get_PHY_Frame_Len_Field(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_Frame_Type(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_Security_Enable(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_Frame_Pending(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_Request_ACK(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_PAN_ID_Compression(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_DST_Addr_Mode(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_Frame_Version(unsigned char *buf);
int IEEE802154_Decode_Frame_Control_SRC_Addr_Mode(unsigned char *buf);
unsigned char IEEE802154_Get_Sequence_Num(unsigned char *buf);
unsigned short IEEE802154_Get_DST_PAN_ID(unsigned char *buf);
unsigned short IEEE802154_Get_DST_Short_Addr(unsigned char *buf);
unsigned short IEEE802154_Get_SRC_PAN_ID(unsigned char *buf);
unsigned short IEEE802154_Get_SRC_Short_Addr(unsigned char *buf);
unsigned char IEEE802154_Get_Data_Frame_Payload_Len(unsigned char *buf);
int CC2520_CRC_Correct(unsigned char *buf, unsigned char count);

void IEEE802154_Fill_ACK_Frame_Control(unsigned char *buf, int frame_pending);
void IEEE802154_Fill_ACK_Frame_PHY_Frame_Len_Field(unsigned char *buf);
int CC2520_IEEE802154_ACK_Frame_Get_SPI_Payload_Len(unsigned char *buf);


#ifdef __cplusplus
}
#endif

#endif
