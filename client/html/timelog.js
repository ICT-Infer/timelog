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

// https://stackoverflow.com/questions/7765767/show-week-number-with-javascript
Date.prototype.getWeek = function ()
{
    var onejan = new Date(this.getFullYear(),0,1);
    return Math.ceil((((this - onejan) / 86400000) + onejan.getDay()+1)/7);
};

const now = new Date();
const isoweek = now.getFullYear() + 'W' + now.getWeek();

let canvas = document.getElementById('timelog');
let ctx = canvas.getContext('2d');

const fontsize_px_topbar = 24;
const fontsize_px_qh = 10;

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

let cw = canvas.width = window.innerWidth;
let ch = canvas.height = window.innerHeight;

// Main background

ctx.fillStyle = colors.silver;
ctx.fillRect(0, 0, cw, ch);

// Content

ctx.font = fontsize_px_topbar + 'px sans-serif';

const dow_maxw = Math.max.apply(null, days_of_week.map((d) => { return ctx.measureText(d).width }));

const topbar_colcell_margin_top =    .4 * dow_maxw;
const topbar_colcell_margin_right =  .5 * dow_maxw;
const topbar_colcell_margin_bottom = .6 * dow_maxw;
const topbar_colcell_margin_left =   .5 * dow_maxw;
const topbar_colcell_height =        fontsize_px_topbar;

const topbar_height = 
	topbar_colcell_margin_top +
	topbar_colcell_height +
	topbar_colcell_margin_bottom;

const bottombar_height = topbar_height; // TODO: Adjust

const vertical_height_avail = ch - (topbar_height + bottombar_height);

const dow_colcell_width = Math.floor(vertical_height_avail / 7);
console.log(dow_colcell_width);

ctx.fillStyle = colors.blue;
ctx.fillRect(0, 0, cw, topbar_height);

const first_col_width = ctx.measureText(isoweek).width + 100; // TODO: Use greatest width in column

let ypos_baseline_text = 
	topbar_colcell_margin_top +
	topbar_colcell_height;

ctx.fillStyle = colors.white;
let xpos = topbar_colcell_margin_left;
ctx.fillText(isoweek, xpos, ypos_baseline_text);
xpos += first_col_width + topbar_colcell_margin_right;

const quarter_hour_height = 24; // TODO: Use text size + margs.
const quarter_hour_margin_top = 1;
const quarter_hour_margin_bottom = 0;
const quarter_hour_width = dow_colcell_width + 2 * topbar_colcell_margin_right - 2;

const qh_text_margin_top = 3;
const qh_text_margin_left = 3;

for (day of days_of_week)
{
	xpos += topbar_colcell_margin_left;
	ctx.fillStyle = colors.white;
	ctx.font = fontsize_px_topbar + 'px sans-serif';
	ctx.fillText(day, xpos, ypos_baseline_text);

	let ypos = topbar_height;

	for (var i = 0 ; i < 24 * 4 ; i++)
	{
		ypos += quarter_hour_margin_top;

		ctx.fillStyle = colors.gray;
		ctx.fillRect(xpos, ypos, quarter_hour_width, quarter_hour_height);
		ctx.fillStyle = colors.white;
		ctx.fillRect(xpos + 1, ypos + 1, quarter_hour_width - 2, quarter_hour_height - 2);

		let minute = ("0" + ((i * 15) % 60));
		minute = minute.substr(minute.length - 2);
		let hour = ("0" + Math.floor(i / 4));
		hour = hour.substr(hour.length - 2);
		let hm = hour + ':' + minute; // TODO + TZ offset

		ctx.fillStyle = colors.gray;
		ctx.font = fontsize_px_qh + 'px sans-serif';
		ctx.fillText(hm, xpos + qh_text_margin_left, ypos + fontsize_px_qh + qh_text_margin_top);

		ypos += quarter_hour_height + quarter_hour_margin_bottom;
	}

	xpos += dow_colcell_width + topbar_colcell_margin_right;
}
