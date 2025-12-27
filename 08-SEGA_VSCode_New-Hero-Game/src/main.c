#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html
// https://www.youtube.com/watch?v=xldCreKST3s
// OHSAT Games Tutorials
// https://www.ohsat.com/tutorial/mdmisc/sor-style-text-crawl/
// PIGSY Retro Game Sega Genesis Tutotials
// https://www.youtube.com/watch?v=BnGqc5OTTY4&list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb

#define COUNTER_LIMIT 5
// 320 X 224
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224

// map
#define MAP_WIDTH 320
#define MAP_HEIGTH 224
// screen
#define LEFT_SCREEN_LIMIT 0
#define RIGHT_SCREEN_LIMIT MAP_WIDTH - HORIZONTAL_RESOLUTION
#define TOP_SCREEN_LIMIT 0
#define BOTTON_SCREEN_LIMIT MAP_HEIGTH - VERTICAL_RESOLUTION

typedef struct
{
	u32 cur_pos_x;
	u32 cur_pos_y;
} Camera;

u32 update_camera_x;
u32 update_camera_y;
u32 player_x_on_map;
u32 player_y_on_map;
u32 player_x_on_screen;
u32 player_y_on_screen;

// Hero
#define PLAYER_1_WIDTH 128
#define PLAYER_1_HEIGTH 72

// player collision box offset
#define BOX_LEFT_OFFSET 56
#define BOX_RIGHT_OFFSET 56
#define BOX_TOP_OFFSET 36
#define BOX_BOTTON_OFFSET 0

#define MIN_POS_X 0
#define MAX_POS_X MAP_WIDTH
#define MIN_POS_Y 0
#define MAX_POS_Y MAP_HEIGTH

//  How far to (left, right, up, down) before camera moves
#define HOW_FAR_TO_LEFT 10
#define HOW_FAR_TO_RIGHT 152
#define HOW_FAR_TO_TOP 10
#define HOW_FAR_TO_BOTTON 116

typedef struct
{
	bool top_left;
	bool top_right;
	bool botton_left;
	bool botton_right;
} CollisionBox;

CollisionBox collision_from[2];

int blocked_left_coord[2] = {MIN_POS_X, MIN_POS_X};
int blocked_right_coord[2] = {MAX_POS_X, MAX_POS_X};
int blocked_top_coord[2] = {MIN_POS_Y, MIN_POS_Y};
int blocked_botton_coord[2] = {MAX_POS_Y, MAX_POS_Y};
// para imprimir coordenadas
char top_buffer[100];
char botton_buffer[100];
char left_buffer[100];
char right_buffer[100];

#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
#define MATRIX_MAX_LIN_INDEX 13
#define MATRIX_MAX_COL_INDEX 19

const int BGA_COLLISION_MATRIX[14][20] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

typedef struct
{
	s16 h_scroll_offset_bg;
	s16 v_scroll_offset_bg;
	u16 pos_x;
	u16 pos_y;
} Background;
#define A 0
#define B 1

#define ANIM_STANDING 0
#define ANIM_STANDING_TURN 1
#define ANIM_WALK 2
#define ANIM_WALK_TURN 3
#define ANIM_RUN 4
#define ANIM_RUN_TO_IDLE 5
#define ANIM_RUN_TURN 6
#define ANIM_SLIDE 7
#define ANIM_WALL_SLIDE 8
#define ANIM_WALL_JUMP 9
#define ANIM_LEDGE_HANG 10
#define ANIM_CLIMB_LEDGE 11
#define ANIM_JUMP 12
#define ANIM_FALL 13
#define ANIM_FALL_LOOP 14
#define ANIM_HURT 15
#define ANIM_DASH 16
#define ANIM_ATTACK_1 17
#define ANIM_ATTACK_1_END 18
#define ANIM_ATTACK_2 19
#define ANIM_ATTACK_2_END 20
#define ANIM_ATTACK_3 21
#define ANIM_ATTACK_3_END 22
#define ANIM_DEATH 23

enum PlayerState
{
	STATE_STANDING,
	STATE_STANDING_TURN,
	STATE_WALK,
	STATE_WALK_TURN,
	STATE_RUN,
	STATE_RUN_TO_IDLE,
	STATE_RUN_TURN,
	STATE_SLIDE,
	STATE_SLIDE_END,
	STATE_WALL_SLIDE,
	STATE_WALL_JUMP,
	STATE_LEDGE_HANG,
	STATE_CLIMB_LEDGE,
	STATE_JUMP,
	STATE_FALL,
	STATE_FALL_LOOP,
	STATE_HURT,
	STATE_DASH,
	STATE_ATTACK_1,
	STATE_ATTACK_1_END,
	STATE_ATTACK_2,
	STATE_ATTACK_2_END,
	STATE_ATTACK_3,
	STATE_ATTACK_3_END,
	STATE_DEATH
};

enum timeOfAnimation
{
	TIME_ATTACK_1 = 30,
	TIME_ATTACK_1_END = 40,
	TIME_ATTACK_2 = 60,
	TIME_ATTACK_2_END = 40,
	TIME_ATTACK_3 = 120,
	TIME_ATTACK_3_END = 60,
	TIME_SLIDE = 80
};

// player sprite array num_of_player
#define ONE 0
#define TWO 1

#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define NEUTRAL 0

typedef struct
{
	Sprite *sprite;
	enum PlayerState state;
	s16 width;
	s16 height;
	bool flip_h;
	bool flip_v;
	s16 anim;
	s16 *ptr_anim;
	s16 frame;
	bool is_full_anim;
	s16 pos_x;
	s16 pos_y;
	s16 order_x;
	s16 order_y;
	s16 last_order_x;
	s16 last_order_y;
	s16 impulse_x;
	s16 impulse_y;
	s16 speed_x;
	s16 speed_y;
	s16 max_speed_x;
	s16 max_speed_y;
	bool is_attacking;
	bool has_stamina;
} Player;

