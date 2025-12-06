#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html

#define ANIM_STANDING 0
#define ANIM_ROLL 1
#define ANIM_DUCK_INTERMEDIATE 2
#define ANIM_DUCK 3
#define ANIM_DUCK_WALK 4
#define ANIM_JUMP 5
#define ANIM_JUMP_MIDDLE 6
#define ANIM_FALL 7
#define ANIM_DASH 8
#define ANIM_SLIDE_START 9
#define ANIM_SLIDE 10
#define ANIM_RUN 11
#define ANIM_TURN 12
#define ANIM_WALL_SLIDE 13
#define ANIM_CLIMB_WALL_START 14
#define ANIM_CLIMB_WALL 15
#define ANIM_DUCK_ATTACK 16
#define ANIM_VERTICAL_ATTACK 17
#define ANIM_HORIZONTAL_ATTACK 18

// player sprite array index
#define PLAYER_1 0
#define PLAYER_2 1
// effect sprite array index
#define EFFECT_1 0
#define EFFECT_2 1
#define COUNTER_LIMIT 5

// How far to (left, right, up, down) before camera moves
#define HOW_FAR_TO_LEFT 64
#define HOW_FAR_TO_RIGHT 128
#define HOW_FAR_TO_TOP 56
#define HOW_FAR_TO_BOTTON 112

#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224

/* #define MAP_WIDTH 10240
#define MAP_HEIGTH 1280 */
/* #define MAP_WIDTH 5120
#define MAP_HEIGTH 256 */
#define MAP_WIDTH 320
#define MAP_HEIGTH 224

#define PLAYER_WIDTH 72
#define PLAYER_HEIGTH 72
// player collision box
#define BOX_LEFT_EDGE 16
#define BOX_RIGHT_EDGE 32
#define BOX_TOP_EDGE 16
#define BOX_BOTTON_EDGE 32

struct Collision
{
	bool top_left;
	bool top_right;
	bool botton_left;
	bool botton_right;
};
struct Collision collision_from[2];

// s16 blocked_left_coord[2] = {32, 32};
// s16 blocked_right_coord[2] = {288, 288};
s16 blocked_left_coord[2] = {0, 0};
s16 blocked_right_coord[2] = {320, 320};
s16 blocked_top_coord[2] = {0, 0};
s16 blocked_botton_coord[2] = {224, 224};

Map *level_1_map;
u16 ind = TILE_USER_INDEX;

const u8 BGA_COLLISION_ARRAY_LENGTH = 20;
#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
char info[10];

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

// background(BGB)
s16 hscroll_offset_bg = 0;
s16 vscroll_offset_bg = 0;
u16 background_x = 0;
u16 background_y = 0;

// foreground(BGA)
// s16 hscroll_offset_fg = 0;
// s16 vscroll_offset_fg = 0;
// u16 foreground_x = 0;
// u16 foreground_y = -4;

// CAMERA
s16 cur_camera_x = 0;
s16 cur_camera_y = 0;

Sprite *player_sprite[2];
s16 player_pos_x[2] = {240, 33};
s16 player_pos_y[2] = {144, 144};


s16 player_order_x[2] = {1, 1}; // 1 = direction to right; -1 = to left
s16 player_order_y[2] = {0, 0}; // 1 = direction to down; -1 = to up; 0 to stationary

bool player_flip_v[2] = {FALSE, FALSE};
bool player_flip_h[2] = {FALSE, FALSE};

s16 player_impulse_x[2] = {0, 0}; // spinning impulse
s16 player_impulse_y[2] = {0, 0}; // jumping impulse

s16 player_speed_x[2] = {0, 0};
s16 player_speed_y[2] = {0, 0};
s16 player_max_speed_x[2] = {4, 4};
s16 player_max_speed_y[2] = {8, 8};
s16 counter_x[2] = {0, 0};
s16 counter_y[2] = {0, 0};

// desvio do efeito do spin em relação ao personagem
Sprite *effect_sprite[2];
s16 effect_offset_x[2] = {16, 16};
s16 effect_pos_x[2] = {240, 33};

