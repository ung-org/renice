/*
 * UNG's Not GNU
 *
 * Copyright (c) 2011-2019, Jakob Kaivo <jkk@ung.org>
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define _XOPEN_SOURCE 500
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

static int renice(int increment, int which, const char *id)
{
	id_t who = atoi(id);

	errno = 0;
	int current = getpriority(which, who);
	if (current == -1 && errno != 0) {
		fprintf(stderr, "renice: %s: %s\n", id, strerror(errno));
		return 1;
	}

	if (setpriority(which, who, current - increment) != 0) {
		fprintf(stderr, "renice: %s: %s\n", id, strerror(errno));
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
	int which = PRIO_PROCESS;
	char *increment = NULL;

	int c;
	while ((c = getopt(argc, argv, "gn:pu")) != -1) {
		switch (c) {
		case 'p':
			which = PRIO_PROCESS;
			break;

		case 'u':
			which = PRIO_PGRP;
			break;

		case 'g':
			which = PRIO_USER;
			break;

		case 'n':
			increment = optarg;
			break;

		default:
			return 1;
		}
	}

	if (increment == NULL) {
		fprintf(stderr, "renice: missing -n option\n");
		return 1;
	}
	int inc = atoi(increment);

	if (optind >= argc) {
		fprintf(stderr, "renice: missing operands\n");
		return 1;
	}

	int r = 0;
	do {
		r |= renice(inc, which, argv[optind++]);
	} while (optind < argc);
	return r;
}