typedef struct
{
	Sprite *sprite;
	s16 offset_x;
	s16 offset_y;
	s16 pos_x;
	s16 pos_y;
} SpriteElement;

s16 counter_x[2] = {0, 0};
s16 counter_y[2] = {0, 0};

u16 attack_timer[2] = {0, 0};
u16 effect_timer[2] = {0, 0};
u16 anim_duration[2] = {0, 0}; // animation _speed * num of frames of animation

typedef struct
{
	bool btn_left;
	bool btn_right;
	bool btn_up;
	bool btn_down;
	bool btn_start;
	bool btn_mode;
	bool btn_a;
	bool btn_b;
	bool btn_c;
	bool btn_x;
	bool btn_y;
	bool btn_z;
} Joystick;

u16 ind;
Map *bga;
Map *title;
Background bg[2]; // BGA & BGB
Camera camera;
Player player[2];
SpriteElement spr_element[10];
Joystick joystick[2];

// static void titleInputHandler(u16 joy, u16 changed, u16 state);
// static void menuInputHandler(u16 joy, u16 changed, u16 state);
static void gameInputHandler(u16 joy, u16 changed, u16 state);

static void finiteStateMachine(int num_of_player);
static void updatePlayerPosition(int num_of_player);
static void controlEffects(int num_of_player);

/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAttackTimer(int num_of_player);

static void controlXAcceleration(int num_of_player);
static void controlYAcceleration(int num_of_player);

// FLIP
static void controlHorizontalFlip(int num_of_player);
static void controlVerticalFlip(int num_of_player);

// COLLISION
static void controlPlayerMapCollision(int num_of_player);

// BOUNDARIES
static void controlMapBoundaries(int num_of_player);
static void updateCamera(int num_of_player);

static void processGameTitle();
static void processMainGame();
static void releaseMemory();

// GAME STATE - https://www.ohsat.com/tutorial/mdmisc/simple-menu/
enum GameState
{
	TITLE,
	GAME,
	MENU
};
enum GameState current_game_state;

typedef struct
{
	u16 x;
	u16 y;
	char label[10];
} Option;

#define NUM_OPTIONS 3
Option options[NUM_OPTIONS] = {
	{10, 20, "START"},
	{10, 21, "OPTIONS"},
	{10, 22, "EXIT"},
};

u8 currentIndex = 0;
Sprite *prt_cursor;

void updateCursorPosition()
{
	SPR_setPosition(prt_cursor, options[currentIndex].x * 8 - 40, options[currentIndex].y * 8 - 12);
}

void moveUp()
{
	if (currentIndex > 0)
	{
		currentIndex--;
		updateCursorPosition();
	}
}

void moveDown()
{
	if (currentIndex < NUM_OPTIONS - 1)
	{
		currentIndex++;
		updateCursorPosition();
	}
}

void select(u16 Option)
{
	switch (Option)
	{
	case 0:
	{
		pickStart();
		break;
	}
	case 1:
	{
		pickOptions();
		break;
	}
	case 2:
	{
		pickExit();
		break;
	}

	default:
		break;
	}
}

void pickStart()
{
	current_game_state = GAME;
}

void pickOptions()
{
	current_game_state = MENU;
}

void pickExit()
{
	current_game_state = TITLE;
}

int main()
{
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();

	SPR_init();
	JOY_init();
	JOY_setEventHandler(&gameInputHandler);
	current_game_state = TITLE;
	// DMA_setBufferSize(9000);

	while (1)
	{
		switch (current_game_state)
		{
		case TITLE:
			processGameTitle();
			break;

		case MENU:

			break;

		case GAME:
			processMainGame();
			break;

		default:
			break;
		}
	}
	return (0);
}

static void releaseMemory()
{
	PAL_fadeOut(0, 63, 20, FALSE);
	SPR_reset();
	SPR_update();
	VDP_clearPlane(BG_A, TRUE);
	VDP_clearPlane(BG_B, TRUE);
	// VDP_clearPlane(WINDOW, TRUE);
	// VDP_setColors(0, , 64);
	bga = NULL;
	title = NULL;
	// clear move pointers
	free(player[ONE].ptr_anim);
	free(player[TWO].ptr_anim);
}

