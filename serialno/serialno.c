/*
 * Copyright 2013 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/hdreg.h>

#include <cutils/properties.h>
#include <android/log.h>

#define LOG_TAG "serialno_applet"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define SERIALNO_SIZE 16

int main(int argc, const char *argv[])
{
	struct hd_driveid id;
	int fd = -1;
	int i;
	char serialno[SERIALNO_SIZE + 1];

	if (argc != 3) {
		LOGE("usage: %s [drive] [property_name]\n", argv[0]);
		goto error1;
	}

	if ((fd = open(argv[1] , O_RDONLY|O_NONBLOCK )) == -1) {
		LOGE("Failed to open drive: %s (%d)\n",
				strerror(errno),
				errno);
		goto error1;
	}

	if (ioctl(fd, HDIO_GET_IDENTITY, &id) != 0) {
		LOGE("Failed to get drive's identity: %s (%d)\n",
				strerror(errno),
				errno);
		goto error2;
	}

	memset(serialno, 'x', sizeof(serialno));
	serialno[SERIALNO_SIZE] = '\0';

	for (i = 0; i < SERIALNO_SIZE ; i++) {
		if (isalnum(id.serial_no[i])) {
			serialno[i] = id.serial_no[i];
		}
	}

	if (property_set(argv[2], serialno)) {
		LOGE("Failed to set property: %s (%d)\n",
				strerror(errno),
				errno);
		goto error2;
	}

	close(fd);

	return 0;

error2:
	close(fd);
error1:
	return -1;
}
