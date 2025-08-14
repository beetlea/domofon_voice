#include "debug.h"
#include "main.h"
#include "gpio.h"


void EXTI7_0_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

///extern volatile uint8_t state;
volatile uint16_t interruptFlag = 0;
volatile uint16_t send_flat = 0;
mode_mcu state_mode_gpio;
volatile uint32_t cnt_impulse = 0;
volatile uint8_t start_cnt = 0;
volatile uint8_t end_talk = 0;
void GPIO_INIT()
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void EXTI1_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

    //Configuration for GPIO Input on C1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* GPIOD ----> EXTI_Line1 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_INT_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);

    //Configuration for GPIO Input on C0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* GPIOC ----> EXTI_Line0 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI7_0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

mode_mcu current_mode()
{
    return state_mode_gpio;
}

void clear_flat_cnt()
{
    interruptFlag = 0;
    interruptFlag = 0;
}

uint8_t GET_ADDR_TRANSMIT_BEGIN()
{
    return start_cnt;
}
void GET_ADDR_TRANSMIT_RESET()
{
    start_cnt = 0;
}


void EXTI7_0_IRQHandler(void)
{
    state_mode_gpio = NORMAL_MODE;
    if((GPIOC->INDR&0b00000010) == 0)
    {
        TIM_Cmd(TIM1, ENABLE);
        TIM_SetCounter(TIM1, 0);
        if(start_cnt == 1)
        {
            interruptFlag++;
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
    else {
     EXTI_ClearITPendingBit(EXTI_Line1);
     TIM_Cmd(TIM1, DISABLE);
     volatile uint32_t t = TIM_GetCounter(TIM1);
     ///printf("time press = %d\r\n", t);
     if((t > MIN_TIME_RESET) &&(t < MAX_TIME_RESET))
     {
        CALL_BEGIN();
        send_flat = 0;
        interruptFlag = 0;
        start_cnt = 0;
        end_talk = 1;
     }
     else {
        start_cnt = 1;
        send_flat = send_flat + 1;
        if(send_flat > 255)
        {
            send_flat = ERROR_SEND_FLAT;
        }
     }
     TIM_Cmd(TIM1, DISABLE);
     
    }
}

/*
void EXTI7_0_IRQHandler(void)
{
    send_flat = send_flat + 1;
    if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
    {
        TIM_Cmd(TIM1, ENABLE);
        TIM_SetCounter(TIM1, 0);
        interruptFlag++;
        EXTI_ClearITPendingBit(EXTI_Line0);
        return;
    }

    if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
    {
     EXTI_ClearITPendingBit(EXTI_Line1);
     TIM_Cmd(TIM1, DISABLE);
     volatile uint32_t t = TIM_GetCounter(TIM1);
     ///printf("time press = %d\r\n", t);
     if((t > 2000) &&(t < 20000))
     {
        CALL_BEGIN();
        send_flat = 0;
        interruptFlag = 0;
     }
     else {
        //send_flat = send_flat + 1;
        if(send_flat > 255)
        {
            ///send_flat = ERROR_SEND_FLAT;
        }
     }
     TIM_Cmd(TIM1, DISABLE);       
    }
}*/


uint16_t GET_SEND_FLAT()
{
    return send_flat;
}




void BEGIN_CALL()
{
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    printf("begin_call!\r\n");
    end_talk = 0;
    while(end_talk == 0)
    {
        if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_1);
            Delay_Ms(10);
            GPIO_SetBits(GPIOA, GPIO_Pin_1);
            Delay_Ms(100);
            while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_RESET);
        }
    }
    RESET_BEGIN();
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
}