#ifndef __LSM303_H_
#define __LSM303_H_

#include "stm32f10x.h"
#include "sensor_config.h"
#include "i2c.h"

/*!
 @addtogroup lsm LSM303DLH Accelerometer + Magnetometer
 @{
 */

//! @name Public Data Structures
//! @{
	
typedef enum {
	LSM_DISABLE = 0,
	LSM_ENABLE
} lsm_func_state_t;

//! Interrupt sources
typedef enum {
	LSM_INTSRC_XLO = 0x01,
	LSM_INTSRC_XHI = 0x02,
	LSM_INTSRC_YLO = 0x04,
	LSM_INTSRC_YHI = 0x08,
	LSM_INTSRC_ZLO = 0x10,
	LSM_INTSRC_ZHI = 0x20
} lsm_intsrc_t;

typedef enum {
	LSM_INTMODE_OR = 0,
	LSM_INTMODE_6DIR_MOVEMENT = 1,
	LSM_INTMODE_AND = 2,
	LSM_INTMODE_6DIR_POSITION = 3
} lsm_intmode_t;

typedef enum {
	LSM_INT_1 = 0,
	LSM_INT_2 = 1
} lsm_int_index_t;

//! Accelerometer data output rate
typedef enum {
	LSM_ACC_RATE_50   = 0,
	LSM_ACC_RATE_100  = 1,
	LSM_ACC_RATE_400  = 3,
	LSM_ACC_RATE_1000 = 4
} lsm_acc_rate_t;

//! Magnetometer data output rate
typedef enum {
	LSM_MAG_RATE_0_75 = 0,
	LSM_MAG_RATE_1_5  = 1,
	LSM_MAG_RATE_3    = 2,
	LSM_MAG_RATE_7_5  = 3,
	LSM_MAG_RATE_15   = 4,
	LSM_MAG_RATE_30   = 5,
	LSM_MAG_RATE_75   = 6
} lsm_mag_rate_t;

//! Power mode
typedef enum {
	LSM_PM_OFF    = 0,
	LSM_PM_NORM   = 1,
	LSM_PM_LP_0_5 = 2,
	LSM_PM_LP_1   = 3,
	LSM_PM_LP_2   = 4,
	LSM_PM_LP_5   = 5,
	LSM_PM_LP_10  = 6
} lsm_pm_t;

//! Full scale range for accelerometer
typedef enum {
	LSM_ACC_FS_2G  = 0,
	LSM_ACC_FS_4G  = 1,
	LSM_ACC_FS_8G  = 2
} lsm_acc_fs_t;

//! Full scale range for magnetometer
typedef enum {
	LSM_MAG_FS_NONE  = 0,
	LSM_MAG_FS_1_3   = 1,
	LSM_MAG_FS_1_9   = 2,
	LSM_MAG_FS_2_5   = 3,
	LSM_MAG_FS_4     = 4,
	LSM_MAG_FS_4_7   = 5,
	LSM_MAG_FS_5_6   = 6,
	LSM_MAG_FS_8_1   = 7
} lsm_mag_fs_t;

typedef enum {
	LSM_HP_0 = 0,
	LSM_HP_1 = 1,
	LSM_HP_2 = 2,
	LSM_HP_3 = 3
} lsm_hp_cutoff_t;

//! A callback to receive interrupts
typedef void (*lsm_callback_t)(void *);

typedef struct {
	//! The GPIO pin to use
	af_gpio_pin_t const gpio;
	lsm_func_state_t state;
	uint8_t src;
	lsm_intmode_t mode;
	uint8_t threshold;
	uint8_t duration;
	lsm_func_state_t highpass;
	lsm_callback_t cb;
	void *arg;
} lsm_int_t;

//! A configuration structure to pass 
typedef struct {
	lsm_int_index_t index;
	uint8_t src;
	lsm_intmode_t mode;
	uint8_t threshold;
	uint8_t duration;
	lsm_func_state_t highpass;
	lsm_callback_t cb;
	void *arg;
} lsm_int_config_t;

//! Configuration and state information for an LSM303DLH
typedef struct {
	//! The current magnetometer reading
	euclidean3_t     mag;
	//! The current accelerometer reading
	euclidean3_t     acc;
	//! The I2C device to use
	i2c_t *const     i2c;
	lsm_int_t        interrupt[2];
	//! The accelerometer output rate
	lsm_acc_rate_t   acc_rate;
	//! The magnetometer output rate
	lsm_mag_rate_t   mag_rate;
	//! The accelerometer full-scale output range
	lsm_acc_fs_t     acc_fs;
	//! The magnetometer full-scale output range
	lsm_mag_fs_t     mag_fs;
	//! The power mode of the device
	lsm_pm_t         pow_mode;
	lsm_hp_cutoff_t  hp_cuttoff;
	lsm_func_state_t hp_enable;
	//! Transfer structures for asynchronous accelerometer reads
	i2c_transfer_t   acc_xfer;
	//! Transfer structure for asynchronous magnetometer reads
	i2c_transfer_t   mag_xfer;
	//! Transfer buffers for asynchronous magnetometer reads
	uint8_t          acc_buff[6];
	//! Transfer buffers for asynchronous magnetometer reads
	uint8_t          mag_buff[6];
} lsm303_t;

//! @}

//! @name Public Methods
//! @{

