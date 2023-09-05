#pragma once

#include "input.h"
#include "memory/stack.h"
#include "memory/utils.h"

StackMemory *stack = NULL;
int j = 0;
void *pools[10];

void memorydebug_create()
{
	stack = make_stack(256);

	size_t a = 0x657;
	unsigned char b = 0x87;
}

void memorydebug_update()
{

	draw_bbox(BBox{{-10, 0, 0}, {10, (float)stack->size, 5}}, color_gray);

	int space = calculate_space(sizeof(StackMemory), sizeof(size_t));
	float end = (float)(space + stack->padding);

	draw_bbox(BBox{{-10, 0, 0}, {10, end, 40}}, color_darkred);
	draw_bbox(BBox{{-10, end, 0}, {10, (float)stack->offset, 6}}, color_yellow);
	draw_bbox(BBox{{-10, end, 0}, {10, (float)stack->peak, 4}}, color_blue);

	if (stack->peak != stack->offset)
	{
		draw_bbox(BBox{{-10, (float)stack->peak, 0}, {10, (float)stack->size, 40}}, color_darkred);
	}

	size_t start = (size_t)stack - stack->padding;
	space = calculate_space(sizeof(StackMemoryNode), sizeof(size_t));
	StackMemoryNode *node = (StackMemoryNode *)stack->head;

	end = (float)stack->offset;
	while (node != NULL)
	{

		size_t offset;
		unsigned char pad;
		byte7d(node->data, &offset, &pad);

		float head = (float)((size_t)node - start);
		float data = (float)((size_t)node - start + pad);

		draw_bbox(BBox{{-10, head, 15}, {10, data, 20}}, color_gray);
		draw_bbox(BBox{{-10, end, 0}, {10, data, 25.0f}}, color_red);

		if (offset == 0)
			break;
		node = (StackMemoryNode *)(start + offset);
		end = head;
	}

	if (input_keydown(KEY_SPACE))
	{
		if (j < 10)
		{
			size_t newSize = (size_t)(randf() * 50) + 3;
			void *ptr = stack_alloc(stack, newSize, 8);
			if (ptr != NULL)
			{
				printf("alloc %d -> %zu \n", j, stack->offset);
				pools[j] = ptr;
				j++;
			}
		}
	}

	if (input_keydown(KEY_M))
	{
		if (j > 0)
		{
			j--;
			if (stack_free(stack, &pools[j]))
			{
				printf("free %d -> %d \n", j, stack->offset);
			}
		}
	}
}