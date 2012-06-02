#include "logger.h"
#include "lsm303.h"
#include "lpry.h"
#include "lps001.h"
#include "ff.h"
#include "jitter_buffer.h"

#include "log_format.h"

/*!
 @addtogroup log Logger
 @brief A utility to record readings to the SD card
 
 @section log_format Log Format
 A log format is defined in log_format.h for various structures.
 All writes are done in multiples of LOG_PAGE_LEN. Each such write is called a
 \a page. This is done to save time on writes and minimize wear on the storage
 medium. Within each page may be multiple @a framesets, each of which contains
 a contiguous set of recorded data. Each frameset has a 4-byte header which
 indicates whether the frameset contains data or silence and the length, in
 @a cycles per second. A cycle is the least common multiple of the sampling
 rates, or the minimum rate required to accomodate all sensor rates.
 
 @section header Header
 Each file is prefixed with a header of one page in length. This contains a
 magic number for verification, a log format version number, a 16-byte device
 ID, the page size used, and all involved sampling rates. From this
 information, data can be read page-by-page
 
 @section framesets Framesets
 A page may contain multiple @a framesets, or sets of data frames representing
 a single contiguous time period. The frameset is identified by a 4-byte
 header indicating the number of cycles the frameset represents and whether
 the data is silence or active sensor readings.
 
 @section data_packing Data Packing
 @p
 The frameset header does not indicate the length of the data set provided,
 though the value could be calculated from the sample rates and number of
 frames present.
 
 @p
 A frameset which contains data must contain a multiple of a complete sensor
 cycle, where all sensors are read an equivalent number of times to allow for
 interpolation on concatenated framesets to provide data at the master sample
 rate. A practically necessary exception to this appears when a period of
 silence arrives at a time not synchronized with a full cycle of data. At this
 time it isn't important as that data will not be concatenated with another
 set.
 @{
 @name Private variables
 @{
 */

static log_config_header_t log_header = {
	LOG_KEY,
	LOG_VERSION,
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	LOG_PAGE_LEN,
	50,
	{
		1, // Gyro 1;
		0, // Gyro 2
		1, // Acc 1
		0, // Acc 2
		2, // Mag 1
		0, // Mag 2
		4, // Pressure 1
		0, // Pressure 2
		0, // Temperature 1
		0, // Temperature 2
		0,
		0,
		0,
		0,
		0,
		0
	}
};

//! A buffer to contain the current page to write to the log
static uint32_t write_buffer[LOG_PAGE_LEN >> 2];

//! Pointer to the current page header in write_buffer
static uint32_t *frameset_header;

//! Number of frames so far in the current frameset
static uint32_t frame_count;

//! Number of frames so far in the current cycle
static uint32_t frame_cycle_count;

//! The data (not header) write head within write_buffer
static union {
	uint32_t  *u;
	float     *f;
} write_ptr;

//! A buffer used for the length reported by f_write()
static UINT write_len;
static char filename[32];
static FATFS fs;
static FIL file;
static FRESULT res;
static uint32_t cycle_len;
static uint32_t cycle_lcm;

//! @}
//! @name Private methods
//! @{
static uint32_t logger_calc_cycle_len(void);
static INLINE uint32_t logger_space_left(void);
//! @}

//! @}

