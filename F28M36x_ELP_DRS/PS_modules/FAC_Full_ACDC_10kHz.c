/*
 * 		FILE: 		FAC_Full_ACDC_10kHz.c
 * 		PROJECT: 	DRS v2.0
 * 		CREATION:
 * 		MODIFIED:	03/05/2016
 *
 * 		AUTHOR: 	Gabriel O. B.  (LNLS/ELP)
 *
 * 		DESCRIPTION: Firmware for control of AC/DC stage of prototype FAC v2.0
 *
 *
 *		TODO:
 *				- Interlocks
 *				- Malhas de corrente e tens�o
 *				- Decima��o do controle de tens�o
 */

#include "F28M36x_ELP_DRS.h"
#include "FAC_Full_ACDC_10kHz.h"

// Prototype statements for functions found within this file.

#pragma CODE_SECTION(isr_ePWM_CTR_ZERO, "ramfuncs");
#pragma CODE_SECTION(isr_ePWM_CTR_ZERO_1st, "ramfuncs");
#pragma CODE_SECTION(PS_turnOn, "ramfuncs");
#pragma CODE_SECTION(PS_turnOff, "ramfuncs");
#pragma CODE_SECTION(Set_SoftInterlock, "ramfuncs");
#pragma CODE_SECTION(Set_HardInterlock, "ramfuncs");
#pragma CODE_SECTION(isr_SoftInterlock, "ramfuncs");
#pragma CODE_SECTION(isr_HardInterlock, "ramfuncs");

static __interrupt void isr_ePWM_CTR_ZERO(void);
static __interrupt void isr_ePWM_CTR_ZERO_1st(void);

void main_FAC_Full_ACDC_10kHz(void);

static void PS_turnOn(void);
static void PS_turnOff(void);

static void Set_SoftInterlock(Uint32 itlk);
static void Set_HardInterlock(Uint32 itlk);
static interrupt void isr_SoftInterlock(void);
static interrupt void isr_HardInterlock(void);

static void InitPeripheralsDrivers(void);
static void InitControllers(void);
static void ResetControllers(void);
static void InitInterruptions(void);

volatile static Uint32 valorCounter;


void main_FAC_Full_ACDC_10kHz(void)
{
	InitPeripheralsDrivers();

	InitControllers();

	InitInterruptions();

	ConfigCpuTimer(&CpuTimer0, C28_FREQ_MHZ, 10000000);
	CpuTimer1Regs.TCR.all = 0xC000;

	stop_DMA();
	DELAY_US(5);
	start_DMA();
	EnablePWM_TBCLK();

	while(1)
	{
		if(PIN_STATUS_DCDC_INTERLOCK)
		{
			if(CHECK_INTERLOCK(DCDC_FAULT))
			{
				Set_HardInterlock(DCDC_FAULT);
			}
		}

		if(IPC_CtoM_Msg.PSModule.OnOff && !PIN_STATUS_AC_CONTACTOR)
		{
			if(CHECK_INTERLOCK(AC_FAULT))
			{
				Set_HardInterlock(AC_FAULT);
			}
		}

		if(CHECK_INTERLOCKS)
		{
			PIN_CLEAR_ACDC_INTERLOCK;
		}

		TunningPWM_MEP_SFO();
	}
}

/*
 * Initialization of peripheral drivers:
 *
 * 		- HRADC boards
 * 		- PWM modules
 * 		- GPIO's
 * 		- Timers
 */