enum PLAYER_STATE
{
	STATE_STANDING,
	STATE_ROLL,
	STATE_LOWER,
	STATE_DUCK,
	STATE_GET_UP,
	STATE_DUCK_WALK,
	STATE_JUMP,
	STATE_JUMP_MIDDLE,
	STATE_FALL,
	STATE_DASH,
	STATE_SLIDE_START,
	STATE_SLIDE,
	STATE_RUN,
	STATE_TURN,
	STATE_WALL_SLIDE,
	STATE_CLIMB_WALL_START,
	STATE_CLIMB_WALL,
	STATE_DUCK_ATTACK,
	STATE_VERTICAL_ATTACK,
	STATE_HORIZONTAL_ATTACK
};

enum PLAYER_STATE player_state[2];

s16 player_animation[2];

int attack_timer[2] = {0, 0};

#define STOP_TIME 40
#define SPIN_TIME 70
#define JUMP_TIME 70
#define TURN_TIME 20
s16 attack_duration[2] = {0, 0}; // animation _speed * num of frames of animation

s16 idle_timer[2] = {0, 0};
s16 idle_duration[2] = {600, 600}; // animation _speed * num of frames of animation

struct Joystick
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
};
struct Joystick joy_state[2];

static void inputHandler(u16 joy, u16 changed, u16 state);
static void finiteStateMachine(int player);
static void updatePlayerPosition(int player);
static void updateCamera(int player);
/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAtackTimer(int player);
static void controlIdleTimer(int player);
static void updateWalkAcceleration(int player);
static void controlMapBoundaries(int player);

static void addJumpImpulse(int player);
static void controlJumpAcceleration(int player);
static void controlFallAcceleration(int player);

static void controlHorizontalFlip(int player);
static void controlVerticalFlip(int player);
static void controlPlayerCollision(int player);