static uint32_t logger_calc_cycle_len(void){
	uint32_t i, j, num_samples, cycle_bytes;
	// Ghetto least-common-multiple calculation
	for(i = 1; i < 1000; i++){
		for(j = 0; j < LOG_MAX_SENSORS; j++){
			if(log_header.sensor_sub_rate[j] && (i % log_header.sensor_sub_rate[j])){
				// If it's not a multiple of this one, try the next one
				break;	
			}
		}
		if(j == LOG_MAX_SENSORS)
			break;
	}
	if(i == 1000){
		// You picked some really messed up sample rates
		while(1);
	}
	// i = LCM
	cycle_lcm = i;
	
	cycle_bytes = 0;
	
	for(i = 0; i < LOG_MAX_SENSORS; i++){
		if(!log_header.sensor_sub_rate[i])
			continue;
		
		num_samples = cycle_lcm / log_header.sensor_sub_rate[i];
		
		switch(i){		
		case LOG_SENSOR_GYR1:
		case LOG_SENSOR_GYR2:
		case LOG_SENSOR_ACC1:
		case LOG_SENSOR_ACC2:
		case LOG_SENSOR_MAG1:
		case LOG_SENSOR_MAG2:
			// For 3-D values, there are 3 floats
			cycle_bytes += num_samples * 3;
			break;
		case LOG_SENSOR_PRESS1:
		case LOG_SENSOR_PRESS2:
		case LOG_SENSOR_TEMP1:
		case LOG_SENSOR_TEMP2:
			// Others only have 1
			cycle_bytes += num_samples;
			break;
		default:
			break;
		}
	}
	return cycle_bytes;
}

static void led_init(void){
	GPIO_InitTypeDef gpio_init_s;
	
	// Initialize GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIOC->BRR = BIT(3);
	
	GPIO_StructInit(&gpio_init_s);
	gpio_init_s.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_s.GPIO_Pin = BIT(3);
	GPIO_Init(GPIOC, &gpio_init_s);
}

static void led_on(void){
	GPIOC->BSRR = BIT(3);
}

static void led_off(void){
	GPIOC->BRR = BIT(3);
}
	
const char *logger_init(const char *prefix){
	char *ptr, *suffix;
	int i, j, tmp;
	FILINFO fi;
	const uint8_t *u_id = (uint8_t *)0x1FFFF7E8;
	
	led_init();
	
	if(f_mount(0, &fs) != FR_OK){
		return NULL;
	}
	// Copy the string and make a pointer to the beginning of the suffix
	for(suffix = filename; *prefix; suffix++, prefix++){
		*suffix = *prefix;
	}
	
	// Test suffixes in order starting from '1'
	for(i = 1; i < 10000; i++){
		ptr = suffix;
		j = i;
		if(j >= 1000){
			tmp = j / 1000;
			*(ptr++) = tmp + '0';
			j -= tmp * 1000;
		}
		if(j >= 100){
			tmp = j / 100;
			*(ptr++) = tmp + '0';
			j -= tmp * 100;
		}
		if(j >= 10){
			tmp = j / 10;
			*(ptr++) = tmp + '0';
			j -= tmp * 10;
		}

		*(ptr++) = j + '0';
		
		*ptr = 0;
		if(f_stat(filename, &fi) == FR_NO_FILE){
			break;
		}
	} if(i == 10000){
		f_mount(0, NULL);
		return NULL;
	}
	
	
	res = f_open(&file, filename, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
	
	if(res != FR_OK){
		return NULL;
	}
	
	// Copy the device ID in the order prescribed by the reference manual
	// The MSB is at the index 0 (big endian)
	log_header.device_id[11] = u_id[1];
	log_header.device_id[10] = u_id[0];
	log_header.device_id[9] = u_id[3];
	log_header.device_id[8] = u_id[2];
	log_header.device_id[7] = u_id[7];
	log_header.device_id[6] = u_id[6];
	log_header.device_id[5] = u_id[5];
	log_header.device_id[4] = u_id[4];
	log_header.device_id[3] = u_id[11];
	log_header.device_id[2] = u_id[10];
	log_header.device_id[1] = u_id[9];
	log_header.device_id[0] = u_id[8];
	
	cycle_len = logger_calc_cycle_len();
	
	// Seek to pre-allocate
	if(f_lseek(&file, LOG_PAGE_LEN) != FR_OK)
		return NULL;
	if(f_lseek(&file, 0) != FR_OK)
		return NULL;
	
	// Write header
	if(f_write(&file, (uint8_t *)&log_header, sizeof(log_header), &write_len) != FR_OK)
		return NULL;
	
	// Seek to end of header
	if(f_lseek(&file, LOG_PAGE_LEN) != FR_OK)
		return NULL;
	
	frameset_header = write_buffer;
	write_ptr.u = write_buffer + 1;
	frame_count = 0;
	frame_cycle_count = 0;
	
	return filename;
}

static INLINE uint32_t logger_space_left(void){
	return &write_buffer[LOG_PAGE_LEN >> 2] - write_ptr.u;
}

void logger_insert_silence(uint32_t duration){
	*(frameset_header++) = duration & 0x7FFFFFFF;
	write_ptr.u = frameset_header + 1;
	frame_count = 0;
	frame_cycle_count = 0;
	if(logger_space_left() < cycle_len){
		if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK){
			while(1);
		}

		// There's enough room for this -- indicate no more framesets in this page
		*frameset_header = 0;
			
		write_ptr.u = write_buffer + 1;
		frameset_header = write_buffer;
	}
}

