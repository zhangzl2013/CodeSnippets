#
# Copyright 2014 Zhang Zhaolong <zhangzhaolong0454@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CC = gcc
OPTFLAGS = -Wall
LIBJPEG_LIBS = -ljpeg
LIBJPEG_CFLAGS =

CFLAGS = $(OPTFLAGS) $(LIBJPEG_CFLAGS)
LDFLAGS = $(LIBJPEG_LIBS)

CSRC = jpeg_dec.c
TARGET = jpeg_dec

OBJ = $(CSRC:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

default: $(TARGET)

clean:
	rm -f *.raw
	rm -f $(OBJ) $(TARGET)
