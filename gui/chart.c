/*
 * Copyright (c) 2018 Ryan Flannery <ryan.flannery@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chart.h"

struct chart*
chart_init(
	size_t   nsamples,
	size_t   nseries,
	bool     is_percents,
	const char  *bgcolor,
	const char **colors)
{
	struct chart *c;
	size_t   i, j;

	if (0 == nseries || 0 == nsamples || NULL == bgcolor || NULL == colors)
		errx(1, "invalid chart parameters %zu %zu", nseries, nsamples);

	if (NULL == (c = malloc(sizeof(struct chart))))
		err(1, "failed to malloc chart");

	c->nseries  = nseries;
	c->nsamples = nsamples;
	c->percents = is_percents;
	c->current  = nsamples - 1;    /* first update will be at 0 */

	if (NULL == (c->values = calloc(nsamples, sizeof(double*))))
		err(1, "chart calloc failed");

	for (i = 0; i < nsamples; i++)
		if (NULL == (c->values[i] = calloc(nseries, sizeof(double))))
			err(1, "charrt calloc failed (2)");

	for (i = 0; i < nsamples; i++)
		for (j = 0; j < nseries; j++)
			c->values[i][j] = 0.0;

	if (NULL == (c->bgcolor = strdup(bgcolor)))
		err(1, "chart strdup failed");

	if (NULL == (c->colors = calloc(nsamples, sizeof(char*))))
		err(1, "chart colors calloc failed");

	for (j = 0; j < nseries; j++) {
		if (NULL == (c->colors[j] = strdup(colors[j])))
			err(1, "chart colors strdup failed(2)");
	}

	return c;
}

void
chart_free(struct chart *c)
{
	size_t i;
	for (i = 0; i < c->nsamples; i++)
		free(c->values[i]);

	free(c->values);
	free(c->bgcolor);
	free(c);
}

void
chart_update(struct chart *c, double data[])
{
	size_t cur = (c->current + 1) % c->nsamples;
	size_t i;

	for (i = 0; i < c->nseries; i++) {
		if (0 > data[i])
			errx(1, "charts don't support negative values\n");

		c->values[cur][i] = data[i];
	}

	c->current = cur;
}

void
chart_get_minmax(struct chart *c, double *min, double *max)
{
	size_t i, j;
	*min = *max = c->values[0][0];
	for (i = 0; i < c->nsamples; i++) {
		for (j = 0; j < c->nseries; j++) {
			if (c->values[i][j] > *max)
				*max = c->values[i][j];

			if (c->values[i][j] < *min)
				*min = c->values[i][j];
		}
	}
}

void
chart_print(struct chart *c)
{
	size_t i, j;

	printf("nseries: %zu\t\tnsamples: %zu\n", c->nseries, c->nsamples);
	printf("current: %zu\t\tpercents? %s\n", c->current,
			c->percents ? "YES" : "NO");
	printf("bgcolor: '%s'\n", c->bgcolor);

	for (i = 0; i < c->nseries; i++)
		printf("  color[%zu] = '%s'\n", i, c->colors[i]);

	for (i = 0; i < c->nseries; i++) {
		for (j = 0; j < c->nsamples; j++)
			printf("%3.1f ", c->values[j][i]);

		printf("\n");
	}
}
