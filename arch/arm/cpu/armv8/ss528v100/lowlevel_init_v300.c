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

#define OK			0
#define ERROR			(-1)
#define DDR_CA0_OFST		0x24
#define DDR_CA1_OFST		0x28
#define DDR_CA2_OFST		0x2c
#define REG_BASE_DDRC           0x11130000

#define DDRC_CTRL_SREF_OFST     (0x8000 + 0x0)
#define DDRC_CFG_DDRMODE_OFST   (0x8000 + 0X50)
#define DDRC_CURR_FUNC_OFST     (0x8000 + 0x294)

#define DDRC1_CTRL_SREF_OFST    (0x9000 + 0x0)
#define DDRC1_CFG_DDRMODE_OFST  (0x9000 + 0X50)
#define DDRC1_CURR_FUNC_OFST    (0x9000 + 0x294)

#define DDRC2_CTRL_SREF_OFST    (0xa000 + 0x0)
#define DDRC2_CFG_DDRMODE_OFST  (0xa000 + 0X50)
#define DDRC2_CURR_FUNC_OFST    (0xa000 + 0x294)

#define DDRC3_CTRL_SREF_OFST    (0xb000 + 0x0)
#define DDRC3_CFG_DDRMODE_OFST  (0xb000 + 0X50)
#define DDRC3_CURR_FUNC_OFST    (0xb000 + 0x294)

#define DDRC_SELF_REFURBISH_MASK    0x1

#define hpm_cpu_delta1(val) ((-704 * (val) + 164800) / 10000)
#define hpm_cpu_delta2(val) ((-704 * (val) + 174090) / 10000)
#define hpm_cpu_delta3(val) ((980  * (val) - 231720) / 10000)
#define hpm_cpu_delta4(val) ((1550 * (val) - 372420) / 10000)
#define hpm_cpu_delta5(val) ((2638 * (val) - 649800) / 10000)

#define hpm_core_delta1(val) 0
#define hpm_core_delta2(val) 0
#define hpm_core_delta3(val) ((1158 * (val) - 265060) / 10000)
#define hpm_core_delta4(val) ((1522 * (val) - 338360) / 10000)
#define hpm_core_delta5(val) ((2158 * (val) - 461780) / 10000)

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
	(*(volatile unsigned *)(uintptr_t)(addr)) = val;
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
	delay(0x100); /* Delay 0x100 Cycle */
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA0_OFST), 0);
	reg_set((unsigned int *)(REG_BASE_MISC + DDR_CA1_OFST), 0);
}

#define REG_BASE_RNG_GEN		0x10130000
#define TRNG_DSTA_FIFO_DATA_OFST	0x204
#define TRNG_DATA_ST_OFST		0x208
#define SEC_COM_TRNG_CTRL_OFST	0x200

#define BIT_TRNG_FIFO_DATA_CNT		0x8
#define TRNG_FIFO_DATA_CNT_MASK		0xff
#define TRNG_CTRL_DEF_VAL		0xa

#define REG_PERI_CRG_TRNG		0x2d80
#define TRNG_CLK_ENABLE			(0x1 << 4)
#define TRNG_DO_SRST			(0x1 << 0)
#define TRNG_CLK_DISABLE		~(0x1 << 4)
#define TRNG_UNDO_SRST			~(0x1 << 0)

