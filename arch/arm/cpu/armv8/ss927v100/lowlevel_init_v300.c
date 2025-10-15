// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 *
 */

#include <asm/arch/platform.h>
#include <config.h>
#include "ddr_interface.h"

#define SYS_CTL_REG  SYS_CTRL_REG_BASE

#define OK 0
#define ERROR (-1)
#define CH0_DMC_CFG_DDRMODE 	0x11148050
#define CH0_DMC_CFG_RNKVOL_0	0x11148060
#define CH0_DMC_CFG_RNKVOL_1    0x11148064

#define CH1_DMC_CFG_DDRMODE 	0x11149050
#define CH1_DMC_CFG_RNKVOL_0	0x11149060
#define CH1_DMC_CFG_RNKVOL_1    0x11149064

#define CH2_DMC_CFG_DDRMODE 	0x1114a050
#define CH2_DMC_CFG_RNKVOL_0	0x1114a060
#define CH2_DMC_CFG_RNKVOL_1    0x1114a064

#define CH3_DMC_CFG_DDRMODE 	0x1114b050
#define CH3_DMC_CFG_RNKVOL_0	0x1114b060
#define CH3_DMC_CFG_RNKVOL_1    0x1114b064

#define CH0_QOS_CFG_DDRMODE     0x11144630
#define CH0_QOS_CFG_RNKVOL_0    0x11144634
#define CH0_QOS_CFG_RNKVOL_1	0x11144638

#define CH1_QOS_CFG_DDRMODE     0x11144640
#define CH1_QOS_CFG_RNKVOL_0	0x11144644
#define CH1_QOS_CFG_RNKVOL_1	0x11144648

#define CH2_QOS_CFG_DDRMODE     0x11144650
#define CH2_QOS_CFG_RNKVOL_0	0x11144654
#define CH2_QOS_CFG_RNKVOL_1	0x11144658

#define CH3_QOS_CFG_DDRMODE     0x11144660
#define CH3_QOS_CFG_RNKVOL_0	0x11144664
#define CH3_QOS_CFG_RNKVOL_1	0x11144668

static inline void delay(unsigned int num)
{
	volatile unsigned int i;

	for (i = 0; i < (100 * num); i++)  /* 100: Cycle */
		__asm__ __volatile__("nop");
}

static inline void dwb(void) /* drain write buffer */
{
}

static inline unsigned int readl(unsigned addr)
{
	unsigned int val;

	val = (*(volatile unsigned int *)(long)(addr));
	return val;
}

static inline void writel(unsigned val, unsigned addr)
{
	dwb();
	(*(volatile unsigned *)(long)(addr)) = (val);
	dwb();
}

#define REG_BASE_MISC			0x11020000
#define DDRCA_REE_RANDOM_L		0x4040
#define DDRCA_REE_RANDOM_H		0x4044
#define DDRCA_TEE_RANDOM_L		0x4048
#define DDRCA_TEE_RANDOM_H		0x404C
#define DDRCA_EN			0x4050
#define DDRCA_REE_UPDATE		0x4054
#define DDRCA_TEE_UPDATE		0x4058
#define DDR_CA_LOCK			0x405c
#define RANDOM_SIZE			4

#define REG_TSENSOR_CTRL	0x1102e000
#define TSENSOR_CTRL0		0x0
#define TSENSOR_CTRL0_CFG	0xc0300000
#define TSENSOR_CTRL1		0x4
#define TSENSOR_CTRL1_CFG	0x8

#define SYSCTRL_REG		0x11020000
#define HPM_CORE_VOL_REG	(SYSCTRL_REG + 0x9004)
#define HPM_MDA_VOL_REG		(SYSCTRL_REG + 0x9104)
#define HPM_NPU_VOL_REG		(SYSCTRL_REG + 0x9204)
#define HPM_MONITOR_CFG		0x60200001
#define HPM_CORE_OFFSET		0xb030
#define HPM_MDA_OFFSET		0xb020
#define HPM_NPU_OFFSET		0xb010
#define CYCLE_NUM		8
#define HPM_NPU_REG0		0xb018
#define HPM_NPU_REG1		0xb01c
#define HPM_MDA_REG0		0xb028
#define HPM_MDA_REG1		0xb02c
#define HPM_CORE_REG0		0xb038
#define HPM_CORE_REG1		0xb03c

