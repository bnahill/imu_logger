#include "stm32f10x.h"
#include "i2c.h"
#include "lsm303.h"
#include "exti.h"
#include "logger.h"
#include "arm_math.h"

/*!
 @addtogroup lsm LSM303 Accelerometer + Magnetometer
 @{
 @addtogroup lsm_priv Private
 @{
 */

//! @name I2C Addresses
//! @{
#define ACC_ADDR 0x33
#define MAG_ADDR 0x3D
//! @}

//! @name Accelerometer register addresses
//! @{
#define ACC_REG_CTRL1     0x20
#define ACC_REG_CTRL2     0x21
#define ACC_REG_CTRL3     0x22
#define ACC_REG_CTRL4     0x23
#define ACC_REG_CTRL5     0x24
#define ACC_REG_HP_RESET  0x25
#define ACC_REG_REFA      0x26
#define ACC_REG_STAT      0x27
#define ACC_REG_X_L       0x28
#define ACC_REG_X_H       0x29
#define ACC_REG_Y_L       0x2A
#define ACC_REG_Y_H       0x2B
#define ACC_REG_Z_L       0x2C
#define ACC_REG_Z_H       0x2D
#define ACC_REG_FIFO_CTL  0x2E
#define ACC_REG_FIFO_SRC  0x2F
#define ACC_REG_INT1_CFG  0x30
#define ACC_REG_INT1_SRC  0x31
#define ACC_REG_INT1_THS  0x32
#define ACC_REG_INT1_DUR  0x33
#define ACC_REG_INT2_CFG  0x34
#define ACC_REG_INT2_SRC  0x35
#define ACC_REG_INT2_THS  0x36
#define ACC_REG_INT2_DUR  0x37
#define ACC_REG_CLCK_CFG  0x38
#define ACC_REG_CLCK_SRC  0x39
#define ACC_REG_CLCK_THS  0x3A
#define ACC_REG_TIME_LIM  0x3B
#define ACC_REG_TIME_LAT  0x3C
#define ACC_REG_TIME_WIN  0x3D
//! @}

//! @name Magnetometer register addresses
//! @{
#define MAG_REG_CRA       0x00
#define MAG_REG_CRB       0x01
#define MAG_REG_MR        0x02
#define MAG_REG_X_H       0x03
#define MAG_REG_X_L       0x04
#define MAG_REG_Z_H       0x05
#define MAG_REG_Z_L       0x06
#define MAG_REG_Y_H       0x07
#define MAG_REG_Y_L       0x08
#define MAG_REG_SR        0x09
#define MAG_REG_IRA       0x0A
#define MAG_REG_IRB       0x0B
#define MAG_REG_IRC       0x0C
#define MAG_REG_TEMP_H    0x31
#define MAG_REG_TEMP_L    0x32
//! @}

static const lsm_calib_t inemos[] = {
	{
		{57, 98, 8, 67, 48, 84, 56, 57, 213, 5, 49, 255}, // iNEMO 2
		{1.005140000000000,  0.028539090000000,     -0.014078770000000,
		 -0.002604419000000, 0.980015600000000,     0.020194060000000,
		 -0.033938110000000, 0.001764096000000,     0.996135500000000,
		 -0.019923740000000, -0.007757963000000,    0.029542460000000}
	}
};

//! Accelerometer scale to be indexed with lsm_acc_fs_t
static const float acc_scale[] = {
	1.0 / 1000.0,
	2.0 / 1000.0,
	3.9 / 1000.0
};

//! Magnetometer X/Y scale to be indexed with lsm_mag_fs_t
static const float mag_scale_xy[] = {
	0,
	1.0 / 1055.0,
	1.0 / 795.0,
	1.0 / 635.0,
	1.0 / 430.0,
	1.0 / 375.0,
	1.0 / 320.0,
	1.0 / 230.0
};

