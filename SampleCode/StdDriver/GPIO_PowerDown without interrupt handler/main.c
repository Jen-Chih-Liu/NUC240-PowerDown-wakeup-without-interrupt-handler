
#include <stdio.h>
#include "NUC230_240.h"


#define PLL_CLOCK           72000000

/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power Down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void PowerDownFunction(void)
{
    /* To check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    SCB->SCR = 4;

    CLK->PWRCON = (CLK->PWRCON & ~(CLK_PWRCON_PWR_DOWN_EN_Msk | CLK_PWRCON_PD_WAIT_CPU_Msk)) |
                  CLK_PWRCON_PD_WAIT_CPU_Msk | CLK_PWRCON_PD_WU_INT_EN_Msk;
    CLK->PWRCON |= CLK_PWRCON_PWR_DOWN_EN_Msk;
    __disable_irq();
    __WFI();
	  __ISB(); 

}

void GPAB_IRQHandler(void)
{
    /* To check if PB.3 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT3)) {
        GPIO_CLR_INT_FLAG(PB, BIT3);
        printf("PB.3 INT occurred.\n");
    } else {
        /* Un-expected interrupt. Just clear all PA, PB interrupts */
        PA->ISRC = PA->ISRC;
        PB->ISRC = PB->ISRC;
        printf("Un-expected interrupts.\n");
    }
}

void PWRWU_IRQHandler(void)
{
		PA1^=1;
   printf("PDWU_IRQHandler running...\n");

   CLK->PWRCON  |= CLK_PWRCON_PD_WU_STS_Msk; /* clear interrupt */
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+-------------------------------------------------------+\n");
    printf("|    GPIO Power-Down and Wake-up by PB.3 Sample Code    |\n");
    printf("+-------------------------------------------------------+\n\n");

    /* Configure PB.3 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PB, BIT3, GPIO_PMD_INPUT);
    GPIO_EnableInt(PB, 3, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPAB_IRQn);

		 CLK->PWRCON |= CLK_PWRCON_PD_WU_INT_EN_Msk;  /* Enable wake up interrupt source */
    NVIC_EnableIRQ(PWRWU_IRQn);   /* Enable IRQ request for PDWU interrupt */
	
   
        printf("Enter to Power-Down ......\n");
        PowerDownFunction();
        UART_WAIT_TX_EMPTY(UART0);
        printf("System waken-up done.\n\n");
				 /* Waiting for PB.3 rising-edge interrupt event */
				 
				 if(GPIO_GET_INT_FLAG(PB, BIT3)) {
        GPIO_CLR_INT_FLAG(PB, BIT3);
        printf("PB.3 INT happend\n");
        }
				 
				if   (CLK->PWRCON& CLK_PWRCON_PD_WU_STS_Msk) 
				{
        CLK->PWRCON  |= CLK_PWRCON_PD_WU_STS_Msk; 
				printf("wakeup happend\n\r");
        }
				
				
				printf("NVIC_ICPR=0x%x\n\r", NVIC->ICPR[0]);
				NVIC->ICPR[0]= 0x10000010;
				printf("clear NVIC_ICPR=0x%x\n\r", NVIC->ICPR[0]);
				
    while(1) {
    }

}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