#define HPM_CLK_REG		0x11014a80
#define HPM_CLK_CFG		0x30

#define CPU_ISO_REG		0x1d821104

#define TSENSOR_CTRL		0x70
#define TSENSOR_STATUS0		0x8

#define OTP_CPU_IF_REG		0x10120000
#define OTP_HPM_CORE_OFFSET  0x1504
#define OTP_HPM_MDA_OFFSET   0x1534
#define OTP_HPM_NPU_OFFSET   0x1530
#define OTP_SHPM_MDA_OFFSET	0x153c

#define SYSCTRL_BASE_REG	0x11020000
#define HPM_CORE_STORAGE_REG	0x340
#define HPM_MDA_STORAGE_REG	0x344
#define HPM_NPU_STORAGE_REG	0x348
#define TEMPERATURE_STORAGE_REG  0x34c
#define DELTA_V_STORAGE_REG  0x350
#define CORE_DUTY_STORAGE_REG  0x354
#define MDA_DUTY_STORAGE_REG  0x358
#define NPU_DUTY_STORAGE_REG  0x35c

/* physical max/min */
#define CORE_VOLT_MAX		920
#define CORE_VOLT_MIN		599

#define MDA_VOLT_MAX		1049
#define MDA_VOLT_MIN		600

#define NPU_VOLT_MAX		1049
#define NPU_VOLT_MIN		600

/* curve max/min; voltage curve:  v = (b - a * hpm) / 10 */
#define CORE_CURVE_VLOT_MAX		850
#define CORE_CURVE_VLOT_MIN		760
#define CORE_CURVE_B			16200
#define CORE_CURVE_A			20

#define MEDIA_CURVE_VLOT_MAX	770
#define MEDIA_CURVE_VLOT_MIN	690
#define MEDIA_CURVE_B			11150
#define MEDIA_CURVE_A			10


#define NPU_CURVE_VLOT_MAX	810
#define NPU_CURVE_VLOT_MIN	730
#define NPU_CURVE_B			127500
#define NPU_CURVE_A			125

#define SVB_VER_REG		0x11020168
#define SVB_VER			0x30303030
#define temperature_formula(val)  (((((val) - 116) * 165) / 806) - 40)
#define duty_formula(max, min, val) ((((max) - (val)) * 416 + ((max) - (min) + 1) / 2) / ((max) - (min)) - 1)
#define volt_regval_formula(val) (((val) << 16) + ((416 - 1) << 4) + 0x5)

static unsigned hpm_value_avg(unsigned int *val, unsigned int num)
{
	unsigned int i;
	unsigned tmp = 0;

	for (i = 0; i < num; i++) /* 4: Cycle */
		tmp += val[i];

	return (tmp / CYCLE_NUM) >> 2;
}

static void get_hpm_value(unsigned int *hpm_core, unsigned int *hpm_npu,
			   unsigned int *hpm_mda)
{
	int i;
	unsigned int temp;
	unsigned int core_value[4] = {0, 0, 0, 0};
	unsigned int mda_value[4] = {0, 0, 0, 0};
	unsigned int npu_value[4] = {0, 0, 0, 0};

	for (i = 0; i < CYCLE_NUM; i++) {
		/*  (at least 16us*4) */
		delay(5); /* delay 5 s */
		/* npu */
		temp = readl(SYSCTRL_REG + HPM_NPU_REG0);
		npu_value[1] += (temp >> 16) & 0x3ff;
		npu_value[0] += temp & 0x3ff;
		temp = readl(SYSCTRL_REG + HPM_NPU_REG1);
		npu_value[3] += (temp >> 16) & 0x3ff;
		npu_value[2] += temp & 0x3ff;

		/* mda */
		temp = readl(SYSCTRL_REG + HPM_MDA_REG0);
		mda_value[1] += (temp >> 16) & 0x3ff;
		mda_value[0] += temp & 0x3ff;
		temp = readl(SYSCTRL_REG + HPM_MDA_REG1);
		mda_value[3] += (temp >> 16) & 0x3ff;
		mda_value[2] += temp & 0x3ff;

		/* core */
		temp = readl(SYSCTRL_REG + HPM_CORE_REG0);
		core_value[1] += (temp >> 16) & 0x3ff;
		core_value[0] += temp & 0x3ff;
		temp = readl(SYSCTRL_REG + HPM_CORE_REG1);
		core_value[3] += (temp >> 16) & 0x3ff;
		core_value[2] += temp & 0x3ff;
	}

	*hpm_core = hpm_value_avg(core_value, 4); /* 4 : Array size */
	*hpm_mda = hpm_value_avg(mda_value, 4); /* 4 : Array size */
	*hpm_npu = hpm_value_avg(npu_value, 4); /* 4 : Array size */
}

