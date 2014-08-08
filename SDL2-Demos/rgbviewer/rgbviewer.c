/**
 * Display raw RGB file.
 *
 * Copyright 2014 Zhang Zhaolong <zhangzhaolong0454@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

#define demo_info printf
#define demo_err printf

/* arguments */
char filename[255];
char format[5];
Uint32 width;
Uint32 height;
Uint8 bpp;

/* masks[0]: RMASK
 * masks[1]: GMASK
 * masks[2]: BMASK
 * masks[3]: AMASK*/
Uint32 masks[4];

Uint32 rmask;
Uint32 gmask;
Uint32 bmask;
Uint32 amask;


int set_mask()
{
	char *tmp;
	int pos;
	int i;
	char color[4] = { 'R', 'G', 'B', 'A' };

	format[4] = 0;

	demo_info("format: %s\n", format);

	if (bpp == 32) {
		for (i = 0; i < 4; i++) {
			tmp = strchr(format, color[i]);
			if (!tmp) {
				demo_err("invalid arguments.\n");
				return -1;
			}
			pos = tmp - format;
			masks[i] = 0x000000ff << (pos * 8);
		}
	}

	demo_info("RMASK: 0x%08x, GMASK: 0x%08x, BMASK: 0x%08x, AMASK: 0x%08x\n",
			masks[0], masks[1], masks[2], masks[3]);
	rmask = masks[0];
	gmask = masks[1];
	bmask = masks[2];
	amask = masks[3];

	return 0;
}

void usage()
{
	printf("Usage: \n"
		"    $rgbplayer <width> <height> <bpp> <format> <file> \n"
		"    \n"
		"      - <bpp>: 32, 24 \n"
		"      - <format>: RGBA, BGRA, ... \n"
		"    eg: \n"
		"      $rgbplayer 800 600 32 RGBA color_800x600_rgba32.raw \n"
		"    \n");
}

int parse_args(int argc, char *argv[])
{
	if (argc != 6) {
		usage();
		exit(1);
	}

	width = atoi(argv[1]);
	height = atoi(argv[2]);
	bpp = atoi(argv[3]);

	if (width < 1 || height < 1 ||
		!(bpp == 32 || bpp == 16 || bpp == 24 || bpp == 8)) {
		demo_err("invalid arguments.\n");
		return -1;
	}

	strncpy(format, (const char*)argv[4], 4);
	strcpy(filename, (const char*)argv[5]);

	return set_mask();
}

int main(int argc, char *argv[])
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Surface *surface;

	void *data;
	FILE *fp;
	int ret = 0;
	size_t nread;

	if ((ret = parse_args(argc, argv)) < 0) {
		return -1;
	}

	data = malloc(width * height * (bpp >> 3));
	if (!data) {
		demo_err("insuffient memory.\n");
		ret = -1;
		goto out;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		demo_err("Unable to initialize SDL: %s\n", SDL_GetError());
		ret = -1;
		goto out;
	}

	atexit(SDL_Quit);

	window = SDL_CreateWindow("rgbviewer",
				SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED,
				width,
				height,
				SDL_WINDOW_OPENGL);
	if (!window) {
		demo_err("Unable to create window: %s\n", SDL_GetError());
		ret = -1;
		goto out;
	}


	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		demo_err("Unable to create renderer: %s\n", SDL_GetError());
		ret = -1;
		goto out;
	}

	fp = fopen(filename, "rb");
	if (!fp) {
		demo_err("unable to open file: %s\n", filename);
		ret = -1;
		goto out;
	}



	nread = fread(data, 1, width * height * (bpp >> 3), fp);
	if (nread != width * height * (bpp >> 3)) {
		demo_err("read error\n");
		ret = -1;
		goto out;
	}

	surface = SDL_CreateRGBSurfaceFrom(data, width, height, bpp,
			width * (bpp >> 3), rmask, gmask, bmask, amask);
	if (!surface) {
		demo_err("Unable to create surface: %s\n", SDL_GetError());
		ret = -1;
		goto out;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		demo_err("Unable to create texture: %s\n", SDL_GetError());
		SDL_FreeSurface(surface);
		ret = -1;
		goto out;
	}

	SDL_FreeSurface(surface);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	while (1) {
		SDL_Event event;

		if (SDL_WaitEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}
	}

out:

	if (texture)
		SDL_DestroyTexture(texture);
	if (fp)
		fclose(fp);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
	if (data)
		free(data);

	return ret;
}
