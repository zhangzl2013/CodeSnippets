/**
 * Decode jpeg file to RGBA32 raw file.
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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "jpeglib.h"

#define demo_dbg 
#define demo_info printf
#define demo_err printf

#define CHMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH)

char filename[255];
char fileout[255];

struct jpeg_decompress_struct *cinfo;
struct jpeg_error_mgr *jerr;
struct jpeg_source_mgr *jsrc;
enum demo_decode_state {
	STATE_READ_HEADER,
	STATE_START_DECOMPRESS,
	STATE_READ_SCANLINES,
	STATE_FINISH_DECOMPRESS
} state;
unsigned int y;

static void demo_init_source(j_decompress_ptr cinfo)
{

}

static boolean demo_fill_input_buffer(j_decompress_ptr cinfo)
{
	demo_info("need more input data.\n");
	return FALSE;
}

static void demo_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{

}

static void demo_term_source(j_decompress_ptr cinfo)
{

}

static int demo_init_libjpeg()
{
	state = STATE_READ_HEADER;
	y = 0;

	cinfo = malloc(sizeof(struct jpeg_decompress_struct));
	jerr = malloc(sizeof(struct jpeg_error_mgr));
	jsrc = malloc(sizeof(struct jpeg_source_mgr));

	memset(cinfo, 0, sizeof(struct jpeg_decompress_struct));
	memset(jerr, 0, sizeof(struct jpeg_error_mgr));
	memset(jsrc, 0, sizeof(struct jpeg_source_mgr));

	cinfo->err = jpeg_std_error(jerr);
	jpeg_create_decompress(cinfo);

	cinfo->src = jsrc;
	cinfo->src->init_source = demo_init_source;
	cinfo->src->fill_input_buffer = demo_fill_input_buffer;
	cinfo->src->skip_input_data = demo_skip_input_data;
	cinfo->src->resync_to_restart = jpeg_resync_to_restart;
	cinfo->src->term_source = demo_term_source;
	
	return 0;
}

static void demo_uninit_libjpeg()
{
	jpeg_destroy_decompress(cinfo);
	free(cinfo);
	free(jerr);
	free(jsrc);
}

static void *demo_decode_data(void *data, size_t length, int *width, int *height)
{
	int res;
	void *rgba = NULL;
	unsigned char *scanline;
	int stride;

	demo_dbg("length: %ld\n", length);

	jsrc->next_input_byte = data;
	jsrc->bytes_in_buffer = length;

	switch (state) {
	case STATE_READ_HEADER:
		res = jpeg_read_header(cinfo, TRUE);
		if (res == JPEG_SUSPENDED) {
			demo_info("JPEG_SUSPENDED\n");
			break;
		}

		cinfo->do_fancy_upsampling = FALSE;
		cinfo->do_block_smoothing = FALSE;
		cinfo->out_color_space = JCS_EXT_RGBA;

		demo_dbg("cinfo.image_width: %d\n", cinfo->image_width);
		demo_dbg("cinfo.image_height: %d\n", cinfo->image_height);

		*width = cinfo->image_width;
		*height = cinfo->image_height;

		rgba = malloc(cinfo->image_width * cinfo->image_height * 4);
		if (!rgba) {
			return NULL;
		}

		state = STATE_START_DECOMPRESS;

		/* fall through */
	case STATE_START_DECOMPRESS:
		res = jpeg_start_decompress(cinfo);
		if (!res) {
			demo_err("jpeg star decompress failed.\n");
			break;
		}

		state = STATE_READ_SCANLINES;

		/* fall through */
	case STATE_READ_SCANLINES:
		res = 0;
		y = 0;
		stride = (cinfo->image_width * 4);

		scanline = malloc(cinfo->image_width * 4);
		if (!scanline) {
			return NULL;
		}
		while (y < cinfo->image_height) {
			res = jpeg_read_scanlines(cinfo, &scanline, 1);
			if (res == 0) {
				break;
			}

			memcpy(rgba + (stride * y), scanline, stride);
			y++;
		}

		free(scanline);
		if (!res) {
			demo_err("jpeg read scanlines failed.\n");
			break;
		}

		state = STATE_FINISH_DECOMPRESS;

		/* fall through */
	case STATE_FINISH_DECOMPRESS:
		res = jpeg_finish_decompress(cinfo);
		if (!res) {
			demo_err("jpeg finish decompress failed.\n");
			break;
		}

		y = 0;

		state = STATE_READ_HEADER;

		break;
	}

	return rgba;
}

int main(int argc, char* argv[])
{
	int fd_in = 0, fd_out = 0;
	int width, height;
	size_t file_in_size;
	void *file_in_raw = NULL;
	void *rgba;
	int ret;
	struct timeval tdec_begin, tdec_end;
	int sec, usec;
	float msec;
	int i, d = 30;

	if (argc < 3) {
		demo_err("usage: $%s <repeat-times> <jpeg file>\n", argv[0]);
		return -1;
	}

	d = atoi(argv[1]);
	strcpy(filename, argv[2]);

	demo_init_libjpeg();

	fd_in = open(filename, O_RDWR, 0);
	if (fd_in < 0) {
		demo_err("open file %s failed.\n", filename);
		ret = -1;
		goto out;
	}

	file_in_size = lseek(fd_in, 0, SEEK_END);
	file_in_raw = mmap(0, file_in_size, PROT_READ, MAP_SHARED, fd_in, 0);
	if (!file_in_raw) {
		demo_err("mmap file %s failed.\n", filename);
		ret = -1;
		goto out;
	}


	gettimeofday(&tdec_begin, NULL);

	for (i = 0; i < d; i++) {
		rgba = demo_decode_data(file_in_raw, file_in_size, &width, &height);
		if (!rgba) {
			demo_err("decode data failed.\n");
			ret = -1;
			goto out;
		}
	}

	gettimeofday(&tdec_end, NULL);
	sec = tdec_end.tv_sec - tdec_begin.tv_sec;
	usec = tdec_end.tv_usec - tdec_begin.tv_usec;
	if (usec < 0) {
		sec--;
		usec = usec + 1000000;
	}

	msec = usec / 1000.0;

	demo_info("decode succeeds in %.3fms.\n", msec);

	sprintf(fileout, "output_%dx%d_rgba32.raw", width, height);

	if (0 == access(fileout, 0)) {
		demo_info("A previous %s exists, removing it.\n", fileout);
		remove(fileout);
	}

	fd_out = open(fileout, O_CREAT | O_RDWR, 0);
	if (fd_out < 0) {
		demo_err("open output file failed.\n");
		ret = -1;
		goto out;
	}

	if ((ret = chmod(fileout, CHMODE)) < 0) {
		demo_err("change file permission failed.\n");
		ret = -1;
		goto out;
	}

	ret = write(fd_out, rgba, width * height * 4);
	if (ret < 0) {
		demo_err("write to output file failed\n");
		ret = -1;
		goto out;
	} else {
		ret = 0;
	}

out:
	demo_uninit_libjpeg();

	if (rgba)
		free(rgba);

	if (fd_out > 0)
		close(fd_out);

	if (file_in_raw)
		munmap(file_in_raw, file_in_size);

	if (fd_in > 0)
		close(fd_in);

	return ret;
}
