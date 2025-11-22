#include <genesis.h>
#include <resources.h>

// https://gameprogrammingpatterns.com/state.html

#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
#define ANIM_RUN 3
#define ANIM_STOP 4
#define ANIM_UP 5
#define ANIM_DOWN 6
#define ANIM_SPIN 7

#define PLAYER_1 0
#define PLAYER_2 1

// How far to (left, right, up, down) before camera moves
// camera size = 256 x 224
#define HOW_FAR_TO_LEFT 64
#define HOW_FAR_TO_RIGHT 128
#define HOW_FAR_TO_TOP 56
#define HOW_FAR_TO_BOTTON 112

// #define HORIZONTAL_RESOLUTION 320
#define HORIZONTAL_RESOLUTION 256
#define VERTICAL_RESOLUTION 224

#define MAP_WIDTH 10240
#define MAP_HEIGTH 1280

// SONIC
#define PLAYER_WIDTH 64
#define PLAYER_HEIGTH 48

Map *level_1_map;
u16 ind = TILE_USER_INDEX;

// background(BGB)
s16 hscroll_offset_bg = 0;
s16 vscroll_offset_bg = 0;
u16 bg_x = 0;
u16 bg_y = -4;

// foreground(BGA)
// s16 hscroll_offset_fg = 0;
// s16 vscroll_offset_fg = 0;
// u16 fg_x = 0;
// u16 fg_y = -4;

int int_cur_camera_x = 0;	// F32_toInt(fix32_pos_x[PLAYER_1]) - 140;
int int_cur_camera_y = 750; // F32_toInt(fix32_pos_y[PLAYER_1]) - 80;
// u32 u32_cur_camera_x = 0;
// u32 u32_cur_camera_y = 768;

Sprite *spr_players[2];							 // screen res = 320 x 224
fix32 fix32_pos_x[2] = {FIX32(160), FIX32(160)}; // camera X + 160 (320/2)
fix32 fix32_pos_y[2] = {FIX32(862), FIX32(862)}; // camera Y + 112 (224/2)
// ou
// s16 s16_pos_x[2] = {70, 30};
// s16 s16_pos_y[2] = {100, 130};
bool flip_v[2] = {FALSE, FALSE};
bool flip_h[2] = {FALSE, FALSE};
fix32 fix32_vel[2] = {FIX32(1.5), FIX32(1.5)};
fix32 impulse_x[2]; // running impulse
fix32 impulse_y[2]; // jumping impulse
fix32 maxSpeed_x[2];
fix32 speed_x[2];
fix32 speed_y[2];

int attack_timer[2] = {0, 0};
int attack_duration = 40; // 56 = animation speed * num of frames of animation

enum PLAYER_STATE
{
	STATE_STILL,
	STATE_IDLE,
	STATE_WALK_LEFT,
	STATE_WALK_RIGHT,
	STATE_RUN_LEFT,
	STATE_RUN_RIGHT,
	STATE_STOP,
	STATE_LOOK_UP,
	STATE_CROUCH,
	STATE_SPIN
};
enum PLAYER_STATE player_state[2] = {STATE_IDLE, STATE_IDLE};
s16 player_animation[2] = {ANIM_IDLE, ANIM_IDLE};


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
static void joyAnimationHandler(int player);
// static void btnAnimationHandler(int player);
static void cameraHandler(int player);
static void playerStateHandler(int player);
static void startAtackTimer(int player);

// TO DO
/* static void handleWalkAcceleration(struct Player *player, int playerID);
static void handleJumpAcceleration(struct Player *player, int playerID);
static void handleWalkImpulse(struct Player *player, int playerID);
static void handleJumpImpulse(struct Player *player, int playerID); */
// END OF TO DO

