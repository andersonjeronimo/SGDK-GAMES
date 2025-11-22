#include <genesis.h>
#include <resources.h>

// axel & sonic
#define ANIM_STILL 0
#define ANIM_IDLE 1
#define ANIM_WALK 2
// axel
#define ANIM_UPPER 3
// sonic
#define ANIM_RUN 3
#define ANIM_STOP 4
#define ANIM_UP 5
#define ANIM_DOWN 6
#define ANIM_SPIN 7

#define PLAYER_1 0
#define PLAYER_2 1

int attack_timer = 0;
int attack_duration = 56; // 56 = animation speed * num of frames of animation

struct Controller
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
struct Controller controllers[2];

u16 ind = TILE_USER_INDEX;

Map *level_1_map;

// background(BGB)
s16 hscroll_offset_bg = 0;
s16 vscroll_offset_bg = 0;
u16 bg_x = 0;
u16 bg_y = -4;

// foreground(BGA)
//s16 hscroll_offset_fg = 0;
//s16 vscroll_offset_fg = 0;
// u16 fg_x = 0;
// u16 fg_y = -4;
u32 fg_x = 0;
u32 fg_y = 768;

Sprite *spr_players[2];
s16 pos_x[2] = {70, 30};
s16 pos_y[2] = {100, 130};

// bool p1_flipV = FALSE;
// bool p1_flipH = FALSE;

// Input
static void inputHandler(u16 joy, u16 changed, u16 state);
static void joyAnimationHandler(int player);
static void btnAnimationHandler(int player);