/*!
 @brief Initialize the LSM303 hardware and associated peripherals
 */
void lsm303_init_all(void);

/*!
 @brief Poll the state of the current transfer. When this returns 1, it is
 fair to call lsm303_update_acc()
 @return 1 if complete; otherwise 0
 
 @pre lsm303_read_mag was called
 @post lsm303_update_mag may be called if return is 1
 */
int lsm303_mag_xfer_complete(lsm303_t *lsm);

/*!
 @brief Poll the state of the current transfer. When this returns 1, it is
 fair to call lsm303_update_acc()
 @return 1 if complete; otherwise 0

 @pre lsm303_read_acc was called
 @post lsm303_update_acc may be called if return is 1
 */
int lsm303_acc_xfer_complete(lsm303_t *lsm);

/*!
 @brief Begin a read of the LSM303 accelerometer output
 @param lsm The LSM303 instance to use
 This only starts the read. The read is complete when lsm303_acc_xfer_complete()
 returns 1.
*/
void lsm303_read_acc(lsm303_t *lsm);

/*!
 @brief Begin a read of the LSM303 magnetometer output
 @param lsm The LSM303 instance to use
 This only starts the read. The read is complete when lsm303_mag_xfer_complete()
 returns 1.
*/
void lsm303_read_mag(lsm303_t *lsm);

/*!
 @brief Format the accelerometer data from lsm303_read_mag
 @param lsm The LSM303 isntance to use
 @pre lsm303_acc_xfer_complete returns 1
 */
void lsm303_update_acc(lsm303_t *lsm);


/*!
 @brief Format the magnetometer data from lsm303_read_mag
 @param lsm The LSM303 isntance to use
 @pre lsm303_mag_xfer_complete returns 1
 */
void lsm303_update_mag(lsm303_t *lsm);

/*!
 @brief Initialize an LSM303 istance
 @param lsm The LSM303 isntance to initialize
 */
void lsm303_init(lsm303_t *lsm);

/*!
 @brief Change the accelerometer full-scale output range
 @param lsm The LSM303 instance
 @param fs The new full-scale output range
 */
void lsm303_set_acc_fs(lsm303_t *lsm, lsm_acc_fs_t fs);

/*!
 @brief Set the output rate for the accelerometer
 @param lsm The LSM303 instance
 @param acc_rate The new accelerometer output rate
 */
void lsm303_set_acc_rate(lsm303_t *lsm, lsm_acc_rate_t acc_rate);

void lsm303_set_pm(lsm303_t *lsm, lsm_pm_t pm);

//! @name Batch Operations
//! @{

/*!
 @brief Begin a read of the LSM303 output data
 
 This only starts the read. The read is complete when lsm303_xfer_complete()
 returns 1.
 */
void lsm303_read_all(void);

/*!
 @brief Begin a read of the magnetometer output data
 
 This only starts the read. The read is complete when lsm303_mag_xfer_complete()
 returns 1.
 */
void lsm303_read_mag_all(void);

/*!
 @brief Begin a read of the accelerometer output data
 
 This only starts the read. The read is complete when lsm303_acc_xfer_complete()
 returns 1.
 */
void lsm303_read_acc_all(void);

/*!
 @brief Poll the state of the current transfer
 When this returns 1, it is fair to call lsm303_update()
 
 @return 1 if complete
 
 @pre lsm303_read_all was called
 @post lsm303_update_all may be called if return is 1
 */
int lsm303_xfer_complete_all(void);

/*!
 @brief Check to see if the magnetometer transfer is complete
 @return 1 if complete
 */
int lsm303_mag_xfer_complete_all(void);

/*!
 @brief Check to see if the accelerometer transfer is complete
 @return 1 if complete
 */
int lsm303_acc_xfer_complete_all(void);

/*!
 @brief Format the data received from the lsm303_read and store in data
 structures
 @pre lsm303_xfer_complete_all() returns 1
 */
void lsm303_update_all(void);

/*!
 @brief Format the accelerometer data from lsm303_read_acc
 @pre lsm303_xfer_complete_acc_all() returns 1
 */
void lsm303_update_acc_all(void);

/*!
 @brief Format the magnetometer data from lsm303_read_mag
 @pre lsm303_xfer_complete_all() returns 1
 */
void lsm303_update_mag_all(void);

/*!
 @brief Shortcut to perform a full read, block until complete, and update
 structures
 */
void lsm303_read_sync_all(void);

//! @}

/*!
 @brief Configure an interrupt channel
 @param lsm The LSM303 instance to use
 @param config The configuration structure to load (can be on stack)
 @return 1 if successful; 0 otherwise
 @note The channel must still be enabled to receive interrupts
 */
int lsm303_int_config(lsm303_t *lsm, lsm_int_config_t const *config);

/*!
 @brief Enable an interrupt channel
 @param lsm The LSM303 instance to use
 @param index The interrupt channel to enable
 */
void lsm303_int_enable(lsm303_t *lsm, lsm_int_index_t index);

/*!
 @brief Disable an interrupt channel
 @param lsm The LSM303 instance to use
 @param index The interrupt channel to disable
 */
void lsm303_int_disable(lsm303_t *lsm, lsm_int_index_t index);

//! @}

//! @}

#if HAS_MAGACC
extern lsm303_t magacc;
#endif

#endif