int main()
{
	VDP_setScreenWidth256();
	VDP_setScreenHeight224();
	SPR_init();
	JOY_init();
	JOY_setEventHandler(&inputHandler);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

	PAL_setPalette(PAL0, background.palette->data, DMA);
	VDP_drawImageEx(BG_B, &background, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), bg_x, bg_y, FALSE, TRUE);
	ind += background.tileset->numTile;

	VDP_loadTileSet(&level_tileset, ind, DMA);
	PAL_setPalette(PAL1, level_palette.data, DMA);
	level_1_map = MAP_create(&level_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	ind += level_tileset.numTile;

	MAP_scrollTo(level_1_map, int_cur_camera_x, int_cur_camera_y);

	PAL_setPalette(PAL2, sonic.palette->data, DMA);
	spr_players[PLAYER_1] = SPR_addSprite(&sonic, F32_toInt(fix32_pos_x[PLAYER_1]), F32_toInt(fix32_pos_y[PLAYER_1]), TILE_ATTR(PAL2, FALSE, flip_v[PLAYER_1], flip_h[PLAYER_1]));
	// or using s16 type
	// spr_players[PLAYER_1] = SPR_addSprite(&sonic, s16_pos_x[PLAYER_1], s16_pos_y[PLAYER_1], TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

	SPR_setAnim(spr_players[PLAYER_1], player_animation[PLAYER_1]);

	while (1)
	{
		// VDP_waitVSync();
		playerStateHandler(PLAYER_1);
		joyAnimationHandler(PLAYER_1);
		startAtackTimer(PLAYER_1);
		cameraHandler(PLAYER_1);

		SPR_setAnim(spr_players[PLAYER_1], player_animation[PLAYER_1]);
		SPR_setHFlip(spr_players[PLAYER_1], flip_h[PLAYER_1]);
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

static void playerStateHandler(int player)
{
	if (joy_state[player].btn_left)
		player_state[player] = STATE_WALK_LEFT;
	else if (joy_state[player].btn_right)
		player_state[player] = STATE_WALK_RIGHT;
	else if (joy_state[player].btn_up)
		player_state[player] = STATE_LOOK_UP;
	else if (joy_state[player].btn_down)
		player_state[player] = STATE_CROUCH;
	else if (joy_state[player].btn_b)
		player_state[player] = STATE_SPIN;
	else if (!(joy_state[player].btn_left) && !(joy_state[player].btn_right) && !(joy_state[player].btn_up) && !(joy_state[player].btn_down) && !(joy_state[player].btn_b))
	{
		player_state[player] = STATE_IDLE;
	}
}

static void joyAnimationHandler(int player)
{
	int state = player_state[player];
	switch (state)
	{
	case STATE_WALK_LEFT:
		if (attack_timer[player] == 0)
		{
			if (!flip_h[player])
				flip_h[player] = TRUE;
			player_animation[player] = ANIM_WALK;
			if (fix32_pos_x[player] < FIX32(0))
				fix32_pos_x[player] = FIX32(0);
			else
				fix32_pos_x[player] -= fix32_vel[player];
		}
		break;

	case STATE_RUN_LEFT:
		if (attack_timer[player] == 0)
		{
			if (!flip_h[player])
				flip_h[player] = TRUE;
			player_animation[player] = ANIM_RUN;
			if (fix32_pos_x[player] < FIX32(0))
				fix32_pos_x[player] = FIX32(0);
			else
				fix32_pos_x[player] -= fix32_vel[player];
		}
		break;

	case STATE_WALK_RIGHT:
		if (attack_timer[player] == 0)
		{
			if (flip_h[player])
				flip_h[player] = FALSE;
			player_animation[player] = ANIM_WALK;
			if (fix32_pos_x[player] > FIX32(MAP_WIDTH - PLAYER_WIDTH))
				fix32_pos_x[player] = FIX32(MAP_WIDTH - PLAYER_WIDTH);
			else
				fix32_pos_x[player] += fix32_vel[player];
		}
		break;

	case STATE_RUN_RIGHT:
		if (attack_timer[player] == 0)
		{
			if (flip_h[player])
				flip_h[player] = FALSE;
			player_animation[player] = ANIM_RUN;
			if (fix32_pos_x[player] > FIX32(MAP_WIDTH - PLAYER_WIDTH))
				fix32_pos_x[player] = FIX32(MAP_WIDTH - PLAYER_WIDTH);
			else
				fix32_pos_x[player] += fix32_vel[player];
		}
		break;

	case STATE_LOOK_UP:
		if (attack_timer[player] == 0)
		{
			player_animation[player] = ANIM_UP;
			if (fix32_pos_y[player] < FIX32(0))
				fix32_pos_y[player] = FIX32(0);
			else
				fix32_pos_y[player] -= fix32_vel[player];
		}
		break;

	case STATE_CROUCH:
		if (attack_timer[player] == 0)
		{
			player_animation[player] = ANIM_DOWN;
			if (fix32_pos_y[player] > FIX32(MAP_HEIGTH - PLAYER_HEIGTH))
				fix32_pos_y[player] = FIX32(MAP_HEIGTH - PLAYER_HEIGTH);
			else
				fix32_pos_y[player] += fix32_vel[player];
		}
		break;

	case STATE_SPIN:
		if (attack_timer[player] == 0)
		{
			player_animation[player] = ANIM_SPIN;
			attack_timer[player] += 1;
		}
		break;

	case STATE_IDLE:
		if (attack_timer[player] == 0)
		{
			player_animation[player] = ANIM_IDLE;
		}
		break;

	default:
		break;
	}
}

static void startAtackTimer(int player)
{
	if (attack_timer[player] > 0 && attack_timer[player] < attack_duration)
	{
		attack_timer[player] += 1;
	}
	else if (attack_timer[player] == attack_duration)
	{
		attack_timer[player] = 0;
	}
}

static void cameraHandler(int player)
{
	int player_x_on_map = F32_toInt(fix32_pos_x[player]);
	int player_y_on_map = F32_toInt(fix32_pos_y[player]);

	int player_x_on_screen = (player_x_on_map - int_cur_camera_x);
	int player_y_on_screen = (player_y_on_map - int_cur_camera_y);

	int update_camera_x;
	int update_camera_y;

	if (player_x_on_screen > HOW_FAR_TO_RIGHT)
		update_camera_x = player_x_on_map - HOW_FAR_TO_RIGHT;
	else if (player_x_on_screen < HOW_FAR_TO_LEFT)
		update_camera_x = player_x_on_map - HOW_FAR_TO_LEFT;
	else
		update_camera_x = int_cur_camera_x;

	if (player_y_on_screen > HOW_FAR_TO_BOTTON)
		update_camera_y = player_y_on_map - HOW_FAR_TO_BOTTON;
	else if (player_y_on_screen < HOW_FAR_TO_TOP)
		update_camera_y = player_y_on_map - HOW_FAR_TO_TOP;
	else
		update_camera_y = int_cur_camera_y;

	// stop camera from going beyond map boudaries
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
	if ((int_cur_camera_x != update_camera_x) || (int_cur_camera_y != update_camera_y))
	{
		if (int_cur_camera_x < update_camera_x)
		{
			hscroll_offset_bg -= 1;
			VDP_setHorizontalScroll(BG_B, hscroll_offset_bg);
		}
		if (int_cur_camera_x > update_camera_x)
		{
			hscroll_offset_bg += 1;
			VDP_setHorizontalScroll(BG_B, hscroll_offset_bg);
		}

		int_cur_camera_x = update_camera_x;
		int_cur_camera_y = update_camera_y;
		MAP_scrollTo(level_1_map, update_camera_x, update_camera_y);
	}

	SPR_setPosition(spr_players[player], F32_toInt(fix32_pos_x[player]) - update_camera_x, F32_toInt(fix32_pos_y[player]) - update_camera_y);
};

//