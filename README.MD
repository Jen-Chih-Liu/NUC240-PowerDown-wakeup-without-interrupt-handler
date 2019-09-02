NUC240 PowerDown wakeup without interrupt handler
the keypoint is disable global interrupt before wfi instruction and set __ISB(); to refresh instruction cache.

    __disable_irq();
    __WFI();
    __ISB(); 

after to clear interrupt flag and NVIC_ICPR, as below.
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
				