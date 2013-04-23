/*
 * circular_buffer.c
 *
 *  Created on: Apr 19, 2013
 *      Author: mading
 */
#include <si32_device.h>
#include "circular_buffer.h"

// in buffers for USB IN endpoint
uint8_t ep1_in_buffer[EP1_BUFFER_SIZE];
uint8_t ep2_in_buffer[EP2_BUFFER_SIZE];
uint8_t ep3_in_buffer[EP3_BUFFER_SIZE];
uint8_t ep4_in_buffer[EP4_BUFFER_SIZE];

// out buffers for USB OUT endpoint
uint8_t ep1_out_buffer[EP1_BUFFER_SIZE];
uint8_t ep2_out_buffer[EP2_BUFFER_SIZE];
uint8_t ep3_out_buffer[EP3_BUFFER_SIZE];
uint8_t ep4_out_buffer[EP4_BUFFER_SIZE];

circular_buffer_pools_t EPn_cb_in[EPn_NUMBER];
circular_buffer_pools_t EPn_cb_out[EPn_NUMBER];

static void circular_buffer_initialize(circular_buffer_pools_t * cb, uint8_t * buf, uint32_t size,uint32_t ep_size)
{
    cb->head = cb->tail = cb->count = 0;
    cb->read_ready = 0;
    cb->write_ready = 1;
    cb->timer_reload = 65536 - (TIMER0_BASE_FREQ / 1000); // 1ms;
    cb->buffer = buf;
    cb->size = size;
    cb->ep_size = ep_size;
}

void circular_buffer_init()
{
    circular_buffer_initialize(&EPn_cb_in[0],ep1_in_buffer,EP1_BUFFER_SIZE,EP1_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_in[1],ep2_in_buffer,EP2_BUFFER_SIZE,EP2_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_in[2],ep3_in_buffer,EP3_BUFFER_SIZE,EP3_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_in[3],ep4_in_buffer,EP4_BUFFER_SIZE,EP4_PACKET_SIZE);

    circular_buffer_initialize(&EPn_cb_out[0],ep1_out_buffer,EP1_BUFFER_SIZE,EP1_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_out[1],ep2_out_buffer,EP2_BUFFER_SIZE,EP2_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_out[2],ep3_out_buffer,EP3_BUFFER_SIZE,EP3_PACKET_SIZE);
    circular_buffer_initialize(&EPn_cb_out[3],ep4_out_buffer,EP4_BUFFER_SIZE,EP4_PACKET_SIZE);
}

circular_buffer_pools_t * circular_buffer_pointer(uint8_t ep_address)
{// ep_address bit 7 indicate direction, 1: IN, 0:OUT
    uint8_t idx = (ep_address & 0x7F) - 1;
    if(ep_address & 0x80)
    {
        return &(EPn_cb_in[idx]);
    }
    else
    {
        return &(EPn_cb_out[idx]);
    }
}

int inline circular_buffer_read_ready(circular_buffer_pools_t * cb)
{
    return cb->read_ready;
}

void inline circular_buffer_put_read_ready(circular_buffer_pools_t * cb, uint8_t val)
{
    cb->read_ready = val;
}

int inline circular_buffer_write_ready(circular_buffer_pools_t * cb)
{
    return cb->write_ready;
}

void inline circular_buffer_put_write_ready(circular_buffer_pools_t * cb,uint8_t val)
{
    cb->write_ready = val;
}

int inline circular_buffer_is_empty(circular_buffer_pools_t * cb)
{
    return (cb->count == 0);
}

int inline circular_buffer_is_full(circular_buffer_pools_t * cb)
{
    return (cb->count == cb->size);
}

uint32_t inline circular_buffer_count(circular_buffer_pools_t * cb)
{
    return (cb->count);
}

uint32_t inline circular_buffer_remain_count(circular_buffer_pools_t * cb)
{
    return (cb->size - cb->count);
}

uint16_t inline circular_buffer_ep_size(circular_buffer_pools_t * cb)
{
    return (cb->ep_size);
}

uint16_t inline circular_buffer_timer_reload(circular_buffer_pools_t * cb)
{
    return cb->timer_reload;
}

void circular_buffer_push(circular_buffer_pools_t * cb, uint8_t *elem)
{
    if(circular_buffer_is_full(cb))
        return ;
    cb->buffer[cb->head] = *elem;
    TURN_OFF_INTERRUPTS;
    cb->head++;
    cb->head %= cb->size;
    cb->count ++;
    TURN_ON_INTERRUPTS;
}

int32_t circular_buffer_pop(circular_buffer_pools_t * cb,uint8_t *elem)
{
    if(circular_buffer_is_empty(cb))
        return -1;
    *elem = cb->buffer[cb->tail];
    TURN_OFF_INTERRUPTS;
    cb->tail++;
    cb->tail %= cb->size;
    cb->count--;
    TURN_ON_INTERRUPTS;
    return 0;
}

/*
 *   elem: is target buffer pointer;
 *   count: number of data to write in.
 *   Increase: 1: elem pointer increase, 0: pointer address fixed
 *   return value: actually write data count
 */
uint32_t circular_buffer_write(circular_buffer_pools_t * cb, uint8_t *elem, uint32_t count, uint8_t increase)
{
    uint32_t cnt = MIN(count,circular_buffer_remain_count(cb));
    uint32_t temp = cnt;

    uint8_t *ptr =&cb->buffer[cb->head];

    while(cnt)
    {
        *ptr++ = *elem;
        if(increase == 1)
        {
            elem++;
        }
        if (ptr == &cb->buffer[cb->size])
        {
            ptr = &cb->buffer[0x00];
        }
        cnt--;
    }
    TURN_OFF_INTERRUPTS;
    cb->head += temp;
    cb->head %= cb->size;
    cb->count += temp;
    TURN_ON_INTERRUPTS;
    return temp;
}

/*
 *   elem: is target buffer pointer;
 *   count: number of data to read out.
 *   Increase: 1: elem pointer increase, 0: pointer address fixed
 *   return value: actually read data count
 */
uint32_t circular_buffer_read(circular_buffer_pools_t * cb,uint8_t *elem,uint32_t count,uint8_t increase)
{
    uint16_t ep_size =  circular_buffer_ep_size(cb);
    uint32_t cnt = MIN(count, circular_buffer_count(cb));
    uint32_t temp = cnt;
    uint8_t *ptr = &cb->buffer[cb->tail];

    while(cnt)
    {
        *elem = *ptr++;
        if(increase == 1)
        {
            elem ++;
        }
        if (ptr == &cb->buffer[cb->size])
        {
            ptr = &cb->buffer[0x00]; // Handle pointer rollover
        }
        cnt--;
    }

    TURN_OFF_INTERRUPTS;
    cb->count -= temp;
    cb->tail += temp;
    cb->tail %= cb->size;
    TURN_ON_INTERRUPTS;

    return temp;
}
