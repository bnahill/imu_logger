/*!
 @file sensor_config.h
 
 This file describes what sensors are connected to the platform and are to be
 used.
 */

#ifndef __SENSOR_CONFIG_H_
#define __SENSOR_CONFIG_H_

#include "toolchain.h"

/*!
 @addtogroup application
 @{
 @name Configuration Values
 @{
 */

/*!
 @brief Threshold for average power threshold for signal comprised of the sum
 of diffentiated accelerometer values to identify as active
 */
static const float activity_threshold = 0.017;

/*!
 @brief The number of consecutive inactive frames required to shut device off
 @sa NFRAMES
 */
static const int32_t activity_num_frames = 3;

//! @} @} Configuration Values


#ifndef BIT
#define BIT(x) (1 << x)
#endif

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define NULL8 ((uint8_t *)0)

//////////////////////////////////////////////////////////////////////////////
// L3GD20
//////////////////////////////////////////////////////////////////////////////

#define HAS_GYRO 1

//////////////////////////////////////////////////////////////////////////////
// LSM303
//////////////////////////////////////////////////////////////////////////////

#define HAS_MAGACC 1

//////////////////////////////////////////////////////////////////////////////
// LPS001
//////////////////////////////////////////////////////////////////////////////

#define HAS_PRESSURE 1

//////////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////////

#define USE_I2C1 ((HAS_MAGACC) ? 1 : 0)
#define USE_I2C2 ((HAS_PRESSURE) ? 1 : 0)
#define USE_I2C3 0

typedef struct {
	GPIO_TypeDef *const gpio;
	uint16_t const pin;
	uint8_t const pinsrc;
} af_gpio_pin_t;

typedef struct {
	GPIO_TypeDef *const gpio;
	uint16_t const pin;
} gpio_pin_t;

//! Representation of 3-dimensional Euclidean coordinates
typedef struct {
	float x;
	float y;
	float z;
} euclidean3_t;

//! Representation of 3-dimensional Euler angles
typedef struct {
	float pitch;
	float roll;
	float yaw;
} euler3_t;

#endif

