/*!
 @file log_format.h
 @brief Description of log file format
 */

#ifndef __LOG_FORMAT_H_
#define __LOG_FORMAT_H_

/*!
 @addtogroup log
 @addtogroup logformat Log Format
 @{
 */

#define LOG_VERSION 1

#define LOG_KEY 0xDEFEC8ED
#define LOG_PAGE_LEN 16384

#define LOG_MAX_SENSORS  16

/*!
 @name Available sensor flags
 Sensors readings will be provided in this order during frames when each is
 available
 @{
 */
#define LOG_SENSOR_GYR1   0
#define LOG_SENSOR_GYR2   1
#define LOG_SENSOR_ACC1   2
#define LOG_SENSOR_ACC2   3
#define LOG_SENSOR_MAG1   4
#define LOG_SENSOR_MAG2   5
#define LOG_SENSOR_PRESS1 6
#define LOG_SENSOR_PRESS2 7
#define LOG_SENSOR_TEMP1  8
#define LOG_SENSOR_TEMP2  9
//! @}

typedef struct {
	//! An key to identifying valid files and byte order
	uint32_t key;
	//! All valid files of any version will have a version ID
	uint32_t version;
	//! Device unique identifier
	uint8_t device_id[16];
	//! The length of a page in bytes
	uint32_t page_size;
	//! Master sample rate in Hz
	uint32_t sample_rate;
	//! The sub-rate of each sensor -- 0 indicates unavailable
	uint8_t sensor_sub_rate[LOG_MAX_SENSORS];
} log_config_header_t;

typedef struct {
	//! This frame type is valid only if this is true
	uint8_t is_active :1; 
	//! Number of samples at master rate 
	uint16_t run_length;
	
} log_page_active_header_t;

//! The header when sleeping is a run length in master samples
typedef uint32_t log_sleep_header_t;

//! @}

#endif
