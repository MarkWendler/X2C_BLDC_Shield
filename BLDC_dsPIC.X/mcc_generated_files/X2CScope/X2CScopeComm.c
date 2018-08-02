/* ************************************************************************** */
/** X2CComm.c

  @Company
    Microchip Technology

  @Summary
    Implements the X2C Lin protocol connection with MCC Peripheral drivers.

 */
/* ************************************************************************** */

#include "X2CScopeComm.h"
#include "../mcc.h"

/** 
  @brief
    Puts the data to the hardware layer. (UART)
   @param[in] serial Serial interface object. (Not used)
   @param[in] data Data to send 
 */
void sendSerial(uint8_t data)
{

  UART1_Write( data );
}

/** 
  @brief
   Get serial data from hardware. Reset the hardware in case of error. (UART2)
  @return
    Return with the received data
 */
uint8_t receiveSerial()
{
  return UART1_Read();
}

/** 
  @brief  Check data availability (UART).
  @return
    True -> Serial data ready to read.
    False -> No data.
 */
uint8_t isReceiveDataAvailable()
{
  return (UART1_DataReady);

}

/** 
  @brief
   Check output buffer. (UART)
  @return    
    True -> Transmit buffer is not full, at least one more character can be written.
    False -> Transmit buffer is full.
 */
uint8_t isSendReady()
{
  return UART1_is_tx_ready();
}
/* *****************************************************************************
 End of File
 */