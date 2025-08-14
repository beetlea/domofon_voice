#define ERROR_SEND_FLAT 256

void EXTI0_INT_INIT(void);
void EXTI1_INT_INIT(void);
void GPIO_INIT();
uint8_t GET_NUM_FLAT(uint8_t  * fl);
void BEGIN_CALL();
uint16_t GET_SEND_FLAT();
void clear_flat_cnt();
uint8_t GET_ADDR_TRANSMIT_BEGIN();
void GET_ADDR_TRANSMIT_RESET();