#ifndef __JITTER_BUFFER_H_
#define __JITTER_BUFFER_H_

#include "stm32f10x.h"
#include "sensor_config.h"

/*!
 @addtogroup log
 @{
 @addtogroup jb Jitter Buffer
 @brief A static FIFO buffer to store frames of data to help accomodate the
 unpredictable write speeds on the SD card
 @{
 */

//! The number of frames to allocate
#define JB_LENGTH 8

//! A single frame of data to be stored
typedef struct {
	euclidean3_t acc;
	euclidean3_t mag;
	euler3_t     gyro;
	float        pressure;
	float        temperature;
	int          valid;
} jb_frame_t;

//! The jitter buffer instance
typedef struct {
	//! Storage for the FIFO
	jb_frame_t frames[JB_LENGTH];
	//! Read head within frames
	jb_frame_t *rd_ptr;
	//! Write head within frames
	jb_frame_t *wr_ptr;
} jitter_buffer_t;

/*!
 @brief Initialize a jitter buffer instance
 @param jb The instance structure to initialize
 @note This may be called multiple times on the same structure
 */
void jb_init(jitter_buffer_t *jb);

/*!
 @brief Take one item off of the jitter buffer FIFO
 @param jb The jitter buffer instance to use
 @param out A buffer to store the popped frame
 @return 1 if valid data was read; 0 otherwise
 */
int jb_pop(jitter_buffer_t *jb, jb_frame_t * RESTRICT out);

/*!
 @brief Put a new frame in the jitter buffer FIFO
 @param jb The jitter buffer instance to use
 @param out A buffer containing the frame to push
 @return 1 if the frame was successfully inserted; 0 otherwise
 */
int jb_push(jitter_buffer_t *jb, jb_frame_t const * in);

//! @} @}

#endif
