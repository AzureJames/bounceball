#include <stdio.h>
#define LA_IMPLEMENTATION
#include <la.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <windows.h>
#include <conio.h>





typedef enum {
	BACK = 0,
	FORE = 1,
	PIXEL_N,
} Pixel;

#define WIDTH 64
#define HEIGHT 64
static_assert(HEIGHT % 2 == 0, "if display rows is not /2 can't properly compress it //show()");
static Pixel display[WIDTH * HEIGHT];


void fill(Pixel p) {
	Pixel* ptr = display;
	size_t n = WIDTH * HEIGHT;
	while (n-- > 0) *ptr++ = p;
	memset(display, p, WIDTH * HEIGHT);
}

void circle(V2f c, float r) {
	V2i b = v2i2f(v2f_floor(v2f_sub(c, v2ff(r))));
	V2i e = v2i2f(v2f_ceil(v2f_sum(c, v2ff(r))));

	for (int y = b.y; y <= e.y; ++y) {
		for (int x = b.x; x <= e.x; ++x) {
			V2f p = v2f_sum(v2f((float)x, (float)y), v2ff(0.5f));
			V2f d = v2f_sub(c, p);

			float dx = c.x - ((float)x + 0.5f);
			float dy = c.y - ((float)y + 0.5f);
			if (v2f_sqrlen(d) <= r * r) {
				if (0 <= x && x < WIDTH && 0 <= y && y < HEIGHT) {
					display[y * WIDTH + x] = FORE;
				}
			}
		}
	}
}

void show(void) {
	static char row[WIDTH];
	//     top btm
	static char table[2][2] = {
		//b   0    1
			{' ', '_'}, //t0
			{'^', 'C'}, //t1
	};
	for (int y = 0; y < HEIGHT / 2; ++y) {
		for (int x = 0; x < WIDTH; x++) {
			Pixel t = display[(2 * y + 0) * WIDTH + x];
			Pixel b = display[(2 * y + 1) * WIDTH + x];
			assert(t < PIXEL_N);
			assert(b < PIXEL_N);
			assert(0 <= b);
			assert(0 <= t);

			row[x] = table[t][b];
		}
		fwrite(row, WIDTH, 1, stdout);
		fputc('\n', stdout);
	}
}



void back(void) {
	printf("\x1b[%dD", WIDTH);
	printf("\x1b[%dA", HEIGHT / 2);
}

int change_color(int is_color, int color) {
	if (is_color > 0) {
		color++;

		switch (color)
		{
		case 1:
			printf("\033[0;31m");
			break;
		case 2:
			printf("\033[0;32m");
			break;
		case 3:
			printf("\033[0;33m");
			break;
		case 4:
			printf("\033[0;36m");
			break;
		default:
			printf("\033[0m");
			break;
		}

	}
	return color;
}

const FPS = 16;
#define RADIUS (WIDTH/4.0f)
#define GRAVITY 37.0f
#define DT (1.0f/FPS)

int main(void) {
	V2f pos = v2f(WIDTH / 2.0, RADIUS);
	V2f vel = v2f(70.0f, GRAVITY);
	V2f gravity = v2f(0.0f, GRAVITY);
	V2f horizontal = v2f(2.0f, 0.0f);
	int frame = 0;
	int frame_limit = 370;
	int quit = 0;
	int is_color = 1;
	int color = 1;



	while (!quit) {
		vel = v2f_sum(vel, v2f_mul(gravity, v2ff(DT)));
		pos = v2f_sum(pos, v2f_mul(vel, v2ff(DT)));
		fill(BACK);
		circle(pos, RADIUS);
		show();
		back();
		Sleep(1000 / FPS);
		if (color > 4) {
			color = 1;
		}
		if (pos.y > HEIGHT - RADIUS) {
			pos.y = HEIGHT - RADIUS;
			vel.y *= -0.9f;
		}
		if (pos.y < -7) { //
			vel.y *= -0.9f;
		}
		if (pos.x > WIDTH - RADIUS) {
			pos.x = WIDTH - RADIUS;
			vel.x *= -0.9f;
			color = change_color(is_color, color);
		}
		else if (pos.x < RADIUS) {
			pos.x = RADIUS;
			vel.x *= -0.8f;
			color = change_color(is_color, color);
		}
		if (frame > frame_limit) {
			vel.x = 0.0f;
			vel.y = 0.0f;
			is_color = 0;
		}
		frame++;

		char c;

		if (_kbhit()) {
			c = _getch();
			if (c == 'r') {
				vel.x *= 1.2;
				vel.y *= 1.15;
				frame_limit += 50;
			}
		}


	}


	return 0;
}