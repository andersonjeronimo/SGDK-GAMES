#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html

#define ANIM_WALK 0
#define ANIM_RUN 1
#define ANIM_PUSH 2
#define ANIM_STOP 3
#define ANIM_SPIN 4
#define ANIM_UP 5
#define ANIM_DOWN 6
#define ANIM_STILL 7
#define ANIM_WAIT 8

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
#define MAP_WIDTH 5120
#define MAP_HEIGTH 256

#define PLAYER_WIDTH 48
#define PLAYER_HEIGTH 48

Map *level_1_map;
u16 ind = TILE_USER_INDEX;

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
s16 cur_camera_y = 32;

Sprite *spr_players[2]; // screen res = 320 x 224
s16 player_pos_x[2] = {160, 160};
s16 player_pos_y[2] = {192, 192};

bool player_flip_v[2] = {FALSE, FALSE};
bool player_flip_h[2] = {FALSE, FALSE};

s16 player_impulse_y[2] = {1, 1}; // jumping impulse

s16 player_speed_x[2] = {0, 0};
s16 player_speed_y[2] = {0, 0};
s16 player_max_speed_x[2] = {8, 8};
s16 player_max_speed_y[2] = {8, 8};
s16 counter_x[2] = {0, 0};
s16 counter_y[2] = {0, 0};

s16 player_order_x[2] = {1, 1}; // 1 = direction to right; -1 = to left
s16 player_order_y[2] = {0, 0}; // 1 = direction to down; -1 = to up

s16 ground = 240;

enum PLAYER_STATE
{
	STATE_STILL,
	STATE_WAIT,
	STATE_WALK_LEFT,
	STATE_WALK_RIGHT,
	STATE_WALK,
	STATE_RUN_LEFT,
	STATE_RUN_RIGHT,
	STATE_RUN,
	STATE_STOP,
	STATE_LOOK_UP,
	STATE_CROUCH,
	STATE_SPIN,
	STATE_JUMP
};
enum PLAYER_STATE player_state[2];

s16 player_animation[2];

int attack_timer[2] = {0, 0};
enum ANIM_DURATION
{
	STOPPING = 40,
	SPINNING = 80,
	JUMPING = 80
};
s16 attack_duration[2] = {40, 40}; // animation _speed * num of frames of animation

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
static void updatePlayerPosition(int player);
static void updateAnimationState(int player);
static void updateCamera(int player);
/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAtackTimer(int player);
static void controlIdleTimer(int player);
static void updateWalkAcceleration(int player);
static void controlMapBoundaries(int player);

static void addJumpImpulse(int player);
static void controlJumpAcceleration(int player);

