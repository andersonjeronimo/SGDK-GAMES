#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html

#define ANIM_DUCK 0
#define ANIM_LOOK_UP 1
#define ANIM_STANDING 2
#define ANIM_BORED 3
#define ANIM_GO 4
#define ANIM_WALK 5
#define ANIM_RUN 6
#define ANIM_STOP 7
#define ANIM_IMPULSE 8
#define ANIM_JUMP 9
#define ANIM_PUSH 10
#define ANIM_SPIN 11
#define ANIM_SPIN_EFFECT 12
#define ANIM_FALL 13

#define PLAYER_1 0
#define PLAYER_2 1
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

#define PLAYER_WIDTH 48
#define PLAYER_HEIGTH 48
// player collision box
#define PLAYER_COLBOX_LEFT 10
#define PLAYER_COLBOX_RIGHT 38
#define PLAYER_COLBOX_TOP 10
#define PLAYER_COLBOX_BOTTON 38

struct Collision
{
	bool top_left;
	bool top_right;
	bool botton_left;
	bool botton_right;
};
struct Collision collision_state[2];

Map *level_1_map;
u16 ind = TILE_USER_INDEX;

const u16 LEVEL_COL_ARRAY_LENGTH = 20;
#define SOLID_TILE 1
char info[10];

// collision for foreground (BGA)
const u8 BGA_COLLISION_ARRAY[280] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
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
// u16 fg_x = 0;
// u16 fg_y = -4;

// CAMERA
s16 cur_camera_x = 0;
s16 cur_camera_y = 0;

Sprite *spr_players[2]; // screen res = 320 x 224
s16 player_pos_x[2] = {48, 48};
s16 player_pos_y[2] = {144, 144};

s16 player_order_x[2] = {1, 1}; // 1 = direction to right; -1 = to left
s16 player_order_y[2] = {0, 0}; // 1 = direction to down; -1 = to up; 0 to stationary

bool player_flip_v[2] = {FALSE, FALSE};
bool player_flip_h[2] = {FALSE, FALSE};

s16 player_impulse_x[2] = {0, 0}; // spinning impulse
s16 player_impulse_y[2] = {7, 7}; // jumping impulse

s16 player_speed_x[2] = {0, 0};
s16 player_speed_y[2] = {0, 0};
s16 player_max_speed_x[2] = {8, 8};
s16 player_max_speed_y[2] = {8, 8};
s16 counter_x[2] = {0, 0};
s16 counter_y[2] = {0, 0};

enum PLAYER_STATE
{
	STATE_DUCK,
	STATE_LOOK_UP,
	STATE_STANDING,
	STATE_BORED,
	STATE_GO,
	STATE_WALK,
	STATE_RUN,
	STATE_STOP,
	STATE_IMPULSE_JUMP,
	STATE_JUMP,
	STATE_PUSH,
	STATE_IMPULSE_SPIN,
	STATE_START_SPIN,
	STATE_SPIN,
	STATE_FALL
};
enum PLAYER_STATE player_state[2];

s16 player_animation[2];

int attack_timer[2] = {0, 0};