//! Magnetometer X/Y scale to be indexed with lsm_mag_fs_t
static const float mag_scale_z[] = {
	0,
	1.0 / 950.0,
	1.0 / 710.0,
	1.0 / 570.0,
	1.0 / 385.0,
	1.0 / 335.0,
	1.0 / 285.0,
	1.0 / 205.0
};

//! @}

static arm_matrix_instance_f32 mat_a, mat_b, mat_c;

#if HAS_MAGACC
//! @ingroup lsm
//! @brief The accelerometer instance and configuration
lsm303_t magacc = {
	{0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0},
	&i2c1,
	{
		{{GPIOB, BIT(12), 12}},
		{{GPIOB, BIT(13), 13}},
	},
	LSM_ACC_RATE_50,
	LSM_MAG_RATE_30,
	LSM_ACC_FS_8G,
	LSM_MAG_FS_4,
	LSM_PM_NORM,
	LSM_HP_0,
	LSM_DISABLE
};
#endif

//! @}

int lsm303_int_config(lsm303_t *lsm, lsm_int_config_t const *config){
	uint8_t buffer[128];

	lsm_int_t * const i = &lsm->interrupt[config->index];
	// Check for valid numbers
	if((config->threshold & 0x80) || (config->duration & 0x80) || (config->src & 0xC0))
		return 0;
	i->src = config->src;
	i->mode = config->mode;
	i->threshold = config->threshold;
	i->duration = config->duration;
	i->cb = config->cb;
	i->arg = config->arg;
	i->highpass = config->highpass;
	
	*buffer = i2c_read_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL2) & ~(0x04 << config->index);
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL2, *buffer | (i->highpass << (2+config->index)));
	
	// Write threshold and duration
	buffer[0] = (i->mode << 6) | i->src;
	buffer[1] = 0;
	buffer[2] = i->threshold;
	buffer[3] = i->duration;
	
	i2c_write_buffer(lsm->i2c, ACC_ADDR, 0x80 + ACC_REG_INT1_CFG + 4*config->index, buffer, 4);
	
	if(!exti_register_handler(i->gpio.gpio,
	                          i->gpio.pinsrc,
	                          EXTI_Trigger_Rising,
	                          config->cb,
	                          config->arg))
		return 0;
	
	EXTI->RTSR &= ~i->gpio.pin;
	EXTI->FTSR &= ~i->gpio.pin;
	
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_INT1_CFG + 4*config->index, (i->mode << 6) | i->src);
	
	i2c_read_buffer(lsm->i2c, ACC_ADDR, 0x80 + ACC_REG_INT1_CFG + 4*config->index, buffer, 4);
	
	return 1;
}

void lsm303_int_enable(lsm303_t *lsm, lsm_int_index_t index){
	lsm_int_t * const i = &lsm->interrupt[index];
	i->state = LSM_ENABLE;
	
	EXTI->RTSR &= ~i->gpio.pin;
	EXTI->FTSR |= i->gpio.pin;
	
	// Read to clear old interrupts
	i2c_read_byte(lsm->i2c, ACC_ADDR, ACC_REG_INT1_SRC + (4*index));
}

void lsm303_int_disable(lsm303_t *lsm, lsm_int_index_t index){
	lsm_int_t * const i = &lsm->interrupt[index];
	i->state = LSM_DISABLE;
	// Mask the interrupt
	//exti_mask(i->gpio.pin);
	
	// Disable edge triggers
	EXTI->RTSR &= ~i->gpio.pin;
	EXTI->FTSR &= ~i->gpio.pin;
}

void lsm303_set_pm(lsm303_t *lsm, lsm_pm_t pm){
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL1, (pm << 5) | (lsm->acc_rate << 3) | 0x07);
	lsm->pow_mode = pm;
}

void lsm303_set_acc_fs(lsm303_t *lsm, lsm_acc_fs_t fs){
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL4, 0x80 | (fs << 4));
	lsm->acc_fs = fs;
}

void lsm303_set_acc_rate(lsm303_t *lsm, lsm_acc_rate_t acc_rate){
	lsm->acc_rate = acc_rate;
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL1, (lsm->pow_mode << 5) | (lsm->acc_rate << 3) | 0x07);
}

