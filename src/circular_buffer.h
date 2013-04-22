/*
 * circular_buffer.h
 *
 *  Created on: Apr 19, 2013
 *      Author: mading
 */

#ifndef __CIRCULAR_BUFFER_H__
#define __CIRCULAR_BUFFER_H__
#include "config.h"

#define  CIRCULAR_BUFFER_SIZE 2048

#define EP1_BUFFER_SIZE CIRCULAR_BUFFER_SIZE
#define EP2_BUFFER_SIZE CIRCULAR_BUFFER_SIZE
#define EP3_BUFFER_SIZE CIRCULAR_BUFFER_SIZE
#define EP4_BUFFER_SIZE CIRCULAR_BUFFER_SIZE

typedef struct
{
    uint8_t  *buffer;
    uint32_t size;  // circular buffer size
    uint32_t count; // total number available in buffer
    uint32_t head;  // write pointer
    uint32_t tail;  // read pointer
    uint8_t read_ready;  // buffer read ready flag. 1: ready to read buffer; 0: not ready to read buffer
    uint8_t write_ready;    // buffer write ready flag. 1: can write into buffer; 0: cannot write into buffer
    uint16_t ep_size;     // endpoint size
    uint16_t timer_reload;  // each buffer timer reload value
}circular_buffer_pools_t;

extern circular_buffer_pools_t EPn_cb_in[EPn_NUMBER];
extern circular_buffer_pools_t EPn_cb_out[EPn_NUMBER];

extern void circular_buffer_init();
extern circular_buffer_pools_t * circular_buffer_pointer(uint8_t ep_address);
extern int inline circular_buffer_read_ready(circular_buffer_pools_t * cb);
extern void inline circular_buffer_put_read_ready(circular_buffer_pools_t * cb, uint8_t val);
extern int inline circular_buffer_write_ready(circular_buffer_pools_t * cb);
extern void inline circular_buffer_put_write_ready(circular_buffer_pools_t * cb,uint8_t val);
extern int inline circular_buffer_is_empty(circular_buffer_pools_t * cb);
extern int inline circular_buffer_is_full(circular_buffer_pools_t * cb);
extern uint32_t inline circular_buffer_count(circular_buffer_pools_t * cb);
extern uint32_t inline circular_buffer_remain_count(circular_buffer_pools_t * cb);
extern uint16_t inline circular_buffer_ep_size(circular_buffer_pools_t * cb);
extern uint16_t inline circular_buffer_timer_reload(circular_buffer_pools_t * cb);
extern void circular_buffer_push(circular_buffer_pools_t * cb, uint8_t *elem);
extern int32_t circular_buffer_pop(circular_buffer_pools_t * cb,uint8_t *elem);
extern uint32_t circular_buffer_write(circular_buffer_pools_t * cb, uint8_t *elem, uint32_t count, uint8_t increase);
extern uint32_t circular_buffer_read(circular_buffer_pools_t * cb,uint8_t *elem,uint32_t count,uint8_t increase);

#endif /* __CIRCULAR_BUFFER_H__ */
