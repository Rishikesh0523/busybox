/* spaceshooter.c - enhanced terminal-based space shooter game
 *
 * Copyright (C) 2025 YourName
 * Licensed under GPLv2.
 *
 * An enhanced version of the classic 90s space shooter game.
 *
 * Controls:
 *   Left/Right arrows or A/D: move spaceship
 *   Space: shoot
 *   Q: quit
 *
 * Features:
 *   - Moving starfield background.
 *   - Enemies that move downward with zigzag behavior.
 *   - Occasional power-ups (extra life).
 *   - Level progression that increases enemy spawn frequency.
 */
//config:config SPACESHOOTER
//config:	bool "spaceshooter (enhanced space shooter game)"
//config:	default y
//config:	help
//config:	Play an enhanced space shooter game in the terminal.

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAX_BULLETS   100
#define MAX_ENEMIES   50
#define MAX_POWERUPS  10
#define MAX_STARS     100

typedef struct {
	int x, y;
	int active;
} Bullet;

typedef struct {
	int x, y;
	int active;
	int dx;  /* horizontal movement: -1, 0, or 1 */
} Enemy;

typedef struct {
	int x, y;
	int active;
	int type; /* 0: extra life (future: other power-ups) */
} PowerUp;

typedef struct {
	int x, y;
} Star;

static Bullet bullets[MAX_BULLETS];
static Enemy enemies[MAX_ENEMIES];
static PowerUp powerups[MAX_POWERUPS];
static Star stars[MAX_STARS];

static int spaceship_x;
static int spaceship_y;
static int spaceship_lives;
static int level;
static int score;
static int game_over;

static int screen_width, screen_height;
static int frame_count;
static int enemy_spawn_rate;   /* frames between enemy spawns */
static int powerup_spawn_rate; /* frames between powerup spawns */

/* Initialize game state */
static void init_game(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++)
		bullets[i].active = 0;
	for (i = 0; i < MAX_ENEMIES; i++)
		enemies[i].active = 0;
	for (i = 0; i < MAX_POWERUPS; i++)
		powerups[i].active = 0;
	for (i = 0; i < MAX_STARS; i++) {
		stars[i].x = rand() % screen_width;
		stars[i].y = rand() % screen_height;
	}
	spaceship_x = screen_width / 2;
	spaceship_y = screen_height - 2;
	spaceship_lives = 3;
	level = 1;
	score = 0;
	game_over = 0;
	frame_count = 0;
	enemy_spawn_rate = 30;   /* initial enemy spawn rate */
	powerup_spawn_rate = 200; /* initial powerup spawn rate */
}

/* Add a bullet at the spaceship's position */
static void add_bullet(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++) {
		if (!bullets[i].active) {
			bullets[i].x = spaceship_x;
			bullets[i].y = spaceship_y - 1;
			bullets[i].active = 1;
			break;
		}
	}
}

/* Add an enemy at a random horizontal position at the top */
static void add_enemy(void)
{
	int i;
	for (i = 0; i < MAX_ENEMIES; i++) {
		if (!enemies[i].active) {
			enemies[i].x = rand() % screen_width;
			enemies[i].y = 1;
			enemies[i].active = 1;
			/* Set a random horizontal direction: -1, 0, or 1 */
			enemies[i].dx = (rand() % 3) - 1;
			break;
		}
	}
}

/* Add a powerup (extra life) at a random horizontal position at the top */
static void add_powerup(void)
{
	int i;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (!powerups[i].active) {
			powerups[i].x = rand() % screen_width;
			powerups[i].y = 1;
			powerups[i].active = 1;
			powerups[i].type = 0; /* extra life */
			break;
		}
	}
}

/* Update bullets: move upward */
static void update_bullets(void)
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			bullets[i].y--;
			if (bullets[i].y < 1)
				bullets[i].active = 0;
		}
	}
}

/* Update enemies: move downward and horizontally (zigzag) */
static void update_enemies(void)
{
	int i;
	for (i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active) {
			enemies[i].y++;
			enemies[i].x += enemies[i].dx;
			/* Reverse direction on boundaries */
			if (enemies[i].x < 0) {
				enemies[i].x = 0;
				enemies[i].dx = -enemies[i].dx;
			}
			if (enemies[i].x >= screen_width) {
				enemies[i].x = screen_width - 1;
				enemies[i].dx = -enemies[i].dx;
			}
			/* If enemy reaches the spaceship row, deduct a life */
			if (enemies[i].y >= spaceship_y) {
				enemies[i].active = 0;
				spaceship_lives--;
				if (spaceship_lives <= 0)
					game_over = 1;
			}
		}
	}
}

/* Update powerups: move downward */
static void update_powerups(void)
{
	int i;
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (powerups[i].active) {
			powerups[i].y++;
			if (powerups[i].y > spaceship_y)
				powerups[i].active = 0;
		}
	}
}

/* Update stars: move downward to simulate a moving background */
static void update_stars(void)
{
	int i;
	for (i = 0; i < MAX_STARS; i++) {
		stars[i].y++;
		if (stars[i].y >= screen_height) {
			stars[i].y = 0;
			stars[i].x = rand() % screen_width;
		}
	}
}

