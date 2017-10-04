/*
 * Jiga_UDC.c
 *
 *  Created on: 22/06/2017
 *      Author: ali.faraj
 */

#include "F28M36x_ELP_DRS.h"
#include "Jiga_UDC.h"

void InitPeripheralsDrivers(void){
	/* Initialization of PWM modules */

	#define PWM_FREQ 10240.0

    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;			// Auxiliar GPIO for which GPTRIP1 is selected
	GpioDataRegs.GPASET.bit.GPIO29 = 1;
	EDIS;

	PWM_Modules.N_modules = 8;
	PWM_Modules.PWM_Regs[0] = &EPwm1Regs;
	PWM_Modules.PWM_Regs[1] = &EPwm2Regs;
	PWM_Modules.PWM_Regs[2] = &EPwm3Regs;
	PWM_Modules.PWM_Regs[3] = &EPwm4Regs;
	PWM_Modules.PWM_Regs[4] = &EPwm5Regs;
	PWM_Modules.PWM_Regs[5] = &EPwm6Regs;
	PWM_Modules.PWM_Regs[6] = &EPwm7Regs;
	PWM_Modules.PWM_Regs[7] = &EPwm8Regs;

	DisablePWMOutputs();
	DisablePWM_TBCLK();
	//InitPWM_MEP_SFO();

	InitPWMModule(PWM_Modules.PWM_Regs[0], PWM_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);
	InitPWMModule(PWM_Modules.PWM_Regs[1], PWM_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);
	InitPWMModule(PWM_Modules.PWM_Regs[2], PWM_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);
	InitPWMModule(PWM_Modules.PWM_Regs[3], PWM_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);

	InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm3Gpio();
	InitEPwm4Gpio();

	EnablePWMOutputs();
	EnablePWM_TBCLK();

	// Set 50% duty cycle
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[0], 0.5);
	SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[0], 0.5);
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[1], 0.5);
	SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[1], 0.5);
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[2], 0.5);
	SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[2], 0.5);
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[3], 0.5);
	SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[3], 0.5);

	/* Initialization of GPIOs */

	EALLOW;

	GpioCtrlRegs.GPDMUX2.bit.GPIO126 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO126 = 1;		// GPDI1
	GpioCtrlRegs.GPDDIR.bit.GPIO126 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO127 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO127 = 1;		// GPDI2
	GpioCtrlRegs.GPDDIR.bit.GPIO127 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO124 = 1;		// GPDI3
	GpioCtrlRegs.GPDDIR.bit.GPIO124 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO125 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO125 = 1;		// GPDI4
	GpioCtrlRegs.GPDDIR.bit.GPIO125 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO195 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO195 = 1;		// GPDI5
	GpioG2CtrlRegs.GPGDIR.bit.GPIO195 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO116 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO116 = 1;		// GPDI6
	GpioCtrlRegs.GPDDIR.bit.GPIO116 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO194 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO194 = 1;		// GPDI7
	GpioG2CtrlRegs.GPGDIR.bit.GPIO194 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO192 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO192 = 1;		// GPDI8
	GpioG2CtrlRegs.GPGDIR.bit.GPIO192 = 0;

	GpioCtrlRegs.GPDMUX1.bit.GPIO109 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO109 = 1;		// GPDI9
	GpioCtrlRegs.GPDDIR.bit.GPIO109 = 0;

	GpioCtrlRegs.GPDMUX1.bit.GPIO110 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO110 = 1;		// GPDI10
	GpioCtrlRegs.GPDDIR.bit.GPIO110 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO113 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO113 = 1;		// GPDI11
	GpioCtrlRegs.GPDDIR.bit.GPIO113 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO112 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO112 = 1;		// GPDI12
	GpioCtrlRegs.GPDDIR.bit.GPIO112 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO197 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO197 = 1;		// GPDI13
	GpioG2CtrlRegs.GPGDIR.bit.GPIO197 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO196 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO196 = 1;		// GPDI14
	GpioG2CtrlRegs.GPGDIR.bit.GPIO196 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO198 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO198 = 1;		// GPDI15
	GpioG2CtrlRegs.GPGDIR.bit.GPIO198 = 0;

	GpioG2CtrlRegs.GPGMUX1.bit.GPIO199 = 0;
	GpioG2DataRegs.GPGCLEAR.bit.GPIO199 = 1;		// GPDI16
	GpioG2CtrlRegs.GPGDIR.bit.GPIO199= 0;


	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
	GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;		// GPDO1
	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;

	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
	GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;		// GPDO2
	GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;

	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;
	GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;		// GPDO3
	GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;

	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;
	GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;		// GPDO4
	GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;


	GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO8 = 1; // EPWM5A ou PWM_OF9
	GpioCtrlRegs.GPADIR.bit.GPIO8 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;   // EPWM5B ou PWM_OF10
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1; // EPWM6A ou PWM_OF11
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;  // EPWM6B ou PWM_OF12
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; // EPWM7A ou PWM_OF13
	GpioCtrlRegs.GPADIR.bit.GPIO12 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO13 = 1; // EPWM7B ou PWM_OF14
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO14 = 1; // EPWM8A ou PWM_OF15
	GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;

	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1; // EPWM8B ou PWM_OF16
	GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;

	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;   // PWM_SOC
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;

	GpioCtrlRegs.GPEMUX1.bit.GPIO129 = 1;
	GpioDataRegs.GPECLEAR.bit.GPIO129 = 1; // GPIO_CS2
	GpioCtrlRegs.GPEDIR.bit.GPIO129 = 1;

	GpioCtrlRegs.GPEMUX1.bit.GPIO130 = 1;
	GpioDataRegs.GPECLEAR.bit.GPIO130 = 1; // GPIO_CS1
	GpioCtrlRegs.GPEDIR.bit.GPIO130 = 1;

	GpioCtrlRegs.GPEMUX1.bit.GPIO131 = 1;
	GpioDataRegs.GPECLEAR.bit.GPIO131 = 1; // GPIO_CONFIG
	GpioCtrlRegs.GPEDIR.bit.GPIO131 = 1;

	EDIS;
}