static void controlHorizontalFlip(int player);
static void controlVerticalFlip(int player);

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
	player_state[PLAYER_1] = STATE_STILL;
	player_animation[PLAYER_1] = ANIM_STILL;

	SPR_setAnim(spr_players[PLAYER_1], player_animation[PLAYER_1]);

	while (1)
	{
		// VDP_waitVSync();
		controlHorizontalFlip(PLAYER_1);
		controlVerticalFlip(PLAYER_1);
		updateAnimationState(PLAYER_1);
		controlAtackTimer(PLAYER_1);
		controlIdleTimer(PLAYER_1);
		updateWalkAcceleration(PLAYER_1);
		addJumpImpulse(PLAYER_1);
		controlJumpAcceleration(PLAYER_1);
		updatePlayerPosition(PLAYER_1);
		controlMapBoundaries(PLAYER_1);
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

static void updateAnimationState(int player)
{
	if (joy_state[player].btn_left)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			player_order_x[player] = -1;
			if (player_speed_x[player] <= 5)
			{
				player_state[player] = STATE_WALK_LEFT;
				player_animation[player] = ANIM_WALK;
			}
			else
			{
				player_state[player] = STATE_RUN_LEFT;
				player_animation[player] = ANIM_RUN;
			}
		}
	}

	if (joy_state[player].btn_right)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			player_order_x[player] = 1;
			if (player_speed_x[player] <= 5)
			{
				player_state[player] = STATE_WALK_RIGHT;
				player_animation[player] = ANIM_WALK;
			}
			else
			{
				player_state[player] = STATE_RUN_RIGHT;
				player_animation[player] = ANIM_RUN;
			}
		}
	}
	if (joy_state[player].btn_up)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			player_state[player] = STATE_LOOK_UP;
			player_animation[player] = ANIM_UP;
		}
	}
	if (joy_state[player].btn_down)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			player_state[player] = STATE_CROUCH;
			player_animation[player] = ANIM_DOWN;
		}
	}
	if (joy_state[player].btn_b)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			attack_timer[player] += 1;
			player_state[player] = STATE_SPIN;
			player_animation[player] = ANIM_SPIN;
		}
	}
	if (joy_state[player].btn_a)
	{
		idle_timer[player] = 0;
		if (attack_timer[player] == 0)
		{
			player_order_y[player] = -1;
			attack_timer[player] += 1;
			player_state[player] = STATE_JUMP;
			player_animation[player] = ANIM_SPIN;
		}
	}

	if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right) && !(joy_state[player].btn_up) && !(joy_state[player].btn_down) && !(joy_state[player].btn_b) && !(joy_state[player].btn_a))
	{
		if (attack_timer[player] == 0)
		{
			if (player_speed_x[player] > 0)
			{
				if (player_speed_x[player] <= 5)
				{
					player_state[player] = STATE_STILL;
					player_animation[player] = ANIM_STILL;
				}
				else if (player_speed_x[player] > 5)
				{
					attack_timer[player] += 1;
					player_state[player] = STATE_STOP;
					player_animation[player] = ANIM_STOP;
				}
			}
			else if (player_speed_x[player] == 0)
			{
				if (idle_timer[player] == 0)
				{
					idle_timer[player] += 1;
				}

				if (idle_timer[player] < 300)
				{
					player_state[player] = STATE_STILL;
					player_animation[player] = ANIM_STILL;
				}
				else
				{
					player_state[player] = STATE_WAIT;
					player_animation[player] = ANIM_WAIT;
				}
			}
		}
	}
}

static void controlAtackTimer(int player)
{
	if (player_state[player] == STATE_SPIN)
	{
		attack_duration[player] = (s16)SPINNING;
	}
	else if (player_state[player] == STATE_STOP)
	{
		attack_duration[player] = (s16)STOPPING;
	}
	else if (player_state[player] == STATE_JUMP)
	{
		attack_duration[player] = (s16)JUMPING;
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
	if (idle_timer[player] > 0 && idle_timer[player] < idle_duration[player])
	{
		idle_timer[player] += 1;
	}
	else if (idle_timer[player] == idle_duration[player])
	{
		idle_timer[player] = 0;
	}
}

static void updatePlayerPosition(int player)
{
	if (player_order_x[player] > 0)
	{
		if (player_speed_x[player] > 0)
		{
			player_pos_x[player] += player_speed_x[player];
		}
	}
	else if (player_order_x[player] < 0)
	{
		if (player_pos_x[player] > 0 && player_speed_x[player] > 0)
		{
			player_pos_x[player] -= player_speed_x[player];
		}
	}
	// jump
	if (player_order_y[player] < 0)
	{
		if (player_speed_y[player] > 0)
		{
			player_pos_y[player] -= player_speed_y[player];
		}
	}
	else if (player_order_y[player] > 0)
	{
		if (player_speed_y[player] > 0)
		{
			player_pos_y[player] += player_speed_y[player];
		}
	}
}

static void updateWalkAcceleration(int player)
{
	if (player_state[player] == STATE_WALK_LEFT || player_state[player] == STATE_RUN_LEFT)
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

	else if (player_state[player] == STATE_WALK_RIGHT || player_state[player] == STATE_RUN_RIGHT)
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
	if (player_state[player] == STATE_JUMP)
	{
		if (player_speed_y[player] == 0)
		{
			// add impulse
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
				else
				{
					player_order_y[player] = 1;
				}
			}
		}
		else if (player_order_y[player] > 0)
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
			else if (player_speed_y[player] == 0)
			{
				player_order_y[player] = 0;
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