/* Check collisions between bullets and enemies,
 * powerups and spaceship, and enemies with spaceship.
 */
static void check_collisions(void)
{
	int i, j;
	/* Bullets vs Enemies */
	for (i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			for (j = 0; j < MAX_ENEMIES; j++) {
				if (enemies[j].active &&
				    bullets[i].x == enemies[j].x &&
				    bullets[i].y == enemies[j].y) {
					bullets[i].active = 0;
					enemies[j].active = 0;
					score += 10;
				}
			}
		}
	}
	/* Spaceship vs Powerups */
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (powerups[i].active &&
		    powerups[i].x == spaceship_x &&
		    powerups[i].y == spaceship_y) {
			powerups[i].active = 0;
			/* Extra life powerup */
			if (powerups[i].type == 0)
				spaceship_lives++;
			score += 20;
		}
	}
	/* Enemies vs Spaceship */
	for (i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active &&
		    enemies[i].x == spaceship_x &&
		    enemies[i].y == spaceship_y) {
			enemies[i].active = 0;
			spaceship_lives--;
			if (spaceship_lives <= 0)
				game_over = 1;
		}
	}
}

/* Draw game state */
static void draw_game(void)
{
	int i;
	clear();
	/* Draw starfield */
	for (i = 0; i < MAX_STARS; i++) {
		attron(COLOR_PAIR(5));
		mvaddch(stars[i].y, stars[i].x, '.');
		attroff(COLOR_PAIR(5));
	}
	/* Draw bullets */
	for (i = 0; i < MAX_BULLETS; i++) {
		if (bullets[i].active) {
			attron(COLOR_PAIR(3));
			mvaddch(bullets[i].y, bullets[i].x, '|');
			attroff(COLOR_PAIR(3));
		}
	}
	/* Draw enemies */
	for (i = 0; i < MAX_ENEMIES; i++) {
		if (enemies[i].active) {
			attron(COLOR_PAIR(2));
			mvaddch(enemies[i].y, enemies[i].x, 'V');
			attroff(COLOR_PAIR(2));
		}
	}
	/* Draw powerups */
	for (i = 0; i < MAX_POWERUPS; i++) {
		if (powerups[i].active) {
			attron(COLOR_PAIR(4));
			mvaddch(powerups[i].y, powerups[i].x, '+');
			attroff(COLOR_PAIR(4));
		}
	}
	/* Draw spaceship */
	attron(COLOR_PAIR(1));
	mvaddch(spaceship_y, spaceship_x, 'A');
	attroff(COLOR_PAIR(1));
	/* Display game info */
	mvprintw(0, 0, "Score: %d  Level: %d  Lives: %d", score, level, spaceship_lives);
	refresh();
}

/* Process user input */
static void process_input(void)
{
	int ch = getch();
	switch (ch) {
		case KEY_LEFT:
		case 'a':
		case 'A':
			if (spaceship_x > 0)
				spaceship_x--;
			break;
		case KEY_RIGHT:
		case 'd':
		case 'D':
			if (spaceship_x < screen_width - 1)
				spaceship_x++;
			break;
		case ' ':
			add_bullet();
			break;
		case 'q':
		case 'Q':
			game_over = 1;
			break;
		default:
			break;
	}
}

/* Main game loop */
int main(void)
{
	int delay = 50000; /* frame delay in microseconds */

	initscr();
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	/* Initialize colors if supported */
	if (has_colors()) {
		start_color();
		init_pair(1, COLOR_YELLOW, COLOR_BLACK); /* spaceship */
		init_pair(2, COLOR_RED, COLOR_BLACK);      /* enemies */
		init_pair(3, COLOR_CYAN, COLOR_BLACK);     /* bullets */
		init_pair(4, COLOR_GREEN, COLOR_BLACK);    /* powerups */
		init_pair(5, COLOR_WHITE, COLOR_BLACK);    /* stars */
	}

	getmaxyx(stdscr, screen_height, screen_width);
	srand(time(NULL));
	init_game();

	while (!game_over) {
		process_input();
		update_bullets();
		update_enemies();
		update_powerups();
		update_stars();
		check_collisions();

		/* Spawn enemy based on spawn rate */
		if (frame_count % enemy_spawn_rate == 0)
			add_enemy();
		/* Spawn powerup occasionally */
		if (frame_count % powerup_spawn_rate == 0)
			add_powerup();

		/* Level progression: every 500 frames increase level and difficulty */
		if (frame_count > 0 && frame_count % 500 == 0) {
			level++;
			if (enemy_spawn_rate > 10)
				enemy_spawn_rate -= 2;
		}

		draw_game();
		frame_count++;
		usleep(delay);
	}

	/* Game Over screen */
	clear();
	mvprintw(screen_height / 2 - 1, (screen_width - 9) / 2, "GAME OVER");
	mvprintw(screen_height / 2, (screen_width - 20) / 2, "Final Score: %d", score);
	mvprintw(screen_height / 2 + 1, (screen_width - 30) / 2, "Press any key to exit...");
	refresh();
	nodelay(stdscr, FALSE);
	getch();
	endwin();
	return 0;
}