void pwm_adc_test(){

	float duty, volt_mean[8], volt_expected[100], volt_measured[8][100], sqres[8], sqtot[8], r2[8];
	volatile unsigned long ulLoop;
	int i, j, samples;

	i = 0;
	for(duty = 0.01; duty<=1.0; duty = duty + 0.01){
		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[0], duty);
		SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[0], duty);
		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[1], duty);
		SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[1], duty);
		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[2], duty);
		SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[2], duty);
		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[3], duty);
		SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[3], duty);
		for (ulLoop=0;ulLoop<50000;ulLoop++){};
		volt_expected[i] = 3.99*duty;
		for(j=0;j<8;j++){
			volt_measured[j][i] = DP_Framework_MtoC.NetSignals[j];
			volt_mean[j] = volt_mean[j] + volt_measured[j][i];
		}
		i++;
	}
	samples = i;
	for(j=0;j<8;j++){
		volt_mean[j] = volt_mean[j]/samples;
	}
	for(i=0;i<samples;i++){
		for(j=0;j<8;j++){
			sqtot[j] = sqtot[j] + pow((volt_measured[j][i] - volt_mean[j]),2);
			sqres[j] = sqres[j] + pow((volt_measured[j][i] - volt_expected[i]),2);
		}
	}
	for(j=0;j<8;j++){
		r2[j] = 1 - sqres[j]/sqtot[j];
	}

	// TODO : Colocar R2 no IPC CtoM

}