static void processGameTitle()
{
	ind = TILE_USER_INDEX;
	VDP_loadTileSet(&title_tileset, ind, DMA);
	PAL_setPalette(PAL0, title_palette.data, DMA);
	title = MAP_create(&title_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(title, 0, 0);
	ind += level_tileset.numTile;

	// MENU
	prt_cursor = SPR_addSprite(&cursor, 0, 0, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
	PAL_setPalette(PAL3, cursor.palette->data, DMA);

	// Draw options
	VDP_setTextPriority(TRUE);
	PAL_setColor(15, RGB24_TO_VDPCOLOR(0xff0000));
	VDP_drawTextBG(BG_A, options[0].label, options[0].x, options[0].y);
	VDP_drawTextBG(BG_A, options[1].label, options[0].x, options[1].y);
	VDP_drawTextBG(BG_A, options[2].label, options[0].x, options[2].y);

	// PAL_fadeIn(0, 63, title_palette.data, 20, FALSE);
	while (current_game_state == TITLE)
	{
		updateCursorPosition();
		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	releaseMemory();
}

static void processMainGame()
{
	ind = TILE_USER_INDEX;
	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	// init
	// BGA (MAP)
	camera.cur_pos_x = 0;
	camera.cur_pos_y = 0;
	/* VDP_loadTileSet(&level_tileset, ind, DMA);
	PAL_setPalette(PAL1, bga_palette.data, DMA);
	bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
	ind += level_tileset.numTile; */

	// BGB
	bg[B].h_scroll_offset_bg = 0;
	bg[B].v_scroll_offset_bg = 0;
	bg[B].pos_x = 0;
	bg[B].pos_y = 0;
	VDP_drawImageEx(BG_B, &background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), bg[B].pos_x, bg[B].pos_y, FALSE, TRUE);
	PAL_setPalette(PAL0, background.palette->data, DMA);
	ind += background.tileset->numTile;

	// Player I
	collision_from[ONE].top_left = FALSE;
	collision_from[ONE].top_right = FALSE;
	collision_from[ONE].botton_left = FALSE;
	collision_from[ONE].botton_right = FALSE;

	player[ONE].state = STATE_STANDING;
	player[ONE].width = 128;
	player[ONE].height = 72;
	player[ONE].anim = 0; // player[ONE].ptr_anim[STATE_STANDING];
	player[ONE].is_full_anim = TRUE;
	player[ONE].flip_h = FALSE;
	player[ONE].flip_v = FALSE;
	player[ONE].impulse_x = 2;
	player[ONE].impulse_y = 5;
	player[ONE].max_speed_x = 2;
	player[ONE].max_speed_y = 5;
	player[ONE].order_x = NEUTRAL;
	player[ONE].order_y = NEUTRAL;
	player[ONE].last_order_x = RIGHT;
	player[ONE].last_order_y = NEUTRAL;
	player[ONE].pos_x = HOW_FAR_TO_LEFT;
	player[ONE].pos_y = HOW_FAR_TO_BOTTON;
	player[ONE].speed_x = 0;
	player[ONE].speed_y = 0;
	player[ONE].is_attacking = FALSE;
	player[ONE].has_stamina = TRUE;
	player[ONE].sprite = SPR_addSprite(&hero, player[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL2, FALSE, player[ONE].flip_v, player[ONE].flip_h));
	PAL_setPalette(PAL2, hero.palette->data, DMA);
	SPR_setAnim(player[ONE].sprite, player[ONE].anim);

	// spr_element[ONE].offset_x = 32;
	// PAL_setPalette(PAL3, dagger.palette->data, DMA);
	// spr_element[ONE].pos_x = player[ONE].flip_h ? (player[ONE].pos_x + PLAYER_1_WIDTH) : (player[ONE].pos_x - spr_element[ONE].offset_x);
	// spr_element[ONE].pos_y = player[ONE].pos_y;
	// spr_element[ONE].sprite = SPR_addSprite(&dagger, spr_element[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL3, TRUE, player[ONE].flip_v, player[ONE].flip_h));
	// SPR_setAnim(spr_element[ONE].sprite, 0);
	// SPR_setVisibility(spr_element[ONE].sprite, HIDDEN);

	// PAL_fadeIn(0, 63, bga_palette.data, 20, FALSE);
	while (current_game_state == GAME)
	{
		finiteStateMachine(ONE);
		updateCamera(ONE);
		updatePlayerPosition(ONE);
		controlMapBoundaries(ONE);
		// controlEffects(ONE);
		controlHorizontalFlip(ONE);
		// controlVerticalFlip(ONE);
		controlXAcceleration(ONE);
		controlYAcceleration(ONE);
		controlAttackTimer(ONE);
		controlPlayerMapCollision(ONE);

		if (player[ONE].is_full_anim)
		{
			SPR_setAnim(player[ONE].sprite, player[ONE].anim);
		}
		else
		{
			SPR_setAnimAndFrame(player[ONE].sprite, player[ONE].anim, player[ONE].frame);
			// SPR_setAnim(player[ONE].sprite, player[ONE].anim);
			// SPR_setFrame(player[ONE].sprite, player[ONE].frame);
		}

		SPR_setHFlip(player[ONE].sprite, player[ONE].flip_h);

		// SPR_setPosition(player[ONE].sprite, (player[ONE].pos_x - update_camera_x), player[ONE].pos_y);
		// MAP_scrollTo(bga, update_camera_x, update_camera_y);
		SPR_setPosition(player[ONE].sprite, (player[ONE].pos_x - camera.cur_pos_x), (player[ONE].pos_y - camera.cur_pos_y));
		// MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);

		// spr_element[ONE].offset_x = 32;
		// spr_element[ONE].pos_x = player[ONE].flip_h ? (player[ONE].pos_x + PLAYER_1_WIDTH) : (player[ONE].pos_x - spr_element[ONE].offset_x);
		// spr_element[ONE].pos_y = player[ONE].pos_y;
		// SPR_setAnim(spr_element[ONE].sprite, 0);
		// SPR_setHFlip(spr_element[ONE].sprite, player[ONE].flip_h);
		// SPR_setPosition(spr_element[ONE].sprite, spr_element[ONE].pos_x, spr_element[ONE].pos_y);
		// SPR_setVisibility(spr_element[ONE].sprite, HIDDEN);
		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	releaseMemory();
}

static void titleInputHandler(u16 joy, u16 changed, u16 state)
{
	/* code */
	/* if (changed & state & BUTTON_UP)
	{
		moveUp();
	}
	else if (changed & state & BUTTON_DOWN)
	{
		moveDown();
	}
	if (changed & state & BUTTON_START)
	{
		select(currentIndex);
	} */
}

static void menuInputHandler(u16 joy, u16 changed, u16 state)
{
	/* code */
}

static void gameInputHandler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_LEFT)
			joystick[ONE].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joystick[ONE].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joystick[ONE].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joystick[ONE].btn_right = FALSE;

		if (state & BUTTON_UP)
		{
			joystick[ONE].btn_up = TRUE;
			if (current_game_state == TITLE)
			{
				moveUp();
			}
		}
		else if (changed & BUTTON_UP)
			joystick[ONE].btn_up = FALSE;

		if (state & BUTTON_DOWN)
		{
			joystick[ONE].btn_down = TRUE;
			if (current_game_state == TITLE)
			{
				moveDown();
			}
		}
		else if (changed & BUTTON_DOWN)
			joystick[ONE].btn_down = FALSE;

		if (state & BUTTON_A)
			joystick[ONE].btn_a = TRUE;
		else if (changed & BUTTON_A)
			joystick[ONE].btn_a = FALSE;

		if (state & BUTTON_B)
			joystick[ONE].btn_b = TRUE;
		else if (changed & BUTTON_B)
			joystick[ONE].btn_b = FALSE;

		if (state & BUTTON_C)
			joystick[ONE].btn_c = TRUE;
		else if (changed & BUTTON_C)
			joystick[ONE].btn_c = FALSE;

		if (state & BUTTON_X)
			joystick[ONE].btn_x = TRUE;
		else if (changed & BUTTON_X)
			joystick[ONE].btn_x = FALSE;

		if (state & BUTTON_Y)
			joystick[ONE].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joystick[ONE].btn_y = FALSE;

		if (state & BUTTON_Z)
			joystick[ONE].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joystick[ONE].btn_z = FALSE;

		if (state & BUTTON_START)
		{
			joystick[ONE].btn_start = TRUE;
			if (current_game_state == TITLE)
			{
				select(currentIndex);
			}
		}
		else if (changed & BUTTON_START)
			joystick[ONE].btn_start = FALSE;

		if (state & BUTTON_MODE)
			joystick[ONE].btn_mode = TRUE;

		else if (changed & BUTTON_MODE)
			joystick[ONE].btn_mode = FALSE;
	}

	if (joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
			joystick[TWO].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joystick[TWO].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joystick[TWO].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joystick[TWO].btn_right = FALSE;

		if (state & BUTTON_UP)
			joystick[TWO].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			joystick[TWO].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			joystick[TWO].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			joystick[TWO].btn_down = FALSE;

		if (state & BUTTON_A)
			joystick[TWO].btn_a = TRUE;
		else if (changed & BUTTON_A)
			joystick[TWO].btn_a = FALSE;

		if (state & BUTTON_B)
			joystick[TWO].btn_b = TRUE;
		else if (changed & BUTTON_B)
			joystick[TWO].btn_b = FALSE;

		if (state & BUTTON_C)
			joystick[TWO].btn_c = TRUE;
		else if (changed & BUTTON_C)
			joystick[TWO].btn_c = FALSE;

		if (state & BUTTON_X)
			joystick[TWO].btn_x = TRUE;
		else if (changed & BUTTON_X)
			joystick[TWO].btn_x = FALSE;

		if (state & BUTTON_Y)
			joystick[TWO].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joystick[TWO].btn_y = FALSE;

		if (state & BUTTON_Z)
			joystick[TWO].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joystick[TWO].btn_z = FALSE;

		if (state & BUTTON_START)
			joystick[TWO].btn_start = TRUE;
		else if (changed & BUTTON_START)
			joystick[TWO].btn_start = FALSE;

		if (state & BUTTON_MODE)
			joystick[TWO].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			joystick[TWO].btn_mode = FALSE;
	}
}