static void InitPeripheralsDrivers(void)
{
	/* Initialization of HRADC boards */

	stop_DMA();

	Init_DMA_McBSP_nBuffers(4, DECIMATION_FACTOR, HRADC_SPI_CLK);

	Init_SPIMaster_McBSP(HRADC_SPI_CLK);
	Init_SPIMaster_Gpio();
	InitMcbspa20bit();

	HRADCs_Info.HRADC_boards[0] = &HRADC0_board;
	HRADCs_Info.HRADC_boards[1] = &HRADC1_board;
	HRADCs_Info.HRADC_boards[2] = &HRADC2_board;
	HRADCs_Info.HRADC_boards[3] = &HRADC3_board;

	Init_HRADC_Info(HRADCs_Info.HRADC_boards[0], 0, DECIMATION_FACTOR, buffers_HRADC.buffer_0, TRANSDUCER_0_GAIN, HRADC_0_R_BURDEN);
	Init_HRADC_Info(HRADCs_Info.HRADC_boards[1], 1, DECIMATION_FACTOR, buffers_HRADC.buffer_1, TRANSDUCER_1_GAIN, HRADC_1_R_BURDEN);
	Init_HRADC_Info(HRADCs_Info.HRADC_boards[2], 2, DECIMATION_FACTOR, buffers_HRADC.buffer_2, TRANSDUCER_2_GAIN, HRADC_2_R_BURDEN);
	Init_HRADC_Info(HRADCs_Info.HRADC_boards[3], 3, DECIMATION_FACTOR, buffers_HRADC.buffer_3, TRANSDUCER_3_GAIN, HRADC_3_R_BURDEN);

	Config_HRADC_board(HRADCs_Info.HRADC_boards[0], TRANSDUCER_0_OUTPUT_TYPE, HEATER_DISABLE, RAILS_DISABLE);
	Config_HRADC_board(HRADCs_Info.HRADC_boards[1], TRANSDUCER_1_OUTPUT_TYPE, HEATER_DISABLE, RAILS_DISABLE);
	Config_HRADC_board(HRADCs_Info.HRADC_boards[2], TRANSDUCER_2_OUTPUT_TYPE, HEATER_DISABLE, RAILS_DISABLE);
	Config_HRADC_board(HRADCs_Info.HRADC_boards[3], TRANSDUCER_3_OUTPUT_TYPE, HEATER_DISABLE, RAILS_DISABLE);

	AverageFilter = 1.0/((float) DECIMATION_FACTOR);

	Config_HRADC_SoC(HRADC_FREQ_SAMP);

	/* Initialization of PWM modules */

    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;
	GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;			// Auxiliar GPIO for which GPTRIP1 is selected
	GpioDataRegs.GPASET.bit.GPIO29 = 1;
	EDIS;

	PWM_Modules.N_modules = 2;
	PWM_Modules.PWM_Regs[0] = &EPwm1Regs;
	PWM_Modules.PWM_Regs[1] = &EPwm2Regs;

	DisablePWMOutputs();
	DisablePWM_TBCLK();
	InitPWM_MEP_SFO();

	InitPWMModule(PWM_Modules.PWM_Regs[0], PWM_FREQ, 0, MasterPWM, 0, NO_COMPLEMENTARY, PWM_DEAD_TIME);
	InitPWMModule(PWM_Modules.PWM_Regs[1], PWM_FREQ, 0, SlavePWM, 0, NO_COMPLEMENTARY, PWM_DEAD_TIME);

	InitPWMDAC(PWM_DAC_MODULE, PWM_DAC_FREQ);

	InitEPwm1Gpio();
	InitEPwm2Gpio();
	InitEPwm4Gpio();

	/* Initialization of GPIOs */

	EALLOW;

	GpioCtrlRegs.GPDMUX2.bit.GPIO126 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO126 = 1;		// GPDI1: AC mains contactor status
	GpioCtrlRegs.GPDDIR.bit.GPIO126 = 0;

	GpioCtrlRegs.GPDMUX2.bit.GPIO127 = 0;
	GpioDataRegs.GPDCLEAR.bit.GPIO127 = 1;		// GPDI2: DC/DC Interlock
	GpioCtrlRegs.GPDDIR.bit.GPIO127 = 0;

	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;
	GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;		// GPDO1: AC mains contactor switch
	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;

	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;
	GpioDataRegs.GPCSET.bit.GPIO65 = 1;			// GPDO2: AC/DC Interlock
	GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;

	INIT_DEBUG_GPIO1;

	EDIS;

	/* Initialization of timers */
	InitCpuTimers();
	CpuTimer0Regs.TCR.bit.TIE = 0;
	CpuTimer1Regs.TCR.bit.TIE = 0;
	CpuTimer2Regs.TCR.bit.TIE = 0;
}