int main()
{
	VDP_setScreenWidth320();
	VDP_setScreenHeight224();
	SPR_init();
	JOY_init();
	JOY_setEventHandler(&inputHandler);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	PAL_setPalette(PAL0, background.palette->data, DMA);
	VDP_drawImageEx(BG_B, &background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), background_x, background_y, FALSE, TRUE);
	ind += background.tileset->numTile;

	VDP_loadTileSet(&level_tileset, ind, DMA);
	PAL_setPalette(PAL1, level_palette.data, DMA);
	level_1_map = MAP_create(&level_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	ind += level_tileset.numTile;

	MAP_scrollTo(level_1_map, cur_camera_x, cur_camera_y);

	PAL_setPalette(PAL2, knight.palette->data, DMA);
	player_sprite[PLAYER_1] = SPR_addSprite(&knight, player_pos_x[PLAYER_1], player_pos_y[PLAYER_1], TILE_ATTR(PAL2, FALSE, player_flip_v[PLAYER_1], player_flip_h[PLAYER_1]));

	/* PAL_setPalette(PAL3, effect.palette->data, DMA);
	effect_pos_x[EFFECT_1] = player_flip_h[PLAYER_1] ? (player_pos_x[PLAYER_1] + effect_offset_x[EFFECT_1]) : (player_pos_x[PLAYER_1] - effect_offset_x[EFFECT_1]);
	effect_sprite[EFFECT_1] = SPR_addSprite(&effect, effect_pos_x[EFFECT_1], player_pos_y[PLAYER_1], TILE_ATTR(PAL3, TRUE, player_flip_v[PLAYER_1], player_flip_h[PLAYER_1]));
	SPR_setVisibility(effect_sprite[EFFECT_1], HIDDEN); */

	// init states
	player_state[PLAYER_1] = STATE_STANDING;
	player_animation[PLAYER_1] = ANIM_STANDING;

	SPR_setAnim(player_sprite[PLAYER_1], player_animation[PLAYER_1]);
	//SPR_setAnim(effect_sprite[EFFECT_1], 0);

	while (1)
	{
		// VDP_waitVSync();
		finiteStateMachine(PLAYER_1);
		controlPlayerCollision(PLAYER_1);
		updatePlayerPosition(PLAYER_1);
		controlHorizontalFlip(PLAYER_1);
		controlVerticalFlip(PLAYER_1);
		updateWalkAcceleration(PLAYER_1);

		addJumpImpulse(PLAYER_1);
		controlJumpAcceleration(PLAYER_1);
		controlFallAcceleration(PLAYER_1);

		controlAtackTimer(PLAYER_1);
		controlIdleTimer(PLAYER_1);
		// controlMapBoundaries(PLAYER_1);

		// updateCamera(PLAYER_1);

		SPR_setAnim(player_sprite[PLAYER_1], player_animation[PLAYER_1]);
		SPR_setHFlip(player_sprite[PLAYER_1], player_flip_h[PLAYER_1]);
		SPR_setPosition(player_sprite[PLAYER_1], (player_pos_x[PLAYER_1] /*  - update_camera_x */), (player_pos_y[PLAYER_1] /*  - update_camera_y */));

		SPR_setAnim(effect_sprite[EFFECT_1], 0);
		SPR_setHFlip(effect_sprite[EFFECT_1], player_flip_h[PLAYER_1]);
		effect_pos_x[EFFECT_1] = player_flip_h[PLAYER_1] ? (player_pos_x[PLAYER_1] + effect_offset_x[EFFECT_1]) : (player_pos_x[PLAYER_1] - effect_offset_x[EFFECT_1]);
		SPR_setPosition(effect_sprite[EFFECT_1], (effect_pos_x[EFFECT_1] /*  - update_camera_x */), (player_pos_y[PLAYER_1] /*  - update_camera_y */));
		// SPR_setVisibility(effect_sprite[EFFECT_1], HIDDEN);
		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	return (0);
}

static void inputHandler(u16 joy, u16 changed, u16 state)
{
	/*
	JOY_update() refreshes joypad status (called every screen refresh so don't call it yourself unless you really need it)
	JOY_readJoypad( joy ) gives you the joy1 status whenever you need it
	JOY_waitPressBtn() waits for any buttons to be pressed (not directions)
	JOY_waitPress(joy, BUTTON_A | BUTTON_UP) waits for wanted buttons to be pressed on specific joypad
	*/
	if (joy == JOY_1)
	{
		if (state & BUTTON_LEFT)
			joy_state[PLAYER_1].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joy_state[PLAYER_1].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joy_state[PLAYER_1].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joy_state[PLAYER_1].btn_right = FALSE;

		if (state & BUTTON_UP)
			joy_state[PLAYER_1].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			joy_state[PLAYER_1].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			joy_state[PLAYER_1].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			joy_state[PLAYER_1].btn_down = FALSE;

		if (state & BUTTON_A)
			joy_state[PLAYER_1].btn_a = TRUE;
		else if (changed & BUTTON_A)
			joy_state[PLAYER_1].btn_a = FALSE;

		if (state & BUTTON_B)
			joy_state[PLAYER_1].btn_b = TRUE;
		else if (changed & BUTTON_B)
			joy_state[PLAYER_1].btn_b = FALSE;

		if (state & BUTTON_C)
			joy_state[PLAYER_1].btn_c = TRUE;
		else if (changed & BUTTON_C)
			joy_state[PLAYER_1].btn_c = FALSE;

		if (state & BUTTON_X)
			joy_state[PLAYER_1].btn_x = TRUE;
		else if (changed & BUTTON_X)
			joy_state[PLAYER_1].btn_x = FALSE;

		if (state & BUTTON_Y)
			joy_state[PLAYER_1].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joy_state[PLAYER_1].btn_y = FALSE;

		if (state & BUTTON_Z)
			joy_state[PLAYER_1].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joy_state[PLAYER_1].btn_z = FALSE;

		if (state & BUTTON_START)
			joy_state[PLAYER_1].btn_start = TRUE;
		else if (changed & BUTTON_START)
			joy_state[PLAYER_1].btn_start = FALSE;

		if (state & BUTTON_MODE)
			joy_state[PLAYER_1].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			joy_state[PLAYER_1].btn_mode = FALSE;
	}

	if (joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
			joy_state[PLAYER_2].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joy_state[PLAYER_2].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joy_state[PLAYER_2].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joy_state[PLAYER_2].btn_right = FALSE;

		if (state & BUTTON_UP)
			joy_state[PLAYER_2].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			joy_state[PLAYER_2].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			joy_state[PLAYER_2].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			joy_state[PLAYER_2].btn_down = FALSE;

		if (state & BUTTON_A)
			joy_state[PLAYER_2].btn_a = TRUE;
		else if (changed & BUTTON_A)
			joy_state[PLAYER_2].btn_a = FALSE;

		if (state & BUTTON_B)
			joy_state[PLAYER_2].btn_b = TRUE;
		else if (changed & BUTTON_B)
			joy_state[PLAYER_2].btn_b = FALSE;

		if (state & BUTTON_C)
			joy_state[PLAYER_2].btn_c = TRUE;
		else if (changed & BUTTON_C)
			joy_state[PLAYER_2].btn_c = FALSE;

		if (state & BUTTON_X)
			joy_state[PLAYER_2].btn_x = TRUE;
		else if (changed & BUTTON_X)
			joy_state[PLAYER_2].btn_x = FALSE;

		if (state & BUTTON_Y)
			joy_state[PLAYER_2].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joy_state[PLAYER_2].btn_y = FALSE;

		if (state & BUTTON_Z)
			joy_state[PLAYER_2].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joy_state[PLAYER_2].btn_z = FALSE;

		if (state & BUTTON_START)
			joy_state[PLAYER_2].btn_start = TRUE;
		else if (changed & BUTTON_START)
			joy_state[PLAYER_2].btn_start = FALSE;

		if (state & BUTTON_MODE)
			joy_state[PLAYER_2].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			joy_state[PLAYER_2].btn_mode = FALSE;
	}
}

static void finiteStateMachine(int player)
{
	switch (player_state[player])
	{
	case STATE_STANDING:
		if (joy_state[player].btn_left)
		{
			player_order_x[player] = -1;
			player_state[player] = STATE_RUN;
			player_animation[player] = ANIM_RUN;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_right)
		{
			player_order_x[player] = 1;
			player_state[player] = STATE_RUN;
			player_animation[player] = ANIM_RUN;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_down)
		{
			player_state[player] = STATE_LOWER;
			player_animation[player] = ANIM_DUCK_INTERMEDIATE;
			SPR_setAnimationLoop(player_sprite[player], FALSE);
		}

		/* if (joy_state[player].btn_a)
		{
			player_state[player] = STATE_JUMP;
			player_animation[player] = ANIM_JUMP;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		} */

		if (joy_state[player].btn_c)
		{
			player_state[player] = STATE_DASH;
			player_animation[player] = ANIM_DASH;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}

		if (joy_state[player].btn_x)
		{
			player_state[player] = STATE_VERTICAL_ATTACK;
			player_animation[player] = ANIM_VERTICAL_ATTACK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}

		if (joy_state[player].btn_y)
		{
			player_state[player] = STATE_HORIZONTAL_ATTACK;
			player_animation[player] = ANIM_HORIZONTAL_ATTACK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}

		if (joy_state[player].btn_z)
		{
			player_state[player] = STATE_ROLL;
			player_animation[player] = ANIM_ROLL;
			SPR_setAnimationLoop(player_sprite[player], FALSE);
		}

		else if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right) && !(joy_state[player].btn_x) && !(joy_state[player].btn_y) && !(joy_state[player].btn_z))
		{
			player_animation[player] = ANIM_STANDING;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_RUN:
		if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right))
		{
			// attack_timer[player] += 1;
			player_state[player] = STATE_TURN;
			player_animation[player] = ANIM_TURN;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_TURN:
		if (player_speed_x[player] == 0)
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
			SPR_setVisibility(effect_sprite[EFFECT_1], HIDDEN);
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_LOWER:
		player_state[player] = STATE_DUCK;
		player_animation[player] = ANIM_DUCK;
		SPR_setAnimationLoop(player_sprite[player], FALSE);
		break;

	case STATE_DUCK:
		if (joy_state[player].btn_left)
		{
			player_order_x[player] = -1;
			player_state[player] = STATE_DUCK_WALK;
			player_animation[player] = ANIM_DUCK_WALK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_right)
		{
			player_order_x[player] = 1;
			player_state[player] = STATE_DUCK_WALK;
			player_animation[player] = ANIM_DUCK_WALK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_up)
		{
			player_state[player] = STATE_GET_UP;
			player_animation[player] = ANIM_DUCK_INTERMEDIATE;
			SPR_setAnimationLoop(player_sprite[player], FALSE);
		}
		if (joy_state[player].btn_x)
		{
			player_state[player] = STATE_DUCK_ATTACK;
			player_animation[player] = ANIM_DUCK_ATTACK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_y)
		{
			player_state[player] = STATE_DUCK_ATTACK;
			player_animation[player] = ANIM_DUCK_ATTACK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		if (joy_state[player].btn_z)
		{
			player_state[player] = STATE_SLIDE_START;
			player_animation[player] = ANIM_SLIDE_START;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_DUCK_WALK:
		if (joy_state[player].btn_left || joy_state[player].btn_right)
		{
			player_state[player] = STATE_DUCK_WALK;
			player_animation[player] = ANIM_DUCK_WALK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		else if (!(joy_state[player].btn_left) || !(joy_state[player].btn_right))
		{
			player_state[player] = STATE_DUCK;
			player_animation[player] = ANIM_DUCK;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_DUCK_ATTACK:
		if (!(joy_state[player].btn_x) && !(joy_state[player].btn_y))
		{
			player_state[player] = STATE_DUCK;
			player_animation[player] = ANIM_DUCK;
			SPR_setAnimationLoop(player_sprite[player], FALSE);
		}
		break;

	case STATE_SLIDE_START:
		player_state[player] = STATE_SLIDE;
		player_animation[player] = ANIM_SLIDE;
		SPR_setAnimationLoop(player_sprite[player], FALSE);
		break;

	case STATE_SLIDE:
		if (!(joy_state[player].btn_z))
		{
			player_state[player] = STATE_DUCK;
			player_animation[player] = ANIM_DUCK;
			SPR_setAnimationLoop(player_sprite[player], FALSE);
		}
		break;

	case STATE_GET_UP:
		player_state[player] = STATE_STANDING;
		player_animation[player] = ANIM_STANDING;
		SPR_setAnimationLoop(player_sprite[player], TRUE);
		break;

	case STATE_DASH:
		if (joy_state[player].btn_c)
		{
			player_state[player] = STATE_DASH;
			player_animation[player] = ANIM_DASH;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		else if (!(joy_state[player].btn_c))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_VERTICAL_ATTACK:
		if (!(joy_state[player].btn_x))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_HORIZONTAL_ATTACK:
		if (!(joy_state[player].btn_y))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

	case STATE_ROLL:
		if (!(joy_state[player].btn_z))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
		}
		break;

		/* STATE_STANDING,
		STATE_ROLL,
		STATE_LOWER,
		STATE_DUCK,
		STATE_GET_UP,
		STATE_DUCK_WALK,
		STATE_JUMP,
		STATE_JUMP_MIDDLE,
		STATE_FALL,
		STATE_DASH,
		STATE_SLIDE_START,
		STATE_SLIDE,
		STATE_RUN,
		STATE_TURN,
		STATE_WALL_SLIDE,
		STATE_CLIMB_WALL_START,
		STATE_CLIMB_WALL,
		STATE_DUCK_ATTACK,
		STATE_VERTICAL_ATTACK,
		STATE_HORIZONTAL_ATTACK	 */

		/* case STATE_IMPULSE_JUMP:
			if (!(joy_state[player].btn_x))
			{
				player_state[player] = STATE_START_JUMP;
				player_animation[player] = ANIM_IMPULSE;
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break; */

		/* case STATE_START_JUMP:
			// attack_timer[player] += 1;
			player_order_y[player] = -1;
			player_state[player] = STATE_JUMP;
			player_animation[player] = ANIM_JUMP;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
			break; */

		/* case STATE_JUMP:
			if (player_speed_y[player] <= 0)
			{
				player_order_y[player] = 1;
				player_state[player] = STATE_FALL;
				player_animation[player] = ANIM_FALL;
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break; */

		/* case STATE_FALL:
			if (player_pos_y[player] >= 144)
			{
				player_order_y[player] = 0;
				player_pos_y[player] = 144;
				player_state[player] = STATE_STANDING;
				player_animation[player] = ANIM_STANDING;
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break; */

		/* case STATE_IMPULSE_SPIN:
			if (!(joy_state[player].btn_b))
			{
				player_state[player] = STATE_START_SPIN;
				player_animation[player] = ANIM_SPIN;
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break;
	 */
		/* case STATE_STOP:
			if (player_speed_x[player] > 0)
			{
				if (player_order_x[player] < 0)
				{
					if (joy_state[player].btn_right)
					{
						attack_timer[player] += 1;
						player_state[player] = STATE_TURN;
						player_animation[player] = ANIM_TURN;
						SPR_setAnimationLoop(player_sprite[player], TRUE);
					}
				}
				else if (player_order_x[player] > 0)
				{
					if (joy_state[player].btn_left)
					{
						attack_timer[player] += 1;
						player_state[player] = STATE_TURN;
						player_animation[player] = ANIM_TURN;
						SPR_setAnimationLoop(player_sprite[player], TRUE);
					}
				}
			}
			else if (player_speed_x[player] == 0)
			{
				player_state[player] = STATE_STANDING;
				player_animation[player] = ANIM_STANDING;
				SPR_setVisibility(effect_sprite[EFFECT_1], HIDDEN);
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break; */

		/* case STATE_START_SPIN:
			// attack_timer[player] += 1;
			player_state[player] = STATE_SPIN;
			player_animation[player] = ANIM_SPIN;
			SPR_setAnimationLoop(player_sprite[player], TRUE);
			break; */

		/* case STATE_SPIN:
			if (player_speed_x[player] == 0)
			{
				player_state[player] = STATE_STANDING;
				player_animation[player] = ANIM_STANDING;
				SPR_setVisibility(effect_sprite[EFFECT_1], HIDDEN);
				SPR_setAnimationLoop(player_sprite[player], TRUE);
			}
			break; */

	default:
		break;
	}
}

static void updateWalkAcceleration(int player)
{
	if (player_state[player] == STATE_RUN || player_state[player] == STATE_DUCK_WALK || player_state[player] == STATE_SLIDE || player_state[player] == STATE_ROLL)
	{
		if (player_speed_x[player] < player_max_speed_x[player])
		{
			// accelerate
			counter_x[player] += 1;
			if (counter_x[player] == (s16)COUNTER_LIMIT)
			{
				counter_x[player] = 0;
				player_speed_x[player] += 1;
			}
		}
	}

	/* else if (player_state[player] == STATE_IMPULSE_SPIN)
	{
		if (player_impulse_x[player] < player_max_speed_x[player])
		{
			// accelerate
			counter_x[player] += 1;
			if (counter_x[player] == (s16)COUNTER_LIMIT)
			{
				counter_x[player] = 0;
				player_impulse_x[player] += 1;
			}
		}
	}
	else if (player_state[player] == STATE_START_SPIN)
	{
		if (player_speed_x[player] == 0)
		{
			player_speed_x[player] = player_impulse_x[player];
			player_impulse_x[player] = 0;
		}
	} */
	else
	{
		if (player_speed_x[player] > 0)
		{
			// slow down
			counter_x[player] += 1;
			if (counter_x[player] == (s16)COUNTER_LIMIT)
			{
				counter_x[player] = 0;
				player_speed_x[player] -= 1;
			}
		}
	}
}

static void addJumpImpulse(int player)
{
	/* if (player_state[player] == STATE_IMPULSE_JUMP)
	{
		if (player_impulse_y[player] < player_max_speed_y[player])
		{
			// accelerate
			counter_y[player] += 1;
			if (counter_y[player] == (s16)COUNTER_LIMIT)
			{
				counter_y[player] = 0;
				player_impulse_y[player] += 1;
			}
		}
	} */
}

static void controlJumpAcceleration(int player)
{
	// sprintf(info, "%10i", player_state[player]);
	// VDP_drawTextBG(BG_A, info, 28, 5);

	/* if (player_state[player] == STATE_START_JUMP)
	{
		player_speed_y[player] = player_impulse_y[player];
		player_impulse_y[player] = 0;
	}
	else if (player_state[player] == STATE_JUMP)
	{
		if (player_speed_y[player] > 0)
		{
			// slow down
			counter_y[player] += 1;
			if (counter_y[player] == (s16)COUNTER_LIMIT)
			{
				counter_y[player] = 0;
				player_speed_y[player] -= 1;
			}
		}
	} */
}

static void controlFallAcceleration(int player)
{
	if (player_state[player] == STATE_FALL)
	{
		if (player_speed_y[player] < player_max_speed_y[player])
		{
			// accelerate
			counter_y[player] += 1;
			if (counter_y[player] == (s16)COUNTER_LIMIT)
			{
				counter_y[player] = 0;
				player_speed_y[player] += 1;
			}
		}
	}
}

static void controlHorizontalFlip(int player)
{
	if (player_order_x[player] > 0)
	{
		player_flip_h[player] = FALSE;
	}
	else if (player_order_x[player] < 0)
	{
		player_flip_h[player] = TRUE;
	}
}

static void controlVerticalFlip(int player)
{
	if (player_order_y[player] > 0)
	{
		player_flip_v[player] = FALSE;
	}
	else if (player_order_y[player] < 0)
	{
		player_flip_v[player] = TRUE;
	}
}

static void updatePlayerPosition(int player)
{
	if (player_order_x[player] < 0)
	{
		player_pos_x[player] -= player_speed_x[player];
		/* if (player_pos_x[player] <= blocked_left_coord[player])
		{
			player_pos_x[player] = blocked_left_coord[player] + 1;
		}
		else
		{
		} */
	}
	else if (player_order_x[player] > 0)
	{
		player_pos_x[player] += player_speed_x[player];
		/* if ((player_pos_x[player] + BOX_RIGHT_EDGE) >= blocked_right_coord[player])
		{
			player_pos_x[player] = (blocked_right_coord[player] - BOX_RIGHT_EDGE);
		}
		else
		{
		} */
	}
	// jump
	if (player_order_y[player] < 0)
	{
		player_pos_y[player] -= player_speed_y[player];
	}
	else if (player_order_y[player] > 0)
	{
		player_pos_y[player] += player_speed_y[player];
	}
}

static void controlAtackTimer(int player)
{
	/* if (player_state[player] == STATE_SPIN)
	{
		attack_duration[player] = (s16)SPIN_TIME;
	}
	else if (player_state[player] == STATE_STOP)
	{
		attack_duration[player] = (s16)STOP_TIME;
	}
	else if (player_state[player] == STATE_JUMP)
	{
		attack_duration[player] = (s16)JUMP_TIME;
	}
	else if (player_state[player] == STATE_TURN)
	{
		attack_duration[player] = (s16)TURN_TIME;
	}
	else
	{
		prevent function call with another state
		attack_duration[player] = 0;
	}

	if (attack_timer[player] > 0 && attack_timer[player] < attack_duration[player])
	{
		attack_timer[player] += 1;
	}
	else if (attack_timer[player] == attack_duration[player])
	{
		attack_timer[player] = 0;
	} */
}

static void controlIdleTimer(int player)
{
	/* if (player_state[player] == STATE_STANDING || player_state[player] == STATE_BORED || player_state[player] == STATE_GO)
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

static void controlMapBoundaries(int player)
{
	if (player_pos_x[player] < 0)
	{
		player_pos_x[player] = 0;
	}
	else if (player_pos_x[player] > (s16)(MAP_WIDTH - PLAYER_WIDTH))
	{
		player_pos_x[player] = (s16)(MAP_WIDTH - PLAYER_WIDTH);
	}

	if (player_pos_y[player] < 0)
	{
		player_pos_y[player] = 0;
	}
	else if (player_pos_y[player] > (s16)(MAP_HEIGTH - PLAYER_HEIGTH))
	{
		player_pos_y[player] = (s16)(MAP_HEIGTH - PLAYER_HEIGTH);
	}
}

static void controlPlayerCollision(int player)
{
	s16 left_edge_collision_box = player_pos_x[player]; /*  + BOX_LEFT_EDGE; */
	s16 right_edge_collision_box = player_pos_x[player] + PLAYER_WIDTH;
	s16 top_edge_collision_box = player_pos_y[player]; /*  + BOX_TOP_EDGE; */
	s16 botton_edge_collision_box = player_pos_y[player] + PLAYER_HEIGTH;

	s16 colbox_left_column_index = left_edge_collision_box >> 4; //(same as '/16')
	s16 colbox_right_column_index = right_edge_collision_box >> 4;
	s16 colbox_top_line_index = top_edge_collision_box >> 4;
	s16 colbox_botton_line_index = botton_edge_collision_box >> 4;
	// edge
	// vertex
	u16 top_left_colbox_vertex_array_index = colbox_left_column_index + (colbox_top_line_index * BGA_COLLISION_ARRAY_LENGTH);
	u16 top_right_colbox_vertex_array_index = colbox_right_column_index + (colbox_top_line_index * BGA_COLLISION_ARRAY_LENGTH);
	u16 botton_left_colbox_vertex_array_index = colbox_left_column_index + (colbox_botton_line_index * BGA_COLLISION_ARRAY_LENGTH);
	u16 botton_right_colbox_vertex_array_index = colbox_right_column_index + (colbox_botton_line_index * BGA_COLLISION_ARRAY_LENGTH);

	u8 top_left_tile_collision_type = BGA_COLLISION_ARRAY[top_left_colbox_vertex_array_index];
	u8 top_right_tile_collision_type = BGA_COLLISION_ARRAY[top_right_colbox_vertex_array_index];
	u8 botton_left_tile_collision_type = BGA_COLLISION_ARRAY[botton_left_colbox_vertex_array_index];
	u8 botton_right_tile_collision_type = BGA_COLLISION_ARRAY[botton_right_colbox_vertex_array_index];

	if (player_order_x[player] < 0)
	{
		collision_from[player].top_left = top_left_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
		if (collision_from[player].top_left)
		{
			// blocked_right_coord[player] = 320;
			blocked_left_coord[player] = (colbox_left_column_index << 4); // equals to 'left_edge_collision_box'
		}
	}
	else if (player_order_x[player] > 0)
	{
		collision_from[player].top_right = top_right_tile_collision_type == SOLID_TILE ? TRUE : FALSE;
		if (collision_from[player].top_right)
		{
			// blocked_left_coord[player] = 0;
			blocked_right_coord[player] = (colbox_right_column_index << 4); // equals to 'right_edge_collision_box'
		}
	}
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
	sprintf(info, "%10i", top_right_tile_collision_type);
	VDP_drawTextBG(BG_A, info, 28, 5);
}

static void updateCamera(int player)
{
	s16 update_camera_x;
	s16 update_camera_y;
	s16 player_x_on_map = player_pos_x[player];
	s16 player_y_on_map = player_pos_y[player];
	s16 player_x_on_screen = (player_x_on_map - cur_camera_x);
	s16 player_y_on_screen = (player_y_on_map - cur_camera_y);

	if (player_x_on_screen > (s16)HOW_FAR_TO_RIGHT)
	{
		update_camera_x = player_x_on_map - (s16)HOW_FAR_TO_RIGHT;
	}
	else if (player_x_on_screen < (s16)HOW_FAR_TO_LEFT)
	{
		update_camera_x = player_x_on_map - (s16)HOW_FAR_TO_LEFT;
	}
	else
	{
		update_camera_x = cur_camera_x;
	}

	if (player_y_on_screen > (s16)HOW_FAR_TO_BOTTON)
	{
		update_camera_y = player_y_on_map - (s16)HOW_FAR_TO_BOTTON;
	}
	else if (player_y_on_screen < (s16)HOW_FAR_TO_TOP)
	{
		update_camera_y = player_y_on_map - (s16)HOW_FAR_TO_TOP;
	}
	else
	{
		update_camera_y = cur_camera_y;
	}

	// stop camera from going beyond map boundaries
	if (update_camera_x < 0)
	{
		update_camera_x = 0;
	}
	else if (update_camera_x > (s16)(MAP_WIDTH - HORIZONTAL_RESOLUTION))
	{
		update_camera_x = (s16)(MAP_WIDTH - HORIZONTAL_RESOLUTION);
	}

	if (update_camera_y < 0)
	{
		update_camera_y = 0;
	}
	else if (update_camera_y > (s16)(MAP_HEIGTH - VERTICAL_RESOLUTION))
	{
		update_camera_y = (s16)(MAP_HEIGTH - VERTICAL_RESOLUTION);
	}

	// update current camera X & Y
	if ((cur_camera_x != update_camera_x) || (cur_camera_y != update_camera_y))
	{
		if (cur_camera_x < update_camera_x)
		{
			hscroll_offset_bg -= 1;
			VDP_setHorizontalScroll(BG_B, hscroll_offset_bg);
		}
		if (cur_camera_x > update_camera_x)
		{
			hscroll_offset_bg += 1;
			VDP_setHorizontalScroll(BG_B, hscroll_offset_bg);
		}

		cur_camera_x = update_camera_x;
		cur_camera_y = update_camera_y;
		MAP_scrollTo(level_1_map, update_camera_x, update_camera_y);
	}

	SPR_setPosition(player_sprite[player], (player_pos_x[player] - update_camera_x), (player_pos_y[player] - update_camera_y));
};

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
//// Set the current animation (e.g., animation index 0)
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