void logger_update(jb_frame_t const * frame){
	int i;
	for(i = 0; i < LOG_MAX_SENSORS; i++){
		if(!log_header.sensor_sub_rate[i] || (frame_count % log_header.sensor_sub_rate[i]))
			continue;
		switch(i){
		case LOG_SENSOR_GYR1:
			*(write_ptr.f++) = frame->gyro.pitch;
			*(write_ptr.f++) = frame->gyro.roll;
			*(write_ptr.f++) = frame->gyro.yaw;
			break;
		case LOG_SENSOR_ACC1:
			*(write_ptr.f++) = frame->acc.x;
			*(write_ptr.f++) = frame->acc.y;
			*(write_ptr.f++) = frame->acc.z;
			break;
		case LOG_SENSOR_MAG1:
			*(write_ptr.f++) = frame->mag.x;
			*(write_ptr.f++) = frame->mag.y;
			*(write_ptr.f++) = frame->mag.z;
			break;
		case LOG_SENSOR_PRESS1:
			*(write_ptr.f++) =  frame->pressure;
			break;
		case LOG_SENSOR_TEMP1:
			*(write_ptr.f++) =  frame->temperature;
			break;
		default:
			break;
		}
	}
	
	frame_count += 1;
	frame_cycle_count += 1;
	
	if(frame_cycle_count != cycle_lcm)
		return;
	
	// We now have a new complete cycle of data
	frame_cycle_count = 0;
	
	if(logger_space_left() < cycle_len){
		led_on();
		// Not enough space left in frame
		*frameset_header = 0x80000000 | frame_count;
		
		if(write_ptr.u < &write_buffer[LOG_PAGE_LEN >> 2]){
			// Need to indicate that the page is over
			*write_ptr.u = 0;
		}
		
		if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK){
			while(1);
		}
		
		frameset_header = write_buffer;
		write_ptr.u = write_buffer + 1;
		frame_count = 0;
		led_off();
	}
}

void logger_sync(void){
	if(write_ptr.u == write_buffer + 1){
		// Nothing has been written to the new page
		f_sync(&file);
		return;
	}
		
	if(frame_count && (frame_count - frame_cycle_count)){
		// We were in the middle of a frameset, finalize it
		// But first rewind to the last full cycle
		frame_count -= frame_cycle_count;
		write_ptr.u = frameset_header + 1 + cycle_len * (frame_count / cycle_lcm);
		*(frameset_header) = 0x80000000 | frame_count;
		// Indicate that there are no further framesets in this page
		*write_ptr.u = 0;
	} else {
		*frameset_header = 0;
	}
	
	// Check to see if it's worth writing the page before doing it
	if((frameset_header != write_buffer) || (*frameset_header != 0)){
		// Commit the data
		if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK)
			while(1);
	}
	
	// Reset pointers
	frameset_header = write_buffer;
	write_ptr.u = write_buffer + 1;
	frame_count = 0;
	frame_cycle_count = 0;
	
	// This doesn't really do anything but perhaps later it will
	f_sync(&file);
}

void logger_close(void){
	f_close(&file);
	f_mount(0, NULL);
}

