/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 // Sprite
#define SPRITE_SIZE 32

// Highlight
#define HIGHTLIGHT_FADE_START 60
#define HIGHTLIGHT_FADE_END 120

// Tile
#define MAX_ELEVATION 24

// Floor
#define SEA_FLOOR 7
#define MAX_Z 15
#define UNDERGROUND_FLOOR SEA_FLOOR + 1
#define AWARE_UNDEGROUND_FLOOR_RANGE 2

// Things
#define INVISIBLE_TICKS_PER_FRAME 500
#define ITEM_TICKS_PER_FRAME 500
#define MISSILE_TICKS_PER_FRAME 75

// Animated/Static Text
#define ANIMATED_TEXT_DURATION 1000
#define STATIC_DURATION_PER_CHARACTER 60
#define MIN_STATIC_TEXT_DURATION 3000
