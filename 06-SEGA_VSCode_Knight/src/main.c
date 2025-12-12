#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html
// https://www.youtube.com/watch?v=xldCreKST3s
// OHSAT Games Tutorials
// https://www.ohsat.com/tutorial/mdmisc/sor-style-text-crawl/
// PIGSY Retro Game Sega Genesis Tutotials
// https://www.youtube.com/watch?v=BnGqc5OTTY4&list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb

#define ANIM_STANDING 0
#define ANIM_CROUCH 1
#define ANIM_ATTACK_SIDE 2
#define ANIM_ATTACK_CROUCH 3
#define ANIM_ATTACK_UP 4
#define ANIM_JUMP_ATTACK 5
#define ANIM_JUMP 6
#define ANIM_RUN 7
#define ANIM_SWORD_SLASH 8
#define ANIM_CROUCH_SWORD_SLASH 9
#define ANIM_PLAYER_HURT 10
#define ANIM_AIR_SWORD_SLASH 11
#define ANIM_CLIMB_LEDGE 12

#define COUNTER_LIMIT 5
// 320 X 224
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224
//  How far to (left, right, up, down) before camera moves
#define HOW_FAR_TO_LEFT 10
#define HOW_FAR_TO_RIGHT 152
#define HOW_FAR_TO_TOP 10
#define HOW_FAR_TO_BOTTON 96
// map
#define MAP_WIDTH 1024
#define MAP_HEIGTH 224
// screen
#define LEFT_SCREEN_LIMIT 0
#define RIGHT_SCREEN_LIMIT MAP_WIDTH - HORIZONTAL_RESOLUTION
#define TOP_SCREEN_LIMIT 0
#define BOTTON_SCREEN_LIMIT MAP_HEIGTH - VERTICAL_RESOLUTION
// player
#define PLAYER_WIDTH 128
#define PLAYER_HEIGTH 96
#define MIN_POS_X 0
#define MAX_POS_X MAP_WIDTH - PLAYER_WIDTH
#define MIN_POS_Y 0
#define MAX_POS_Y MAP_HEIGTH - PLAYER_HEIGTH

enum GameState
{
	MENU,
	GAME,
};
enum GameState current_game_state;
bool game_on = FALSE;

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

// player collision box
#define BOX_LEFT_EDGE 16
#define BOX_RIGHT_EDGE 32
#define BOX_TOP_EDGE 16
#define BOX_BOTTON_EDGE 32

typedef struct
{
	bool top_left;
	bool top_right;
	bool botton_left;
	bool botton_right;
} CollisionBox;

CollisionBox collision_from[2];

s16 blocked_left_coord[2] = {0, 0};
s16 blocked_right_coord[2] = {320, 320};
s16 blocked_top_coord[2] = {0, 0};
s16 blocked_botton_coord[2] = {224, 224};

#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
const u8 BGA_COLLISION_ARRAY_LENGTH = 20;
// collision for foreground (BGA)
const u8 BGA_COLLISION_ARRAY[280] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

typedef struct
{
	s16 h_scroll_offset_bg;
	s16 v_scroll_offset_bg;
	u16 pos_x;
	u16 pos_y;
} Background;
#define A 0
#define B 1

enum PlayerState
{
	STATE_STANDING,
	STATE_CROUCH,
	STATE_GET_UP,
	STATE_ATTACK_SIDE,
	STATE_ATTACK_CROUCH,
	STATE_ATTACK_UP,
	STATE_JUMP_ATTACK,
	STATE_JUMP,
	STATE_FALL,
	STATE_RUN,
	STATE_SWORD_SLASH,
	STATE_CROUCH_SWORD_SLASH,
	STATE_PLAYER_HURT,
	STATE_AIR_SWORD_SLASH,
	STATE_CLIMB_LEDGE
};

// player sprite array numOfPlayer
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
	bool flip_h;
	bool flip_v;
	s16 anim;
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
	bool attacking;
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

s16 ground_position[2] = {96, 96};
s16 counter_x[2] = {0, 0};
s16 counter_y[2] = {0, 0};

