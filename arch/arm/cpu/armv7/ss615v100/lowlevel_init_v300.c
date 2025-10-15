// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <asm/arch/platform.h>
#include <config.h>
#include <compiler.h>
#include "ddr_training_impl.h"

#define SYS_CTL_REG  SYS_CTRL_REG_BASE

#define OK 0
#define ERROR (-1)
#define DDR_CA0_OFST		0x24
#define DDR_CA1_OFST		0x28
#define DDR_CA2_OFST		0x2c
#define REG_BASE_DDRC		   0x11130000

#define DDRC_CTRL_SREF_OFST	 (0x8000 + 0x0)
#define DDRC_CFG_DDRMODE_OFST    (0x8000 + 0X50)
#define DDRC_CURR_FUNC_OFST	 (0x8000 + 0x294)

#define DDRC1_CTRL_SREF_OFST	(0x9000 + 0x0)
#define DDRC1_CFG_DDRMODE_OFST  (0x9000 + 0X50)
#define DDRC1_CURR_FUNC_OFST	(0x9000 + 0x294)

#define DDRC2_CTRL_SREF_OFST	(0xa000 + 0x0)
#define DDRC2_CFG_DDRMODE_OFST  (0xa000 + 0X50)
#define DDRC2_CURR_FUNC_OFST	(0xa000 + 0x294)

#define DDRC3_CTRL_SREF_OFST	(0xb000 + 0x0)
#define DDRC3_CFG_DDRMODE_OFST  (0xb000 + 0X50)
#define DDRC3_CURR_FUNC_OFST	(0xb000 + 0x294)

#define DDRC_SELF_REFURBISH_MASK	0x1

static inline void delay(unsigned int num)
{
	volatile unsigned int i;

	for (i = 0; i < (100 * num); i++) /* 100: Cycle */
		__asm__ __volatile__("nop");
}

static inline void dwb(void) /* drain write buffer */
{
}

static inline unsigned int readl(unsigned addr)
{
	unsigned int val;

	val = (*(volatile unsigned int *)(uintptr_t)(addr));
	return val;
}

static inline void writel(unsigned val, unsigned addr)
{
	dwb();
	(*(volatile unsigned *)(uintptr_t)(addr)) = (uintptr_t)(val);
	dwb();
}

#define REG_BASE_MISC		0x11024000

#ifdef DDR_SCRAMB_ENABLE

#undef reg_get
#define reg_get(addr) (*(volatile unsigned int *)((long)addr))

#undef reg_set
#define reg_set(addr, val) (*(volatile unsigned int *)((long)addr) = (val))
void ddr_scramb_start(unsigned int random1, unsigned int random2)
{
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA0_OFST), random1);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA1_OFST), random2);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA2_OFST), 0);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA2_OFST), 0x10);
	delay(0x100);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA0_OFST), 0);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA1_OFST), 0);
}

#define REG_BASE_RNG_GEN	0x10130000
#define TRNG_DSTA_FIFO_DATA_OFST	0x204
#define TRNG_DATA_ST_OFST	0x208
#define SEC_COM_TRNG_CTRL_OFST	0x200

#define BIT_TRNG_FIFO_DATA_CNT	  0x8
#define TRNG_FIFO_DATA_CNT_MASK	 0xff
#define TRNG_CTRL_DEF_VAL	   0xa

#define REG_PERI_CRG_TRNG	   0x2d80
#define TRNG_CLK_ENABLE		 (0x1 << 4)
#define TRNG_DO_SRST			(0x1 << 0)
#define TRNG_CLK_DISABLE		~(0x1 << 4)
#define TRNG_UNDO_SRST		  ~(0x1 << 0)


/* get random number */
int get_random_num(void)
{
	unsigned int reg_val;

	do {
		reg_val = reg_get(REG_BASE_RNG_GEN + TRNG_DATA_ST_OFST);
	} while (!((reg_val >> BIT_TRNG_FIFO_DATA_CNT) & TRNG_FIFO_DATA_CNT_MASK));

	reg_val = reg_get(REG_BASE_RNG_GEN + TRNG_DSTA_FIFO_DATA_OFST);

	return reg_val;
}

