#ifndef FAC_ACDC_10KHZ_H
#define FAC_ACDC_10KHZ_H

/*
 * PS Specs
 */

#define FONTE_MODO		OnePS1Q				// Topologia da fonte a ser controlada
#define	n_PS			1

/*
 * PWM Specs
 */

#define PWM_FREQ	   		10240.0		// Frequencia do sinal PWM gerado [Hz]
#define PWM_DEAD_TIME		0			// Dead-time dos sinais PWM [ns]
#define PWM_MAX_DUTY    	0.9         // Maximo ciclo de trabalho para o PWM (em p.u.)
#define PWM_MIN_DUTY    	0.0			// Minimo ciclo de trabalho para o PWM (em p.u.)
#define PWM_MAX_DUTY_OL		0.8			// Maximo ciclo de trabalho para o PWM em malha aberta (em p.u.)
#define PWM_MIN_DUTY_OL		0.0			// Minimo ciclo de trabalho para o PWM em malha aberta (em p.u.)

/*
 * Controller specs
 */

#define MAX_REF				450.0 // 90.0		// Valor m�ximo da refer�ncia de tens�o no banco de capacitores [V]
#define MIN_REF				0.0			// Valor m�ximo da refer�ncia de tens�o no banco de capacitores [V]
#define MAX_LOAD			500.0 // 101.0		// Limite de tens�o no banco de capacitores para interlock [V]
#define MAX_IIN_REF			150.0	//170.0		// Limite da refer�ncia de corrente m�xima na entrada do regulador da tens�o no banco de capacitores [A]
#define MIN_IIN_REF			0.0			// Limite da refer�ncia de corrente m�nima na entrada do regulador da tens�o no banco de capacitores [A]
#define MAX_IIN				180.0		// Limite de corrente m�xima na entrada para interlock [A]

#define MAX_REF_SLEWRATE		50.0	//9.0		// Slew-rate m�ximo [V/s]
#define MAX_SR_SIGGEN_OFFSET	50.0	//9.0		// Slew-rate m�ximo do offset do gerador senoidal [V/s]
#define MAX_SR_SIGGEN_AMP		50.0	//9.0		// Slew-rate m�ximo da amplitude do gerador senoidal [V/s]

#define KP						5.0264
#define KI						3.154

#define KP2						0.002
#define KI2						1.5

#define CONTROL_FREQ			PWM_FREQ
#define CONTROL_PERIOD			(1.0/CONTROL_FREQ)
#define V_CONTROL_DECIMATION	10
#define V_CONTROL_FREQ			1024.0

#define	BUFFER_DECIMATION		5

#define NF_ALPHA				0.99

#define PWM_DAC_FREQ			(200.0*CONTROL_FREQ)
#define PWM_DAC_MODULE			&EPwm4Regs

/*
 * HRADC boards specs
 */

#define DECIMATION_FACTOR				6
#define TRANSFER_BUFFER_SIZE			DECIMATION_FACTOR
#define HRADC_FREQ_SAMP					(float) CONTROL_FREQ*DECIMATION_FACTOR
#define HRADC_SPI_CLK					SPI_15MHz

#define TRANSDUCER_0_INPUT_RATED		500.0	//110.0				// Divisor de Tens�o + Isolador Verivolt IsoBlock V-4:
#define TRANSDUCER_0_OUTPUT_RATED		10.0				//   In_rated 	= +/- 500 V // 110 V
#define TRANSDUCER_0_OUTPUT_TYPE		Vin_bipolar			//   Out_rated 	= +/- 10 V
#define TRANSDUCER_0_GAIN				(TRANSDUCER_0_INPUT_RATED/TRANSDUCER_0_OUTPUT_RATED)
#define HRADC_0_R_BURDEN				20.0				// Resistor Burden = 20 R

#define TRANSDUCER_1_INPUT_RATED		300.0 	//200.0				// LEM LF 310-S // LEM LF 210-S
#define TRANSDUCER_1_OUTPUT_RATED		-0.15	// -0.1				//   In_rated 	= +/- 300 A // 200 A
#define TRANSDUCER_1_OUTPUT_TYPE		Iin_bipolar			//   Out_rated 	= +/- 150 mA // 100 mA
#define TRANSDUCER_1_GAIN				(TRANSDUCER_1_INPUT_RATED/TRANSDUCER_1_OUTPUT_RATED)
#define HRADC_1_R_BURDEN				10.0				// Resistor Burden = 10 R

/*#define HRADC_R_BURDEN				5.0						// Resistor Burden = 5 R
#define HRADC_VIN_BI_P_GAIN			(20.0/262144.0)
#define HRADC_IIN_BI_P_GAIN			(1.0/(HRADC_R_BURDEN * 131072.0))*/

/*
 * DP modules defines
 */

#define SRLIM_V_CAPBANK_REFERENCE 	&DP_Framework.DPlibrary.ELP_SRLim[0]
#define V_ERROR_CALCULATOR			&DP_Framework.DPlibrary.ELP_Error[0]
#define	PI_DAWU_CONTROLLER_VCAPBANK	&DP_Framework.DPlibrary.ELP_PI_dawu[0]
#define	NF_V_CAPBANK_2HZ			&DP_Framework.DPlibrary.ELP_IIR_2P2Z[0]
#define	NF_V_CAPBANK_4HZ			&DP_Framework.DPlibrary.ELP_IIR_2P2Z[1]

#define I_ERROR_CALCULATOR			&DP_Framework.DPlibrary.ELP_Error[1]
#define	PI_DAWU_CONTROLLER_IIN		&DP_Framework.DPlibrary.ELP_PI_dawu[1]
#define	IIR_3P3Z_CONTROLLER_IIN		&DP_Framework.DPlibrary.ELP_IIR_3P3Z[0]

#define SRLIM_SIGGEN_AMP	 		&DP_Framework.DPlibrary.ELP_SRLim[1]
#define SRLIM_SIGGEN_OFFSET 		&DP_Framework.DPlibrary.ELP_SRLim[2]

/*
 * Timeouts
 */

#define TIMEOUT_uS_AC_CONTACTOR		1000000

/*
 * Digital IO's defines
 */

#define PIN_STATUS_AC_CONTACTOR 	GpioDataRegs.GPDDAT.bit.GPIO126
#define PIN_STATUS_EXT_INTERLOCK	!(GpioDataRegs.GPDDAT.bit.GPIO127)
#define PIN_STATUS_DCDC_INTERLOCK	!(GpioDataRegs.GPDDAT.bit.GPIO124)

#define PIN_OPEN_AC_CONTACTOR		GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;
#define PIN_CLOSE_AC_CONTACTOR		GpioDataRegs.GPCSET.bit.GPIO67 = 1;

#define PIN_SET_ACDC_INTERLOCK		GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;
#define PIN_CLEAR_ACDC_INTERLOCK	GpioDataRegs.GPCSET.bit.GPIO65 = 1;

/*
 * Functions prototypes
 */

extern void main_FAC_ACDC_10kHz(void);

#endif