/* get random number */
int get_random_num(void)
{
	unsigned int reg_val;

	do {
		reg_val = reg_get(REG_BASE_RNG_GEN + TRNG_DATA_ST_OFST);
	} while (!((reg_val >> BIT_TRNG_FIFO_DATA_CNT) &
		TRNG_FIFO_DATA_CNT_MASK));

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
	unsigned int ddrca_val[4] = {0, 0, 0, 0};
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
		reg_val[0] = ddrc_isvalid[0] ? (reg_get(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST) & 0x1) : 1;
		reg_val[1] = ddrc_isvalid[1] ? (reg_get(REG_BASE_DDRC + DDRC1_CURR_FUNC_OFST) & 0x1) : 1;
		reg_val[2] = ddrc_isvalid[2] ? (reg_get(REG_BASE_DDRC + DDRC2_CURR_FUNC_OFST) & 0x1) : 1;
		reg_val[3] = ddrc_isvalid[3] ? (reg_get(REG_BASE_DDRC + DDRC3_CURR_FUNC_OFST) & 0x1) : 1;
	} while (!(reg_val[0] & reg_val[1] & reg_val[2] & reg_val[3]));

	if (ddrc_isvalid[0]) {
		ddrca_val[0] = reg_get(REG_BASE_DDRC + 0x8164);
		reg_set(REG_BASE_DDRC + 0x8184, ddrca_val[0]);

		ddrca_val[1] = reg_get(REG_BASE_DDRC + 0x8168);
		reg_set(REG_BASE_DDRC + 0x8188, ddrca_val[1]);

		ddrca_val[2] = reg_get(REG_BASE_DDRC + 0x816c);
		reg_set(REG_BASE_DDRC + 0x8048, ddrca_val[2]);
	}

	if (ddrc_isvalid[1]) {
		ddrca_val[0] = reg_get(REG_BASE_DDRC + 0x9164);
		reg_set(REG_BASE_DDRC + 0x9184, ddrca_val[0]);

		ddrca_val[1] = reg_get(REG_BASE_DDRC + 0x9168);
		reg_set(REG_BASE_DDRC + 0x9188, ddrca_val[1]);

		ddrca_val[2] = reg_get(REG_BASE_DDRC + 0x916c);
		reg_set(REG_BASE_DDRC + 0x9048, ddrca_val[2]);
	}

	if (ddrc_isvalid[2]) {
		ddrca_val[0] = reg_get(REG_BASE_DDRC + 0xa164);
		reg_set(REG_BASE_DDRC + 0xa184, ddrca_val[0]);

		ddrca_val[1] = reg_get(REG_BASE_DDRC + 0xa168);
		reg_set(REG_BASE_DDRC + 0xa188, ddrca_val[1]);

		ddrca_val[2] = reg_get(REG_BASE_DDRC + 0xa16c);
		reg_set(REG_BASE_DDRC + 0xa048, ddrca_val[2]);
	}

	if (ddrc_isvalid[3]) {
		ddrca_val[0] = reg_get(REG_BASE_DDRC + 0xb164);
		reg_set(REG_BASE_DDRC + 0xb184, ddrca_val[0]);

		ddrca_val[1] = reg_get(REG_BASE_DDRC + 0xb168);
		reg_set(REG_BASE_DDRC + 0xb188, ddrca_val[1]);

		ddrca_val[2] = reg_get(REG_BASE_DDRC + 0xb16c);
		reg_set(REG_BASE_DDRC + 0xb048, ddrca_val[2]);
	}

	trng_init();
	/* get random number */
	random_num1 = get_random_num();
	random_num2 = get_random_num();

	/* start ddr scrambling */
	ddr_scramb_start(random_num1, random_num2);

	/* clear random number */
	(void)get_random_num();
	(void)get_random_num();
	(void)get_random_num();
	(void)get_random_num();
	trng_deinit();

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
		reg_val[0] = ddrc_isvalid[0] ? (reg_get(REG_BASE_DDRC + DDRC_CURR_FUNC_OFST) & 0x1) : 0;
		reg_val[1] = ddrc_isvalid[1] ? (reg_get(REG_BASE_DDRC + DDRC1_CURR_FUNC_OFST) & 0x1) : 0;
		reg_val[2] = ddrc_isvalid[2] ? (reg_get(REG_BASE_DDRC + DDRC2_CURR_FUNC_OFST) & 0x1) : 0;
		reg_val[3] = ddrc_isvalid[3] ? (reg_get(REG_BASE_DDRC + DDRC3_CURR_FUNC_OFST) & 0x1) : 0;
	} while (reg_val[0] | reg_val[1] | reg_val[2] | reg_val[3]);

	return OK;
}

#endif /* DDR_SCRAMB_ENABLE */

#define HPM_CORE_VOL_REG		0x11029000
#define HPM_CPU_VOL_REG			0x11029004

#define HMP_CLK_REG			0x11014A80
#define CPU_HPM_CTRL0_REG		0x1102B000
#define CORE_HPM_CTRL0_REG		0x1102B010
#define HPM_CPU_REG0			0x1102B008
#define HPM_CPU_REG1			0x1102B00c
#define HPM_CORE_REG0			0x1102B018
#define HPM_CORE_REG1			0x1102B01c
#define CYCLE_NUM			32
#define SVB_RECORD_REG0			0x11020340
#define SVB_RECORD_REG1			0x11020344

#define VOLTAGE_RECOED_REG		0x1102015C
#define SVB_VERSION_REG			0x11020168
#define SVB_VERSION			0x300
#define OTP_HPM_CORE_REG		0x11021504
#define OTP_HPM_CPU_REG			0x11021530
#define OTP_VOLTAGE_DELTA_CORE_REG	0x1102150C
#define OTP_VOLTAGE_DELTA_CPU_REG	0x11021518

#define TSENSOR_STATUS0			0X1102A008

#define HPM_CORE_MIN	180
#define HPM_CORE_MAX    320

#define HPM_CPU_MIN   230
#define HPM_CPU_MAX   340

#define TEMPERATURE_MIN   (-40)
#define TEMPERATURE_MAX    110