static void InitControllers(void)
{
	/* Initialization of IPC module */
	InitIPC(&PS_turnOn, &PS_turnOff, &isr_SoftInterlock, &isr_HardInterlock);

	/* Initiaization of DP Framework */
	InitDP_Framework(&DP_Framework, &(IPC_CtoM_Msg.PSModule.IRef));

	/*****************************************************/
	/* INITIALIZATION OF CAP BANKS VOLTAGE CONTROL LOOPS */
	/*****************************************************/

	/*
	 * 	      name: 	SRLIM_V_CAPBANK_REFERENCE
	 * description: 	Cap bank voltage reference slew-rate limiter
	 *    DP class:     ELP_SRLim
	 *     	    in:		DP_Framework.Ref
	 * 		   out:		NetSignals[0]
	 */

	Init_ELP_SRLim(SRLIM_V_CAPBANK_REFERENCE, MAX_REF_SLEWRATE, (CONTROL_FREQ/V_CONTROL_DECIMATION), DP_Framework.Ref, &DP_Framework.NetSignals[0]);

	/*********** MODULE 1 ***********/

	/*
	 * 	      name: 	V_CAPBANK1_ERROR_CALCULATOR
	 * description: 	Cap bank 1 voltage reference error
	 *    DP class:     ELP_Error
	 *     	    +:		NetSignals[0]
	 *     	    -:		NetSignals[9]
	 * 		   out:		NetSignals[10]
	 */

	Init_ELP_Error(V_CAPBANK1_ERROR_CALCULATOR, &DP_Framework.NetSignals[0], &DP_Framework.NetSignals[9], &DP_Framework.NetSignals[10]);

	/*
	 * 	      name: 	PI_DAWU_CONTROLLER_VCAPBANK1
	 * description: 	Cap bank 1 voltage PI controller
	 *    DP class:     ELP_PI_dawu
	 *     	    in:		NetSignals[10]
	 * 		   out:		NetSignals[11]
	 */

	Init_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK1, KP_VCAPBANK, KI_VCAPBANK, (CONTROL_FREQ/V_CONTROL_DECIMATION), MAX_IIN_REF, -MAX_IIN_REF, &DP_Framework.NetSignals[10], &DP_Framework.NetSignals[11]);

	/*
	 * 	      name: 	NF_V_CAPBANK1_2HZ
	 * description: 	Cap bank 1 voltage notch filter (Fcut = 2 Hz)
	 *    DP class:     ELP_IIR_2P2Z
	 *     	    in:		NetSignals[6]
	 * 		   out:		NetSignals[8]
	 */

	Init_ELP_NF_IIR(NF_V_CAPBANK1_2HZ, NF_ALPHA, 2.0, (CONTROL_FREQ/V_CONTROL_DECIMATION), FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[6], &DP_Framework.NetSignals[8]);
	//Init_ELP_IIR_2P2Z(NF_V_CAPBANK1_2HZ, 0.998441278934479, -1.996732234954830, 0.998441278934479,
	//					  -1.996732234954830, 0.996882617473602, FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[6], &DP_Framework.NetSignals[8]);

	/*
	 * 	      name: 	NF_V_CAPBANK1_4HZ
	 * description: 	Cap bank 1 voltage notch filter (Fcut = 4 Hz)
	 *    DP class:     ELP_IIR_2P2Z
	 *     	    in:		NetSignals[8]
	 * 		   out:		NetSignals[9]
	 */

	Init_ELP_NF_IIR(NF_V_CAPBANK1_4HZ, NF_ALPHA, 4.0, (CONTROL_FREQ/V_CONTROL_DECIMATION), FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[8], &DP_Framework.NetSignals[9]);
	//Init_ELP_IIR_2P2Z(NF_V_CAPBANK1_4HZ, 0.998441278934479, -1.996281147003170, 0.998441278934479,
	//					  -1.996281147003170, 0.996882617473602, FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[8], &DP_Framework.NetSignals[9]);

	/*********** MODULE 2 ***********/

	/*
	 * 	      name: 	V_CAPBANK2_ERROR_CALCULATOR
	 * description: 	Cap bank 2 voltage reference error
	 *    DP class:     ELP_Error
	 *     	    +:		NetSignals[0]
	 *     	    -:		NetSignals[14]
	 * 		   out:		NetSignals[15]
	 */

	Init_ELP_Error(V_CAPBANK2_ERROR_CALCULATOR, &DP_Framework.NetSignals[0], &DP_Framework.NetSignals[14], &DP_Framework.NetSignals[15]);

	/*
	 * 	      name: 	PI_DAWU_CONTROLLER_VCAPBANK2
	 * description: 	Cap bank 2 voltage PI controller
	 *    DP class:     ELP_PI_dawu
	 *     	    in:		NetSignals[15]
	 * 		   out:		NetSignals[16]
	 */

	Init_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK2, KP_VCAPBANK, KI_VCAPBANK, (CONTROL_FREQ/V_CONTROL_DECIMATION), MAX_IIN_REF, -MAX_IIN_REF, &DP_Framework.NetSignals[15], &DP_Framework.NetSignals[16]);

	/*
	 * 	      name: 	NF_V_CAPBANK2_2HZ
	 * description: 	Cap bank 2 voltage notch filter (Fcut = 2 Hz)
	 *    DP class:     ELP_IIR_2P2Z
	 *     	    in:		NetSignals[7]
	 * 		   out:		NetSignals[13]
	 */

	Init_ELP_NF_IIR(NF_V_CAPBANK2_2HZ, NF_ALPHA, 2.0, (CONTROL_FREQ/V_CONTROL_DECIMATION), FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[7], &DP_Framework.NetSignals[13]);
	//Init_ELP_IIR_2P2Z(NF_V_CAPBANK2_2HZ, 0.998441278934479, -1.996732234954830, 0.998441278934479,
	//					  -1.996732234954830, 0.996882617473602, FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[7], &DP_Framework.NetSignals[13]);

	/*
	 * 	      name: 	NF_V_CAPBANK2_4HZ
	 * description: 	Cap bank 2 voltage notch filter (Fcut = 4 Hz)
	 *    DP class:     ELP_IIR_2P2Z
	 *     	    in:		NetSignals[13]
	 * 		   out:		NetSignals[14]
	 */

	Init_ELP_NF_IIR(NF_V_CAPBANK2_4HZ, NF_ALPHA, 4.0, (CONTROL_FREQ/V_CONTROL_DECIMATION), FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[13], &DP_Framework.NetSignals[14]);
	//Init_ELP_IIR_2P2Z(NF_V_CAPBANK2_4HZ, 0.998441278934479, -1.996281147003170, 0.998441278934479,
	//					  -1.996281147003170, 0.996882617473602, FLT_MAX, -FLT_MAX, &DP_Framework.NetSignals[13], &DP_Framework.NetSignals[14]);


	/***********************************************************/
	/* INITIALIZATION OF INPUT INDUCTOR CURRENTS CONTROL LOOPS */
	/***********************************************************/

	/*********** MODULE 1 ***********/

	/*
	 * 	      name: 	IIN1_ERROR_CALCULATOR
	 * description: 	Input inductor current 1 reference error
	 *    DP class:     ELP_Error
	 *     	     +:		NetSignals[11]
	 *     	     -:		NetSignals[4]
	 * 		   out:		NetSignals[12]
	 */

	Init_ELP_Error(IIN1_ERROR_CALCULATOR, &DP_Framework.NetSignals[11], &DP_Framework.NetSignals[4], &DP_Framework.NetSignals[12]);

	/*
	 * 	      name: 	PI_DAWU_CONTROLLER_IIN1
	 * description: 	Input inductor current 1 PI controller
	 *    DP class:     ELP_PI_dawu
	 *     	    in:		NetSignals[12]
	 * 		   out:		DutySignals[0]
	 */
	Init_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN1, KP_IIN, KI_IIN, CONTROL_FREQ, PWM_MAX_DUTY, -PWM_MAX_DUTY, &DP_Framework.NetSignals[12], &DP_Framework.DutySignals[0]);

	/*
	 * 	      name: 	IIR_3P3Z_CONTROLLER_IIN1
	 * description: 	Input inductor current IIR 1 3P3Z controller
	 *    DP class:     ELP_IIR_3P3Z
	 *     	    in:		NetSignals[12]
	 * 		   out:		DutySignals[0]
	 */

	Init_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN1, 4.018979021166166e-04, -3.464285079728809e-04, -4.002609202934738e-04, 3.480654897960237e-04, -2.459643017305727e+00,
					  1.983576904041343e+00, -5.239338867356150e-01, PWM_MAX_DUTY, -PWM_MAX_DUTY, &DP_Framework.NetSignals[12], &DP_Framework.DutySignals[0]);

	/*********** MODULE 2 ***********/

	/*
	 * 	      name: 	IIN2_ERROR_CALCULATOR
	 * description: 	Input inductor current 2 reference error
	 *    DP class:     ELP_Error
	 *     	     +:		NetSignals[16]
	 *     	     -:		NetSignals[5]
	 * 		   out:		NetSignals[17]
	 */

	Init_ELP_Error(IIN2_ERROR_CALCULATOR, &DP_Framework.NetSignals[16], &DP_Framework.NetSignals[5], &DP_Framework.NetSignals[17]);

	/*
	 * 	      name: 	PI_DAWU_CONTROLLER_IIN2
	 * description: 	Input inductor current 2 PI controller
	 *    DP class:     ELP_PI_dawu
	 *     	    in:		NetSignals[17]
	 * 		   out:		DutySignals[1]
	 */
	Init_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN2, KP_IIN, KI_IIN, CONTROL_FREQ, PWM_MAX_DUTY, -PWM_MAX_DUTY, &DP_Framework.NetSignals[17], &DP_Framework.DutySignals[1]);

	/*
	 * 	      name: 	IIR_3P3Z_CONTROLLER_IIN2
	 * description: 	Input inductor current 2 IIR 3P3Z controller
	 *    DP class:     ELP_IIR_3P3Z
	 *     	    in:		NetSignals[17]
	 * 		   out:		DutySignals[1]
	 */

	Init_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN2, 4.018979021166166e-04, -3.464285079728809e-04, -4.002609202934738e-04, 3.480654897960237e-04, -2.459643017305727e+00,
					  1.983576904041343e+00, -5.239338867356150e-01, PWM_MAX_DUTY, -PWM_MAX_DUTY, &DP_Framework.NetSignals[17], &DP_Framework.DutySignals[1]);

	/*********************************************/
	/* INITIALIZATION OF SIGNAL GENERATOR MODULE */
	/*********************************************/

	/*
	 * 	      name: 	SignalGenerator
	 * description: 	Signal generator module
	 * 		   out:		DP_Framework.Ref
	 */

	Disable_ELP_SigGen(&SignalGenerator);
	Init_ELP_SigGen(&SignalGenerator, Sine, 0.0, 0.0, 10, (CONTROL_FREQ/V_CONTROL_DECIMATION), &IPC_MtoC_Msg.SigGen.Freq,
					&DP_Framework.NetSignals[N_MAX_NET_SIGNALS-2], &DP_Framework.NetSignals[N_MAX_NET_SIGNALS-1], &IPC_MtoC_Msg.SigGen.Aux, DP_Framework.Ref);

	/*
	 * 	      name: 	SRLIM_SIGGEN_AMP
	 * description: 	Signal generator amplitude slew-rate limiter
	 *    DP class:     ELP_SRLim
	 *     	    in:		IPC_MtoC_Msg.SigGen.Amplitude
	 * 		   out:		NetSignals[N_MAX_NET_SIGNALS-2]
	 */

	Init_ELP_SRLim(SRLIM_SIGGEN_AMP, MAX_SR_SIGGEN_AMP, (CONTROL_FREQ/V_CONTROL_DECIMATION), IPC_MtoC_Msg.SigGen.Amplitude, &DP_Framework.NetSignals[N_MAX_NET_SIGNALS-2]);

	/*
	 * 	      name: 	SRLIM_SIGGEN_OFFSET
	 * description: 	Signal generator offset slew-rate limiter
	 *    DP class:     ELP_SRLim
	 *     	    in:		IPC_MtoC_Msg.SigGen.Amplitude
	 * 		   out:		NetSignals[N_MAX_NET_SIGNALS-1]
	 */

	Init_ELP_SRLim(SRLIM_SIGGEN_OFFSET, MAX_SR_SIGGEN_OFFSET, (CONTROL_FREQ/V_CONTROL_DECIMATION), &IPC_MtoC_Msg.SigGen.Offset, &DP_Framework.NetSignals[N_MAX_NET_SIGNALS-1]);


	/**********************************/
	/* INITIALIZATION OF TIME SLICERS */
	/**********************************/

	// 0: Time-slicer for WfmRef sweep decimation
	Set_TimeSlicer(0, (CONTROL_FREQ/V_CONTROL_DECIMATION));

	// 1: Time-slicer for SamplesBuffer
	Set_TimeSlicer(1, BUFFER_DECIMATION);

	// 2: Time-slicer for cap bank voltage control loop
	Set_TimeSlicer(2, V_CONTROL_DECIMATION);

	ResetControllers();
}