#define STOP_TIME 40
#define SPIN_TIME 70
#define JUMP_TIME 70
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
static void updateAnimationState(int player);
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
static void controlCollision(int player);

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

	PAL_setPalette(PAL2, sonic.palette->data, DMA);
	spr_players[PLAYER_1] = SPR_addSprite(&sonic, player_pos_x[PLAYER_1], player_pos_y[PLAYER_1], TILE_ATTR(PAL2, FALSE, player_flip_v[PLAYER_1], player_flip_h[PLAYER_1]));

	// init states
	player_state[PLAYER_1] = STATE_STANDING;
	player_animation[PLAYER_1] = ANIM_STANDING;

	SPR_setAnim(spr_players[PLAYER_1], player_animation[PLAYER_1]);

	while (1)
	{
		// VDP_waitVSync();
		// updateAnimationState(PLAYER_1);
		finiteStateMachine(PLAYER_1);
		updatePlayerPosition(PLAYER_1);
		controlHorizontalFlip(PLAYER_1);
		controlVerticalFlip(PLAYER_1);
		updateWalkAcceleration(PLAYER_1);

		// controlCollision(PLAYER_1);
		addJumpImpulse(PLAYER_1);
		controlJumpAcceleration(PLAYER_1);
		// controlFallAcceleration(PLAYER_1);
		controlAtackTimer(PLAYER_1);
		controlIdleTimer(PLAYER_1);
		// controlMapBoundaries(PLAYER_1);
		updateCamera(PLAYER_1);

		SPR_setAnim(spr_players[PLAYER_1], player_animation[PLAYER_1]);
		SPR_setHFlip(spr_players[PLAYER_1], player_flip_h[PLAYER_1]);
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
			player_state[player] = STATE_WALK;
			player_animation[player] = ANIM_WALK;
		}
		else if (joy_state[player].btn_right)
		{
			player_order_x[player] = 1;
			player_state[player] = STATE_WALK;
			player_animation[player] = ANIM_WALK;
		}
		if (joy_state[player].btn_up)
		{
			player_state[player] = STATE_LOOK_UP;
			player_animation[player] = ANIM_LOOK_UP;
		}
		if (joy_state[player].btn_down)
		{
			player_state[player] = STATE_DUCK;
			player_animation[player] = ANIM_DUCK;
		}
		else if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right) && !(joy_state[player].btn_up) && !(joy_state[player].btn_down))
		{
			if (idle_timer[player] > (idle_duration[player] / 2) && idle_timer[player] <= 320)
			{				
				player_animation[player] = ANIM_BORED;
			}
			else if (idle_timer[player] > 320 && idle_timer[player] < idle_duration[player])
			{				
				player_animation[player] = ANIM_GO;
			}
			else if (idle_timer[player] == idle_duration[player])
			{				
				player_animation[player] = ANIM_STANDING;
			}
		}
		break;

		/* case STATE_BORED:
			if (idle_timer[player] > 320 && idle_timer[player] < idle_duration[player])
			{
				player_state[player] = STATE_GO;
				player_animation[player] = ANIM_GO;
			}
			break;

		case STATE_GO:
			if (idle_timer[player] == idle_duration[player])
			{
				player_state[player] = STATE_STANDING;
				player_animation[player] = ANIM_STANDING;
			}
			break; */

	case STATE_WALK:
		if (joy_state[player].btn_left || joy_state[player].btn_right)
		{
			if (player_speed_x[player] > (player_max_speed_x[player] / 2))
			{
				player_state[player] = STATE_RUN;
				player_animation[player] = ANIM_RUN;
			}
		}

		if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
		}
		break;

	case STATE_RUN:
		if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right))
		{
			attack_timer[player] += 1;
			player_state[player] = STATE_STOP;
			player_animation[player] = ANIM_STOP;
		}
		break;

	case STATE_LOOK_UP:
		if (!(joy_state[player].btn_up))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
		}
		break;

	case STATE_DUCK:
		if (joy_state[player].btn_b)
		{
			player_state[player] = STATE_IMPULSE_SPIN;
			player_animation[player] = ANIM_SPIN;
		}

		if (!(joy_state[player].btn_down))
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
		}
		break;

	case STATE_IMPULSE_SPIN:
		if (!(joy_state[player].btn_b))
		{
			player_state[player] = STATE_START_SPIN;
			player_animation[player] = ANIM_SPIN;
		}
		break;

	case STATE_STOP:
		if (player_speed_x[player] == 0)
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
		}
		break;

	case STATE_START_SPIN:
		attack_timer[player] += 1;
		player_state[player] = STATE_SPIN;
		player_animation[player] = ANIM_SPIN;
		break;

	case STATE_SPIN:
		if (player_speed_x[player] == 0)
		{
			player_state[player] = STATE_STANDING;
			player_animation[player] = ANIM_STANDING;
		}
		break;

	default:
		break;
	}
}

static void updateWalkAcceleration(int player)
{
	if (player_state[player] == STATE_WALK || player_state[player] == STATE_RUN)
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

	else if (player_state[player] == STATE_IMPULSE_SPIN)
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
		}
	}
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
	if (player_order_x[player] > 0)
	{
		player_pos_x[player] += player_speed_x[player];
	}
	else if (player_order_x[player] < 0)
	{
		player_pos_x[player] -= player_speed_x[player];
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
	if (player_state[player] == STATE_SPIN)
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
	else
	{
		/*prevent function call with another state*/
		attack_duration[player] = 0;
	}

	if (attack_timer[player] > 0 && attack_timer[player] < attack_duration[player])
	{
		attack_timer[player] += 1;
	}
	else if (attack_timer[player] == attack_duration[player])
	{
		attack_timer[player] = 0;
	}
}

static void controlIdleTimer(int player)
{
	if (player_state[player] == STATE_STANDING || player_state[player] == STATE_BORED || player_state[player] == STATE_GO)
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
	}
}

