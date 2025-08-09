#include "debug.h"

void INIT_TIMER()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    TIM_Cmd(TIM1, DISABLE );
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0}; 
    TIM_TimeBaseInitStructure.TIM_Period = 65000;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 7999; 
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
}