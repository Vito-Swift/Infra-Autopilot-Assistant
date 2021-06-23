#include "CC2520_Helper.h"
#include <string.h>

void CC2520_Send(unsigned char *buf, unsigned int len, CC2520_addr_t *addr) {
    unsigned char txbuf[129] = {0};
    IEEE802154_Fill_Data_Frame_Control(txbuf,
                                       IEEE802154_NO_FRAME_PENDING,
                                       IEEE802154_NO_ACK,
                                       IEEE802154_ADDR_MODE_SHORT_ADDR,
                                       IEEE802154_ADDR_MODE_SHORT_ADDR);
    IEEE802154_Fill_Sequence_Num(txbuf, 0);
    IEEE802154_Fill_Data_Frame_Short_Addr(txbuf,
                                          addr->dst_pan_id,
                                          addr->dst_addr,
                                          addr->src_pan_id,
                                          addr->src_addr);
    unsigned char *tx_payload = IEEE802154_Data_Frame_Get_Payload_Pointer(txbuf);
    memcpy(tx_payload, buf, len);
    IEEE802154_Fill_Data_Frame_PHY_Frame_Len_Field(txbuf, len);
    int send_count = CC2520_IEEE802154_Data_Frame_Get_SPI_Payload_Len(txbuf, len);
    CC2520_Send_Packet_Blocking(addr->dev_id, txbuf, send_count);
}

unsigned int CC2520_Recv() {

}