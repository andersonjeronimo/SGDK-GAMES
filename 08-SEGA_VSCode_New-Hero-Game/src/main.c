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
#define MAP_WIDTH 640
#define MAP_HEIGTH 224
// screen
#define LEFT_SCREEN_LIMIT 0
#define RIGHT_SCREEN_LIMIT MAP_WIDTH - HORIZONTAL_RESOLUTION
#define TOP_SCREEN_LIMIT 0
#define BOTTON_SCREEN_LIMIT MAP_HEIGTH - VERTICAL_RESOLUTION

typedef struct
{
	fix16 cur_pos_x;
	fix16 cur_pos_y;
} Camera;

fix16 update_camera_x;
fix16 update_camera_y;
fix16 player_x_on_map;
fix16 player_y_on_map;
fix16 player_x_on_screen;
fix16 player_y_on_screen;

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

int min_x_coord[2] = {(MIN_POS_X - BOX_LEFT_OFFSET), (MIN_POS_X - BOX_LEFT_OFFSET)};
int max_x_coord[2] = {MAX_POS_X, MAX_POS_X};
int min_y_coord[2] = {(MIN_POS_Y - BOX_TOP_OFFSET), (MIN_POS_Y - BOX_TOP_OFFSET)};
int max_y_coord[2] = {(MAX_POS_Y - PLAYER_1_HEIGTH), MAX_POS_Y};
// para imprimir coordenadas
char top_buffer[100];
char botton_buffer[100];
char left_buffer[100];
char right_buffer[100];

#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
#define MATRIX_MAX_LIN_INDEX 13
#define MATRIX_MAX_COL_INDEX 39

const int BGA_COLLISION_MATRIX[14][40] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

typedef struct
{
	fix16 h_scroll_offset_bg;
	fix16 v_scroll_offset_bg;
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
	TIME_ATTACK_1 = 24,
	TIME_ATTACK_1_END = 32,
	TIME_ATTACK_2 = 48,
	TIME_ATTACK_2_END = 32,
	TIME_ATTACK_3 = 96,
	TIME_ATTACK_3_END = 24,
	TIME_SLIDE = 64
};

// player sprite array player_id
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
	fix16 width;
	fix16 height;
	bool flip_h;
	bool flip_v;
	fix16 anim;
	fix16 *ptr_anim;
	fix16 frame;
	bool is_full_anim;
	fix16 pos_x;
	fix16 pos_y;
	fix16 order_x;
	fix16 order_y;
	fix16 last_order_x;
	fix16 last_order_y;
	fix16 impulse_x;
	fix16 impulse_y;
	fix16 speed_x;
	fix16 speed_y;
	fix16 max_speed_x;
	fix16 max_speed_y;
	bool is_attacking;
	bool has_stamina;
} Player;

typedef struct
{
	Sprite *sprite;
	fix16 offset_x;
	fix16 offset_y;
	fix16 pos_x;
	fix16 pos_y;
} SpriteElement;

fix16 counter_x[2] = {0, 0};
fix16 counter_y[2] = {0, 0};

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

static void finiteStateMachine(int player_id);
static void updatePlayerPosition(int player_id);
static void controlEffects(int player_id);

/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAttackTimer(int player_id);

static void controlXAcceleration(int player_id);
static void controlYAcceleration(int player_id);

// FLIP
static void controlHorizontalFlip(int player_id);
static void controlVerticalFlip(int player_id);

// COLLISION
static void controlPlayerMapCollision(int player_id);
static void checkBottonCollision(int player_id);
static void checkTopCollision(int player_id);