void lsm303_init(lsm303_t *lsm){
	GPIO_InitTypeDef gpio_init_s;
	uint8_t devid[12];
	lsm_calib_t const * calib;
	int i;
	
	// Init CTRL1-5
	uint8_t conf_buffer[5];
	
	// Initialize interrupt pins
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_s.GPIO_Pin = lsm->interrupt[0].gpio.pin;
	GPIO_Init(lsm->interrupt[0].gpio.gpio, &gpio_init_s);
	gpio_init_s.GPIO_Pin = lsm->interrupt[1].gpio.pin;
	GPIO_Init(lsm->interrupt[1].gpio.gpio, &gpio_init_s);
	
	// Initialize both to not appear busy
	lsm->mag_xfer.done = 0;
	lsm->acc_xfer.done = 0;
	lsm->mag_xfer.busy = 0;
	lsm->acc_xfer.busy = 0;
	
	lsm->interrupt[0].src = 0;
	lsm->interrupt[1].src = 0;
	
	// Find the calibration constants
	lsm->calib = NULL;
	logger_read_devid(devid);
	for(calib = &inemos[0]; calib < inemos + (sizeof(inemos) / sizeof(*calib)); calib++){
		for(i = 0; i < 12; i++){
			if(calib->id[i] != devid[i])
				break;
		}
		if(i == 12){
			// Success!
			lsm->calib = calib;
			// Matrix for raw data
			mat_a.numCols = 4;
			mat_a.numRows = 1;
			// Matrix for calibration constants
			mat_b.numCols = 3;
			mat_b.numRows = 4;
			// Matrix for output
			mat_c.numCols = 3;
			mat_c.numRows = 1;

			break;
		}
	}
	
	conf_buffer[0] = (lsm->pow_mode << 5) | (lsm->acc_rate << 3) | 0x07;
	conf_buffer[1] = lsm->hp_cuttoff | (lsm->hp_enable << 4);
	conf_buffer[2] = 0xA4;
	// BDU
	//conf_buffer[3] = 0x80 | (lsm->acc_fs << 4);
	// No BDU
	conf_buffer[3] = (lsm->acc_fs << 4);
	// Enable sleep-to-wake
	conf_buffer[4] = 0x03;
	

	// Initialize I2C device
	i2c_init(lsm->i2c, I2C_MODE_MASTER, 400000);

	// Reset memory
	i2c_write_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL2, 0x80);
	while(i2c_read_byte(lsm->i2c, ACC_ADDR, ACC_REG_CTRL2) != 0x00);
	
	// Apply accelerometer configuration
	i2c_write_buffer(lsm->i2c, ACC_ADDR, ACC_REG_CTRL1 | 0x80, conf_buffer, 5);

	conf_buffer[0] = lsm->mag_rate << 2;
	conf_buffer[1] = lsm->mag_fs << 5;
	conf_buffer[2] = 0;

	// Apply magnetometer configuration
	i2c_write_buffer(lsm->i2c, MAG_ADDR, MAG_REG_CRA | 0x80, conf_buffer, 3);
}

void lsm303_read_mag(lsm303_t *lsm){
	i2c_mk_transfer(&lsm->mag_xfer,
	                I2C_OP_READ,
	                MAG_ADDR,
	                MAG_REG_X_H | 0x80,
	                lsm->mag_buff,
	                6
	);

	// Start the transfer
	i2c_transfer(lsm->i2c, &lsm->mag_xfer);
}

void lsm303_read_acc(lsm303_t *lsm){
	i2c_mk_transfer(&lsm->acc_xfer,
	                I2C_OP_READ,
	                ACC_ADDR,
	                ACC_REG_X_L | 0x80,
	                lsm->acc_buff,
	                6
	);
	
	// Start the acc transfer
	i2c_transfer(lsm->i2c, &lsm->acc_xfer);
}


