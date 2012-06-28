#include "activity_detect.h"
#include "sensor_config.h"
#include "arm_math.h"

/*!
 @brief The number of accelerometer frames to use for detection
 @sa LOG_PAGE_LEN
 */
#define NFRAMES 400

static struct {
	float activity_per_frame[NFRAMES - 1];
	int frame_count;
	euclidean3_t last;
} ac;

void ac_init(void){
	ac.frame_count = 0;
}

static INLINE void euclidean3_sub(euclidean3_t *s1, euclidean3_t *s2, euclidean3_t *dst){
	dst->x = s1->x - s2->x;
	dst->y = s1->y - s2->y;
	dst->z = s1->z - s2->z;
}

static INLINE void euclidean3_copy(euclidean3_t *src, euclidean3_t *dst){
	dst->x = src->x;
	dst->y = src->y;
	dst->z = src->z;
}

/*!
 @brief Generate some measure of change between frames
 */
static INLINE float euclidean3_measure(euclidean3_t *diff){
	return (diff->x * diff->x) +
	       (diff->y * diff->y) +
	       (diff->z * diff->z);
}

ac_result_t ac_add_frame(jb_frame_t *frame){
	euclidean3_t tmp;
	float result;
	if(ac.frame_count){
		// This isn't the first frame...
		
		euclidean3_copy(&frame->acc, &tmp);
		euclidean3_sub(&tmp, &ac.last, &tmp);
		
		ac.activity_per_frame[ac.frame_count - 1] = euclidean3_measure(&tmp);
		
	}
	ac.frame_count += 1;
	if(ac.frame_count == NFRAMES - 1){
		arm_power_f32(ac.activity_per_frame, NFRAMES - 1, &result);
		// Convert to average power
		result /= (NFRAMES - 1);
		
		ac_init();
		
		if(result < activity_threshold){
			return AC_RES_INACTIVE;
		}
		return AC_RES_ACTIVE;
	}
	euclidean3_copy(&frame->acc, &ac.last);
	return AC_RES_INCOMPLETE;
}