static void addJumpImpulse(int player)
{
	if (player_state[player] == STATE_IMPULSE_JUMP)
	{
		if (player_speed_y[player] == 0)
		{
			player_order_y[player] = -1;
			attack_timer[player] += 1;
			player_state[player] = STATE_JUMP;
			player_animation[player] = ANIM_JUMP;
			player_speed_y[player] = player_impulse_y[player];
		}
	}
}

static void controlJumpAcceleration(int player)
{
	if (player_state[player] == STATE_JUMP)
	{
		if (player_order_y[player] < 0)
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
				/* if (player_speed_y[player] < player_max_speed_y[player])
				{
					// accelerate
					counter_y[player] += 1;
					if (counter_y[player] == (s16)COUNTER_LIMIT)
					{
						counter_y[player] = 0;
						player_speed_y[player] += 1;
					}
				}
				else
				{
					player_speed_y[player] = 0;
					player_order_y[player] = 0;
				} */
			}
			else
			{
				// player_state[player] = STATE_FALL;
				player_speed_y[player] = 0;
				player_order_y[player] = 0;
			}
		}
	}
}

static void controlFallAcceleration(int player)
{
	if (player_state[player] == STATE_FALL)
	{
		if (player_order_y[player] > 0)
		{
			if (player_speed_y[player] > 0)
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
	}
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

static void controlCollision(int player)
{
	s16 player_left_collision_coord = player_pos_x[player];						   // + PLAYER_COLBOX_LEFT;
	s16 player_right_collision_coord = player_pos_x[player] + (s16)PLAYER_WIDTH;   // PLAYER_COLBOX_RIGHT;
	s16 player_top_collision_coord = player_pos_y[player];						   //  + PLAYER_COLBOX_TOP;
	s16 player_botton_collision_coord = player_pos_y[player] + (s16)PLAYER_HEIGTH; // PLAYER_COLBOX_BOTTON;

	// posição(índice) das arestas da caixa de colisão(player) no array de colisões	(coordenada apenas do index da linha)
	s16 xtilecoord_left_collision = player_left_collision_coord >> 4; //(same as '/16')
	s16 xtilecoord_right_collision = player_right_collision_coord >> 4;
	s16 ytilecoord_top_collision = player_top_collision_coord >> 4;
	s16 ytilecoord_botton_collision = player_botton_collision_coord >> 4;

	// posição(índice) dos vértices da caixa de colisões(player) no array de colisões (coordenada precisa - linha x coluna)
	s16 array_index_topleft_colbox = xtilecoord_left_collision + (ytilecoord_top_collision * LEVEL_COL_ARRAY_LENGTH);
	s16 array_index_topright_colbox = xtilecoord_right_collision + (ytilecoord_top_collision * LEVEL_COL_ARRAY_LENGTH);
	s16 array_index_bottonleft_colbox = xtilecoord_left_collision + (ytilecoord_botton_collision * LEVEL_COL_ARRAY_LENGTH);
	s16 array_index_bottonright_colbox = xtilecoord_left_collision + (ytilecoord_botton_collision * LEVEL_COL_ARRAY_LENGTH);

	// checa se houve colisão (0 | 1)
	u8 tile_collision_type_topleft = BGA_COLLISION_ARRAY[array_index_topleft_colbox];
	u8 tile_collision_type_topright = BGA_COLLISION_ARRAY[array_index_topright_colbox];
	u8 tile_collision_type_bottonleft = BGA_COLLISION_ARRAY[array_index_bottonleft_colbox];
	u8 tile_collision_type_bottonright = BGA_COLLISION_ARRAY[array_index_bottonright_colbox];

	// set the collision box state
	collision_state[player].top_left = tile_collision_type_topleft == SOLID_TILE ? TRUE : FALSE;
	collision_state[player].top_right = tile_collision_type_topright == SOLID_TILE ? TRUE : FALSE;
	collision_state[player].botton_left = tile_collision_type_bottonleft == SOLID_TILE ? TRUE : FALSE;
	collision_state[player].botton_right = tile_collision_type_bottonright == SOLID_TILE ? TRUE : FALSE;

	sprintf(info, "%10i", tile_collision_type_topleft);
	// sprintf(info, "%10i", player_speed_y[player]);
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

	SPR_setPosition(spr_players[player], (player_pos_x[player] - update_camera_x), (player_pos_y[player] - update_camera_y));
};