// EPWM 9 =  4Hz
// EPWM 11 = 16Hz
void buzzer_led_test(){
	DisablePWMOutputs();
	DisablePWM_TBCLK();

	InitPWMModule(&EPwm9Regs, BUZZER_PITCH_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);
	InitPWMModule(&EPwm11Regs, 0.0, 0, MasterPWM, 0, NO_COMPLEMENTARY, 0);

	EALLOW;

	EPwm11Regs.TBPRD = BUZZER_MOD_PERIOD;					// Set frequency manually

	EPwm11Regs.TBCTL.bit.CLKDIV = BUZZER_MOD_CLKDIV;		// Set clock pre-scaler, due to
	EPwm11Regs.TBCTL.bit.HSPCLKDIV = BUZZER_MOD_HSPCLKDIV;	// very low frequency

	GpioCtrlRegs.GPEMUX1.bit.GPIO128 = 1;   				// Configure GPIO128 as EPWM9A
	GpioCtrlRegs.GPEMUX1.bit.GPIO132 = 1; 	  				// Configure GPIO132 as EPWM11A

	// Disable trip
	EPwm9Regs.TZSEL.bit.OSHT1 = 0;
	EPwm11Regs.TZSEL.bit.OSHT1 = 0;

	// Enable PWM outputs
	EPwm9Regs.TZCLR.bit.OST = 1;
	EPwm11Regs.TZCLR.bit.OST = 1;



	// Set 50% duty cycle
	SetPWMDutyCycle_ChA(&EPwm9Regs, 0.8);
	SetPWMDutyCycle_ChA(&EPwm11Regs, 0.5);

	EDIS;

	EnablePWMOutputs();
	EnablePWM_TBCLK();




}


void gpio_test(){
	Uint16 read_low, read_high, gpio_read = 0;

	GpioDataRegs.GPCSET.bit.GPIO67 = 1;		// GPDO1
	read_high = GpioDataRegs.GPDDAT.bit.GPIO126;		// GPDI1
	GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;		// GPDO1
	read_low = GpioDataRegs.GPDDAT.bit.GPIO126;
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0b1;
	}

	GpioDataRegs.GPCSET.bit.GPIO65 = 1;		// GPDO2
	read_high = GpioDataRegs.GPDDAT.bit.GPIO127;	// GPDI2
	GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;		// GPDO2
	read_low = GpioDataRegs.GPDDAT.bit.GPIO127;	// GPDI2
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x2;
	}

	GpioDataRegs.GPCSET.bit.GPIO66 = 1;		// GPDO3
	read_high = GpioDataRegs.GPDDAT.bit.GPIO124;		// GPDI3
	GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;		// GPDO3
	read_low = GpioDataRegs.GPDDAT.bit.GPIO124;		// GPDI3
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x4;
	}

	GpioDataRegs.GPCSET.bit.GPIO64 = 1;		// GPDO4
	read_high = GpioDataRegs.GPDDAT.bit.GPIO125;		// GPDI4
	GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;		// GPDO4
	read_low = GpioDataRegs.GPDDAT.bit.GPIO125;		// GPDI4
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x8;
	}

	GpioDataRegs.GPESET.bit.GPIO130 = 1; // GPIO_CS1
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO195;		// GPDI5
	GpioDataRegs.GPECLEAR.bit.GPIO130 = 1; // GPIO_CS1
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO195;		// GPDI5
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x10;
	}

	GpioDataRegs.GPESET.bit.GPIO129 = 1; // GPIO_CS2
	read_high = GpioDataRegs.GPDDAT.bit.GPIO116;		// GPDI6
	GpioDataRegs.GPECLEAR.bit.GPIO129 = 1; // GPIO_CS2
	read_low = GpioDataRegs.GPDDAT.bit.GPIO116;		// GPDI6
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x20;
	}

	GpioDataRegs.GPESET.bit.GPIO131 = 1; // GPIO_CONFIG
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO194;		// GPDI7
	GpioDataRegs.GPECLEAR.bit.GPIO131 = 1; // GPIO_CONFIG
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO194;		// GPDI7
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x40;
	}

	GpioDataRegs.GPBSET.bit.GPIO32 = 1;   // PWM_SOC
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO192;		// GPDI8
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;   // PWM_SOC
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO192;		// GPDI8
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x80;
	}

	GpioDataRegs.GPASET.bit.GPIO8 = 1; // EPWM5A ou PWM_OF9
	read_high = GpioDataRegs.GPDDAT.bit.GPIO109;		// GPDI9
	GpioDataRegs.GPACLEAR.bit.GPIO8 = 1; // EPWM5A ou PWM_OF9
	read_low = GpioDataRegs.GPDDAT.bit.GPIO109;		// GPDI9
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x100;
	}

	GpioDataRegs.GPASET.bit.GPIO9 = 1;   // EPWM5B ou PWM_OF10
	read_high = GpioDataRegs.GPDDAT.bit.GPIO110;		// GPDI10
	GpioDataRegs.GPACLEAR.bit.GPIO9 = 1;   // EPWM5B ou PWM_OF10
	read_low = GpioDataRegs.GPDDAT.bit.GPIO110;		// GPDI10
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x200;
	}

	GpioDataRegs.GPASET.bit.GPIO10 = 1; // EPWM6A ou PWM_OF11
	read_high = GpioDataRegs.GPDDAT.bit.GPIO113;		// GPDI11
	GpioDataRegs.GPACLEAR.bit.GPIO10 = 1; // EPWM6A ou PWM_OF11
	read_low = GpioDataRegs.GPDDAT.bit.GPIO113;		// GPDI11
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x400;
	}

	GpioDataRegs.GPASET.bit.GPIO11 = 1;  // EPWM6B ou PWM_OF12
	read_high = GpioDataRegs.GPDDAT.bit.GPIO112;		// GPDI12
	GpioDataRegs.GPACLEAR.bit.GPIO11 = 1;  // EPWM6B ou PWM_OF12
	read_low = GpioDataRegs.GPDDAT.bit.GPIO112;		// GPDI12
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x800;
	}

	GpioDataRegs.GPASET.bit.GPIO12 = 1; // EPWM7A ou PWM_OF13
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO197;		// GPDI13
	GpioDataRegs.GPACLEAR.bit.GPIO12 = 1; // EPWM7A ou PWM_OF13
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO197;		// GPDI13
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x1000;
	}

	GpioDataRegs.GPASET.bit.GPIO13 = 1; // EPWM7B ou PWM_OF14
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO196;		// GPDI14
	GpioDataRegs.GPACLEAR.bit.GPIO13 = 1; // EPWM7B ou PWM_OF14
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO196;		// GPDI14
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x2000;
	}

	GpioDataRegs.GPASET.bit.GPIO14 = 1; // EPWM8A ou PWM_OF15
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO198;		// GPDI15
	GpioDataRegs.GPACLEAR.bit.GPIO14 = 1; // EPWM8A ou PWM_OF15
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO198;		// GPDI15
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x4000;
	}

	GpioDataRegs.GPASET.bit.GPIO15 = 1; // EPWM8B ou PWM_OF16
	read_high = GpioG2DataRegs.GPGDAT.bit.GPIO199;		// GPDI16
	GpioDataRegs.GPACLEAR.bit.GPIO15 = 1; // EPWM8B ou PWM_OF16
	read_low = GpioG2DataRegs.GPGDAT.bit.GPIO199;		// GPDI16
	if(read_low == 0 && read_high == 1){
		gpio_read |= 0x8000;
	}

	// TODO: colocar gpio_read em IPC_CtoM
}