static void finiteStateMachine(int num_of_player)
{
	switch (player[num_of_player].state)
	{
	case STATE_STANDING:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_left)
		{
			if (player[num_of_player].last_order_x == LEFT)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
				player[num_of_player].state = STATE_WALK;
				player[num_of_player].anim = ANIM_WALK;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
			else if (player[num_of_player].last_order_x == RIGHT)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
				player[num_of_player].state = STATE_WALK_TURN;
				player[num_of_player].anim = ANIM_WALK_TURN;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
		}
		else if (joystick[num_of_player].btn_right)
		{
			if (player[num_of_player].last_order_x == RIGHT)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
				player[num_of_player].state = STATE_WALK;
				player[num_of_player].anim = ANIM_WALK;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
			else if (player[num_of_player].last_order_x == LEFT)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
				player[num_of_player].state = STATE_WALK_TURN;
				player[num_of_player].anim = ANIM_WALK_TURN;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
		}
		if (joystick[num_of_player].btn_z /*  && player[num_of_player].has_stamina */)
		{
			// player[num_of_player].has_stamina = FALSE;
			player[num_of_player].state = STATE_JUMP;
			player[num_of_player].anim = ANIM_JUMP;
			player[num_of_player].order_y = UP;
			if (player[num_of_player].speed_y == 0)
			{
				player[num_of_player].speed_y += player[num_of_player].impulse_y;
			}
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, FALSE);
		}

		if (joystick[num_of_player].btn_b /*  && player[num_of_player].has_stamina */)
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].is_attacking = TRUE;
			// player[num_of_player].has_stamina = FALSE;
			anim_duration[num_of_player] = TIME_ATTACK_1;
			player[num_of_player].state = STATE_ATTACK_1;
			player[num_of_player].anim = ANIM_ATTACK_1;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_WALK_TURN:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_left)
		{
			if (player[num_of_player].last_order_x == LEFT)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
				player[num_of_player].state = STATE_WALK;
				player[num_of_player].anim = ANIM_WALK;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
			else if (player[num_of_player].last_order_x == RIGHT)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
				player[num_of_player].state = STATE_WALK_TURN;
				player[num_of_player].anim = ANIM_WALK_TURN;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
		}
		else if (joystick[num_of_player].btn_right)
		{
			if (player[num_of_player].last_order_x == RIGHT)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
				player[num_of_player].state = STATE_WALK;
				player[num_of_player].anim = ANIM_WALK;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
			else if (player[num_of_player].last_order_x == LEFT)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
				player[num_of_player].state = STATE_WALK_TURN;
				player[num_of_player].anim = ANIM_WALK_TURN;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
		}
		if (!(joystick[num_of_player].btn_left) && !(joystick[num_of_player].btn_right))
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_WALK:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_z /* && player[num_of_player].has_stamina */)
		{
			// player[num_of_player].has_stamina = FALSE;
			player[num_of_player].state = STATE_JUMP;
			player[num_of_player].anim = ANIM_JUMP;
			player[num_of_player].order_y = UP;
			if (player[num_of_player].speed_y == 0)
			{
				player[num_of_player].speed_y += player[num_of_player].impulse_y;
			}
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_c)
		{
			player[num_of_player].max_speed_x = 4;
			player[num_of_player].state = STATE_RUN;
			player[num_of_player].anim = ANIM_RUN;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_b /*  && player[num_of_player].has_stamina */)
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].is_attacking = TRUE;
			// player[num_of_player].has_stamina = FALSE;
			anim_duration[num_of_player] = TIME_ATTACK_1;
			player[num_of_player].state = STATE_ATTACK_1;
			player[num_of_player].anim = ANIM_ATTACK_1;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}

		if (!(joystick[num_of_player].btn_left) && !(joystick[num_of_player].btn_right))
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_RUN:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_z /* && player[num_of_player].has_stamina */)
		{
			// player[num_of_player].has_stamina = FALSE;
			player[num_of_player].state = STATE_JUMP;
			player[num_of_player].anim = ANIM_JUMP;
			player[num_of_player].order_y = UP;
			if (player[num_of_player].speed_y == 0)
			{
				player[num_of_player].speed_y += player[num_of_player].impulse_y;
			}
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (joystick[num_of_player].btn_b /*  && player[num_of_player].has_stamina */)
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].is_attacking = TRUE;
			// player[num_of_player].has_stamina = FALSE;
			anim_duration[num_of_player] = TIME_ATTACK_1;
			player[num_of_player].state = STATE_ATTACK_1;
			player[num_of_player].anim = ANIM_ATTACK_1;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}

		if (joystick[num_of_player].btn_a /*  && player[num_of_player].has_stamina */)
		{
			player[num_of_player].is_attacking = TRUE;
			// player[num_of_player].has_stamina = FALSE;
			anim_duration[num_of_player] = TIME_SLIDE;
			player[num_of_player].state = STATE_SLIDE;
			player[num_of_player].anim = ANIM_SLIDE;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, FALSE);
		}

		if (!joystick[num_of_player].btn_c)
		{
			player[num_of_player].max_speed_x = 2;
			player[num_of_player].state = STATE_WALK;
			player[num_of_player].anim = ANIM_WALK;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}

		if (!(joystick[num_of_player].btn_left) && !(joystick[num_of_player].btn_right))
		{
			player[num_of_player].anim = ANIM_RUN_TO_IDLE;
			player[num_of_player].state = STATE_RUN_TO_IDLE;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_RUN_TO_IDLE:
		player[num_of_player].order_x = NEUTRAL;
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (player[num_of_player].speed_x <= 0)
		{
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_JUMP:
		if (player[num_of_player].order_y == UP)
		{
			if (joystick[num_of_player].btn_left)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
			}
			else if (joystick[num_of_player].btn_right)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
			}
			else if (!(joystick[num_of_player].btn_left) && !(joystick[num_of_player].btn_right))
			{
				player[num_of_player].order_x = NEUTRAL;
			}
		}
		else if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_SLIDE:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (!joystick[num_of_player].btn_a)
		{
			player[num_of_player].state = STATE_SLIDE_END;
			player[num_of_player].anim = ANIM_SLIDE;
			player[num_of_player].is_full_anim = FALSE;
			player[num_of_player].frame = 5;
			player[num_of_player].is_attacking = TRUE;
			anim_duration[num_of_player] = 20;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		else if (!player[num_of_player].is_attacking)
		{
			player[num_of_player].state = STATE_RUN;
			player[num_of_player].anim = ANIM_RUN;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_SLIDE_END:
		if (player[num_of_player].order_y == DOWN)
		{
			player[num_of_player].state = STATE_FALL;
			player[num_of_player].anim = ANIM_FALL;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		if (!player[num_of_player].is_attacking)
		{
			player[num_of_player].state = STATE_RUN;
			player[num_of_player].anim = ANIM_RUN;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_FALL:
		if (player[num_of_player].order_y == DOWN)
		{
			if (joystick[num_of_player].btn_left)
			{
				player[num_of_player].order_x = LEFT;
				player[num_of_player].last_order_x = LEFT;
			}
			else if (joystick[num_of_player].btn_right)
			{
				player[num_of_player].order_x = RIGHT;
				player[num_of_player].last_order_x = RIGHT;
			}
			else if (!(joystick[num_of_player].btn_left) && !(joystick[num_of_player].btn_right))
			{
				player[num_of_player].order_x = NEUTRAL;
			}
		}
		else if (player[num_of_player].order_y == NEUTRAL)
		{
			player[num_of_player].order_x = NEUTRAL;
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_1:
		if (!joystick[num_of_player].btn_b)
		{
			if (!player[num_of_player].is_attacking)
			{
				if (joystick[num_of_player].btn_x)
				{
					player[num_of_player].order_x = NEUTRAL;
					player[num_of_player].is_attacking = TRUE;
					// player[num_of_player].has_stamina = FALSE;
					anim_duration[num_of_player] = TIME_ATTACK_2;
					player[num_of_player].state = STATE_ATTACK_2;
					player[num_of_player].anim = ANIM_ATTACK_2;
					player[num_of_player].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
				}
				else
				{
					player[num_of_player].is_attacking = TRUE;
					anim_duration[num_of_player] = TIME_ATTACK_1_END;
					player[num_of_player].state = STATE_ATTACK_1_END;
					player[num_of_player].anim = ANIM_ATTACK_1_END;
					player[num_of_player].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[num_of_player].sprite, FALSE);
				}
			}
		}
		break;

	case STATE_ATTACK_2:
		if (!joystick[num_of_player].btn_x)
		{
			if (!player[num_of_player].is_attacking)
			{
				if (joystick[num_of_player].btn_y)
				{
					player[num_of_player].order_x = NEUTRAL;
					player[num_of_player].is_attacking = TRUE;
					// player[num_of_player].has_stamina = FALSE;
					anim_duration[num_of_player] = TIME_ATTACK_3;
					player[num_of_player].state = STATE_ATTACK_3;
					player[num_of_player].anim = ANIM_ATTACK_3;
					player[num_of_player].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
				}
				else
				{
					player[num_of_player].is_attacking = TRUE;
					anim_duration[num_of_player] = TIME_ATTACK_2_END;
					player[num_of_player].state = STATE_ATTACK_2_END;
					player[num_of_player].anim = ANIM_ATTACK_2_END;
					player[num_of_player].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
				}
			}
		}
		break;
	case STATE_ATTACK_3:
		if (!joystick[num_of_player].btn_y)
		{
			if (!player[num_of_player].is_attacking)
			{
				player[num_of_player].is_attacking = TRUE;
				anim_duration[num_of_player] = TIME_ATTACK_3_END;
				player[num_of_player].state = STATE_ATTACK_3_END;
				player[num_of_player].anim = ANIM_ATTACK_3_END;
				player[num_of_player].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_1_END:
		if (!player[num_of_player].is_attacking)
		{
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_2_END:
		if (!player[num_of_player].is_attacking)
		{
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_3_END:
		if (!player[num_of_player].is_attacking)
		{
			player[num_of_player].state = STATE_STANDING;
			player[num_of_player].anim = ANIM_STANDING;
			player[num_of_player].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[num_of_player].sprite, TRUE);
		}
		break;

	default:
		break;
	}
}

static void controlXAcceleration(int num_of_player)
{
	if (player[num_of_player].order_x != NEUTRAL)
	{
		if (player[num_of_player].speed_x < player[num_of_player].max_speed_x)
		{
			counter_x[num_of_player] += 1;
			if (counter_x[num_of_player] == COUNTER_LIMIT)
			{
				counter_x[num_of_player] = 0;
				player[num_of_player].speed_x += 1;
			}
		}
		else if (player[num_of_player].speed_x > player[num_of_player].max_speed_x)
		{
			counter_x[num_of_player] += 1;
			if (counter_x[num_of_player] == COUNTER_LIMIT)
			{
				counter_x[num_of_player] = 0;
				player[num_of_player].speed_x -= 1;
			}
		}
	}
	else if (player[num_of_player].order_x == NEUTRAL)
	{
		if (player[num_of_player].speed_x > 0)
		{
			counter_x[num_of_player] += 1;
			if (counter_x[num_of_player] == COUNTER_LIMIT)
			{
				counter_x[num_of_player] = 0;
				player[num_of_player].speed_x -= 1;
			}
		}
	}
}

static void controlYAcceleration(int num_of_player)
{
	if (player[num_of_player].order_y == UP)
	{
		if (player[num_of_player].speed_y > 0)
		{
			counter_y[num_of_player] += 1;
			if (counter_y[num_of_player] == COUNTER_LIMIT)
			{
				counter_y[num_of_player] = 0;
				player[num_of_player].speed_y -= 1;
			}
		}
		else if (player[num_of_player].speed_y <= 0)
		{
			player[num_of_player].order_y = DOWN;
			player[num_of_player].speed_y = 0;
		}
	}
	else if (player[num_of_player].order_y == DOWN)
	{
		counter_y[num_of_player] += 1;
		if (counter_y[num_of_player] == COUNTER_LIMIT)
		{
			counter_y[num_of_player] = 0;
			if (player[num_of_player].speed_y < player[num_of_player].max_speed_y)
			{
				player[num_of_player].speed_y += 1;
			}
		}
	}
	else if (player[num_of_player].order_y == NEUTRAL)
	{
		counter_y[num_of_player] = 0;
		player[num_of_player].speed_y = 0;
	}
}

static void updatePlayerPosition(int num_of_player)
{
	if (player[num_of_player].speed_x > 0)
	{
		if (player[num_of_player].last_order_x == LEFT)
		{
			if ((player[num_of_player].pos_x + BOX_LEFT_OFFSET) > blocked_left_coord[num_of_player])
			{
				player[num_of_player].pos_x -= player[num_of_player].speed_x;
			}
		}
		else if (player[num_of_player].last_order_x == RIGHT)
		{
			if ((player[num_of_player].pos_x - BOX_RIGHT_OFFSET) < blocked_right_coord[num_of_player])
			{
				player[num_of_player].pos_x += player[num_of_player].speed_x;
			}
		}
	}
	if (player[num_of_player].speed_y > 0)
	{
		if (player[num_of_player].order_y == UP)
		{
			if ((player[num_of_player].pos_y + BOX_TOP_OFFSET) > blocked_top_coord[num_of_player])
			{
				player[num_of_player].pos_y -= player[num_of_player].speed_y;
			}
		}
		else if (player[num_of_player].order_y == DOWN)
		{
			if ((player[num_of_player].pos_y + player[num_of_player].height) < blocked_botton_coord[num_of_player])
			{
				player[num_of_player].pos_y += player[num_of_player].speed_y;
			}
		}
	}
}

static void controlHorizontalFlip(int num_of_player)
{
	if (player[num_of_player].order_x == RIGHT)
	{
		player[num_of_player].flip_h = FALSE;
	}
	else if (player[num_of_player].order_x == LEFT)
	{
		player[num_of_player].flip_h = TRUE;
	}
}

static void controlVerticalFlip(int num_of_player)
{
	if (player[num_of_player].order_y == UP)
	{
		player[num_of_player].flip_v = FALSE;
	}
	else if (player[num_of_player].order_y == DOWN)
	{
		player[num_of_player].flip_v = TRUE;
	}
	else
	{
		player[num_of_player].flip_v = player[num_of_player].flip_v;
	}
}

static void controlEffects(int num_of_player)
{
}

static void controlAttackTimer(int num_of_player)
{
	if (player[num_of_player].is_attacking)
	{
		if (attack_timer[num_of_player] < anim_duration[num_of_player])
			attack_timer[num_of_player]++;
		else
		{
			attack_timer[num_of_player] = 0;
			player[num_of_player].is_attacking = FALSE;
		}
	}
}

static void controlIdleTimer(int num_of_player)
{
	/* if (player[player] == STATE_STANDING || player[player] == STATE_BORED || player[player] == STATE_GO)
	{
		if (idle_timer[player] < idle_duration[player])
		{
			idle_timer[player] += 1;
		}
		else if (idle_timer[player] == idle_duration[player])
		{
			idle_timer[player] = 0;
		}
	}
	else
	{
		idle_timer[player] = 0;
	} */
}

static void controlMapBoundaries(int num_of_player)
{
	if ((player[num_of_player].pos_x + BOX_LEFT_OFFSET) < MIN_POS_X)
	{
		player[num_of_player].pos_x = MIN_POS_X - BOX_LEFT_OFFSET;
	}
	else if ((player[num_of_player].pos_x + player[num_of_player].width - BOX_RIGHT_OFFSET) > MAX_POS_X)
	{
		player[num_of_player].pos_x = (MAX_POS_X - player[num_of_player].width) + BOX_RIGHT_OFFSET;
	}

	if (player[num_of_player].pos_y < MIN_POS_Y)
	{
		player[num_of_player].pos_y = MIN_POS_Y;
	}
	else if ((player[num_of_player].pos_y + player[num_of_player].height) > MAX_POS_Y)
	{
		player[num_of_player].pos_y = (MAX_POS_Y - player[num_of_player].height);
	}
}

static void updateCamera(int num_of_player)
{
	player_x_on_map = player[num_of_player].pos_x;
	player_y_on_map = player[num_of_player].pos_y;
	player_x_on_screen = (player_x_on_map - camera.cur_pos_x);
	player_y_on_screen = (player_y_on_map - camera.cur_pos_y);

	if (player_x_on_screen > HOW_FAR_TO_RIGHT)
	{
		update_camera_x = player_x_on_map - HOW_FAR_TO_RIGHT;
	}
	else if (player_x_on_screen < HOW_FAR_TO_LEFT)
	{
		update_camera_x = player_x_on_map - HOW_FAR_TO_LEFT;
	}
	else
	{
		update_camera_x = camera.cur_pos_x;
	}

	if (player_y_on_screen > HOW_FAR_TO_BOTTON)
	{
		update_camera_y = player_y_on_map - HOW_FAR_TO_BOTTON;
	}
	else if (player_y_on_screen < HOW_FAR_TO_TOP)
	{
		update_camera_y = player_y_on_map - HOW_FAR_TO_TOP;
	}
	else
	{
		update_camera_y = camera.cur_pos_y;
	}

	// stop camera from going beyond map boundaries
	if (update_camera_x < 0)
	{
		update_camera_x = 0;
	}
	else if (update_camera_x > (MAP_WIDTH - HORIZONTAL_RESOLUTION))
	{
		update_camera_x = (MAP_WIDTH - HORIZONTAL_RESOLUTION);
	}

	if (update_camera_y < 0)
	{
		update_camera_y = 0;
	}
	else if (update_camera_y > (MAP_HEIGTH - VERTICAL_RESOLUTION))
	{
		update_camera_y = (MAP_HEIGTH - VERTICAL_RESOLUTION);
	}

	// update current camera X & Y
	if ((camera.cur_pos_x != update_camera_x) || (camera.cur_pos_y != update_camera_y))
	{
		camera.cur_pos_x = update_camera_x;
		camera.cur_pos_y = update_camera_y;
	}
};

static void controlPlayerMapCollision(int num_of_player)
{
	//	#define MIN_POS_X 0 - BOX_LEFT_OFFSET
	//	#define MAX_POS_X MAP_WIDTH - PLAYER_1_WIDTH + BOX_RIGHT_OFFSET
	//	#define MIN_POS_Y 0 - BOX_TOP_OFFSET
	//	#define MAX_POS_Y MAP_HEIGTH - PLAYER_1_HEIGTH + BOX_BOTTON_OFFSET
	// define a caixa de colisão do personagem
	int left_edge = player[num_of_player].pos_x + BOX_LEFT_OFFSET;
	int right_edge = (player[num_of_player].pos_x + player[num_of_player].width) - BOX_RIGHT_OFFSET;
	int top_edge = player[num_of_player].pos_y - BOX_TOP_OFFSET;
	int botton_edge = player[num_of_player].pos_y + player[num_of_player].height;

	// arestas:
	// verifica em qual indice da matriz cada aresta está colidindo
	int left_edge_column_index = (left_edge / TILE_IN_PIXELS);
	int right_edge_column_index = (right_edge / TILE_IN_PIXELS);
	int top_edge_line_index = (top_edge / TILE_IN_PIXELS);
	int botton_edge_line_index = (botton_edge / TILE_IN_PIXELS);

	if (right_edge_column_index > MATRIX_MAX_COL_INDEX)
	{
		right_edge_column_index = MATRIX_MAX_COL_INDEX;
	}
	if (botton_edge_line_index > MATRIX_MAX_LIN_INDEX)
	{
		botton_edge_line_index = MATRIX_MAX_LIN_INDEX;
	}
	// vértices: cruzamentos de linhas com colunas
	int top_left_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index][left_edge_column_index];
	int top_right_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index][right_edge_column_index];
	int botton_left_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index][left_edge_column_index];
	int botton_right_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index][right_edge_column_index];

	if (player[num_of_player].order_y == UP)
	{
		if (top_left_tile_collision_type == SOLID_TILE || top_right_tile_collision_type == SOLID_TILE)
		{
			blocked_top_coord[num_of_player] = top_edge;
		}
		else
		{
			blocked_top_coord[num_of_player] = MIN_POS_Y;
		}
	}

	if (player[num_of_player].order_y == DOWN || player[num_of_player].order_y == NEUTRAL)
	{
		if (botton_left_tile_collision_type == SOLID_TILE || botton_right_tile_collision_type == SOLID_TILE)
		{
			blocked_botton_coord[num_of_player] = botton_edge;
		}
		else
		{
			blocked_botton_coord[num_of_player] = MAX_POS_Y;
		}
	}

	if (player[num_of_player].last_order_x == LEFT)
	{
		if (top_left_tile_collision_type == SOLID_TILE || botton_left_tile_collision_type == SOLID_TILE)
		{
			blocked_left_coord[num_of_player] = left_edge;
		}
		else
		{
			blocked_left_coord[num_of_player] = MIN_POS_X;
		}
	}

	if (player[num_of_player].last_order_x == RIGHT)
	{
		if (top_right_tile_collision_type == SOLID_TILE || botton_right_tile_collision_type == SOLID_TILE)
		{
			blocked_right_coord[num_of_player] = right_edge;
		}
		else
		{
			blocked_right_coord[num_of_player] = MAX_POS_X;
		}
	}

	// sprintf(top_buffer, "B.L:%d", blocked_left_coord[num_of_player] /* blocked_top_coord[num_of_player] */);
	// sprintf(botton_buffer, "B.R:%d", blocked_right_coord[num_of_player] /* blocked_botton_coord[num_of_player] */);
	sprintf(left_buffer, "B.L.C.T:%d", botton_left_tile_collision_type/* blocked_left_coord[num_of_player] */);
	sprintf(right_buffer, "B.R.C.T:%d", botton_right_tile_collision_type /* blocked_right_coord[num_of_player] */);
	// sprintf(top_buffer, "L.E.:%d", left_edge_column_index /* blocked_top_coord[num_of_player] */);
	// sprintf(botton_buffer, "R.E.:%d", right_edge_column_index /* blocked_botton_coord[num_of_player] */);
	// sprintf(left_buffer, "T.E.:%d", top_edge_line_index /* blocked_left_coord[num_of_player] */);
	// sprintf(right_buffer, "B.E.:%d", botton_edge_line_index /* blocked_right_coord[num_of_player] */);

	// VDP_clearTextBG(BG_A, 28, 5, 10);
	// VDP_clearTextBG(BG_A, 28, 6, 10);
	VDP_clearTextBG(BG_A, 28, 7, 10);
	VDP_clearTextBG(BG_A, 28, 8, 10);

	// VDP_drawTextBG(BG_A, top_buffer, 28, 5);
	// VDP_drawTextBG(BG_A, botton_buffer, 28, 6);
	VDP_drawTextBG(BG_A, left_buffer, 28, 7);
	VDP_drawTextBG(BG_A, right_buffer, 28, 8);
}

// SGDK provides specific functions to control animation looping for sprites
//. You can use the built-in functions to manage this, or handle the animation manually if needed.
// Controlling Animation Loop
// The primary function to control if a sprite animation loops is SPR_setAnimationLoop().
//
//     SPR_setAnimationLoop(Sprite* sprite, bool loop): This function enables or disables the automatic looping for a specific sprite's current animation.
//         To make an animation loop continuously, set loop to TRUE.
//         To make an animation play only once and stop on the last frame, set loop to FALSE.
//
//		// Assuming 'playerSprite' is a pointer to your Sprite structure
//// and you have already initialized and added the sprite to the VDP.
//
//// Set the current animation (e.g., animation num_of_player 0)
//	 SPR_setAnim(playerSprite, 0);
//
//// Enable looping for the current animation (it will loop indefinitely)
//   SPR_setAnimationLoop(playerSprite, TRUE);
//
//// ... in your main game loop ...
// while (1)
//{
//	// Update sprite animations (handles the actual frame updates and looping based on the flag)
//	SPR_update();
//
//	// ... other game logic ...
//
//	SYS_doVBlankProcess();
// }
//
// Manual Control(Alternative Method)
//	If you need more granular control(e.g., looping only a specific range of frames, or triggering a different event after a non - looping animation finishes),
//	you can disable the automatic system and manage the frames yourself within your game loop.
//
//	Disable automatic animation : Set the sprite's timer to 0 to stop SGDK from automatically advancing frames. Manually update : In your main game loop,
//	you can use functions like SPR_setFrame() and SPR_nextFrame() within a conditional check(e.g., after a certain amount of time has passed or in the V - blank process) to manage the animation sequence and looping logic manually.
//
//	For most standard use cases,
//	SPR_setAnimationLoop() is the recommended and simplest method.