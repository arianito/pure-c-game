/******************************************************************************
 *                                                                            *
 *  Copyright (c) 2023 Aryan Alikhani                                      *
 *  GitHub: github.com/arianito                                               *
 *  Email: alikhaniaryan@gmail.com                                            *
 *                                                                            *
 *  Permission is hereby granted, free of charge, to any person obtaining a   *
 *  copy of this software and associated documentation files (the "Software"),*
 *  to deal in the Software without restriction, including without limitation *
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,  *
 *  and/or sell copies of the Software, and to permit persons to whom the      *
 *  Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 *  The above copyright notice and this permission notice shall be included   *
 *  in all copies or substantial portions of the Software.                    *
 *                                                                            *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   *
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                *
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN *
 *  NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     *
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 *  USE OR OTHER DEALINGS IN THE SOFTWARE.                                   *
 *                                                                            *
 *****************************************************************************/
#include "memory/pool.h"

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "memory/utils.h"

void pool_enqueue(PoolMemory *self, PoolMemoryNode *node)
{
	node->next = BYTE71((size_t)self->head, 0);
	self->head = node;
}

PoolMemoryNode *pool_dequeue(PoolMemory *self)
{
	if (self->head == NULL)
		return NULL;

	PoolMemoryNode *node = self->head;
	node->next = BYTE71_SET_1(node->next, 1);
	self->head = (PoolMemoryNode *)(BYTE71_GET_7(node->next));
	return node;
}

void *pool_alloc(PoolMemory *self)
{
	if (self == NULL)
	{
		printf("pool: alloc failed, invalid instance\n");
		return NULL;
	}
	if (self->head == NULL)
	{
		printf("pool: alloc failed, insufficient memory\n");
		return NULL;
	}
	PoolMemoryNode *node = pool_dequeue(self);
	self->capacity--;
	const unsigned int space = MEMORY_SPACE_STD(PoolMemoryNode);
	return (void *)((size_t)node + space);
}

unsigned char pool_free(PoolMemory *self, void **p)
{
	if (self == NULL)
	{
		printf("pool: free failed, invalid instance\n");
		return 0;
	}
	if (p == NULL || (*p) == NULL)
	{
		printf("pool: free failed, invalid pointer\n");
		return 0;
	}
	size_t start = (size_t)self - self->padding;
	size_t address = (size_t)(*p);
	size_t end = start + self->size;

	if (!(address >= start && address < end))
	{
		printf("pool: free failed, out of boundary\n");
		return 0;
	}

	const unsigned int space = MEMORY_SPACE_STD(PoolMemoryNode);
	PoolMemoryNode *node = (PoolMemoryNode *)(address - space);

	unsigned char used = BYTE71_GET_1(node->next);
	if (!used)
	{
		printf("pool: free failed, already freed\n");
		return 0;
	}

	pool_enqueue(self, node);
	self->capacity++;
	(*p) = NULL;
	return 1;
}

void pool_destroy(PoolMemory **self)
{
	if (self == NULL || *self == NULL)
	{
		printf("pool: destroy failed, invalid instance\n");
		return;
	}
	size_t op = (size_t)(*self) - (*self)->padding;
	free((void *)(op));
	(*self) = NULL;
}

PoolMemory *pool_create(void *m, size_t size, unsigned int chunkSize)
{
	size_t start = (size_t)m;
	unsigned int space = MEMORY_SPACE_STD(PoolMemory);
	unsigned int padding = MEMORY_PADDING_STD(start);
	PoolMemory *self = (PoolMemory *)(start + padding);
	self->head = NULL;
	self->size = size;
	self->padding = padding;
	self->capacity = 0;
	size_t cursor = padding + space;
	space = MEMORY_SPACE_STD(PoolMemoryNode);
	while (1)
	{
		size_t address = start + cursor;
		padding = MEMORY_ALIGNMENT_STD(address, PoolMemoryNode);
		cursor += padding + chunkSize;
		if (cursor > size)
			break;

		pool_enqueue(self, (PoolMemoryNode *)(address + padding - space));
		self->capacity++;
	}
	return self;
}

PoolMemory *make_pool(size_t size, unsigned int chunkSize)
{
	void *m = malloc(size);
	if (m == NULL)
	{
		printf("pool: make failed, system can't provide free memory\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	return pool_create(m, size, chunkSize);
}

PoolMemory *make_pool_exact(size_t size, unsigned int chunkSize)
{
	if (size % chunkSize != 0)
	{
		printf("pool: make failed, invalid chunk size\n");
		exit(EXIT_FAILURE);
		return NULL;
	}
	unsigned int n = size / chunkSize;
	size += MEMORY_SPACE_STD(PoolMemory) + sizeof(size_t);
	size += n * sizeof(PoolMemoryNode);
	return make_pool(size, chunkSize);
}