static void ResetControllers(void)
{
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[0], 0.0);
	SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[1], 0.0);

	Reset_ELP_SRLim(SRLIM_V_CAPBANK_REFERENCE);

	Reset_ELP_Error(V_CAPBANK1_ERROR_CALCULATOR);
	Reset_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK1);
	Reset_ELP_IIR_2P2Z(NF_V_CAPBANK1_2HZ);
	Reset_ELP_IIR_2P2Z(NF_V_CAPBANK1_4HZ);

	Reset_ELP_Error(V_CAPBANK2_ERROR_CALCULATOR);
	Reset_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK2);
	Reset_ELP_IIR_2P2Z(NF_V_CAPBANK2_2HZ);
	Reset_ELP_IIR_2P2Z(NF_V_CAPBANK2_4HZ);

	Reset_ELP_Error(IIN1_ERROR_CALCULATOR);
	Reset_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN1);
	Reset_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN1);

	Reset_ELP_Error(IIN2_ERROR_CALCULATOR);
	Reset_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN2);
	Reset_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN2);

	Reset_ELP_SRLim(SRLIM_SIGGEN_AMP);
	Reset_ELP_SRLim(SRLIM_SIGGEN_OFFSET);

	IPC_CtoM_Msg.PSModule.IRef = 0.0;

	Reset_TimeSlicers();
}

