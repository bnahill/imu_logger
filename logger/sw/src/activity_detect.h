#ifndef __ACTIVITY_DETECT_H_
#define __ACTIVITY_DETECT_H_

#include "stm32f10x.h"
#include "sensor_config.h"
#include "jitter_buffer.h"

typedef enum {
	AC_RES_INCOMPLETE = 0,
	AC_RES_ACTIVE,
	AC_RES_INACTIVE
} ac_result_t;

/*!
 @brief Initialize or reset activity detector
 */
void ac_init(void);

/*!
 @brief Add a new frame of sensor data
 @param frame The new readings
 @return result
 
 @note Accelerometer and gyro must both be valid on this frame
 */
ac_result_t ac_add_frame(jb_frame_t *frame);

#endif
