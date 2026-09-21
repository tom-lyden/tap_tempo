//
// Created by tomly on 19/09/2026.
//

#ifndef CQUEUE_H
#define CQUEUE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint32_t* buffer;
	uint8_t capacity;
	uint8_t count;
	uint8_t head;
	uint8_t tail;
} cqueue_t;

void CQueue_Init(cqueue_t* queue, uint32_t* buffer, uint8_t capacity);
bool CQueue_Push(cqueue_t* queue, uint32_t value);
bool CQueue_Pop(cqueue_t* queue, uint32_t* value);
bool CQueue_IsEmpty(const cqueue_t* queue);
bool CQueue_IsFull(const cqueue_t* queue);
uint8_t CQueue_Count(const cqueue_t* queue);
void CQueue_Clear(cqueue_t* queue);

#endif // CQUEUE_H