/*
 * Initialization of application interruptions
 *
 * 		- PWM interruptions as main ISR for control loop (INT3)
 * 		- IPC interruptions (INT11)
 * 		-
 */

static void InitInterruptions(void)
{
	EALLOW;
	PieVectTable.EPWM1_INT =  &isr_ePWM_CTR_ZERO_1st;
	EDIS;

	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;  // ePWM1
	EnablePWMInterrupt(PWM_Modules.PWM_Regs[0]);

	IER |= M_INT3;
	IER |= M_INT11;

	DELAY_US(3000000);

	/* Enable global interrupts (EINT) */
	EINT;
	ERTM;
}

//*****************************************************************************
// Esvazia buffer FIFO com valores amostrados e recebidos via SPI
//*****************************************************************************
__interrupt void isr_ePWM_CTR_ZERO(void)
{
	static Uint16 i, bypass_SRLim;
	static float temp0, temp1, temp2, temp3;

	StartCpuTimer0();
	//SET_DEBUG_GPIO1;

	temp0 = 0.0;
	temp1 = 0.0;
	temp2 = 0.0;
	temp3 = 0.0;

	bypass_SRLim = USE_MODULE;

	for(i = 0; i < DECIMATION_FACTOR; i++)
	{
		temp0 += (float) *(HRADCs_Info.HRADC_boards[0]->SamplesBuffer++);
		temp1 += (float) *(HRADCs_Info.HRADC_boards[1]->SamplesBuffer++);
		temp2 += (float) *(HRADCs_Info.HRADC_boards[2]->SamplesBuffer++);
		temp3 += (float) *(HRADCs_Info.HRADC_boards[3]->SamplesBuffer++);
	}

	HRADCs_Info.HRADC_boards[0]->SamplesBuffer = buffers_HRADC.buffer_0;
	HRADCs_Info.HRADC_boards[1]->SamplesBuffer = buffers_HRADC.buffer_1;
	HRADCs_Info.HRADC_boards[2]->SamplesBuffer = buffers_HRADC.buffer_2;
	HRADCs_Info.HRADC_boards[3]->SamplesBuffer = buffers_HRADC.buffer_3;

	temp0 *= AverageFilter;
	temp1 *= AverageFilter;
	temp2 *= AverageFilter;
	temp3 *= AverageFilter;

	temp0 -= *(HRADCs_Info.HRADC_boards[0]->offset);
	temp0 *= *(HRADCs_Info.HRADC_boards[0]->gain);

	temp1 -= *(HRADCs_Info.HRADC_boards[1]->offset);
	temp1 *= *(HRADCs_Info.HRADC_boards[1]->gain);

	temp2 -= *(HRADCs_Info.HRADC_boards[2]->offset);
	temp2 *= *(HRADCs_Info.HRADC_boards[2]->gain);

	temp3 -= *(HRADCs_Info.HRADC_boards[3]->offset);
	temp3 *= *(HRADCs_Info.HRADC_boards[3]->gain);

	DP_Framework.NetSignals[6] = temp0;
	DP_Framework.NetSignals[4] = temp1;
	DP_Framework.NetSignals[7] = temp2;
	DP_Framework.NetSignals[5] = temp3;

	if((fabs(temp0) > MAX_LOAD) || (fabs(temp2) > MAX_LOAD))
	{
		if(CHECK_INTERLOCK(LOAD_OVERVOLTAGE))
		{
			Set_HardInterlock(LOAD_OVERVOLTAGE);
		}
	}

	if((fabs(temp1) > MAX_IIN) || (fabs(temp3) > MAX_IIN))
	{
		if(CHECK_INTERLOCK(LOAD_OVERCURRENT))
		{
			Set_HardInterlock(LOAD_OVERCURRENT);
		}
	}

	RUN_TIMESLICE(2); /************************************************************/

		SET_DEBUG_GPIO1;

		Run_ELP_IIR_2P2Z(NF_V_CAPBANK1_2HZ);
		Run_ELP_IIR_2P2Z(NF_V_CAPBANK1_4HZ);

		Run_ELP_IIR_2P2Z(NF_V_CAPBANK2_2HZ);
		Run_ELP_IIR_2P2Z(NF_V_CAPBANK2_4HZ);

		if(IPC_CtoM_Msg.PSModule.OnOff)
		{
			switch(IPC_CtoM_Msg.PSModule.OpMode)
			{
				case FastRef:
					bypass_SRLim = BYPASS_MODULE;
					break;

				case WfmRef:
					RUN_TIMESLICE(0); /************************************************************/

						if(IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferK <= IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferEnd)
						{
							IPC_CtoM_Msg.PSModule.IRef = *(IPC_CtoM_Msg.WfmRef.BufferInfo.PtrBufferK++) * IPC_CtoM_Msg.WfmRef.Gain + IPC_CtoM_Msg.WfmRef.Offset;
						}
						else
						{
							CLEAR_DEBUG_GPIO1;
						}

					END_TIMESLICE(0); /************************************************************/

					bypass_SRLim = BYPASS_MODULE;
					break;

				case SigGen:
					Run_ELP_SRLim(SRLIM_SIGGEN_AMP, USE_MODULE);
					Run_ELP_SRLim(SRLIM_SIGGEN_OFFSET, USE_MODULE);
					SignalGenerator.Run_ELP_SigGen(&SignalGenerator);
					bypass_SRLim = BYPASS_MODULE;
					break;

				default:
					break;
			}

			Run_ELP_SRLim(SRLIM_V_CAPBANK_REFERENCE,bypass_SRLim);

			if(IPC_CtoM_Msg.PSModule.OpenLoop)
			{
				DP_Framework.DutySignals[0] = 0.01*DP_Framework.NetSignals[0];				//
				DP_Framework.DutySignals[1] = 0.01*DP_Framework.NetSignals[0];				// For open loop, Iref value represents duty-cycle
				SATURATE(DP_Framework.DutySignals[0], PWM_MAX_DUTY_OL, PWM_MIN_DUTY_OL);	// in percentage (0 - 100 A => 0 - 100 %)
				SATURATE(DP_Framework.DutySignals[1], PWM_MAX_DUTY_OL, PWM_MIN_DUTY_OL);	//
			}
			else
			{
				SATURATE(DP_Framework.NetSignals[0], MAX_REF, MIN_REF);

				Run_ELP_Error(V_CAPBANK1_ERROR_CALCULATOR);
				Run_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK1);
				SATURATE(DP_Framework.NetSignals[11], MAX_IIN_REF, MIN_IIN_REF);

				Run_ELP_Error(V_CAPBANK2_ERROR_CALCULATOR);
				Run_ELP_PI_dawu(PI_DAWU_CONTROLLER_VCAPBANK2);
				SATURATE(DP_Framework.NetSignals[16], MAX_IIN_REF, MIN_IIN_REF);
			}
		}

	END_TIMESLICE(2); /************************************************************/

	if(IPC_CtoM_Msg.PSModule.OnOff)
	{
		if(!IPC_CtoM_Msg.PSModule.OpenLoop)
		{
			Run_ELP_Error(IIN1_ERROR_CALCULATOR);
			Run_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN1);
			//Run_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN1);
			SATURATE(DP_Framework.DutySignals[0], PWM_MAX_DUTY, PWM_MIN_DUTY);

			Run_ELP_Error(IIN2_ERROR_CALCULATOR);
			Run_ELP_PI_dawu(PI_DAWU_CONTROLLER_IIN2);
			//Run_ELP_IIR_3P3Z(IIR_3P3Z_CONTROLLER_IIN2);
			SATURATE(DP_Framework.DutySignals[1], PWM_MAX_DUTY, PWM_MIN_DUTY);
		}

		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[0], DP_Framework.DutySignals[0]);
		SetPWMDutyCycle_ChA(PWM_Modules.PWM_Regs[1], DP_Framework.DutySignals[1]);
	}


	RUN_TIMESLICE(1); /************************************************************/

		WriteBuffer(&IPC_CtoM_Msg.SamplesBuffer, DP_Framework.NetSignals[9]);
		WriteBuffer(&IPC_CtoM_Msg.SamplesBuffer, DP_Framework.NetSignals[14]);

	END_TIMESLICE(1); /************************************************************/

	for(i = 0; i < PWM_Modules.N_modules; i++)
	{
		PWM_Modules.PWM_Regs[i]->ETCLR.bit.INT = 1;
	}

	CLEAR_DEBUG_GPIO1;

	StopCpuTimer0();
	valorCounter = ReadCpuTimer0Counter();
	ReloadCpuTimer0();

	PieCtrlRegs.PIEACK.all |= M_INT3;
}

