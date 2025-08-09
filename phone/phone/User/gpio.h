#define ERROR_SEND_FLAT 256

void EXTI0_INT_INIT();
void GPIO_INIT();
uint8_t GET_NUM_FLAT(uint8_t  * fl);
void BEGIN_CALL();
uint16_t GET_SEND_FLAT();