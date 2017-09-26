/*
 * Copyright (c) 2017 Erik Nordstrøm <erik@nordstroem.no>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
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

let canvas = document.getElementById('timelog');
let ctx = canvas.getContext('2d');

const fontsize_px_topbar = 24;

const days_of_week = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

const colors =
{
	'navy': '#001f3f',
	'blue': '#0074d9',
	'aqua': '#7fdbff',
	'teal': '#39cccc',
	'olive': '#3d9970',
	'green': '#2ecc40',
	'lime': '#01ff70',
	'yellow': '#ffdc00',
	'orange': '#ff851b',
	'red': '#ff4136',
	'maroon': '#85144b',
	'fuchsia': '#f012be',
	'purple': '#b10dc9',
	'black': '#111111',
	'gray': '#aaaaaa',
	'silver': '#dddddd',
	'white': '#ffffff'
};

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

// Main background

ctx.fillStyle = colors.silver;
ctx.fillRect(0, 0, canvas.width, canvas.height);

// Content

ctx.font = fontsize_px_topbar + 'px sans-serif';

const dow_maxw = Math.max.apply(null, days_of_week.map((d) => { return ctx.measureText(d).width }));

const dow_colcells =
{
	'margin-top':    .4 * dow_maxw,
	'margin-right':  .5 * dow_maxw,
	'margin-bottom': .6 * dow_maxw,
	'margin-left':   .5 * dow_maxw,
	'width':         dow_maxw,
	'height':        fontsize_px_topbar
};

ctx.fillStyle = colors.blue;
ctx.fillRect(0, 0, canvas.width, dow_colcells['margin-top'] + dow_colcells['height'] + dow_colcells['margin-bottom']);

const first_colcell =
{
	'margin-top':    dow_colcells['margin-top'],
	'margin-right':  dow_colcells['margin-right'],
	'margin-bottom': dow_colcells['margin-bottom'],
	'margin-left':   dow_colcells['margin-left'],
	'width':         0, // TODO
	'height':        dow_colcells['height'],
};
ctx.fillStyle = colors.white;
ctx.fillText('2017W35', first_colcell['margin-left'], first_colcell['margin-top'] + first_colcell['height']);