static __interrupt void isr_ePWM_CTR_ZERO_1st(void)
{
	// Contador auxiliar
	static Uint16 i;

    // Remapeia a ISR que esvazia buffer FIFO
	EALLOW;
    PieVectTable.EPWM1_INT = &isr_ePWM_CTR_ZERO;
    EDIS;

	for(i = 0; i < PWM_Modules.N_modules; i++)
	{
		PWM_Modules.PWM_Regs[i]->ETSEL.bit.INTSEL = ET_CTR_ZERO;
		PWM_Modules.PWM_Regs[i]->ETCLR.bit.INT = 1;
	}

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all |= M_INT3;
}

static void Set_SoftInterlock(Uint32 itlk)
{
	PS_turnOff();
	IPC_CtoM_Msg.PSModule.SoftInterlocks |= itlk;
	//SendIpcFlag(SOFT_INTERLOCK_CTOM);
}

static void Set_HardInterlock(Uint32 itlk)
{
	PS_turnOff();
	IPC_CtoM_Msg.PSModule.HardInterlocks |= itlk;
	SendIpcFlag(HARD_INTERLOCK_CTOM);

	if(itlk != DCDC_FAULT)
	{
		PIN_SET_ACDC_INTERLOCK;
	}
}

static interrupt void isr_SoftInterlock(void)
{
	CtoMIpcRegs.MTOCIPCACK.all = SOFT_INTERLOCK_MTOC;

	PS_turnOff();
	IPC_CtoM_Msg.PSModule.SoftInterlocks |= IPC_MtoC_Msg.PSModule.SoftInterlocks;
	//SendIpcFlag(SOFT_INTERLOCK_CTOM);

	PieCtrlRegs.PIEACK.all |= M_INT11;
}

