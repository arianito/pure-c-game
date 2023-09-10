#pragma once
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

#include "../input.h"
#include "../mathf.h"
#include "../draw.h"
#include "../game.h"
#include "../debug.h"
#include "../sort.h"
#include "../camera.h"
#include "../memory/buddy.h"
#include "../memory/utils.h"

BuddyMemory *buddy;
enum
{
	npool = 200
};
size_t pools[npool];
float lastHit = 0;

void memorydebug_create()
{
	buddy = make_buddy(9);
}

void memorydebug_update()
{

	draw_cubef(vec3_zero, color_blue, 20);
	draw_cubef(vec3_zero, color_blue, 40);

	if (input_keypress(KEY_SPACE) && (time->time - lastHit > 0.07f))
	{
		sort_quick(pools, 0, npool - 1);
		void *ptr = (void *)pools[0];
		if (ptr == NULL)
		{
			size_t size = (int)powf(2, (int)(randf() * 6 + 3));
			void *newPtr = buddy_alloc(buddy, size);
			if (newPtr != NULL)
			{
				pools[0] = (size_t)newPtr;
			}
		}
		lastHit = time->time;
	}
	if (input_keypress(KEY_M) && (time->time - lastHit > 0.1f))
	{
		sort_quick(pools, 0, npool - 1);
		int a = npool - 1;
		for (int i = npool - 1; i >= 0; i--)
		{
			if (pools[i] != 0 && i < a)
			{
				a = i;
			}
		}
		sort_shuffle(pools, a, npool - 1);
		void *ptr = (void *)pools[npool - 1];
		if (ptr != NULL)
		{
			if (buddy_free(buddy, &ptr))
			{
				pools[npool - 1] = 0;
			}
		}
		lastHit = time->time;
	}
}