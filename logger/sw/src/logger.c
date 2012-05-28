#include "logger.h"
#include "lsm303.h"
#include "lpry.h"
#include "lps001.h"
#include "ff.h"

#include "log_format.h"


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
		0 // Temperature 2
	}
};

//! A buffer to contain the current page to write to the log
static uint32_t write_buffer[LOG_PAGE_LEN >> 2];

//! Pointer to the current page header in write_buffer
static uint32_t *frameset_header;

//! Number of cycles so far in the current frameset
static uint32_t cycle_count;

//! The data (not header) write head within write_buffer
static union {
	uint32_t  *u;
	float     *f;
} write_ptr;

static UINT write_len;
static char filename[32];
static FATFS fs;
static FIL file;
static FRESULT res;

const char *logger_init(const char *prefix){
	char *ptr, *suffix;
	int i, j, tmp;
	FILINFO fi;
	const uint8_t *u_id = (uint8_t *)0x1FFFF7E8;
	
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
	cycle_count = 0;
	
	return filename;
}

static INLINE uint32_t logger_space_left(void){
	return (&write_buffer[LOG_PAGE_LEN >> 2] - (write_ptr.u - 1)) << 2;
}

void logger_insert_silence(uint32_t duration){
	*(frameset_header++) = duration & 0x7FFFFFFF;
	write_ptr.u += 1;
	cycle_count = 0;
	if(logger_space_left() < LOG_MAX_FRAME_SIZE){
		if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK){
			while(1);
		}

		// There's enough room for this -- indicate no more framesets in this page
		*frameset_header = 0;
			
		write_ptr.u = write_buffer + 1;
		frameset_header = write_buffer;
		cycle_count = 0;
	}
}

void logger_update(void){
	int i;
	
	for(i = 0; i < LOG_MAX_SENSORS; i++){
		if(!log_header.sensor_sub_rate[i] || (cycle_count % log_header.sensor_sub_rate[i]))
			continue;
		switch(i){
		case LOG_SENSOR_GYR1:
			*(write_ptr.f++) = gyro.reading.pitch;
			*(write_ptr.f++) = gyro.reading.roll;
			*(write_ptr.f++) = gyro.reading.yaw;
			break;
		case LOG_SENSOR_ACC1:
			*(write_ptr.f++) = magacc.acc.x;
			*(write_ptr.f++) = magacc.acc.y;
			*(write_ptr.f++) = magacc.acc.z;
			break;
		case LOG_SENSOR_MAG1:
			*(write_ptr.f++) = magacc.mag.x;
			*(write_ptr.f++) = magacc.mag.y;
			*(write_ptr.f++) = magacc.mag.z;
			break;
		case LOG_SENSOR_PRESS1:
			*(write_ptr.f++) =  pressure.pressure;
			break;
		case LOG_SENSOR_TEMP1:
			*(write_ptr.f++) =  pressure.temperature;
			break;
		default:
			break;
		}
	}
	
	if(logger_space_left() < LOG_MAX_FRAME_SIZE){
		// Not enough space left in frame
		*frameset_header = 0x80000000 | cycle_count;
		
		if(write_ptr.u < &write_buffer[LOG_PAGE_LEN >> 2]){
			// Need to indicate that the page is over
			*(frameset_header+1) = 0;
		}
		
		if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK){
			while(1);
		}
		
		frameset_header = write_buffer;
		write_ptr.u = write_buffer + 1;
		cycle_count = 0;
	} else {
		cycle_count += 1;
	}
}

void logger_sync(void){
	if(cycle_count){
		// We were in the middle of a frameset, finalize it
		*(frameset_header++) = 0x80000000 | cycle_count;
	}
	
	// Indicate that there are no further framesets in this page
	*frameset_header = 0;
	
	// Commit the data
	if(f_write(&file, (uint8_t *)&write_buffer, LOG_PAGE_LEN, &write_len) != FR_OK)
		while(1);
	
	// Reset pointers
	frameset_header = write_buffer;
	write_ptr.u = write_buffer + 1;
	cycle_count = 0;
	
	// This doesn't really do anything but perhaps later it will
	f_sync(&file);
}

void logger_close(void){
	f_close(&file);
	f_mount(0, NULL);
}