int lsm303_mag_xfer_complete(lsm303_t *lsm){
	return lsm->mag_xfer.done;
}

int lsm303_acc_xfer_complete(lsm303_t *lsm){
	return lsm->acc_xfer.done;
}

int lsm303_mag_xfer_complete_all(void){
#if HAS_MAGACC
	if(!magacc.mag_xfer.done)
		return 0;
#endif
	return 1;	
}

int lsm303_acc_xfer_complete_all(void){
#if HAS_MAGACC
	if(!magacc.acc_xfer.done)
		return 0;
#endif
	return 1;
}

int lsm303_xfer_complete_all(void){
#if HAS_MAGACC
	if(!magacc.mag_xfer.done || !magacc.acc_xfer.done)
		return 0;
#endif
	return 1;	
}

void lsm303_read_sync_all(void){
	lsm303_read_all();
	while(!lsm303_xfer_complete_all());
	lsm303_update_all();
}

void lsm303_update_acc(lsm303_t *lsm){
	int16_t tmp16;
	float xyz[4];
	float calibrated[3];
	
	lsm->acc_xfer.done = 0;
	
	// Read the raw sensor value
	tmp16 = lsm->acc_buff[0] | (lsm->acc_buff[1] << 8);
	// Scale it to match the sensitivity
	xyz[0] = (tmp16 >> 4) * acc_scale[lsm->acc_fs];
	tmp16 = lsm->acc_buff[2] | (lsm->acc_buff[3] << 8);
	xyz[1] = (tmp16 >> 4) * acc_scale[lsm->acc_fs];
	tmp16 = lsm->acc_buff[4] | (lsm->acc_buff[5] << 8);
	xyz[2] = -(tmp16 >> 4) * acc_scale[lsm->acc_fs];
	
	if(lsm->calib){
		// Perform calibration
		mat_b.pData = (float32_t *)lsm->calib->calib;
		mat_a.pData = xyz;
		mat_c.pData = calibrated;
		xyz[3] = 1;
		arm_mat_mult_f32(&mat_a, &mat_b, &mat_c);
		lsm->acc.x = calibrated[0];
		lsm->acc.y = calibrated[1];
		lsm->acc.z = calibrated[2];
	} else {
		lsm->acc.x = xyz[0];
		lsm->acc.y = xyz[1];
		lsm->acc.z = xyz[2];
	}
}

void lsm303_update_mag(lsm303_t *lsm){
	int16_t tmp16;
	
	lsm->acc_xfer.done = 0;

	tmp16 = lsm->mag_buff[1] | (lsm->mag_buff[0] << 8);
	lsm->mag.x = tmp16 * mag_scale_xy[lsm->mag_fs];
	tmp16 = lsm->mag_buff[3] | (lsm->mag_buff[2] << 8);
	lsm->mag.z = tmp16 * mag_scale_xy[lsm->mag_fs];
	tmp16 = lsm->mag_buff[5] | (lsm->mag_buff[4] << 8);
	lsm->mag.y = tmp16 * mag_scale_z[lsm->mag_fs];
}

void lsm303_update_acc_all(void){
#if HAS_MAGACC
	lsm303_update_acc(&magacc);
#endif	
}

void lsm303_update_mag_all(void){
#if HAS_MAGACC
	lsm303_update_mag(&magacc);
#endif	
}

void lsm303_update_all(void){
#if HAS_MAGACC
	lsm303_update_acc(&magacc);
	lsm303_update_mag(&magacc);
#endif	
}

void lsm303_init_all(void){
#if HAS_MAGACC
	lsm303_init(&magacc);
#endif
}

void lsm303_read_all(void){
#if HAS_MAGACC
	lsm303_read_acc(&magacc);
	lsm303_read_mag(&magacc);
#endif
}

void lsm303_read_mag_all(void){
#if HAS_MAGACC
	lsm303_read_mag(&magacc);
#endif
}
void lsm303_read_acc_all(void){
#if HAS_MAGACC
	lsm303_read_acc(&magacc);
#endif
}