void main_jiga_udc(void){
	InitPeripheralsDrivers();
	//gpio_test();
	buzzer_led_test();

	/*
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[0], (arm_controls.PWM1 * 0.01));
    SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[0], (arm_controls.PWM2 * 0.01));
    SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[1], (arm_controls.PWM3 * 0.01));
    SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[1], (arm_controls.PWM4 * 0.01));
    SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[2], (arm_controls.PWM5 * 0.01));
    SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[2], (arm_controls.PWM6 * 0.01));
    SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[3], (arm_controls.PWM7 * 0.01));
    SetPWMDutyCycle_ChB(PWM_Modules.PWM_Regs[3], (arm_controls.PWM8 * 0.01));

    c28_values.GPDI13 = GpioG2DataRegs.GPGDAT.bit.GPIO197;      // GPDI13
    c28_values.GPDI14 = GpioG2DataRegs.GPGDAT.bit.GPIO196;      // GPDI14
    c28_values.GPDI15 = GpioG2DataRegs.GPGDAT.bit.GPIO198;      // GPDI15
    c28_values.GPDI16 = GpioG2DataRegs.GPGDAT.bit.GPIO199;      // GPDI16
    c28_values.GPDI5 = GpioG2DataRegs.GPGDAT.bit.GPIO195;       // GPDI5
    c28_values.GPDI7 = GpioG2DataRegs.GPGDAT.bit.GPIO194;       // GPDI7
    c28_values.GPDI8 = GpioG2DataRegs.GPGDAT.bit.GPIO192;       // GPDI8
    */

}
