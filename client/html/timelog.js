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

Date.prototype.getWeek = function ()
{
	// https://stackoverflow.com/questions/7765767/show-week-number-with-javascript
	var onejan = new Date(this.getFullYear(),0,1);
	return Math.ceil((((this - onejan) / 86400000) + onejan.getDay()+1)/7);
};

const now = new Date();
const isoweek = now.getFullYear() + 'W' + now.getWeek();

const toprow = document.getElementById('top-row');

const canvas_isoweek = document.getElementById('isoweek');
const ctx_isoweek = canvas_isoweek.getContext('2d');

const canvas_weekdays = document.getElementById('weekdays');
const ctx_weekdays = canvas_weekdays.getContext('2d');

const canvas_toprightbar = document.getElementById('top-right-bar');
const ctx_toprightbar = canvas_toprightbar.getContext('2d');

const middlerow = document.getElementById('middle-row');

const canvas_middleleftbar = document.getElementById('middle-left-bar');
const ctx_middleleftbar = canvas_middleleftbar.getContext('2d');

const canvas = document.getElementById('content');
const ctx = canvas.getContext('2d');

const canvas_middlerightbar = document.getElementById('middle-right-bar');
const ctx_middlerightbar = canvas_middlerightbar.getContext('2d');

const bottomrow = document.getElementById('bottom-row');

const canvas_bottomleftbar = document.getElementById('bottom-left-bar');
const ctx_bottomleftbar = canvas_bottomleftbar.getContext('2d');

const canvas_wdactions = document.getElementById('weekday-actions');
const ctx_wdactions = canvas.getContext('2d');

const canvas_bottomrightbar = document.getElementById('bottom-right-bar');
const ctx_bottomrightbar = canvas_bottomrightbar.getContext('2d');

const days_of_week = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

const colors =
{
	'navy':    '#001f3f',
	'blue':    '#0074d9',
	'aqua':    '#7fdbff',
	'teal':    '#39cccc',
	'olive':   '#3d9970',
	'green':   '#2ecc40',
	'lime':    '#01ff70',
	'yellow':  '#ffdc00',
	'orange':  '#ff851b',
	'red':     '#ff4136',
	'maroon':  '#85144b',
	'fuchsia': '#f012be',
	'purple':  '#b10dc9',
	'black':   '#111111',
	'gray':    '#aaaaaa',
	'silver':  '#dddddd',
	'white':   '#ffffff'
};

let fontsize_px_topbar,
    fontsize_px_qh,
    pad_top_top_row,
    pad_right_top_row,
    pad_bottom_top_row,
    pad_left_top_row,
    cellwidth_top_row,
    quarter_hour_margin_top,
    quarter_hour_margin_right,
    quarter_hour_margin_bottom,
    quarter_hour_margin_left,
    quarter_hour_height,
    quarter_hour_width,
    qh_text_margin_top,
    qh_text_margin_left,
    content_margin_top,
    content_margin_bottom;

