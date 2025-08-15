/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                     PD6 -- Tx
 *
 */

#include "debug.h"
#include "gpio.h"
#include "main.h"
#include "timer.h"
/* Global define */


/* Global Variable */
vu8 val;
volatile uint8_t program_flat = 0;
volatile uint8_t state = 0;
uint8_t service_addr = 255;

mode_mcu state_mode;
/*********************************************************************
 * @fn      USARTx_CFG
 *
 * @brief   Initializes the USART2 & USART3 peripheral.
 *
 * @return  none
 */
void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}


uint8_t GET_NUM_FLAT(uint8_t  * fl)
{
    volatile uint8_t flat = 0;
    Delay_Ms(20);
    flat = 0;
    uint16_t port_value = GPIOC->INDR;
    flat = (port_value&0b00100000)<<2;
    Delay_Ms(1);
    flat |= (port_value&0b01000000);
    Delay_Ms(1);
    flat |= (port_value&0b10000000)>>2;
    Delay_Ms(1);
    flat |= (port_value&0b00010000)>>1;

    port_value = GPIOD->INDR;
    flat |= (port_value&0b00000001)<<4;
    Delay_Ms(1);
    //flat |= (GPIOD->INDR&0b00000010)<<2;

    Delay_Ms(1);
    flat |= (port_value&0b00000100);
    Delay_Ms(1);
    flat |= (port_value&0b00001000)>>2;
    Delay_Ms(1);
    flat |= (port_value&0b00010000)>>4;
    *fl = flat;
    return flat;
}

void CALL_BEGIN()
{
    //uint8_t fl = 0;
    //program_flat = GET_NUM_FLAT(&fl);
    //program_flat = GET_NUM_FLAT(&fl);
    state = 1;
    state = 1;
}

void RESET_BEGIN()
{
    //uint8_t fl = 0;
    //program_flat = GET_NUM_FLAT(&fl);
    //program_flat = GET_NUM_FLAT(&fl);
    state = 0;
    state = 0;
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
#if (SDI_PRINT == SDI_PR_OPEN)
    SDI_Printf_Enable();
#else
    USART_Printf_Init(115200);
#endif
    printf("SystemClk:%d\r\n",SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    USARTx_CFG();
    ///while(1);
    //EXTI0_INT_INIT();
    EXTI1_INT_INIT();
    GPIO_INIT();
    //BEGIN_CALL();
    uint8_t fl = 0;
    program_flat = GET_NUM_FLAT(&fl);
    INIT_TIMER();

    while(1)
    {


        if(GET_ADDR_TRANSMIT_BEGIN() > 0)
        {
                uint16_t prev_flat = 0;
                uint16_t recieve_flat = 0;
                Delay_Ms(70);
                recieve_flat = GET_SEND_FLAT();
                printf("num flat = %d\r\n", recieve_flat);
                clear_flat_cnt();
                if(recieve_flat != ERROR_SEND_FLAT)
                {
                    if((recieve_flat == program_flat)  && (TEST_FEED() == 0))
                    {
                        BEGIN_CALL();
                        state_mode = SLEEP_MODE;
                        break;
                    }
                    else {
                        if((recieve_flat == program_flat)  && (TEST_FEED() == 1))
                        {
                            ANSWER_REAUEST();
                        }
                        else {
                            if(recieve_flat == PROGRAM_FLAT)
                            {
                                BEGIN_CALL();
                            }
                        }
                    }
                    while(1);
                }
        }
/*        state= state;
        if(state == 1)
        {
            uint16_t prev_flat = 0;
            uint16_t recieve_flat = 0;
            clear_flat_cnt();
            while(1)
            {
                state = 2;
                state = 2;
                uint16_t cnt_wait = 0;
                while(GET_ADDR_TRANSMIT_BEGIN() == 0)
                {
                    Delay_Ms(1);
                    cnt_wait++;
                    if(cnt_wait == WAIT_TIME_NUMBER)
                    {
                        break;
                    }
                }
                Delay_Ms(70);

                recieve_flat = GET_SEND_FLAT();
                printf("num flat = %d\r\n", recieve_flat);
                if(recieve_flat != ERROR_SEND_FLAT)
                {
                    if(recieve_flat ==  program_flat)
                    {
                        BEGIN_CALL();
                        state_mode = SLEEP_MODE;
                        break;
                    }
                    else {
                        state = 2;
                        state = 2;
                        state_mode = SLEEP_MODE;
                        break;
                    }
                }
                else {
                    break;
                }
            }

        }
        else {
            if(state_mode == SLEEP_MODE)
            {
                ///PWR_EnterSTANDBYMode(PWR_STANDBYEntry_WFE);
            }
        }*/
    }
}