static void start_hpm(unsigned int *hpm_core, unsigned int *hpm_npu,
						unsigned int *hpm_mda)
{
	get_hpm_value(hpm_core, hpm_npu, hpm_mda);
}

static void adjust_hpm(unsigned int *hpm_core, unsigned int *hpm_mda,
		       unsigned int *hpm_npu, int temperature)
{
	unsigned int otp_hpm_core = readl(SYSCTRL_BASE_REG + OTP_HPM_CORE_OFFSET);
	unsigned int otp_hpm_npu  = readl(SYSCTRL_BASE_REG + OTP_HPM_NPU_OFFSET);
	unsigned int otp_hpm_mda  = readl(SYSCTRL_BASE_REG + OTP_HPM_MDA_OFFSET);

	if (temperature <= 0) {
		*hpm_npu  -= 2; /* 2 : Adjustment Value */
		*hpm_mda  -= 3; /* 3 : Adjustment Value */
		*hpm_core -= 4; /* 4 : Adjustment Value */
	} else if (temperature > 100) {   /* 100: temperature value */
		*hpm_npu  += 5; /* 5 : Adjustment Value */
		*hpm_mda  += 5; /* 5 : Adjustment Value */
		*hpm_core += 8; /* 8 : Adjustment Value */
	}else if(temperature > 70) {   /* 70: temperature value */
		*hpm_npu  += 3; /* 2 : Adjustment Value */
		*hpm_mda  += 3; /* 2 : Adjustment Value */
		*hpm_core += 4; /* 3 : Adjustment Value */
	}
    

	if (otp_hpm_core) {
		if (*hpm_core > (otp_hpm_core + 59)) /* 10: Increment */
			*hpm_core = otp_hpm_core + 59;   /* 5: Increment */
	}

	if (otp_hpm_npu) {
		if (*hpm_npu > (otp_hpm_npu + 62)) /* 15: Increment */
			*hpm_npu = otp_hpm_npu + 62;
	}

	if (otp_hpm_mda) {
		if (*hpm_mda > (otp_hpm_mda + 61)) /* 15: Increment */
			*hpm_mda = otp_hpm_mda + 61;
	}
}

static void save_hpm(unsigned int hpm_core, unsigned int hpm_npu,
		      unsigned int hpm_mda)
{
	writel(hpm_mda, SYSCTRL_BASE_REG + HPM_MDA_STORAGE_REG);
	writel(hpm_npu, SYSCTRL_BASE_REG + HPM_NPU_STORAGE_REG);
	writel(hpm_core, SYSCTRL_BASE_REG + HPM_CORE_STORAGE_REG);
}

static unsigned int calc_volt_regval(unsigned int volt_val, unsigned int volt_max,
				     unsigned int volt_min)
{
	unsigned int duty;

	if (volt_val >= volt_max)
		volt_val = volt_max - 1;
	if (volt_val <= volt_min)
		volt_val = volt_min + 1;
	duty =  duty_formula(volt_max, volt_min, volt_val);

	return duty;
}

static void set_hpm_core_volt(unsigned int hpm_core_value, int delta_v)
{
	unsigned int volt_val;
	unsigned int reg_val;

	volt_val = (CORE_CURVE_B - CORE_CURVE_A * hpm_core_value) / 10;

	if (volt_val > CORE_CURVE_VLOT_MAX)
		volt_val = CORE_CURVE_VLOT_MAX;
	else if (volt_val < CORE_CURVE_VLOT_MIN)
		volt_val = CORE_CURVE_VLOT_MIN;

	volt_val += delta_v;
	reg_val = calc_volt_regval(volt_val, CORE_VOLT_MAX, CORE_VOLT_MIN);
	writel(reg_val, SYSCTRL_BASE_REG + CORE_DUTY_STORAGE_REG);
	writel(reg_val, HPM_CORE_VOL_REG);
}