function sizeCanvases ()
{
	const total_width  = document.body.clientWidth;
	const total_height = window.innerHeight;

	// TODO: If possible, take dpi into account.
	const min_fontsize_topbar = 16;
	const max_fontsize_topbar = 32;
	const min_height_topbar = 2 * min_fontsize_topbar;
	const max_height_topbar = 2 * max_fontsize_topbar;
	const min_fontsize_qh = 12;

	const relaref = (total_width > total_height) ? total_height : total_width;

	const height_top_row = Math.min(Math.floor(Math.max(min_height_topbar, min_fontsize_topbar + 0.12 * relaref)), max_height_topbar);
	toprow.style.height = height_top_row + 'px';

	fontsize_px_topbar = Math.max(min_fontsize_qh, Math.floor(0.3 * height_top_row));
	fontsize_px_qh = Math.max(min_fontsize_qh, Math.floor(10 * fontsize_px_topbar / 24));
	
	pad_top_top_row = Math.ceil(5 * (height_top_row - fontsize_px_topbar) / 12);
	pad_bottom_top_row = Math.ceil(height_top_row - (fontsize_px_topbar + pad_top_top_row));
	pad_left_top_row = pad_top_top_row;
	pad_right_top_row = pad_top_top_row;

	const height_bottom_row = Math.floor(0.5 * height_top_row);
	bottomrow.style.height = height_bottom_row + 'px';

	const height_middle_row =
		total_height - (height_top_row + height_bottom_row);
	middlerow.style.height = height_middle_row + 'px';

	canvas_isoweek.height = height_top_row;
	canvas_weekdays.height = height_top_row;
	canvas_toprightbar.height = height_top_row;

	canvas_middleleftbar.style.top = height_top_row + 'px';
	canvas_middleleftbar.height = height_middle_row;

	canvas.style.top = height_top_row + 'px';
	canvas.height = height_middle_row;

	canvas_middlerightbar.style.top = height_top_row + 'px';
	canvas_middlerightbar.height = height_middle_row;

	canvas_bottomleftbar.height = height_bottom_row;
	canvas_wdactions.height = height_bottom_row;
	canvas_bottomrightbar.height = height_bottom_row;

	ctx_isoweek.font = fontsize_px_topbar + 'px sans-serif';
	const isoweek_minwidth = ctx_isoweek.measureText(isoweek).width + 3 * pad_left_top_row;
	const middleleftbar_minwidth = 0; // TODO
	const bottomleftbar_minwidth = 0;
	const left_column_minwidth = Math.max(isoweek_minwidth, Math.max(middleleftbar_minwidth, bottomleftbar_minwidth));

	canvas_isoweek.width = left_column_minwidth;
	canvas_middleleftbar.width = left_column_minwidth;
	canvas_bottomleftbar.width = left_column_minwidth;

	canvas.style.left = left_column_minwidth + 'px';
	const content_margin_min_right = left_column_minwidth;
	const content_width_avail =
		total_width - (left_column_minwidth + content_margin_min_right);

	ctx_weekdays.font = fontsize_px_topbar + 'px sans-serif';

	const dow_maxw = Math.max.apply(null, days_of_week.map((d) => { return ctx_weekdays.measureText(d).width }));

	const cellmaxwidth_top_row = 185;
	cellwidth_top_row = Math.min(Math.max(dow_maxw, Math.floor(content_width_avail / 7)), cellmaxwidth_top_row);
	const content_width = 7 * cellwidth_top_row;

	canvas.width = content_width;

	canvas_weekdays.style.left = left_column_minwidth  + 'px';
	const weekdays_width = content_width;
	canvas_weekdays.width = weekdays_width;

	content_margin_top = Math.floor(1.2 * fontsize_px_qh);
	content_margin_bottom = 0;
	qh_text_margin_top = 5;
	qh_text_margin_right = 0;
	qh_text_margin_bottom = 5;
	qh_text_margin_left = 5;
	quarter_hour_margin_top = 1;
	quarter_hour_margin_right = 2;
	quarter_hour_margin_bottom = 0;
	quarter_hour_margin_left = 0;
	quarter_hour_height = 2 * qh_text_margin_top + fontsize_px_qh;
	quarter_hour_width = cellwidth_top_row - (quarter_hour_margin_left + quarter_hour_margin_right);

	canvas_wdactions.style.left = left_column_minwidth + 'px';
	const wdactions_width = content_width;
	canvas_wdactions.width = wdactions_width;

	const height_content = height_top_row + content_margin_top + 24 * 4 * (quarter_hour_margin_top + quarter_hour_height + quarter_hour_margin_bottom) + content_margin_bottom + height_bottom_row;
	canvas.height = height_content;

	const right_column_minwidth = 200;
	const right_column_width = Math.max(right_column_minwidth, total_width - (left_column_minwidth + content_width));

	canvas_toprightbar.width = right_column_width;
	canvas_middlerightbar.width = right_column_width;
	canvas_bottomrightbar.width = right_column_width;
}

sizeCanvases();

window.scroll(0, 8 * 4 * (quarter_hour_margin_top + quarter_hour_height + quarter_hour_margin_bottom));

function fullDrawIsoweek ()
{
	ctx_isoweek.fillStyle = colors.white;
	ctx_isoweek.font = fontsize_px_topbar + 'px sans-serif';
	ctx_isoweek.fillText(isoweek, pad_left_top_row, pad_top_top_row + fontsize_px_topbar);
}

fullDrawIsoweek();

function fullDrawWeekdays ()
{
	ctx_weekdays.fillStyle = colors.white;
	ctx_weekdays.font = fontsize_px_topbar + 'px sans-serif';
	let xpos = 0;
	for (day of days_of_week)
	{
		ctx_weekdays.fillText(day, xpos, pad_top_top_row + fontsize_px_topbar);
		xpos += cellwidth_top_row;
	}
}

fullDrawWeekdays();

function fullDrawContent ()
{
	ctx.font = fontsize_px_qh + 'px sans-serif';
	let xpos = quarter_hour_margin_left;
	for (day of days_of_week)
	{
		let ypos = content_margin_top;
		for (var i = 0 ; i < 24 * 4 ; i++)
		{
			ypos += quarter_hour_margin_top;

			ctx.fillStyle = colors.gray;
			ctx.fillRect(xpos, ypos, quarter_hour_width, quarter_hour_height);
			ctx.fillStyle = colors.white;
			ctx.fillRect(xpos + 1, ypos + 1, quarter_hour_width - 2, quarter_hour_height - 2);

			let minute = String((i * 15) % 60).padStart(2, '0');
			let hour = String(Math.floor(i / 4)).padStart(2, '0');
			let hm = hour + ':' + minute; // TODO + TZ offset

			ctx.fillStyle = colors.gray;
			ctx.fillText(hm, xpos + qh_text_margin_left, ypos + fontsize_px_qh + qh_text_margin_top);

			ypos += quarter_hour_height + quarter_hour_margin_bottom;
		}

		xpos += quarter_hour_width + quarter_hour_margin_right + quarter_hour_margin_left;
	}
}

fullDrawContent();

function windowResized ()
{
	sizeCanvases();
	fullDrawIsoweek();
	fullDrawWeekdays();
	fullDrawContent();
}

let timer_resize;
window.onresize = () =>
{
	clearTimeout(timer_resize);
	timer_resize = setTimeout(windowResized, 100);
};
