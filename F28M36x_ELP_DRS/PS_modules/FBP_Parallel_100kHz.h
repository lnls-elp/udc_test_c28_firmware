#ifndef FBP_PARALLEL_100KHZ_H
#define FBP_PARALLEL_100KHZ_H

/*
 * Especifica��es da fonte
 */

#define FONTE_MODO		OnePS4Q				// Topologia da fonte a ser controlada
#define	n_PS			1

#define PWM_FREQ	   			49152.0		// Frequencia do sinal PWM gerado [Hz]
#define PWM_DEAD_TIME			300			// Dead-time dos sinais PWM [ns]
#define PWM_MAX_DUTY    		0.9         // Maximo ciclo de trabalho para o PWM (em p.u.)
#define PWM_MIN_DUTY    		-0.9		// Minimo ciclo de trabalho para o PWM (em p.u.)
#define PWM_MAX_DUTY_OL			0.9			// Maximo ciclo de trabalho para o PWM em malha aberta (em p.u.)
#define PWM_MIN_DUTY_OL			-0.9		// Minimo ciclo de trabalho para o PWM em malha aberta (em p.u.)

#define MAX_REF					20.0		// Valor m�ximo da refer�ncia de corrente na carga [A]
#define MIN_REF					-20.0		// Valor m�nimo da refer�ncia de corrente na carga [A]
#define MAX_LOAD				20.5		// Limite de corrente na carga para interlock [A]

#define MAX_REF_SLEWRATE		1000000.0	// Slew-rate m�ximo (A/s)
#define MAX_SR_SIGGEN_OFFSET	50.0		// Slew-rate m�ximo do offset do gerador senoidal (A/s)
#define MAX_SR_SIGGEN_AMP		100.0		// Slew-rate m�ximo da amplitude do gerador senoidal (A/s)

#define KP						2.8
#define KI						470.0

#define CONTROL_FREQ			(2.0*PWM_FREQ)
#define CONTROL_PERIOD			(1.0/CONTROL_FREQ)
#define DECIMATION_FACTOR		5
#define TRANSFER_BUFFER_SIZE	DECIMATION_FACTOR
#define HRADC_FREQ_SAMP			(float) CONTROL_FREQ*DECIMATION_FACTOR
#define HRADC_SPI_CLK			SPI_15MHz

#define PWM_DAC_FREQ			(100.0*CONTROL_FREQ)
#define PWM_DAC_MODULE			&EPwm4Regs

#define	BUFFER_DECIMATION		48

#define TRANSDUCER_0_INPUT_RATED	10.0			// 		 HITEC 10A/10V:
#define TRANSDUCER_0_OUTPUT_RATED	10.0			//   In_rated 	= +/- 10 A
#define TRANSDUCER_0_OUTPUT_TYPE	Vin_bipolar		//   Out_rated 	= +/- 10 V
#define TRANSDUCER_0_GAIN			TRANSDUCER_0_INPUT_RATED/TRANSDUCER_0_OUTPUT_RATED
#define	HRADC_0_R_BURDEN			20

#define TRANSDUCER_1_INPUT_RATED	110.0			// Divisor de Tens�o + Isolador Verivolt IsoBlock V-4:
#define TRANSDUCER_1_OUTPUT_RATED	10.0			//   In_rated 	= +/- 110 V
#define TRANSDUCER_1_OUTPUT_TYPE	Vin_bipolar		//   Out_rated 	= +/- 10 V
#define TRANSDUCER_1_GAIN			TRANSDUCER_1_INPUT_RATED/TRANSDUCER_1_OUTPUT_RATED

#define HRADC_R_BURDEN				1.0				// Resistor Burden = 1 R
#define HRADC_VIN_BI_P_GAIN			(20.0/262144.0)
#define HRADC_IIN_BI_P_GAIN			(1.0/(HRADC_R_BURDEN * 131072.0))

extern void main_FBP_Parallel_100kHz(void);

#endif
