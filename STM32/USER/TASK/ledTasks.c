#include "ledTasks.h"
void LED_Show2(void);//LED…¡À∏
extern int led_flag;
void led_Task_Function(void const * argument)
{
    int flag = 1;
    while(1)
    {
#ifdef DEBUG

#else
        if(flag)
        {
            flag=0;
            LED_Show();
        }

        if(led_flag == 1)
        {
            LED_Show2();
            led_flag = 0;
        }
#endif
        osDelay(3000);
    }
}

//add by LiWenwen
void LED_Show2(void)
{
    int i;
    for(i=0; i<2; i++)
    {

        LED_SW(1);
        delay_ms(100);
        LED_SW(0);
        delay_ms(100);
    }
}
