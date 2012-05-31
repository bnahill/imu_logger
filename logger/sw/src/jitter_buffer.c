#include "stm32f10x.h"
#include "jitter_buffer.h"

//! @addtogroup jb
//! @{
	
//! @name Private methods
//! @{
void jb_copy(jb_frame_t * RESTRICT dst, jb_frame_t const * src);
//! @}

//! @}

void jb_init(jitter_buffer_t *jb){
	int i;
	__disable_irq();
	for(i = 0; i < JB_LENGTH; i++){
		jb->frames[i].valid = 0;
	}
	jb->rd_ptr = jb->frames;
	jb->wr_ptr = jb->frames;
	__enable_irq();
}

int jb_pop(jitter_buffer_t *jb, jb_frame_t * RESTRICT out){
	__disable_irq();
	if(jb->rd_ptr->valid){
		jb->rd_ptr->valid = 0;
		jb_copy(out, jb->rd_ptr);
		if(++jb->rd_ptr == &jb->frames[JB_LENGTH])
			jb->rd_ptr = jb->frames;
		__enable_irq();
		return 1;
	}
	__enable_irq();
	return 0;
}

int jb_push(jitter_buffer_t *jb, jb_frame_t const * in){
	__disable_irq();
	if(jb->wr_ptr->valid){
		// No space left!
		__enable_irq();
		return 0;
	}
	jb_copy(jb->wr_ptr, in);
	jb->wr_ptr->valid = 1;
	if(++jb->wr_ptr == &jb->frames[JB_LENGTH])
		jb->wr_ptr = jb->frames;
	__enable_irq();
	return 1;
}

void jb_copy(jb_frame_t * RESTRICT dst, jb_frame_t const * src){
	uint32_t const *iter1;
	uint32_t *iter2;
	iter1 = (uint32_t const *) src;
	iter2 = (uint32_t * RESTRICT) dst;
	while(iter1 < (uint32_t const *)(&src[1])){
		*iter2 = *iter1;
		iter1 += 1;
		iter2 += 1;
	}
}