void trng_init(void)
{
	unsigned int reg_val;

	/* open trng clock */
	reg_val = reg_get(CRG_REG_BASE + REG_PERI_CRG_TRNG);
	reg_val |= TRNG_CLK_ENABLE;
	reg_val &= TRNG_UNDO_SRST;
	reg_set(CRG_REG_BASE + REG_PERI_CRG_TRNG, reg_val);

	/* set trng ctrl register */
	reg_set(REG_BASE_RNG_GEN + SEC_COM_TRNG_CTRL_OFST,
		TRNG_CTRL_DEF_VAL);
}

void trng_deinit(void)
{
	unsigned int reg_val;

	/* close trng clock */
	reg_val = reg_get(CRG_REG_BASE + REG_PERI_CRG_TRNG);
	reg_val &= TRNG_CLK_DISABLE;
	reg_set(CRG_REG_BASE + REG_PERI_CRG_TRNG, reg_val);
}

int ddr_scramb(void)
{
	unsigned int random_num1;
	unsigned int random_num2;
	unsigned int reg_val[4] = {0, 0, 0, 0};
	unsigned int ddrc_isvalid[4] = {0, 0, 0, 0};

	/* read ddrc_cfg_ddrmode register,
	 * if value[3:0] is not 0x0 ,the channel is valid.
	 */
	ddrc_isvalid[0] = (reg_get(REG_BASE_DDRC + DDRC_CFG_DDRMODE_OFST) & 0xf) ? 1 : 0;
	ddrc_isvalid[1] = (reg_get(REG_BASE_DDRC + DDRC1_CFG_DDRMODE_OFST) & 0xf) ? 1 : 0;
	ddrc_isvalid[2] = (reg_get(REG_BASE_DDRC + DDRC2_CFG_DDRMODE_OFST) & 0xf) ? 1 : 0;
	ddrc_isvalid[3] = (reg_get(REG_BASE_DDRC + DDRC3_CFG_DDRMODE_OFST) & 0xf) ? 1 : 0;

	/* set ddrc to do self-refurbish */
	if (ddrc_isvalid[0])
		reg_set(REG_BASE_DDRC + DDRC_CTRL_SREF_OFST, 0x1);
	if (ddrc_isvalid[1])
		reg_set(REG_BASE_DDRC + DDRC1_CTRL_SREF_OFST, 0x1);
	if (ddrc_isvalid[2])
		reg_set(REG_BASE_DDRC + DDRC2_CTRL_SREF_OFST, 0x1);
	if (ddrc_isvalid[3])
		reg_set(REG_BASE_DDRC + DDRC3_CTRL_SREF_OFST, 0x1);

	/* wait the status of ddrc to be sef-refurbish */
	do {
		reg_val[0] = ddrc_isvalid[0] ? (reg_get(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 1;
		reg_val[1] = ddrc_isvalid[1] ? (reg_get(REG_BASE_DDRC + DDRC1_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 1;
		reg_val[2] = ddrc_isvalid[2] ? (reg_get(REG_BASE_DDRC + DDRC2_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 1;
		reg_val[3] = ddrc_isvalid[3] ? (reg_get(REG_BASE_DDRC + DDRC3_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 1;
	} while (!(reg_val[0] & reg_val[1] & reg_val[2] & reg_val[3]));

	trng_init();

	/* get random number */
	random_num1 = get_random_num();
	random_num2 = get_random_num();

	/* start ddr scrambling */
	ddr_scramb_start(random_num1, random_num2);

	/* clear random number */
	random_num1 = get_random_num();
	random_num2 = get_random_num();
	random_num1 = get_random_num();
	random_num2 = get_random_num();

	/* set ddrc to exit self-refurbish */
	if (ddrc_isvalid[0])
		reg_set(REG_BASE_DDRC + DDRC_CTRL_SREF_OFST, (0x1 << 1));
	if (ddrc_isvalid[1])
		reg_set(REG_BASE_DDRC + DDRC1_CTRL_SREF_OFST, (0x1 << 1));
	if (ddrc_isvalid[2])
		reg_set(REG_BASE_DDRC + DDRC2_CTRL_SREF_OFST, (0x1 << 1));
	if (ddrc_isvalid[3])
		reg_set(REG_BASE_DDRC + DDRC3_CTRL_SREF_OFST, (0x1 << 1));

	/* wait the status of ddrc to be normal */
	do {
		reg_val[0] = ddrc_isvalid[0] ? (reg_get(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 0;
		reg_val[1] = ddrc_isvalid[1] ? (reg_get(REG_BASE_DDRC + DDRC1_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 0;
		reg_val[2] = ddrc_isvalid[2] ? (reg_get(REG_BASE_DDRC + DDRC2_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 0;
		reg_val[3] = ddrc_isvalid[3] ? (reg_get(REG_BASE_DDRC + DDRC3_CURR_FUNC_OFST) &
						DDRC_SELF_REFURBISH_MASK) : 0;
	} while (reg_val[0] | reg_val[1] | reg_val[2] | reg_val[3]);

	return OK;
}

#endif /* DDR_SCRAMB_ENABLE */

#define CORE_SVB_PWM_CTRL		0x11029000
#define CPU_SVB_PWM_CTRL		0x11029004

#define HPM_CLK_REG			0x11014A80
#define CPU_HPM_CTRL0_REG		0x1102B000
#define CORE_HPM_CTRL0_REG		0x1102B010
#define HPM_CPU_REG0			0x1102B008
#define HPM_CPU_REG1			0x1102B00c
#define HPM_CORE_REG0			0x1102B018
#define HPM_CORE_REG1			0x1102B01c
#define CYCLE_NUM			 32
#define HPM_RECORD_REG0		0x11020340
#define HPM_RECORD_REG1		0x11020344
#define PCB_LAYER_NUMBER_FLAG   0X11020158
#define VOLTAGE_RECOED_REG		0x1102015C

#define SVB_VERSION_REG		0x11020168
#define SVB_VERSION_SS615V100	0x0003
#define OTP_HPM_CORE_REG		0x11021504
#define OTP_HPM_CPU_REG		0x11021530
#define OTP_VOLTAGE_DELTA_CORE_REG	0x1102150C
#define OTP_VOLTAGE_DELTA_CPU_REG   	0x11021514
#define OTP_VOLTAGE_DELTA_CORE_FOR_TRAINING_REG   	0x11021518

#define TSENSOR_STATUS0			0X1102A008

#define USE_ATE_HPM 0

#define TEMPERATURE_MIN  (-40)
#define TEMPERATURE_MAX  120

#define HPM_CORE_MIN 220
#define HPM_CORE_MAX 350

#define HPM_CPU_MIN 220
#define HPM_CPU_MAX 350

static unsigned hpm_value_avg(const unsigned int val[], unsigned int size)
{
	unsigned int i;
	unsigned tmp = 0;

	for (i = 0; i < size; i++)
		tmp += val[i] >> 2;

	return tmp >> 5;
}

static void get_hpm_value(unsigned int *hpm_core, unsigned int *hpm_cpu)
{
	int i;
	unsigned int temp;
	unsigned int core_value[4] = {0, 0, 0, 0};
	unsigned int cpu_value[4] = {0, 0, 0, 0};

	for (i = 0; i < CYCLE_NUM; i++) {
		delay(18);  /* Delay 18 Cycle */

		/* cpu */
		temp = readl(HPM_CPU_REG0);
		cpu_value[1] += (temp >> 16) & 0x3ff;
		cpu_value[0] += temp & 0x3ff;
		temp = readl(HPM_CPU_REG1);
		cpu_value[3] += (temp >> 16) & 0x3ff;
		cpu_value[2] += temp & 0x3ff;

		/* core */
		temp = readl(HPM_CORE_REG0);
		core_value[1] += (temp >> 16) & 0x3ff;
		core_value[0] += temp & 0x3ff;
		temp = readl(HPM_CORE_REG1);
		core_value[3] += (temp >> 16) & 0x3ff;
		core_value[2] += temp & 0x3ff;
	}

	*hpm_core = hpm_value_avg(core_value, 4);
	*hpm_cpu = hpm_value_avg(cpu_value, 4);
}

static void start_hpm(unsigned int *hpm_core, unsigned int *hpm_cpu)
{
	/* core */
	writel(0x60080001, CORE_HPM_CTRL0_REG);
	/* cpu  */
	writel(0x60080001, CPU_HPM_CTRL0_REG);

	delay(180); /* 180 10ms */

	get_hpm_value(hpm_core, hpm_cpu);
}

union union_hpm_reg0 {
	struct {
		unsigned int sys_hpm_core : 9; /* [8..0] */
		unsigned int reserved_0   : 7; /* [15..9] */
		unsigned int sys_hpm_cpu  : 9; /* [24..16] */
		unsigned int reserved_1   : 7; /* [31..25] */
	} bits;
	unsigned int u32;
};

union union_hpm_reg1 {
	struct {
		unsigned int temperature : 8; /* [7..0] */
		unsigned int reserved_0	: 20; /* [27..8] */
		unsigned int temperature_err : 1; /* [28] */
		unsigned int hpm_cpu_err   : 1; /* [29] */
		unsigned int hpm_core_err  : 1; /* [30] */
		unsigned int from_otp	  : 1; /* [31] */
	} bits;
	unsigned int u32;
};

static void hpm_check(unsigned int *hpm_core, unsigned int *hpm_cpu, int *temperature, unsigned int hpm_from_otp_flag)
{
	union union_hpm_reg0 hpm_reg0; /* SVB_RECORD_REG0 0x11020340 */
	union union_hpm_reg1 hpm_reg1; /* SVB_RECORD_REG1 0x11020344 */

	hpm_reg0.u32 = 0;
	hpm_reg1.u32 = 0;

	if (*hpm_core < HPM_CORE_MIN) {
		*hpm_core = HPM_CORE_MIN;
		hpm_reg1.bits.hpm_core_err = 1;
	}

	if (*hpm_core > HPM_CORE_MAX) {
		*hpm_core = HPM_CORE_MAX;
		hpm_reg1.bits.hpm_core_err = 1;
	}

	if (*hpm_cpu < HPM_CPU_MIN) {
		*hpm_cpu = HPM_CPU_MIN;
		hpm_reg1.bits.hpm_cpu_err = 1;
	}

	if (*hpm_cpu > HPM_CPU_MAX) {
		*hpm_cpu = HPM_CPU_MAX;
		hpm_reg1.bits.hpm_cpu_err = 1;
	}

	if (*temperature < TEMPERATURE_MIN) {
		*temperature = TEMPERATURE_MIN;
		hpm_reg1.bits.temperature_err = 1;
	}

	if (*temperature > TEMPERATURE_MAX) {
		*temperature = TEMPERATURE_MAX;
		hpm_reg1.bits.temperature_err = 1;
	}

	hpm_reg0.bits.sys_hpm_core = *hpm_core;
	hpm_reg0.bits.sys_hpm_cpu = *hpm_cpu;
	hpm_reg1.bits.temperature  =  (unsigned int)*temperature;
	hpm_reg1.bits.from_otp	 = hpm_from_otp_flag;
	writel(hpm_reg0.u32, HPM_RECORD_REG0);
	writel(hpm_reg1.u32, HPM_RECORD_REG1);
}

#define calc_core_bese_formula(val) (326 - ((((unsigned int)* (val)) * 102 + 256) >> 9))
#define calc_cpu_bese_formula(val) (316 - ((((unsigned int)* (val)) * 102 + 256) >> 9))

static void hpm_adjust(unsigned int *hpm_core, unsigned int *hpm_cpu,
		const int *temperature, unsigned int hpm_from_otp_flag)
{
	unsigned int hpm_core_tmp;
	unsigned int hpm_cpu_tmp;
	unsigned int hpm_core_bese_line;
	unsigned int hpm_cpu_bese_line;

	if (hpm_from_otp_flag == 1)
		return;

	if (*temperature < 90) /* 90: temperature value */
		return;

	hpm_core_tmp = *hpm_core;
	hpm_cpu_tmp  = *hpm_cpu;
	hpm_core_bese_line = calc_core_bese_formula(temperature);
	if (hpm_core_tmp > hpm_core_bese_line)
		*hpm_core = hpm_core_tmp + 10; /* 10 : Adjustment Value */

	hpm_cpu_bese_line = calc_cpu_bese_formula(temperature);
	if (hpm_cpu_tmp > hpm_cpu_bese_line)
		*hpm_cpu = hpm_cpu_tmp + 10; /* 10 : Adjustment Value */
}

static void voltage_adjust(unsigned int *core_volt, unsigned int *cpu_volt)
{
	unsigned int core_volt_tmp;
	unsigned int cpu_volt_tmp;
	short otp_voltage_delta_core;
	short otp_voltage_delta_cpu;
	unsigned int pcb_layer_number_flag; /* 0: 4 layers pcb; 1: 2 layers pcb */

	union {
		struct {
			unsigned int voltage_core : 12; /* [11..0] */
			unsigned int voltage_cpu  : 12; /* [23..12] */
			unsigned int reserved : 8; /* [31..24] */
		} bits;
		unsigned int u32;
	} voltage_reg;

	core_volt_tmp = *core_volt;
	cpu_volt_tmp = *cpu_volt;
	otp_voltage_delta_core = (short)(readl(OTP_VOLTAGE_DELTA_CORE_REG) & 0xffff);
	otp_voltage_delta_cpu = (short)(readl(OTP_VOLTAGE_DELTA_CPU_REG) & 0xffff);

	pcb_layer_number_flag = readl(PCB_LAYER_NUMBER_FLAG) & 0x1;
	*core_volt = (unsigned int)((int)core_volt_tmp + otp_voltage_delta_core);

	if (pcb_layer_number_flag == 1)
		*cpu_volt  = (unsigned int)((int)cpu_volt_tmp + otp_voltage_delta_cpu + 10);  /* 10 : Adjustment Value */
	else
		*cpu_volt  = (unsigned int)((int)cpu_volt_tmp + otp_voltage_delta_cpu);

	voltage_reg.u32 = 0;
	voltage_reg.bits.voltage_core = *core_volt;
	voltage_reg.bits.voltage_cpu  = *cpu_volt;

	writel(voltage_reg.u32, VOLTAGE_RECOED_REG);
}

#define calc_core_volt_formula(mv) ((((((1078595 - (mv) * 1000) * 1337) >> 4) - 0x10000) & \
						0xffff0000) + 0x19f5)

static void set_core_volt(unsigned int mv)
{
	unsigned int svb_value;

	svb_value = calc_core_volt_formula(mv);

	writel(svb_value, CORE_SVB_PWM_CTRL);
}

#define calc_cpu_volt_formula(mv) ((((((1097582 - (mv) * 1000) * 1462) >> 4) - 0x10000) & \
							0xffff0000) + 0x19f5)

static void set_cpu_volt(unsigned int mv)
{
	unsigned int svb_value;

	svb_value = calc_cpu_volt_formula(mv);

	writel(svb_value, CPU_SVB_PWM_CTRL);
}

#define CORE_VOLT_MAX    984
#define CORE_VOLT_MIN    858

#define CPU_VOLT_MAX     1030
#define CPU_VOLT_MIN     880

#define calc_hpm_core_formula(value) (1245 - (((value) * 297 * 1000) >> 18))

static void convert_hpm_to_volt_core(unsigned int hpm_core, unsigned int *mv)
{
	unsigned int volt;

	volt = calc_hpm_core_formula(hpm_core);
	if (volt > CORE_VOLT_MAX)
		*mv = CORE_VOLT_MAX;
	else if (volt < CORE_VOLT_MIN)
		*mv = CORE_VOLT_MIN;
	else
		*mv = volt;
}

#define calc_hpm_cpu_formula(value) (1489 - (((value) * 251 * 1000) >> 17))

static void convert_hpm_to_volt_cpu(unsigned int hpm_cpu, unsigned int *mv)
{
	unsigned int volt;

	volt = calc_hpm_cpu_formula(hpm_cpu);

	if (volt > CPU_VOLT_MAX)
		*mv = CPU_VOLT_MAX;
	else if (volt < CPU_VOLT_MIN)
		*mv = CPU_VOLT_MIN;
	else
		*mv = volt;
}

#define calc_temperature_formula(value) ((int)((((int)(value) - 127) * 431) >> 11) - 40)

static void get_temperature(int *temperature)
{
	unsigned int value;

	value = (unsigned int)(readl(TSENSOR_STATUS0) & 0x3ff);
	*temperature = calc_temperature_formula(value);
}

static void get_ate_hpm(unsigned int *pate_hpm_core, unsigned int *pate_hpm_cpu)
{
	*pate_hpm_core = (unsigned int)(readl(OTP_HPM_CORE_REG) & 0xffff);
	*pate_hpm_cpu  = (unsigned int)(readl(OTP_HPM_CPU_REG) & 0xffff);
}

static void get_volt_delta_for_training(short *volt_delta_val)
{
	short volt_delta_tmp;

	volt_delta_tmp = (short)(readl(OTP_VOLTAGE_DELTA_CORE_FOR_TRAINING_REG) & 0xffff);
	if (volt_delta_tmp >= 40) /* 40 : Threshold */
		*volt_delta_val = 40; /* 40 : Adjusting Parameters */
	else if (volt_delta_tmp <= -40) /* -40 : Threshold */
		*volt_delta_val = -40; /* -40 : Adjusting Parameters */
	else
		*volt_delta_val = volt_delta_tmp;
}
typedef struct {
	unsigned int ate_hpm_core;
	unsigned int ate_hpm_cpu;
	unsigned int hpm_core;
	unsigned int hpm_cpu;
	unsigned int core_volt;
	unsigned int cpu_volt;
	unsigned int core_volt_for_training;
} start_svb_val;

static void start_svb(void)
{
	start_svb_val svb_val = {0};
	short volt_delta_val;
	int temperature;
	unsigned int hpm_from_otp;
	unsigned int version;
	unsigned int pcb_layer_number_flag; /* 0: 4 layers pcb; 1: 2 layers pcb */

	version = readl(SVB_VERSION_REG);
	version = (version & 0x0000FFFF) | (SVB_VERSION_SS615V100 << 16);
	writel(version, SVB_VERSION_REG);

	pcb_layer_number_flag = readl(PCB_LAYER_NUMBER_FLAG) & 0x1;

	/* open hmp clock */
	writel(0x10, HPM_CLK_REG); /* 0x10 */
	delay(18);  /* 18: Cycle */
	get_temperature(&temperature);

	if (!USE_ATE_HPM) {
		set_core_volt(990); /* 990: Adjusting Parameters */
		set_cpu_volt(990); /* 990: Adjusting Parameters */
		start_hpm(&svb_val.hpm_core, &svb_val.hpm_cpu);
		hpm_from_otp = 0;
	} else {
		get_ate_hpm(&svb_val.ate_hpm_core, &svb_val.ate_hpm_cpu);
		/* ATE SVB transfer to board SVB */
		svb_val.hpm_core = svb_val.ate_hpm_core;
		svb_val.hpm_cpu = svb_val.ate_hpm_cpu;
		hpm_from_otp = 1;
	}

	hpm_adjust(&svb_val.hpm_core, &svb_val.hpm_cpu, &temperature, hpm_from_otp);

	hpm_check(&svb_val.hpm_core, &svb_val.hpm_cpu, &temperature, hpm_from_otp);

	convert_hpm_to_volt_core(svb_val.hpm_core, &svb_val.core_volt);
	convert_hpm_to_volt_cpu(svb_val.hpm_cpu, &svb_val.cpu_volt);

	voltage_adjust(&svb_val.core_volt, &svb_val.cpu_volt);

	get_volt_delta_for_training(&volt_delta_val);

	if (pcb_layer_number_flag == 1)
		svb_val.core_volt_for_training = (unsigned int)((int)svb_val.core_volt + volt_delta_val - 40); /* 40 : Adjustment Value */
	else
		svb_val.core_volt_for_training = (unsigned int)((int)svb_val.core_volt + volt_delta_val - 20); /* 20 : Adjustment Value */

	set_core_volt(svb_val.core_volt_for_training);
	set_cpu_volt(svb_val.cpu_volt);

	/* delay 10ms do not delete */
	delay(180); /* 180 10ms */
}

static void end_svb(void)
{
	unsigned int core_volt;

	union {
		struct {
			unsigned int voltage_core : 12; /* [11..0] */
			unsigned int voltage_cpu : 12; /* [23..12] */
			unsigned int reserved : 8; /* [31..24] */
		} bits;
		unsigned int u32;
	} voltage_reg;

	voltage_reg.u32 = 0;
	voltage_reg.u32 = readl(VOLTAGE_RECOED_REG);
	core_volt = voltage_reg.bits.voltage_core;

	set_core_volt(core_volt);

	/* delay 10ms do not delete */
	delay(90); /* 90: 5ms */
}

void start_ddr_training(unsigned int base)
{
	start_svb();

	/* ddr hw training */
	ddr_hw_training_if();

	/* ddr sw training */
	ddr_sw_training_if();

	/* ddr DMC auto power down config */
	ddr_dmc_auto_power_down_cfg();

#ifdef DDR_SCRAMB_ENABLE
	/* enable ddr scramb */
	ddr_scramb();
#endif

	end_svb();
}

void prepare_hw(void)
{
	*(unsigned int*)0x11012000 = 0;
	*(unsigned int*)0x11012040 = 0;
}