enum AttackDuration
{
	CROUCH = 20,
	GET_UP = 20,
	ATTACK_SIDE = 20,
	ATTACK_CROUCH = 30,
	ATTACK_UP = 30,
	JUMP_ATTACK = 30,
	JUMP = 40,
	FALL = 40,
	// RUN = 120,
	SWORD_SLASH = 60,
	CROUCH_SWORD_SLASH = 40,
	PLAYER_HURT = 30,
	AIR_SWORD_SLASH = 60,
	CLIMB_LEDGE = 50
};

u16 attack_timer[2] = {0, 0};
u16 effect_timer[2] = {0, 0};
u16 attack_duration[2] = {0, 0}; // animation _speed * num of frames of animation

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

u16 ind = TILE_USER_INDEX;
Map *bga;
Background bg[2]; // BGA & BGB
Camera camera;
Player player[2];
SpriteElement spr_element[10];
Joystick joystick[2];

static void inputHandler(u16 joy, u16 changed, u16 state);
static void finiteStateMachine(int numOfPlayer);
static void updatePlayerPosition(int numOfPlayer);
static void controlEffects(int numOfPlayer);

/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAttackTimer(int numOfPlayer);

static void controlXAcceleration(int numOfPlayer);
static void controlYAcceleration(int numOfPlayer);

// FLIP
static void controlHorizontalFlip(int numOfPlayer);
static void controlVerticalFlip(int numOfPlayer);

// COLLISION
static void controlPlayerCollision(int numOfPlayer);

// BOUNDARIES
static void controlMapBoundaries(int numOfPlayer);
static void updateCamera(int numOfPlayer);

int main()
{
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();

	SPR_init();
	JOY_init();
	JOY_setEventHandler(&inputHandler);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	// init
	// BGA (MAP)
	camera.cur_pos_x = 0;
	camera.cur_pos_y = 0;
	VDP_loadTileSet(&level_tileset, ind, DMA);
	PAL_setPalette(PAL0, bga_palette.data, DMA);
	bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
	ind += level_tileset.numTile;

	// BGB
	/* bg[B].h_scroll_offset_bg = 0;
	bg[B].v_scroll_offset_bg = 0;
	bg[B].pos_x = 0;
	bg[B].pos_y = 0;
	PAL_setPalette(PAL0, background.palette->data, DMA);
	VDP_drawImageEx(BG_B, &background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), bg[B].pos_x, bg[B].pos_y, FALSE, TRUE);
	ind += background.tileset->numTile; */

	// Player
	player[ONE].anim = ANIM_STANDING;
	player[ONE].is_full_anim = TRUE;
	player[ONE].flip_h = FALSE;
	player[ONE].flip_v = FALSE;
	player[ONE].impulse_x = 0;
	player[ONE].impulse_y = 4;
	player[ONE].max_speed_x = 2;
	player[ONE].max_speed_y = 4;
	player[ONE].order_x = NEUTRAL;
	player[ONE].order_y = DOWN;
	player[ONE].last_order_x = NEUTRAL;
	player[ONE].last_order_y = NEUTRAL;
	player[ONE].pos_x = HOW_FAR_TO_LEFT;
	player[ONE].pos_y = HOW_FAR_TO_BOTTON;
	player[ONE].speed_x = 0;
	player[ONE].speed_y = 0;
	player[ONE].state = STATE_STANDING;
	player[ONE].attacking = FALSE;
	player[ONE].has_stamina = TRUE;
	player[ONE].sprite = SPR_addSprite(&knight, player[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL2, FALSE, player[ONE].flip_v, player[ONE].flip_h));
	PAL_setPalette(PAL2, knight.palette->data, DMA);
	SPR_setAnim(player[ONE].sprite, player[ONE].anim);

	spr_element[ONE].offset_x = 32;
	PAL_setPalette(PAL3, dagger.palette->data, DMA);
	spr_element[ONE].pos_x = player[ONE].flip_h ? (player[ONE].pos_x + PLAYER_WIDTH) : (player[ONE].pos_x - spr_element[ONE].offset_x);
	spr_element[ONE].pos_y = player[ONE].pos_y;
	spr_element[ONE].sprite = SPR_addSprite(&dagger, spr_element[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL3, TRUE, player[ONE].flip_v, player[ONE].flip_h));
	SPR_setAnim(spr_element[ONE].sprite, 0);
	SPR_setVisibility(spr_element[ONE].sprite, VISIBLE);

	// DMA_setBufferSize(9000);

	while (1)
	{
		// VDP_waitVSync();
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
		// controlPlayerCollision(ONE);
		if (player[ONE].is_full_anim)
		{
			SPR_setAnim(player[ONE].sprite, player[ONE].anim);
		}
		else
		{
			SPR_setAnimAndFrame(player[ONE].sprite, player[ONE].anim, player[ONE].frame);
		}

		SPR_setHFlip(player[ONE].sprite, player[ONE].flip_h);

		// SPR_setPosition(player[ONE].sprite, (player[ONE].pos_x - update_camera_x), player[ONE].pos_y);
		// MAP_scrollTo(bga, update_camera_x, update_camera_y);
		SPR_setPosition(player[ONE].sprite, (player[ONE].pos_x - camera.cur_pos_x), (player[ONE].pos_y - camera.cur_pos_y));
		MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);

		spr_element[ONE].offset_x = 32;
		spr_element[ONE].pos_x = player[ONE].flip_h ? (player[ONE].pos_x + PLAYER_WIDTH) : (player[ONE].pos_x - spr_element[ONE].offset_x);
		spr_element[ONE].pos_y = player[ONE].pos_y;
		SPR_setAnim(spr_element[ONE].sprite, 0);
		SPR_setHFlip(spr_element[ONE].sprite, player[ONE].flip_h);
		SPR_setPosition(spr_element[ONE].sprite, spr_element[ONE].pos_x, spr_element[ONE].pos_y);
		SPR_setVisibility(spr_element[ONE].sprite, VISIBLE);

		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	return (0);
}