static void set_hpm_mda_volt(unsigned int hpm_mda_value, int delta_v)
{
	unsigned int volt_val;
	unsigned int reg_val;

	volt_val = (MEDIA_CURVE_B - MEDIA_CURVE_A * hpm_mda_value) / 10;
	if (volt_val > MEDIA_CURVE_VLOT_MAX)
		volt_val = MEDIA_CURVE_VLOT_MAX;
	else if (volt_val < MEDIA_CURVE_VLOT_MIN)
		volt_val = MEDIA_CURVE_VLOT_MIN;

	volt_val += delta_v;
	reg_val = calc_volt_regval(volt_val, MDA_VOLT_MAX, MDA_VOLT_MIN);
	writel(reg_val, SYSCTRL_BASE_REG + MDA_DUTY_STORAGE_REG);

	writel(reg_val, HPM_MDA_VOL_REG);
}

static void set_hpm_npu_volt(unsigned int hpm_npu_value, int delta_v)
{
	unsigned int volt_val;
	unsigned int reg_val;

	volt_val = (NPU_CURVE_B - NPU_CURVE_A * hpm_npu_value) / 100; 
	if (volt_val > NPU_CURVE_VLOT_MAX)
		volt_val = NPU_CURVE_VLOT_MAX;
	else if (volt_val < NPU_CURVE_VLOT_MIN)
		volt_val = NPU_CURVE_VLOT_MIN;

	volt_val += delta_v;
	reg_val = calc_volt_regval(volt_val, NPU_VOLT_MAX, NPU_VOLT_MIN);
	writel(reg_val, SYSCTRL_BASE_REG + NPU_DUTY_STORAGE_REG);

	writel(reg_val, HPM_NPU_VOL_REG);
}

static void get_delta_v(int *core_delta_v, int *npu_delta_v,  int *mda_delta_v)
{
	unsigned int value = readl(SYSCTRL_REG + OTP_SHPM_MDA_OFFSET);
	writel(value, SYSCTRL_BASE_REG + DELTA_V_STORAGE_REG);
	/* core:bit 11-8,
	 * bit11 equal to 1 means negative, equal to 0 means positive,
	 * bit 8-10 is the  absolute delta_v 
	 */
	int flag = (value & 0x800) ? -1 : 1;
	*core_delta_v = flag * (int)((value >> 8) & 0x7) * 10;

	/* mda:bit 7-4,
	 * bit7 equal to 1 means negative, equal to 0 means positive,
	 * bit 4-6 is the  absolute delta_v 
	 */
	flag = (value & 0x80) ? -1 : 1;
	*mda_delta_v = flag * (int)((value >> 4) & 0x7)  * 10;

	/* npu:bit 3-0,
	 * bit3 equal to 1 means negative, equal to 0 means positive,
	 * bit 0-2 is the  absolute delta_v 
	 */
	flag = (value & 0x8) ? -1 : 1;
	*npu_delta_v = flag * (int)(value & 0x7)  * 10;
}


static void set_volt(unsigned int hpm_core, unsigned int hpm_npu,
		      unsigned int hpm_mda)
{
	int  core_delta_v = 0;
	int  npu_delta_v = 0;
	int  mda_delta_v = 0;
	get_delta_v(&core_delta_v, &npu_delta_v, &mda_delta_v);

	set_hpm_core_volt(hpm_core, core_delta_v);
	set_hpm_mda_volt(hpm_mda, mda_delta_v);
	set_hpm_npu_volt(hpm_npu, npu_delta_v);

	/* delay 300 Cycle */
	delay(300);
}

static void get_temperature(int *temperature)
{
	int value = 0;
	int tsensor_chn;
	float m = 0.5;
	float temperature_temp[3] = {0};
	for(tsensor_chn = 0; tsensor_chn < 3; tsensor_chn++) {
		value = (int)(readl(REG_TSENSOR_CTRL + TSENSOR_STATUS0 +
					0x100 * tsensor_chn) & 0x3ff);

		m = ((float)value - 146) / 718 * 165 - 40;

		if(m > 0)
			temperature_temp[tsensor_chn] = m + 0.5;
		else
			temperature_temp[tsensor_chn] = m - 0.5;
	}

	*temperature = (temperature_temp[0]+temperature_temp[1]+temperature_temp[2])/3;
	writel(*temperature, SYSCTRL_BASE_REG + TEMPERATURE_STORAGE_REG);
}