static interrupt void isr_HardInterlock(void)
{
	CtoMIpcRegs.MTOCIPCACK.all = HARD_INTERLOCK_MTOC;

	PS_turnOff();
	IPC_CtoM_Msg.PSModule.HardInterlocks |= IPC_MtoC_Msg.PSModule.HardInterlocks;
	//SendIpcFlag(HARD_INTERLOCK_CTOM);

	if(IPC_MtoC_Msg.PSModule.HardInterlocks != DCDC_FAULT)
	{
		PIN_SET_ACDC_INTERLOCK;
	}

	PieCtrlRegs.PIEACK.all |= M_INT11;
}

static void PS_turnOn(void)
{
	if(CHECK_INTERLOCKS && CHECK_SOFTINTERLOCK(DCDC_FAULT))
	{
		//ResetControllers();

		PIN_CLOSE_AC_CONTACTOR;
		DELAY_US(TIMEOUT_AC_CONTACTOR);

		if(PIN_STATUS_AC_CONTACTOR)
		{
			IPC_CtoM_Msg.PSModule.IRef = 0.0;
			IPC_CtoM_Msg.PSModule.OpenLoop = 1;
			IPC_CtoM_Msg.PSModule.OnOff = 1;
			EnablePWMOutputs();
		}
		else
		{
			Set_HardInterlock(AC_FAULT);
		}
	}
}

static void PS_turnOff(void)
{
	DisablePWMOutputs();

	PIN_OPEN_AC_CONTACTOR;

	IPC_CtoM_Msg.PSModule.OnOff = 0;
	IPC_CtoM_Msg.PSModule.OpenLoop = 1;

	ResetControllers();
}