#define PCORE_MV_MAX 1027
#define PCORE_MV_MIN 857
#define PCPU_MV_MAX 1050
#define PCPU_MV_MIN 910

static unsigned hpm_value_avg(unsigned int* const val, int num)
{
	unsigned int i;
	unsigned tmp = 0;

	for (i = 0; i < num; i++)
		tmp += val[i] >> 2;

	return tmp >> 5;
}

static void get_hpm_value(unsigned int *hpm_core, unsigned int *hpm_cpu)
{
	int i;
	unsigned int temp;
	unsigned int core_value[4];
	unsigned int cpu_value[4];

	core_value[0] = 0;
	core_value[1] = 0;
	core_value[2] = 0;
	core_value[3] = 0;

	cpu_value[0] = 0;
	cpu_value[1] = 0;
	cpu_value[2] = 0;
	cpu_value[3] = 0;

	for (i = 0; i < CYCLE_NUM; i++) {
		delay(24);

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

	delay(240); /* Delay 240 Cycle */

	get_hpm_value(hpm_core, hpm_cpu);
}

static void hpm_check(unsigned int *hpm_core, unsigned int *hpm_cpu,
			int *temperature)
{
	union {
		struct {
			unsigned int sys_hpm_core : 9; /* [8..0]*/
			unsigned int reserved_0   : 7; /* [15..9]*/
			unsigned int sys_hpm_cpu  : 9; /* [24..16]*/
			unsigned int reserved_1   : 7; /* [31..25]*/
		} bits;
		unsigned int u32;
	} hpm_reg0; /*SVB_RECORD_REG0 0x11020340*/

	union {
		struct {
			signed char  temperature   : 8;		/* [7..0]*/
			unsigned int reserved_0    : 20;	/* [27..8]*/
			unsigned int temperature_err : 1;	/* [28]*/
			unsigned int hpm_cpu_err   : 1;		/* [29]*/
			unsigned int hpm_core_err  : 1;		/* [30]*/
			unsigned int from_otp      : 1;		/* [31]*/
		} bits;
		unsigned int u32;
	} hpm_reg1;	/*SVB_RECORD_REG1 0x11020344*/

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
	hpm_reg1.bits.temperature  = (signed char) *temperature;
	writel(hpm_reg0.u32, SVB_RECORD_REG0);
	writel(hpm_reg1.u32, SVB_RECORD_REG1);
}

static void voltage_check(int *pcore_mv, int *pcpu_mv, int core_delta_mv)
{
	short otp_voltage_delta_core;
	short otp_voltage_delta_cpu;
	union {
		struct {
			unsigned int voltage_core : 12; /* [11..0]*/
			unsigned int voltage_cpu  : 12; /* [23..12]*/
			unsigned int delta_core   : 8; /* [31..24]*/
		} bits;
		unsigned int u32;
	} voltage_reg;

	if (*pcore_mv > PCORE_MV_MAX)
		*pcore_mv = PCORE_MV_MAX;
	if (*pcore_mv < PCORE_MV_MIN)
		*pcore_mv = PCORE_MV_MIN;

	if (*pcpu_mv > PCPU_MV_MAX)
		*pcpu_mv = PCPU_MV_MAX;
	if (*pcpu_mv < PCPU_MV_MIN)
		*pcpu_mv = PCPU_MV_MIN;

	otp_voltage_delta_core = (short)(readl(OTP_VOLTAGE_DELTA_CORE_REG) & 0xffff);
	otp_voltage_delta_cpu = (short)(readl(OTP_VOLTAGE_DELTA_CPU_REG) & 0xffff);
	*pcore_mv += otp_voltage_delta_core;
	*pcpu_mv  += otp_voltage_delta_cpu;

	voltage_reg.u32 = 0;
	voltage_reg.bits.voltage_core = (unsigned int)*pcore_mv;
	voltage_reg.bits.voltage_cpu  = (unsigned int)*pcpu_mv;
	voltage_reg.bits.delta_core   = (unsigned int)core_delta_mv;

	writel(voltage_reg.u32, VOLTAGE_RECOED_REG);
}

#define calc_hpm_core_volt(mv)  (((unsigned int)((1078000 - (mv) * 1000) * 84) & 0xffff0000) + 0x19f5)

static void set_hpm_core_volt(int mv)
{
	unsigned int svb_value;

	svb_value = calc_hpm_core_volt(mv);

	writel(svb_value, HPM_CORE_VOL_REG);
}

#define calc_hpm_cpu_volt(mv)  (((unsigned int)((1078000 - (mv) * 1000) * 84) & 0xffff0000) + 0x19f5)

static void set_hpm_cpu_volt(int mv)
{
	unsigned int svb_value;

	svb_value = calc_hpm_cpu_volt(mv);

	writel(svb_value, HPM_CPU_VOL_REG);
}

#define calc_get_temperature_formula(value) ((int)((((value) - 127) * 165) / 784.0) - 40)

static void get_temperature(int *temperature)
{
	int value;

	value = (int)(readl(TSENSOR_STATUS0) & 0x3ff);
	*temperature = calc_get_temperature_formula(value);
}

static unsigned int hpm_cpu_calibration(unsigned int hpm_cpu, int temperature)
{
	int hpm_cpu_delta = 0;
	int hpm_cpu_int = 0;

	hpm_cpu_int = (int)hpm_cpu;

	if (hpm_cpu_int >= 265) {
		if (temperature < -25) { /* -25: temperature */
			hpm_cpu_delta = hpm_cpu_delta1(hpm_cpu_int);
		} else if (temperature < 0) { /* 0: temperature */
			hpm_cpu_delta = hpm_cpu_delta2(hpm_cpu_int);
		} else if (temperature < 60) { /* 60: temperature */
			hpm_cpu_delta = 0;
		} else if (temperature < 75) { /* 75: temperature */
			hpm_cpu_delta = hpm_cpu_delta3(hpm_cpu_int);
		} else if (temperature < 85) { /* 85: temperature */
			hpm_cpu_delta = hpm_cpu_delta4(hpm_cpu_int);
		} else {
			hpm_cpu_delta = hpm_cpu_delta5(hpm_cpu_int);
		}
	}

	hpm_cpu_int = hpm_cpu_int + hpm_cpu_delta;

	return (unsigned int)hpm_cpu_int;
}


static unsigned int hpm_core_calibration(unsigned int hpm_core, int temperature)
{
	int hpm_core_delta = 0;
	int hpm_core_int = 0;

	hpm_core_int = (int)hpm_core;

	if (hpm_core_int >= 240) { /* 240: range */
		if (temperature < -25) { /* -25: temperature */
			hpm_core_delta = 0;
		} else if (temperature < 0) { /* 0: temperature */
			hpm_core_delta = 0;
		} else if (temperature < 60) { /* 60: temperature */
			hpm_core_delta = 0;
		} else if (temperature < 75) { /* 75: temperature */
			hpm_core_delta = hpm_core_delta3(hpm_core_int);
		} else if (temperature < 85) { /* 85: temperature */
			hpm_core_delta = hpm_core_delta4(hpm_core_int);
		} else {
			hpm_core_delta = hpm_core_delta5(hpm_core_int);
		}
	}

	hpm_core_int = hpm_core_int + hpm_core_delta;

	return (unsigned int)hpm_core_int;
}

#define calc_pcore_mv_formula(hpm_core) ((1366000 - 1697 * (hpm_core)) / 1000)
#define calc_cpu_mv_formula(hpm_cpu) ((1728384 - 2545 * (hpm_cpu)) / 1000)
#define TEMPERATURE_DELTA_MAX 85


static void start_svb(int *pcore_mv)
{
	unsigned int hpm_core;
	unsigned int hpm_cpu;
	int ddr_training_delta_mv = 20;
	int temperature;
	unsigned int version;
	int cpu_mv;

	version = readl(SVB_VERSION_REG);
	version = (version & 0x0000FFFF) | (SVB_VERSION << 16);
	writel(version, SVB_VERSION_REG);

	get_temperature(&temperature);

	set_hpm_core_volt(900); /* 900 : variant */
	set_hpm_cpu_volt(1000); /* 1000 : variant */

	/* open hmp clock */
	writel(0x10, HMP_CLK_REG);
	start_hpm(&hpm_core, &hpm_cpu);
	hpm_cpu = hpm_cpu_calibration(hpm_cpu, temperature);
	hpm_core = hpm_core_calibration(hpm_core, temperature);
	hpm_check(&hpm_core, &hpm_cpu, &temperature);

	*pcore_mv = calc_pcore_mv_formula(hpm_core);
	cpu_mv = calc_cpu_mv_formula(hpm_cpu); /* svb001 */

	voltage_check(pcore_mv, &cpu_mv, ddr_training_delta_mv);
	set_hpm_core_volt(*pcore_mv - ddr_training_delta_mv);
	set_hpm_cpu_volt(cpu_mv);

	/* 240: delay 10ms do not delete */
	delay(240);
}

static void end_svb(const int pcore_mv)
{
	set_hpm_core_volt(pcore_mv);

	/* 120: delay 10ms do not delete */
	delay(120);
}

void start_ddr_training(unsigned int base)
{
	int core_mv_record;

	start_svb(&core_mv_record);

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

	end_svb(core_mv_record);
}
