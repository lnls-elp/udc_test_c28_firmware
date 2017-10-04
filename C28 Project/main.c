/*
 * 		FILE: 		main_FAC_ACDC_v2_0_2.c
 * 		PROJECT: 	DRS v2.0
 * 		CREATION:	28/10/2015
 * 		MODIFIED:
 *
 * 		AUTHOR: 	Gabriel O. B.  (LNLS/ELP)
 *
 * 		DESCRIPTION: Firmware for control of AC/DC stage of prototype FAC v2.0
 *
 *
 *		TODO:
 *				- Incluir teste da flag do ARM que libera o C28 para inicializa��o
 */

#include <string.h>
#include "F28M36x_ELP_DRS.h"

/*extern Uint16 RamfuncsLoadStart_RAML3;
extern Uint16 RamfuncsLoadSize_RAML3;
extern Uint16 RamfuncsRunStart_RAML3;*/

extern void main_FBP_100kHz(void);
extern void main_FAC_ACDC_10kHz(void);
extern void main_FAC_DCDC_20kHz(void);
extern void main_FAC_Full_ACDC_10kHz(void);
extern void main_FAC_Full_DCDC_20kHz(void);
extern void main_FAP_ACDC(void);
extern void main_FAP_DCDC_20kHz(void);
extern void main_Test_HRPWM(void);
extern void main_Test_HRADC(void);
extern void main_Jiga_HRADC(void);
extern void main_FAP_DCDC_15kHz_225A(void);
extern void main_FBPx4_100kHz(void);


void main(void)
{
	volatile unsigned long ulLoop;


	// Initialize the Control System:
	// Enable peripheral clocks
	// This example function is found in the F28M36x_SysCtrl.c file.
	InitSysCtrl();

	// Copy time critical code and Flash setup code to RAM
	// This includes the following functions:  InitFlash();
	// The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
	// symbols are created by the linker. Refer to the device .cmd file.
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    //memcpy(&RamfuncsRunStart_RAML3, &RamfuncsLoadStart_RAML3, (size_t)&RamfuncsLoadSize_RAML3);

	// Call Flash Initialization to setup flash waitstates
	// This function must reside in RAM
	InitFlash();

	// Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;

	// Initialize the PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the F28M36x_PieCtrl.c file.
	InitPieCtrl();

	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in F28M36x_DefaultIsr.c.
	// This function is found in F28M36x_PieVect.c.
	InitPieVectTable();


	/*
	 * 	TODO: Antes de realizar esta inicializa��o, garantir que ARM j� fez a sua
	 */


	main_jiga_udc();

	while(1)
    {
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
    }

}