static void inputHandler(u16 joy, u16 changed, u16 state)
{
	/*
	JOY_update() refreshes joypad status (called every screen refresh so don't call it yourself unless you really need it)
	JOY_readJoypad( joy ) gives you the joystick[ONE] status whenever you need it
	JOY_waitPressBtn() waits for any buttons to be pressed (not directions)
	JOY_waitPress(joy, BUTTON_A | BUTTON_UP) waits for wanted buttons to be pressed on specific joypad
	*/
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
			joystick[ONE].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			joystick[ONE].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			joystick[ONE].btn_down = TRUE;
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
			joystick[ONE].btn_start = TRUE;
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

static void finiteStateMachine(int numOfPlayer)
{
	switch (player[numOfPlayer].state)
	{
	case STATE_STANDING:
		if (joystick[numOfPlayer].btn_left)
		{
			player[numOfPlayer].order_x = LEFT;
			player[numOfPlayer].last_order_x = LEFT;
			player[numOfPlayer].state = STATE_RUN;
			player[numOfPlayer].anim = ANIM_RUN;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		else if (joystick[numOfPlayer].btn_right)
		{
			player[numOfPlayer].order_x = RIGHT;
			player[numOfPlayer].last_order_x = RIGHT;
			player[numOfPlayer].state = STATE_RUN;
			player[numOfPlayer].anim = ANIM_RUN;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (joystick[numOfPlayer].btn_down)
		{
			attack_duration[numOfPlayer] = CROUCH;
			player[numOfPlayer].state = STATE_CROUCH;
			player[numOfPlayer].anim = ANIM_CROUCH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
		}

		if (joystick[numOfPlayer].btn_a /*  && player[numOfPlayer].has_stamina */)
		{
			// player[numOfPlayer].has_stamina = FALSE;
			player[numOfPlayer].state = STATE_JUMP;
			player[numOfPlayer].anim = ANIM_JUMP;
			player[numOfPlayer].order_y = UP;
			if (player[numOfPlayer].speed_y == 0)
			{
				player[numOfPlayer].speed_y += player[numOfPlayer].impulse_y;
			}
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
		}

		if (joystick[numOfPlayer].btn_b /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = ATTACK_SIDE;
			player[numOfPlayer].state = STATE_ATTACK_SIDE;
			player[numOfPlayer].anim = ANIM_ATTACK_SIDE;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}

		if (joystick[numOfPlayer].btn_x /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = SWORD_SLASH;
			player[numOfPlayer].state = STATE_SWORD_SLASH;
			player[numOfPlayer].anim = ANIM_SWORD_SLASH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}

		if (joystick[numOfPlayer].btn_y /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = ATTACK_UP;
			player[numOfPlayer].state = STATE_ATTACK_UP;
			player[numOfPlayer].anim = ANIM_ATTACK_UP;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_RUN:
		if (joystick[numOfPlayer].btn_a /* && player[numOfPlayer].has_stamina */)
		{
			// player[numOfPlayer].has_stamina = FALSE;
			player[numOfPlayer].state = STATE_JUMP;
			player[numOfPlayer].anim = ANIM_JUMP;
			player[numOfPlayer].order_y = UP;
			if (player[numOfPlayer].speed_y == 0)
			{
				player[numOfPlayer].speed_y += player[numOfPlayer].impulse_y;
			}
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (joystick[numOfPlayer].btn_b /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = ATTACK_SIDE;
			player[numOfPlayer].state = STATE_ATTACK_SIDE;
			player[numOfPlayer].anim = ANIM_ATTACK_SIDE;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (joystick[numOfPlayer].btn_x /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = SWORD_SLASH;
			player[numOfPlayer].state = STATE_SWORD_SLASH;
			player[numOfPlayer].anim = ANIM_SWORD_SLASH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (joystick[numOfPlayer].btn_y)
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = ATTACK_UP;
			player[numOfPlayer].state = STATE_ATTACK_UP;
			player[numOfPlayer].anim = ANIM_ATTACK_UP;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (!(joystick[numOfPlayer].btn_left) && !(joystick[numOfPlayer].btn_right))
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].state = STATE_STANDING;
			player[numOfPlayer].anim = ANIM_STANDING;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_CROUCH:
		if (joystick[numOfPlayer].btn_x /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = CROUCH_SWORD_SLASH;
			player[numOfPlayer].state = STATE_CROUCH_SWORD_SLASH;
			player[numOfPlayer].anim = ANIM_CROUCH_SWORD_SLASH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (joystick[numOfPlayer].btn_b /*  && player[numOfPlayer].has_stamina */)
		{
			player[numOfPlayer].attacking = TRUE;
			// player[numOfPlayer].has_stamina = FALSE;
			attack_duration[numOfPlayer] = ATTACK_CROUCH;
			player[numOfPlayer].state = STATE_ATTACK_CROUCH;
			player[numOfPlayer].anim = ANIM_ATTACK_CROUCH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		if (!joystick[numOfPlayer].btn_down)
		{
			player[numOfPlayer].state = STATE_STANDING;
			player[numOfPlayer].anim = ANIM_STANDING;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_CROUCH_SWORD_SLASH:
		if (!joystick[numOfPlayer].btn_x)
		{
			if (!player[numOfPlayer].attacking)
			{
				player[numOfPlayer].state = STATE_CROUCH;
				player[numOfPlayer].anim = ANIM_CROUCH;
				player[numOfPlayer].is_full_anim = FALSE;
				player[numOfPlayer].frame = 1;
				SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
			}
		}
		break;

	case STATE_ATTACK_CROUCH:
		if (!joystick[numOfPlayer].btn_b)
		{
			if (!player[numOfPlayer].attacking)
			{
				player[numOfPlayer].state = STATE_CROUCH;
				player[numOfPlayer].anim = ANIM_CROUCH;
				player[numOfPlayer].is_full_anim = FALSE;
				player[numOfPlayer].frame = 1;
				SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
			}
		}
		break;

	case STATE_JUMP:
		if (joystick[numOfPlayer].btn_b)
		{
			player[numOfPlayer].state = STATE_JUMP_ATTACK;
			player[numOfPlayer].anim = ANIM_JUMP_ATTACK;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
		}
		if (joystick[numOfPlayer].btn_x)
		{
			player[numOfPlayer].state = STATE_AIR_SWORD_SLASH;
			player[numOfPlayer].anim = ANIM_AIR_SWORD_SLASH;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, FALSE);
		}
		if (joystick[numOfPlayer].btn_left && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = LEFT;
			player[numOfPlayer].last_order_x = LEFT;
		}
		else if (joystick[numOfPlayer].btn_right && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = RIGHT;
			player[numOfPlayer].last_order_x = RIGHT;
		}
		else if (!(joystick[numOfPlayer].btn_left) && !(joystick[numOfPlayer].btn_right) && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
		}
		else if ((player[numOfPlayer].order_y == NEUTRAL) && (player[numOfPlayer].pos_y == ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].state = STATE_STANDING;
			player[numOfPlayer].anim = ANIM_STANDING;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_JUMP_ATTACK:
		if (joystick[numOfPlayer].btn_left && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = LEFT;
			player[numOfPlayer].last_order_x = LEFT;
		}
		else if (joystick[numOfPlayer].btn_right && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = RIGHT;
			player[numOfPlayer].last_order_x = RIGHT;
		}
		else if (!(joystick[numOfPlayer].btn_left) && !(joystick[numOfPlayer].btn_right) && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
		}
		else if ((player[numOfPlayer].order_y == NEUTRAL) && (player[numOfPlayer].pos_y == ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].state = STATE_STANDING;
			player[numOfPlayer].anim = ANIM_STANDING;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_SIDE:
		if (!joystick[numOfPlayer].btn_b)
		{
			if (!player[numOfPlayer].attacking)
			{
				// player[numOfPlayer].has_stamina = TRUE;
				player[numOfPlayer].state = STATE_STANDING;
				player[numOfPlayer].anim = ANIM_STANDING;
				player[numOfPlayer].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
			}
		}
		break;

	case STATE_AIR_SWORD_SLASH:
		if (joystick[numOfPlayer].btn_left && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = LEFT;
			player[numOfPlayer].last_order_x = LEFT;
		}
		else if (joystick[numOfPlayer].btn_right && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = RIGHT;
			player[numOfPlayer].last_order_x = RIGHT;
		}
		else if (!(joystick[numOfPlayer].btn_left) && !(joystick[numOfPlayer].btn_right) && (player[numOfPlayer].pos_y != ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
		}
		else if ((player[numOfPlayer].order_y == NEUTRAL) && (player[numOfPlayer].pos_y == ground_position[numOfPlayer]))
		{
			player[numOfPlayer].order_x = NEUTRAL;
			player[numOfPlayer].state = STATE_STANDING;
			player[numOfPlayer].anim = ANIM_STANDING;
			player[numOfPlayer].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
		}
		break;

	case STATE_SWORD_SLASH:
		if (!joystick[numOfPlayer].btn_x)
		{
			if (!player[numOfPlayer].attacking)
			{
				// player[numOfPlayer].has_stamina = TRUE;
				player[numOfPlayer].state = STATE_STANDING;
				player[numOfPlayer].anim = ANIM_STANDING;
				player[numOfPlayer].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_UP:
		if (!(joystick[numOfPlayer].btn_y))
		{
			if (!player[numOfPlayer].attacking)
			{
				// player[numOfPlayer].has_stamina = TRUE;
				player[numOfPlayer].state = STATE_STANDING;
				player[numOfPlayer].anim = ANIM_STANDING;
				player[numOfPlayer].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[numOfPlayer].sprite, TRUE);
			}
		}
		break;

	default:
		break;
	}
}

static void controlXAcceleration(int numOfPlayer)
{
	if (player[numOfPlayer].order_x != NEUTRAL)
	{
		if (player[numOfPlayer].speed_x < player[numOfPlayer].max_speed_x)
		{
			counter_x[numOfPlayer] += 1;
			if (counter_x[numOfPlayer] == COUNTER_LIMIT)
			{
				counter_x[numOfPlayer] = 0;
				player[numOfPlayer].speed_x += 1;
			}
		}
	}
	else if (player[numOfPlayer].order_x == NEUTRAL)
	{
		if (player[numOfPlayer].speed_x > 0)
		{
			counter_x[numOfPlayer] += 1;
			if (counter_x[numOfPlayer] == COUNTER_LIMIT)
			{
				counter_x[numOfPlayer] = 0;
				player[numOfPlayer].speed_x -= 1;
			}
		}
	}
}

static void controlYAcceleration(int numOfPlayer)
{
	if (player[numOfPlayer].order_y == UP)
	{
		if (player[numOfPlayer].speed_y > 0)
		{
			counter_y[numOfPlayer] += 1;
			if (counter_y[numOfPlayer] == COUNTER_LIMIT)
			{
				counter_y[numOfPlayer] = 0;
				player[numOfPlayer].speed_y -= 1;
			}
		}
		else if (player[numOfPlayer].speed_y <= 0)
		{
			player[numOfPlayer].order_y = DOWN;
			player[numOfPlayer].speed_y = 0;
		}
	}
	else if (player[numOfPlayer].order_y == DOWN)
	{
		if (player[numOfPlayer].pos_y < ground_position[numOfPlayer])
		{
			counter_y[numOfPlayer] += 1;
			if (counter_y[numOfPlayer] == COUNTER_LIMIT)
			{
				counter_y[numOfPlayer] = 0;
				if (player[numOfPlayer].speed_y < player[numOfPlayer].max_speed_y)
				{
					player[numOfPlayer].speed_y += 1;
				}
			}
		}
		else if (player[numOfPlayer].pos_y >= ground_position[numOfPlayer])
		{
			counter_y[numOfPlayer] = 0;
			player[numOfPlayer].order_y = NEUTRAL;
		}
	}
	else if (player[numOfPlayer].order_y == NEUTRAL)
	{
		player[numOfPlayer].speed_y = 0;
		player[numOfPlayer].pos_y = ground_position[numOfPlayer];
	}
}

static void updatePlayerPosition(int numOfPlayer)
{
	if (player[numOfPlayer].speed_x > 0)
	{
		if (player[numOfPlayer].last_order_x == LEFT)
		{
			player[numOfPlayer].pos_x -= player[numOfPlayer].speed_x;
		}
		else if (player[numOfPlayer].last_order_x == RIGHT)
		{
			player[numOfPlayer].pos_x += player[numOfPlayer].speed_x;
		}
	}

	if (player[numOfPlayer].speed_y > 0)
	{
		if (player[numOfPlayer].order_y == UP)
		{
			player[numOfPlayer].pos_y -= player[numOfPlayer].speed_y;
		}
		else if (player[numOfPlayer].order_y == DOWN)
		{
			player[numOfPlayer].pos_y += player[numOfPlayer].speed_y;
		}
	}
}

static void controlHorizontalFlip(int numOfPlayer)
{
	if (player[numOfPlayer].order_x == RIGHT)
	{
		player[numOfPlayer].flip_h = FALSE;
	}
	else if (player[numOfPlayer].order_x == LEFT)
	{
		player[numOfPlayer].flip_h = TRUE;
	}
}

static void controlVerticalFlip(int numOfPlayer)
{
	if (player[numOfPlayer].order_y == UP)
	{
		player[numOfPlayer].flip_v = FALSE;
	}
	else if (player[numOfPlayer].order_y == DOWN)
	{
		player[numOfPlayer].flip_v = TRUE;
	}
	else
	{
		player[numOfPlayer].flip_v = player[numOfPlayer].flip_v;
	}
}

static void controlEffects(int numOfPlayer)
{
}

static void controlAttackTimer(int numOfPlayer)
{
	if (player[numOfPlayer].attacking)
	{
		if (attack_timer[numOfPlayer] < attack_duration[numOfPlayer])
			attack_timer[numOfPlayer]++;
		else
		{
			attack_timer[numOfPlayer] = 0;
			player[numOfPlayer].attacking = FALSE;
		}
	}
}

static void controlIdleTimer(int numOfPlayer)
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

static void controlMapBoundaries(int numOfPlayer)
{
	if (player[numOfPlayer].pos_x < HOW_FAR_TO_LEFT)
	{
		player[numOfPlayer].pos_x = HOW_FAR_TO_LEFT;
	}
	else if (player[numOfPlayer].pos_x > MAX_POS_X)
	{
		player[numOfPlayer].pos_x = MAX_POS_X;
	}

	if (player[numOfPlayer].pos_y < HOW_FAR_TO_TOP)
	{
		player[numOfPlayer].pos_y = HOW_FAR_TO_TOP;
	}
	else if (player[numOfPlayer].pos_y > MAX_POS_Y)
	{
		player[numOfPlayer].pos_y = MAX_POS_Y;
	}
}

static void updateCamera(int numOfPlayer)
{
	player_x_on_map = player[numOfPlayer].pos_x;
	player_y_on_map = player[numOfPlayer].pos_y;
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

static void controlPlayerCollision(int numOfPlayer)
{
	// s16 left_edge_collision_box = player_pos_x[player]; /*  + BOX_LEFT_EDGE; */
	// s16 right_edge_collision_box = player_pos_x[player] + PLAYER_WIDTH;
	// s16 top_edge_collision_box = player_pos_y[player]; /*  + BOX_TOP_EDGE; */
	// s16 botton_edge_collision_box = player_pos_y[player] + PLAYER_HEIGTH;
	//
	// s16 colbox_left_column_index = left_edge_collision_box >> 4; //(same as '/16')
	// s16 colbox_right_column_index = right_edge_collision_box >> 4;
	// s16 colbox_top_line_index = top_edge_collision_box >> 4;
	// s16 colbox_botton_line_index = botton_edge_collision_box >> 4;
	//// edge
	//// vertex
	// u16 top_left_colbox_vertex_array_index = colbox_left_column_index + (colbox_top_line_index * BGA_COLLISION_ARRAY_LENGTH);
	// u16 top_right_colbox_vertex_array_index = colbox_right_column_index + (colbox_top_line_index * BGA_COLLISION_ARRAY_LENGTH);
	// u16 botton_left_colbox_vertex_array_index = colbox_left_column_index + (colbox_botton_line_index * BGA_COLLISION_ARRAY_LENGTH);
	// u16 botton_right_colbox_vertex_array_index = colbox_right_column_index + (colbox_botton_line_index * BGA_COLLISION_ARRAY_LENGTH);
	//
	// u8 top_left_tile_collision_type = BGA_COLLISION_ARRAY[top_left_colbox_vertex_array_index];
	// u8 top_right_tile_collision_type = BGA_COLLISION_ARRAY[top_right_colbox_vertex_array_index];
	// u8 botton_left_tile_collision_type = BGA_COLLISION_ARRAY[botton_left_colbox_vertex_array_index];
	// u8 botton_right_tile_collision_type = BGA_COLLISION_ARRAY[botton_right_colbox_vertex_array_index];
	//
	// if (player_order_x[player] < 0)
	//{
	//	collision_from[player].top_left = top_left_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
	//	if (collision_from[player].top_left)
	//	{
	//		// blocked_right_coord[player] = 320;
	//		blocked_left_coord[player] = (colbox_left_column_index << 4); // equals to 'left_edge_collision_box'
	//	}
	//}
	// else if (player_order_x[player] > 0)
	//{
	//	collision_from[player].top_right = top_right_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
	//	if (collision_from[player].top_right)
	//	{
	//		// blocked_left_coord[player] = 0;
	//		blocked_right_coord[player] = (colbox_right_column_index << 4); // equals to 'right_edge_collision_box'
	//	}
	//}
	/* else if (player_order_y[player] < 0)
	{
		collision_from[player].botton_left = botton_left_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
		if (collision_from[player].botton_left)
		{
			blocked_top_coord[player] = (colbox_top_line_index << 4) + 16 - (s16)BOX_TOP_EDGE;
		}
	}
	else if (player_order_y[player] > 0)
	{
		collision_from[player].botton_right = botton_right_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
		if (collision_from[player].botton_right)
		{
			blocked_botton_coord[player] = (colbox_botton_line_index << 4) + 16 - (s16)BOX_BOTTON_EDGE;
		}
	} */
	// sprintf(info, "%10i", top_right_tile_collision_type);
	// VDP_drawTextBG(BG_A, info, 28, 5);
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
//// Set the current animation (e.g., animation numOfPlayer 0)
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