void init_temperature(void)
{
	int tsensor_chn;

	for(tsensor_chn = 0; tsensor_chn < 3; tsensor_chn++) {
		writel(TSENSOR_CTRL0_CFG, REG_TSENSOR_CTRL + TSENSOR_CTRL0 +
				0x100 * tsensor_chn);
		writel(TSENSOR_CTRL1_CFG, REG_TSENSOR_CTRL + TSENSOR_CTRL1 +
				0x100 * tsensor_chn);
	}

}

void init_hpm(void)
{
	/* open hmp clock */
	writel(HPM_CLK_CFG, HPM_CLK_REG);

	/* npu  */
	writel(HPM_MONITOR_CFG, SYSCTRL_REG + HPM_NPU_OFFSET);
	/* mda  */
	writel(HPM_MONITOR_CFG, SYSCTRL_REG + HPM_MDA_OFFSET);
	/* core  */
	writel(HPM_MONITOR_CFG, SYSCTRL_REG + HPM_CORE_OFFSET);

}
static void start_svb(void)
{
	unsigned int hpm_core;
	unsigned int hpm_npu;
	unsigned int hpm_mda;
	int temperature;

	/* init temperature and hpm*/
	init_temperature();
	init_hpm();
	delay(1000);
	start_hpm(&hpm_core, &hpm_npu, &hpm_mda);

	/*get temperature */
	get_temperature(&temperature);

	adjust_hpm(&hpm_core, &hpm_mda, &hpm_npu, temperature);
	set_volt(hpm_core, hpm_npu, hpm_mda);
	save_hpm(hpm_core, hpm_npu, hpm_mda);

	/* add SVB VER*/
	writel(SVB_VER, SVB_VER_REG);
}

static void set_qosbuf_cfg(void)
{
	unsigned int val;
	
	val = readl(CH0_DMC_CFG_DDRMODE);
	writel(val, CH0_QOS_CFG_DDRMODE);
	val = readl(CH0_DMC_CFG_RNKVOL_0);
	writel(val, CH0_QOS_CFG_RNKVOL_0);
	val = readl(CH0_DMC_CFG_RNKVOL_1);
	writel(val, CH0_QOS_CFG_RNKVOL_1);

	val = readl(CH1_DMC_CFG_DDRMODE);
	writel(val, CH1_QOS_CFG_DDRMODE);
	val = readl(CH1_DMC_CFG_RNKVOL_0);
	writel(val, CH1_QOS_CFG_RNKVOL_0);
	val = readl(CH1_DMC_CFG_RNKVOL_1);
	writel(val, CH1_QOS_CFG_RNKVOL_1);

	val = readl(CH2_DMC_CFG_DDRMODE);
	writel(val, CH2_QOS_CFG_DDRMODE);
	val = readl(CH2_DMC_CFG_RNKVOL_0);
	writel(val, CH2_QOS_CFG_RNKVOL_0);
	val = readl(CH2_DMC_CFG_RNKVOL_1);
	writel(val, CH2_QOS_CFG_RNKVOL_1);
	
	val = readl(CH3_DMC_CFG_DDRMODE);
	writel(val, CH3_QOS_CFG_DDRMODE);
	val = readl(CH3_DMC_CFG_RNKVOL_0);
	writel(val, CH3_QOS_CFG_RNKVOL_0);
	val = readl(CH3_DMC_CFG_RNKVOL_1);
	writel(val, CH3_QOS_CFG_RNKVOL_1);
}

void start_ddr_training(unsigned int base)
{
	start_svb();

	set_qosbuf_cfg();

	ddr_set_rdqbdl_def_val();

	/* ddr hw training */
	ddr_hw_training_init();

	/* ddr sw training */
	ddr_sw_training_if();

	ddr_retrain_anti_aging_enable();

	/* ddr DMC auto power down config */
	ddr_dmc_auto_power_down_cfg();
}