// BOUNDARIES
static void controlMapBoundaries(int player_id);
static void updateCamera(int player_id);

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
	SPR_setPosition(prt_cursor, options[currentIndex].x * 8 - 50, options[currentIndex].y * 8 - 20);
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
	PAL_setColor(15, RGB24_TO_VDPCOLOR(0xffff00));
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
	camera.cur_pos_y = 44;
	VDP_loadTileSet(&level_tileset, ind, DMA);
	PAL_setPalette(PAL1, bga_palette.data, DMA);
	bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
	ind += level_tileset.numTile;

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
	player[ONE].pos_y = ((MAX_POS_Y - 44) - PLAYER_1_HEIGTH);
	player[ONE].speed_x = 0;
	player[ONE].speed_y = 0;
	player[ONE].is_attacking = FALSE;
	player[ONE].has_stamina = TRUE;
	player[ONE].sprite = SPR_addSprite(&hero, player[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL2, FALSE, player[ONE].flip_v, player[ONE].flip_h));
	PAL_setPalette(PAL2, hero.palette->data, DMA);
	SPR_setAnim(player[ONE].sprite, player[ONE].anim);
	
	while (current_game_state == GAME)
	{
		finiteStateMachine(ONE);
		updateCamera(ONE);
		updatePlayerPosition(ONE);
		controlMapBoundaries(ONE);
		controlPlayerMapCollision(ONE);
		checkBottonCollision(ONE);
		checkTopCollision(ONE);
		// controlEffects(ONE);
		controlHorizontalFlip(ONE);
		// controlVerticalFlip(ONE);
		controlXAcceleration(ONE);
		controlYAcceleration(ONE);
		controlAttackTimer(ONE);

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
		SPR_setPosition(player[ONE].sprite, (player[ONE].pos_x - camera.cur_pos_x), (player[ONE].pos_y - camera.cur_pos_y));
		MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
		
		
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

static void finiteStateMachine(int player_id)
{
	switch (player[player_id].state)
	{
	case STATE_STANDING:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_left)
		{
			if (player[player_id].last_order_x == LEFT)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
				player[player_id].state = STATE_WALK;
				player[player_id].anim = ANIM_WALK;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
			else if (player[player_id].last_order_x == RIGHT)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
				player[player_id].state = STATE_WALK_TURN;
				player[player_id].anim = ANIM_WALK_TURN;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		else if (joystick[player_id].btn_right)
		{
			if (player[player_id].last_order_x == RIGHT)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
				player[player_id].state = STATE_WALK;
				player[player_id].anim = ANIM_WALK;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
			else if (player[player_id].last_order_x == LEFT)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
				player[player_id].state = STATE_WALK_TURN;
				player[player_id].anim = ANIM_WALK_TURN;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		if (joystick[player_id].btn_z /*  && player[player_id].has_stamina */)
		{
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_JUMP;
			player[player_id].anim = ANIM_JUMP;
			player[player_id].order_y = UP;
			if (player[player_id].speed_y == 0)
			{
				player[player_id].speed_y += player[player_id].impulse_y;
			}
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (joystick[player_id].btn_b /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_WALK_TURN:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_left)
		{
			if (player[player_id].last_order_x == LEFT)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
				player[player_id].state = STATE_WALK;
				player[player_id].anim = ANIM_WALK;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
			else if (player[player_id].last_order_x == RIGHT)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
				player[player_id].state = STATE_WALK_TURN;
				player[player_id].anim = ANIM_WALK_TURN;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		else if (joystick[player_id].btn_right)
		{
			if (player[player_id].last_order_x == RIGHT)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
				player[player_id].state = STATE_WALK;
				player[player_id].anim = ANIM_WALK;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
			else if (player[player_id].last_order_x == LEFT)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
				player[player_id].state = STATE_WALK_TURN;
				player[player_id].anim = ANIM_WALK_TURN;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_WALK:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_z /* && player[player_id].has_stamina */)
		{
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_JUMP;
			player[player_id].anim = ANIM_JUMP;
			player[player_id].order_y = UP;
			if (player[player_id].speed_y == 0)
			{
				player[player_id].speed_y += player[player_id].impulse_y;
			}
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_c)
		{
			player[player_id].max_speed_x = 4;
			player[player_id].state = STATE_RUN;
			player[player_id].anim = ANIM_RUN;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_b /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}

		if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_RUN:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_z /* && player[player_id].has_stamina */)
		{
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_JUMP;
			player[player_id].anim = ANIM_JUMP;
			player[player_id].order_y = UP;
			if (player[player_id].speed_y == 0)
			{
				player[player_id].speed_y += player[player_id].impulse_y;
			}
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (joystick[player_id].btn_b /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}

		if (joystick[player_id].btn_a /*  && player[player_id].has_stamina */)
		{
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_SLIDE;
			player[player_id].state = STATE_SLIDE;
			player[player_id].anim = ANIM_SLIDE;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (!joystick[player_id].btn_c)
		{
			player[player_id].max_speed_x = 2;
			player[player_id].state = STATE_WALK;
			player[player_id].anim = ANIM_WALK;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}

		if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
		{
			player[player_id].anim = ANIM_RUN_TO_IDLE;
			player[player_id].state = STATE_RUN_TO_IDLE;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_RUN_TO_IDLE:
		player[player_id].order_x = NEUTRAL;
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (player[player_id].speed_x <= 0)
		{
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_JUMP:
		if (player[player_id].order_y == UP)
		{
			if (joystick[player_id].btn_left)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
			}
			else if (joystick[player_id].btn_right)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
			}
			else if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
			{
				player[player_id].order_x = NEUTRAL;
			}
		}
		else if (player[player_id].order_y == NEUTRAL)
		{
			if (joystick[player_id].btn_left)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
			}
			else if (joystick[player_id].btn_right)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
			}
			else if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
			{
				player[player_id].order_x = NEUTRAL;
			}
		}
		else if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_SLIDE:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (!joystick[player_id].btn_a)
		{
			player[player_id].state = STATE_SLIDE_END;
			player[player_id].anim = ANIM_SLIDE;
			player[player_id].is_full_anim = FALSE;
			player[player_id].frame = 5;
			player[player_id].is_attacking = TRUE;
			anim_duration[player_id] = 20;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		else if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_RUN;
			player[player_id].anim = ANIM_RUN;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_SLIDE_END:
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_FALL;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_RUN;
			player[player_id].anim = ANIM_RUN;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_FALL:
		if (player[player_id].order_y == DOWN)
		{
			if (player[player_id].speed_y < player[player_id].max_speed_y)
			{
				if (joystick[player_id].btn_left)
				{
					player[player_id].order_x = LEFT;
					player[player_id].last_order_x = LEFT;
				}
				else if (joystick[player_id].btn_right)
				{
					player[player_id].order_x = RIGHT;
					player[player_id].last_order_x = RIGHT;
				}
				else if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
				{
					player[player_id].order_x = NEUTRAL;
				}
			}
			else if (player[player_id].speed_y == player[player_id].max_speed_y)
			{
				player[player_id].state = STATE_FALL_LOOP;
				player[player_id].anim = ANIM_FALL_LOOP;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		else if (player[player_id].order_y == NEUTRAL)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_FALL_LOOP:
		if (player[player_id].order_y == DOWN)
		{
			if (joystick[player_id].btn_left)
			{
				player[player_id].order_x = LEFT;
				player[player_id].last_order_x = LEFT;
			}
			else if (joystick[player_id].btn_right)
			{
				player[player_id].order_x = RIGHT;
				player[player_id].last_order_x = RIGHT;
			}
			else if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
			{
				player[player_id].order_x = NEUTRAL;
			}
		}
		else if (player[player_id].order_y == NEUTRAL)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_1:
		if (!joystick[player_id].btn_b)
		{
			if (!player[player_id].is_attacking)
			{
				if (joystick[player_id].btn_x)
				{
					player[player_id].order_x = NEUTRAL;
					player[player_id].is_attacking = TRUE;
					// player[player_id].has_stamina = FALSE;
					anim_duration[player_id] = TIME_ATTACK_2;
					player[player_id].state = STATE_ATTACK_2;
					player[player_id].anim = ANIM_ATTACK_2;
					player[player_id].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[player_id].sprite, TRUE);
				}
				else
				{
					player[player_id].is_attacking = TRUE;
					anim_duration[player_id] = TIME_ATTACK_1_END;
					player[player_id].state = STATE_ATTACK_1_END;
					player[player_id].anim = ANIM_ATTACK_1_END;
					player[player_id].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[player_id].sprite, FALSE);
				}
			}
		}
		break;

	case STATE_ATTACK_2:
		if (!joystick[player_id].btn_x)
		{
			if (!player[player_id].is_attacking)
			{
				if (joystick[player_id].btn_y)
				{
					player[player_id].order_x = NEUTRAL;
					player[player_id].is_attacking = TRUE;
					// player[player_id].has_stamina = FALSE;
					anim_duration[player_id] = TIME_ATTACK_3;
					player[player_id].state = STATE_ATTACK_3;
					player[player_id].anim = ANIM_ATTACK_3;
					player[player_id].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[player_id].sprite, TRUE);
				}
				else
				{
					player[player_id].is_attacking = TRUE;
					anim_duration[player_id] = TIME_ATTACK_2_END;
					player[player_id].state = STATE_ATTACK_2_END;
					player[player_id].anim = ANIM_ATTACK_2_END;
					player[player_id].is_full_anim = TRUE;
					SPR_setAnimationLoop(player[player_id].sprite, TRUE);
				}
			}
		}
		break;
	case STATE_ATTACK_3:
		if (!joystick[player_id].btn_y)
		{
			if (!player[player_id].is_attacking)
			{
				player[player_id].is_attacking = TRUE;
				anim_duration[player_id] = TIME_ATTACK_3_END;
				player[player_id].state = STATE_ATTACK_3_END;
				player[player_id].anim = ANIM_ATTACK_3_END;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_1_END:
		if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_2_END:
		if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_3_END:
		if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	default:
		break;
	}
}

static void controlXAcceleration(int player_id)
{
	if (player[player_id].order_x != NEUTRAL)
	{
		if (player[player_id].speed_x < player[player_id].max_speed_x)
		{
			counter_x[player_id] += 1;
			if (counter_x[player_id] == COUNTER_LIMIT)
			{
				counter_x[player_id] = 0;
				player[player_id].speed_x += 1;
			}
		}
		else if (player[player_id].speed_x > player[player_id].max_speed_x)
		{
			counter_x[player_id] += 1;
			if (counter_x[player_id] == COUNTER_LIMIT)
			{
				counter_x[player_id] = 0;
				player[player_id].speed_x -= 1;
			}
		}
	}
	else if (player[player_id].order_x == NEUTRAL)
	{
		if (player[player_id].speed_x > 0)
		{
			counter_x[player_id] += 1;
			if (counter_x[player_id] == COUNTER_LIMIT)
			{
				counter_x[player_id] = 0;
				player[player_id].speed_x -= 1;
			}
		}
	}
}

static void controlYAcceleration(int player_id)
{
	if (player[player_id].order_y == UP)
	{
		if (player[player_id].speed_y > 0)
		{
			counter_y[player_id] += 1;
			if (counter_y[player_id] == COUNTER_LIMIT)
			{
				counter_y[player_id] = 0;
				player[player_id].speed_y -= 1;
			}
		}
		else if (player[player_id].speed_y == 0)
		{
			player[player_id].order_y = NEUTRAL;
			counter_y[player_id] = 0;
		}
	}
	else if (player[player_id].order_y == DOWN)
	{
		counter_y[player_id] += 1;
		if (counter_y[player_id] == COUNTER_LIMIT)
		{
			counter_y[player_id] = 0;
			if (player[player_id].speed_y < player[player_id].max_speed_y)
			{
				player[player_id].speed_y += 1;
			}
		}
	}
	else if (player[player_id].order_y == NEUTRAL)
	{
		counter_y[player_id] = 0;
		player[player_id].speed_y = 0;
	}
}

static void updatePlayerPosition(int player_id)
{
	if (player[player_id].speed_x > 0)
	{
		if (player[player_id].last_order_x == LEFT)
		{
			if (player[player_id].pos_x > min_x_coord[player_id])
			{
				player[player_id].pos_x -= player[player_id].speed_x;
			}
			/* else if (player[player_id].pos_x <= min_x_coord[player_id])
			{
				player[player_id].pos_x += player[player_id].speed_x;
			} */
		}
		else if (player[player_id].last_order_x == RIGHT)
		{
			if (player[player_id].pos_x < max_x_coord[player_id])
			{
				player[player_id].pos_x += player[player_id].speed_x;
			}
			/* else if (player[player_id].pos_x >= max_x_coord[player_id])
			{
				player[player_id].pos_x -= player[player_id].speed_x;
			} */
		}
	}
	if (player[player_id].speed_y > 0)
	{
		if (player[player_id].order_y == UP)
		{
			if (player[player_id].pos_y > min_y_coord[player_id])
			{
				player[player_id].pos_y -= player[player_id].speed_y;
			}
			/* else if (player[player_id].pos_y <= min_y_coord[player_id])
			{
				player[player_id].pos_y += player[player_id].speed_y;
			} */
		}
		else if (player[player_id].order_y == DOWN)
		{
			if (player[player_id].pos_y < max_y_coord[player_id])
			{
				player[player_id].pos_y += player[player_id].speed_y;
			}
			/* else if (player[player_id].pos_y < max_y_coord[player_id])
			{
				player[player_id].pos_y -= player[player_id].speed_y;
			} */
		}
	}
}

static void controlHorizontalFlip(int player_id)
{
	if (player[player_id].order_x == RIGHT)
	{
		player[player_id].flip_h = FALSE;
	}
	else if (player[player_id].order_x == LEFT)
	{
		player[player_id].flip_h = TRUE;
	}
}

static void controlVerticalFlip(int player_id)
{
	if (player[player_id].order_y == UP)
	{
		player[player_id].flip_v = FALSE;
	}
	else if (player[player_id].order_y == DOWN)
	{
		player[player_id].flip_v = TRUE;
	}
	else
	{
		player[player_id].flip_v = player[player_id].flip_v;
	}
}

static void controlEffects(int player_id)
{
}

static void controlAttackTimer(int player_id)
{
	if (player[player_id].is_attacking)
	{
		if (attack_timer[player_id] < anim_duration[player_id])
			attack_timer[player_id]++;
		else
		{
			attack_timer[player_id] = 0;
			player[player_id].is_attacking = FALSE;
		}
	}
}

static void controlIdleTimer(int player_id)
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

static void controlMapBoundaries(int player_id)
{
	if ((player[player_id].pos_x + BOX_LEFT_OFFSET) == MIN_POS_X)
	{
		player[player_id].pos_x = (MIN_POS_X - BOX_LEFT_OFFSET);
	}
	else if (((player[player_id].pos_x + player[player_id].width) - BOX_RIGHT_OFFSET) > MAX_POS_X)
	{
		player[player_id].pos_x = (MAX_POS_X - player[player_id].width) + BOX_RIGHT_OFFSET;
	}

	if ((player[player_id].pos_y + BOX_TOP_OFFSET) == MIN_POS_Y)
	{
		player[player_id].pos_y = (MIN_POS_Y - BOX_TOP_OFFSET);
	}
	else if ((player[player_id].pos_y + player[player_id].height) > MAX_POS_Y)
	{
		player[player_id].pos_y = (MAX_POS_Y - player[player_id].height);
	}
}

static void updateCamera(int player_id)
{
	player_x_on_map = player[player_id].pos_x;
	player_y_on_map = player[player_id].pos_y;
	player_x_on_screen = (player_x_on_map - camera.cur_pos_x);
	player_y_on_screen = (player_y_on_map - camera.cur_pos_y);

	if (player_x_on_screen > HOW_FAR_TO_RIGHT)
	{
		update_camera_x = (player_x_on_map - HOW_FAR_TO_RIGHT);
	}
	else if (player_x_on_screen < HOW_FAR_TO_LEFT)
	{
		update_camera_x = (player_x_on_map - HOW_FAR_TO_LEFT);
	}
	else
	{
		update_camera_x = camera.cur_pos_x;
	}

	if (player_y_on_screen > HOW_FAR_TO_BOTTON)
	{
		update_camera_y = (player_y_on_map - HOW_FAR_TO_BOTTON);
	}
	else if (player_y_on_screen < HOW_FAR_TO_TOP)
	{
		update_camera_y = (player_y_on_map - HOW_FAR_TO_TOP);
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

static void controlPlayerMapCollision(int player_id)
{
	//	#define MIN_POS_X 0 - BOX_LEFT_OFFSET
	//	#define MAX_POS_X MAP_WIDTH - PLAYER_1_WIDTH + BOX_RIGHT_OFFSET
	//	#define MIN_POS_Y 0 - BOX_TOP_OFFSET
	//	#define MAX_POS_Y MAP_HEIGTH - PLAYER_1_HEIGTH + BOX_BOTTON_OFFSET
	// define a caixa de colisão do personagem
	int left_edge = (player[player_id].pos_x + BOX_LEFT_OFFSET);
	int right_edge = (player[player_id].pos_x + player[player_id].width) - BOX_RIGHT_OFFSET;
	int top_edge = (player[player_id].pos_y + BOX_TOP_OFFSET);
	int botton_edge = (player[player_id].pos_y + player[player_id].height);

	// arestas:
	// verifica em qual indice da matriz cada aresta está colidindo
	int left_edge_column_index = (left_edge / TILE_IN_PIXELS);
	int right_edge_column_index = (right_edge / TILE_IN_PIXELS);
	int top_edge_line_index = (top_edge / TILE_IN_PIXELS);
	int botton_edge_line_index = (botton_edge / TILE_IN_PIXELS);

	if (right_edge_column_index > MATRIX_MAX_COL_INDEX)
		right_edge_column_index = MATRIX_MAX_COL_INDEX;

	if (botton_edge_line_index > MATRIX_MAX_LIN_INDEX)
		botton_edge_line_index = MATRIX_MAX_LIN_INDEX;

	// vértices: cruzamentos de linhas com colunas
	int top_left_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index][left_edge_column_index];
	int left_top_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index + 1][left_edge_column_index];

	int top_right_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index][right_edge_column_index];
	int right_top_tile_collision_type = BGA_COLLISION_MATRIX[top_edge_line_index + 1][right_edge_column_index];

	int botton_left_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index][left_edge_column_index];
	int left_botton_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index - 1][left_edge_column_index];

	int botton_right_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index][right_edge_column_index];
	int right_botton_tile_collision_type = BGA_COLLISION_MATRIX[botton_edge_line_index - 1][right_edge_column_index];

	if (player[player_id].order_y == UP)
	{
		if (top_left_tile_collision_type == SOLID_TILE || top_right_tile_collision_type == SOLID_TILE)
		{
			min_y_coord[player_id] = (top_edge_line_index * TILE_IN_PIXELS) + (TILE_IN_PIXELS - BOX_TOP_OFFSET);
		}
		else
		{
			min_y_coord[player_id] = (MIN_POS_Y - BOX_TOP_OFFSET);
		}
	}

	if (player[player_id].order_y == DOWN || player[player_id].order_y == NEUTRAL)
	{
		if (botton_left_tile_collision_type == SOLID_TILE || botton_right_tile_collision_type == SOLID_TILE)
		{
			max_y_coord[player_id] = (botton_edge_line_index * TILE_IN_PIXELS) - player[player_id].height;
		}
		else
		{
			max_y_coord[player_id] = (MAX_POS_Y - player[player_id].height);
		}
	}

	if (player[player_id].last_order_x == LEFT)
	{
		if (left_top_tile_collision_type == SOLID_TILE || left_botton_tile_collision_type == SOLID_TILE)
		{
			min_x_coord[player_id] = (left_edge_column_index * TILE_IN_PIXELS) + TILE_IN_PIXELS - BOX_LEFT_OFFSET;
		}
		else
		{
			min_x_coord[player_id] = (MIN_POS_X - BOX_LEFT_OFFSET);
		}
	}

	if (player[player_id].last_order_x == RIGHT)
	{
		if (right_top_tile_collision_type == SOLID_TILE || right_botton_tile_collision_type == SOLID_TILE)
		{
			max_x_coord[player_id] = ((right_edge_column_index * TILE_IN_PIXELS) - player[player_id].width) + BOX_RIGHT_OFFSET;
		}
		else
		{
			max_x_coord[player_id] = (MAX_POS_X - player[player_id].width) + BOX_RIGHT_OFFSET;
		}
	}

	//sprintf(top_buffer, "Px:%d", player[player_id].pos_x /* min_y_coord[player_id] */);
	//sprintf(botton_buffer, "Pstt:%d", player[player_id].state /* max_y_coord[player_id] */);
	//sprintf(left_buffer, "B.L.C.T:%d", botton_left_tile_collision_type /* min_x_coord[player_id] */);
	//sprintf(right_buffer, "B.R.C.T:%d", botton_right_tile_collision_type /* max_x_coord[player_id] */);		
	//VDP_clearTextBG(BG_A, 28, 5, 10);
	//VDP_clearTextBG(BG_A, 28, 6, 10);
	//VDP_clearTextBG(BG_A, 28, 7, 10);
	//VDP_clearTextBG(BG_A, 28, 8, 10);	
	//VDP_drawTextBG(BG_A, top_buffer, 28, 5);
	//VDP_drawTextBG(BG_A, botton_buffer, 28, 6);
	//VDP_drawTextBG(BG_A, left_buffer, 28, 7);
	//VDP_drawTextBG(BG_A, right_buffer, 28, 8);
}

static void checkBottonCollision(int player_id)
{
	if ((player[player_id].order_y == NEUTRAL) && (player[player_id].pos_y < max_y_coord[player_id]))
	{
		player[player_id].order_y = DOWN;
	}
	else if ((player[player_id].order_y == DOWN) && (player[player_id].pos_y >= max_y_coord[player_id]))
	{
		player[player_id].order_y = NEUTRAL;
		player[player_id].pos_y = max_y_coord[player_id];
	}
}

static void checkTopCollision(int player_id)
{
	if ((player[player_id].order_y == UP) && (player[player_id].pos_y <= min_y_coord[player_id]))
	{
		player[player_id].order_y = NEUTRAL;
	}
}