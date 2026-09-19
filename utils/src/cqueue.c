//
// Created by tomly on 19/09/2026.
//

#include <cqueue.h>

void CQueue_Init(cqueue_t* queue, uint32_t* buffer, uint8_t capacity)
{
	queue->buffer = buffer;
	queue->capacity = capacity;
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
}

bool_t CQueue_Push(cqueue_t* queue, uint32_t value)
{
	if (CQueue_IsFull(queue))
		return FALSE;
	
	queue->buffer[queue->head] = value;
	queue->head = (queue->head + 1) % queue->capacity;
	queue->count++;
	
	return TRUE;
}

bool_t CQueue_Pop(cqueue_t* queue, uint32_t* value)
{
	if (CQueue_IsEmpty(queue))
		return FALSE;
	
	*value = queue->buffer[queue->tail];
	queue->tail = (queue->tail + 1) % queue->capacity;
	queue->count--;
	
	return TRUE;
}

bool_t CQueue_IsEmpty(const cqueue_t* queue)
{
	return queue->count == 0;
}

bool_t CQueue_IsFull(const cqueue_t* queue)
{
	return queue->count == queue->capacity;
}

uint8_t CQueue_Count(const cqueue_t* queue)
{
	return queue->count;
}

void CQueue_Clear(cqueue_t* queue)
{
	queue->head = 0;
	queue->tail = 0;
	queue->count = 0;
}