int main()
{
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
	MAP_scrollTo(level_1_map, fg_x, fg_y);

	PAL_setPalette(PAL2, sonic.palette->data, DMA);
	spr_players[PLAYER_1] = SPR_addSprite(&sonic, pos_x[PLAYER_1], pos_y[PLAYER_1], TILE_ATTR(PAL2, FALSE, FALSE, FALSE));

	SPR_setAnim(spr_players[PLAYER_1], ANIM_IDLE);

	while (1)
	{		
		// VDP_waitVSync();
		if (attack_timer == 0)
		{
			joyAnimationHandler(PLAYER_1);
			// joyAnimationHandler(PLAYER_2);
		}
		else if (attack_timer > 0 && attack_timer < attack_duration)
		{
			attack_timer += 1;
		}
		else if (attack_timer == attack_duration)
		{
			attack_timer = 0;
		}

		btnAnimationHandler(PLAYER_1);
		// btnAnimationHandler(PLAYER_2);

		SPR_setPosition(spr_players[PLAYER_1], pos_x[PLAYER_1], pos_y[PLAYER_1]);
		// SPR_setPosition(spr_players[PLAYER_2], pos_x[PLAYER_2], pos_y[PLAYER_2]);
		SPR_update();

		MAP_scrollTo(level_1_map, fg_x, fg_y);
		fg_x += 3;

		VDP_setHorizontalScroll(BG_B, hscroll_offset_bg);
		hscroll_offset_bg -= 1;

		// VDP_setHorizontalScroll(BG_A, hscroll_offset_fg);
		// hscroll_offset_fg -= 2;

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
			controllers[PLAYER_1].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			controllers[PLAYER_1].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			controllers[PLAYER_1].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			controllers[PLAYER_1].btn_right = FALSE;

		if (state & BUTTON_UP)
			controllers[PLAYER_1].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			controllers[PLAYER_1].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			controllers[PLAYER_1].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			controllers[PLAYER_1].btn_down = FALSE;

		if (state & BUTTON_A)
			controllers[PLAYER_1].btn_a = TRUE;
		else if (changed & BUTTON_A)
			controllers[PLAYER_1].btn_a = FALSE;

		if (state & BUTTON_B)
			controllers[PLAYER_1].btn_b = TRUE;
		else if (changed & BUTTON_B)
			controllers[PLAYER_1].btn_b = FALSE;

		if (state & BUTTON_C)
			controllers[PLAYER_1].btn_c = TRUE;
		else if (changed & BUTTON_C)
			controllers[PLAYER_1].btn_c = FALSE;

		if (state & BUTTON_X)
			controllers[PLAYER_1].btn_x = TRUE;
		else if (changed & BUTTON_X)
			controllers[PLAYER_1].btn_x = FALSE;

		if (state & BUTTON_Y)
			controllers[PLAYER_1].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			controllers[PLAYER_1].btn_y = FALSE;

		if (state & BUTTON_Z)
			controllers[PLAYER_1].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			controllers[PLAYER_1].btn_z = FALSE;

		if (state & BUTTON_START)
			controllers[PLAYER_1].btn_start = TRUE;
		else if (changed & BUTTON_START)
			controllers[PLAYER_1].btn_start = FALSE;

		if (state & BUTTON_MODE)
			controllers[PLAYER_1].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			controllers[PLAYER_1].btn_mode = FALSE;
	}

	if (joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
			controllers[PLAYER_2].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			controllers[PLAYER_2].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			controllers[PLAYER_2].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			controllers[PLAYER_2].btn_right = FALSE;

		if (state & BUTTON_UP)
			controllers[PLAYER_2].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			controllers[PLAYER_2].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			controllers[PLAYER_2].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			controllers[PLAYER_2].btn_down = FALSE;

		if (state & BUTTON_A)
			controllers[PLAYER_2].btn_a = TRUE;
		else if (changed & BUTTON_A)
			controllers[PLAYER_2].btn_a = FALSE;

		if (state & BUTTON_B)
			controllers[PLAYER_2].btn_b = TRUE;
		else if (changed & BUTTON_B)
			controllers[PLAYER_2].btn_b = FALSE;

		if (state & BUTTON_C)
			controllers[PLAYER_2].btn_c = TRUE;
		else if (changed & BUTTON_C)
			controllers[PLAYER_2].btn_c = FALSE;

		if (state & BUTTON_X)
			controllers[PLAYER_2].btn_x = TRUE;
		else if (changed & BUTTON_X)
			controllers[PLAYER_2].btn_x = FALSE;

		if (state & BUTTON_Y)
			controllers[PLAYER_2].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			controllers[PLAYER_2].btn_y = FALSE;

		if (state & BUTTON_Z)
			controllers[PLAYER_2].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			controllers[PLAYER_2].btn_z = FALSE;

		if (state & BUTTON_START)
			controllers[PLAYER_2].btn_start = TRUE;
		else if (changed & BUTTON_START)
			controllers[PLAYER_2].btn_start = FALSE;

		if (state & BUTTON_MODE)
			controllers[PLAYER_2].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			controllers[PLAYER_2].btn_mode = FALSE;
	}
}

static void joyAnimationHandler(int player)
{
	if (controllers[player].btn_left)
	{
		SPR_setAnim(spr_players[player], ANIM_WALK);
		SPR_setHFlip(spr_players[player], TRUE);
		pos_x[player] -= 1;
	}

	if (controllers[player].btn_right)
	{
		SPR_setAnim(spr_players[player], ANIM_WALK);
		SPR_setHFlip(spr_players[player], FALSE);
		pos_x[player] += 1;
	}

	if (controllers[player].btn_up)
	{
		SPR_setAnim(spr_players[player], ANIM_UP);
		pos_y[player] -= 1;
	}

	if (controllers[player].btn_down)
	{
		SPR_setAnim(spr_players[player], ANIM_DOWN);
		pos_y[player] += 1;
	}

	if (!(controllers[player].btn_left) && !(controllers[player].btn_right) && !(controllers[player].btn_up) && !(controllers[player].btn_down))
	{
		SPR_setAnim(spr_players[player], ANIM_IDLE);
	}
}

static void btnAnimationHandler(int player)
{
	if (controllers[player].btn_b)
	{
		if (attack_timer == 0)
		{
			SPR_setAnim(spr_players[player], ANIM_SPIN);
			attack_timer += 1;
		}
	}
}