#include <genesis.h>
#include <resources.h>

// make GDK=/path/to/sgdk -f /path/to/sgdk/makefile_wine.gen
// make GDK=${HOME}/SGDK -f ${HOME}/SGDK/makefile_wine.gen

// https://gameprogrammingpatterns.com/state.html
// https://www.youtube.com/watch?v=xldCreKST3s
// OHSAT Games Tutorials
// https://www.ohsat.com/tutorial/mdmisc/sor-style-text-crawl/
// PIGSY Retro Game Sega Genesis Tutotials
// https://www.youtube.com/watch?v=BnGqc5OTTY4&list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb

// VDP_setTileMapXY() and VDP_fillTileMapRect(): You can dynamically draw or change individual tiles or sections
// of a plane's tilemap in real time using these functions.
// This is how new map data is loaded into the VDP as the player moves through large levels
//(often combined with an understanding of how planes wrap around at their boundaries).

#define COUNTER_LIMIT 5
// 320 X 224
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224

// map
#define MAP_WIDTH 640
#define MAP_HEIGTH 448
// screen
#define LEFT_SCREEN_LIMIT 0
#define RIGHT_SCREEN_LIMIT MAP_WIDTH - HORIZONTAL_RESOLUTION // VDP_getScreenWidth()
#define TOP_SCREEN_LIMIT 0
#define BOTTON_SCREEN_LIMIT MAP_HEIGTH - VERTICAL_RESOLUTION // VDP_getScreenHeight()

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
#define PLAYER_1_WIDTH 96
#define PLAYER_1_HEIGTH 64

// player collision box offset
#define BOX_LEFT_OFFSET 40
#define BOX_RIGHT_OFFSET 40
#define BOX_TOP_OFFSET 28
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

int min_x_coord[2] = {(MIN_POS_X - BOX_LEFT_OFFSET), (MIN_POS_X - BOX_LEFT_OFFSET)};
int max_x_coord[2] = {MAX_POS_X, MAX_POS_X};
int min_y_coord[2] = {(MIN_POS_Y - BOX_TOP_OFFSET), (MIN_POS_Y - BOX_TOP_OFFSET)};
int max_y_coord[2] = {(MAX_POS_Y - PLAYER_1_HEIGTH) + BOX_BOTTON_OFFSET, MAX_POS_Y};
// para imprimir coordenadas
char buffer_a[100];
char buffer_b[100];
char buffer_c[100];
char buffer_d[100];

#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
#define MATRIX_MAX_LIN_INDEX 27
#define MATRIX_MAX_COL_INDEX 39

// pilar = 32 x 64
// cubo = 32 x 32
// piso = 40 x 48
// solo = 224 - 32

const int BGA_COLLISION_MATRIX[28][40] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

#define ANIM_STANDING 0
#define ANIM_WALK 1
#define ANIM_RUN 2
#define ANIM_JUMP 3
#define ANIM_ATTACK_1 4
#define ANIM_ATTACK_2 5
#define ANIM_ATTACK_3 6
#define ANIM_DEFEND 7
#define ANIM_HURT 8
#define ANIM_DEATH 9

enum PlayerState
{
	STATE_STANDING,
	STATE_WALK,
	STATE_RUN,
	STATE_JUMP,
	STATE_FALL,
	STATE_FALL_LOOP,
	STATE_ATTACK_1,
	STATE_ATTACK_2,
	STATE_ATTACK_3,
	STATE_DEFEND,
	STATE_HURT,
	STATE_DEATH
};

enum TimeOfAnimation
{
	TIME_ATTACK_1 = 60,
	TIME_ATTACK_2 = 50,
	TIME_ATTACK_3 = 60
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
	fix16 pos_x;
	fix16 pos_y;
	bool is_visible;
	bool is_active;
	fix16 width;
	fix16 height;
	bool flip_h;
	bool flip_v;
	fix16 order_x;
	fix16 order_y;
	fix16 impulse_x;
	fix16 impulse_y;
	fix16 speed_x;
	fix16 speed_y;
	fix16 max_speed_x;
	fix16 max_speed_y;
	int trap_switch_id;
} SpriteElement;

typedef struct
{
	Sprite *sprite;
	fix16 pos_x;
	fix16 pos_y;
	bool is_active;
	int spr_element_id;
} TrapSwitch;

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
u16 base_tile_index[3]; // BGA, BGB, WINDOW
Map *bga;
Map *bgb;
Map *title;
// Background bg[2]; // BGA & BGB
Camera camera;
Player player[2];
SpriteElement spr_element[10]; // max sprites
TrapSwitch trap_switch[10];
Joystick joystick[2];

static void gameInputHandler(u16 joy, u16 changed, u16 state);

static void finiteStateMachine(int player_id);
static void updatePlayerPosition(int player_id);

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

// CAMERA
// static void controlMapBoundaries(int player_id);
static void updateCamera(int player_id);

// SPRITES
static void controlTrap(int trap_switch_id);
static void controlProjectile(int sprite_id);

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
Sprite *ptr_cursor;
static void updateCursorPosition();
static void moveUp();
static void moveDown();
static void select(u16 Option);
static void pickStart();
static void pickOptions();
static void pickExit();

int main(bool resetType)
{
	// Soft resets don't clear RAM, this can bring some bugs so we hard reset every time we detect a soft reset
	if (!resetType)
		SYS_hardReset();

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

static void updateCursorPosition()
{
	SPR_setPosition(ptr_cursor, options[currentIndex].x * 8 - TILE_IN_PIXELS, options[currentIndex].y * 8);
}

static void moveUp()
{
	if (currentIndex > 0)
	{
		currentIndex--;
		updateCursorPosition();
	}
}

static void moveDown()
{
	if (currentIndex < NUM_OPTIONS - 1)
	{
		currentIndex++;
		updateCursorPosition();
	}
}

static void select(u16 Option)
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

static void pickStart()
{
	current_game_state = GAME;
}

static void pickOptions()
{
	current_game_state = MENU;
}

static void pickExit()
{
	current_game_state = TITLE;
}

static void releaseMemory()
{
	SPR_reset();
	SPR_update();
	VDP_clearPlane(BG_A, TRUE);
	VDP_clearPlane(BG_B, TRUE);
	VDP_clearPlane(WINDOW, TRUE);
}

static void processGameTitle()
{
	// u16 palette[64];
	// PAL_setColors(0, (u16 *)palette_black, 64, CPU);
	//// prepare palettes
	// memcpy(&palette[0], title_palette.data, 16 * 2); // PAL0 index
	//// memcpy(&palette[16], cursor.palette->data, 16 * 2); //PAL1 index
	//// memcpy(&palette[32], hero.palette->data, 16 * 2); //PAL2 index
	// memcpy(&palette[48], key.palette->data, 16 * 2); // PAL3 index
	//
	//// fade in
	// PAL_fadeIn(0, (4 * 16) - 1, palette, 20, TRUE);

	ind = TILE_USER_INDEX;
	base_tile_index[BG_B] = ind;
	VDP_loadTileSet(&title_tileset, base_tile_index[BG_B], DMA);
	PAL_setPalette(PAL0, title_palette.data, DMA);
	title = MAP_create(&title_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(title, 0, 0);

	// ind += title_tileset.numTile;
	// base_tile_index[BG_A] = ind;

	// MENU
	ptr_cursor = SPR_addSprite(&key, 0, 0, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
	PAL_setPalette(PAL3, key.palette->data, DMA);

	// Draw options
	VDP_setTextPriority(TRUE);
	PAL_setColor(15, RGB24_TO_VDPCOLOR(0xffff00));
	VDP_drawTextBG(BG_A, options[0].label, options[0].x, options[0].y);
	VDP_drawTextBG(BG_A, options[1].label, options[1].x, options[1].y);
	VDP_drawTextBG(BG_A, options[2].label, options[2].x, options[2].y);

	while (current_game_state == TITLE)
	{
		updateCursorPosition();
		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	VDP_clearTextBG(BG_A, options[0].x, options[0].y, 20);
	VDP_clearTextBG(BG_A, options[1].x, options[1].y, 20);
	VDP_clearTextBG(BG_A, options[2].x, options[2].y, 20);
	MEM_free(title);
	MEM_free(ptr_cursor);
	releaseMemory();
}

static void processMainGame()
{
	ind = TILE_USER_INDEX;

	// u16 palette[64];
	// PAL_setColors(0, (u16 *)palette_black, 64, CPU);
	//// prepare palettes
	// memcpy(&palette[0], bgb_palette.data, 16 * 2);		// PAL0 index
	// memcpy(&palette[16], bga_palette.data, 16 * 2);		// PAL1 index
	// memcpy(&palette[32], knight.palette->data, 16 * 2); // PAL2 index
	// memcpy(&palette[48], ball.palette->data, 16 * 2);	// PAL3 index
	//
	//// fade in
	// PAL_fadeIn(0, (4 * 16) - 1, palette, 20, TRUE);

	VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
	// init
	// BGA (MAP)
	camera.cur_pos_x = 0;
	camera.cur_pos_y = 0;

	base_tile_index[BG_A] = ind;

	VDP_loadTileSet(&bga_tileset, base_tile_index[BG_A], DMA);
	PAL_setPalette(PAL1, bga_palette.data, DMA);
	bga = MAP_create(&bga_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, base_tile_index[BG_A]));
	MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);

	// update index
	ind += bga_tileset.numTile;
	base_tile_index[BG_B] = ind;

	VDP_loadTileSet(&bgb_tileset, base_tile_index[BG_B], DMA);
	PAL_setPalette(PAL0, bgb_palette.data, DMA);
	bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, base_tile_index[BG_B]));
	MAP_scrollTo(bgb, camera.cur_pos_x, camera.cur_pos_y);

	// ind += bgb_tileset.numTile;
	// base_tile_index[WINDOW] = ind;

	player[ONE].state = STATE_STANDING;
	player[ONE].width = PLAYER_1_WIDTH;
	player[ONE].height = PLAYER_1_HEIGTH;
	player[ONE].anim = ANIM_STANDING;
	player[ONE].is_full_anim = TRUE;
	player[ONE].flip_v = FALSE;
	player[ONE].flip_h = FALSE;
	player[ONE].impulse_x = 4;
	player[ONE].impulse_y = 4;
	player[ONE].max_speed_x = 2;
	player[ONE].max_speed_y = 4;
	player[ONE].order_x = NEUTRAL;
	player[ONE].order_y = NEUTRAL;
	player[ONE].last_order_x = RIGHT;
	player[ONE].last_order_y = NEUTRAL;
	player[ONE].pos_x = HOW_FAR_TO_LEFT;
	player[ONE].pos_y = MIN_POS_Y - PLAYER_1_WIDTH;
	player[ONE].speed_x = 0;
	player[ONE].speed_y = 0;
	player[ONE].is_attacking = FALSE;
	player[ONE].has_stamina = TRUE;
	player[ONE].sprite = SPR_addSprite(&knight, player[ONE].pos_x, player[ONE].pos_y, TILE_ATTR(PAL2, FALSE, player[ONE].flip_v, player[ONE].flip_h));
	PAL_setPalette(PAL2, knight.palette->data, DMA_QUEUE);
	SPR_setAnim(player[ONE].sprite, player[ONE].anim);

	// ponteiro para array
	// considerando uma struct (SpriteElement) e um array (spr_elements[10])
	// SpriteElement spr_elements[10]; // 10 max sprites

	// forma 1: apontar para 1º elemento
	// SpriteElement* ptr_elements = spr_elements;
	// Modificar o 1º elemento
	// ptr_elements->flip_h = TRUE;
	// então alterar o índice:
	// ptr_element++
	// ptr_elements->flip_h = TRUE; <--esse é o 2º elemento do array

	// forma 2: apontar para todo array
	// SpriteElement (*ptr_elements)[10] = &spr_elements;
	// acessando os índices (*ptr_elements)[5].flip_h = TRUE;

	// Other sprites...
	spr_element[0].is_active = FALSE;
	spr_element[0].is_visible = FALSE;
	spr_element[0].pos_x = MAX_POS_X - 16;
	spr_element[0].pos_y = 16;
	spr_element[0].width = 16;
	spr_element[0].height = 16;
	spr_element[0].flip_v = FALSE;
	spr_element[0].flip_h = TRUE;
	spr_element[0].sprite = SPR_addSprite(&ball, spr_element[0].pos_x, spr_element[0].pos_y, TILE_ATTR(PAL3, FALSE, spr_element[0].flip_v, spr_element[0].flip_h));
	spr_element[0].order_x = LEFT;
	spr_element[0].order_y = NEUTRAL;
	spr_element[0].speed_x = 1;
	spr_element[0].speed_y = 1;
	spr_element[0].trap_switch_id = 0;
	PAL_setPalette(PAL3, ball.palette->data, DMA_QUEUE);	
	SPR_setVisibility(spr_element[0].sprite, HIDDEN);

	trap_switch[0].is_active = FALSE;
	trap_switch[0].pos_x = 160;
	trap_switch[0].pos_y = 48;
	trap_switch[0].sprite = SPR_addSprite(&brick, trap_switch[0].pos_x, trap_switch[0].pos_y, TILE_ATTR(PAL1, FALSE, FALSE, FALSE));
	// PAL_setPalette(PAL3, key.palette->data, DMA);
	trap_switch[0].spr_element_id = 0;

	while (current_game_state == GAME)
	{
		finiteStateMachine(ONE);
		updateCamera(ONE);
		updatePlayerPosition(ONE);
		// controlMapBoundaries(ONE);
		controlPlayerMapCollision(ONE);
		checkBottonCollision(ONE);
		checkTopCollision(ONE);
		controlHorizontalFlip(ONE);
		// controlVerticalFlip(ONE);
		controlXAcceleration(ONE);
		controlYAcceleration(ONE);
		controlAttackTimer(ONE);

		controlTrap(0);
		controlProjectile(0);

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
		SPR_setPosition(trap_switch[0].sprite, (trap_switch[0].pos_x - camera.cur_pos_x), (trap_switch[0].pos_y - camera.cur_pos_y));

		if (player[ONE].pos_x > trap_switch[0].pos_x && player[ONE].pos_y < trap_switch[0].pos_y)
		{
			trap_switch[0].is_active = TRUE;
		}
		else if (player[ONE].pos_x < trap_switch[0].pos_x && player[ONE].pos_y < trap_switch[0].pos_y)
		{
			trap_switch[0].is_active = FALSE;
		}

		if (spr_element[0].is_active)
		{
			SPR_setPosition(spr_element[0].sprite, (spr_element[0].pos_x - camera.cur_pos_x), (spr_element[0].pos_y - camera.cur_pos_y));
		}

		MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
		MAP_scrollTo(bgb, camera.cur_pos_x, camera.cur_pos_y);

		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	MEM_free(bga);
	MEM_free(bgb);
	MEM_free(player[ONE].sprite);
	MEM_free(spr_element[0].sprite);
	MEM_free(trap_switch[0].sprite);
	releaseMemory();
}

static void controlTrap(int trap_switch_id)
{
	if (trap_switch[trap_switch_id].is_active)
	{
		int id = trap_switch[trap_switch_id].spr_element_id;
		spr_element[id].is_active = TRUE;
		spr_element[id].is_visible = TRUE;
		SPR_setVisibility(spr_element[id].sprite, VISIBLE);
	}
	else if (!trap_switch[trap_switch_id].is_active)
	{
		int id = trap_switch[trap_switch_id].spr_element_id;
		spr_element[id].is_active = FALSE;
		spr_element[id].is_visible = FALSE;
		SPR_setVisibility(spr_element[id].sprite, HIDDEN);
	}
}

static void controlProjectile(int sprite_id)
{
	if (spr_element[sprite_id].is_active)
	{
		if (spr_element[sprite_id].order_x == LEFT)
		{
			spr_element[sprite_id].pos_x -= spr_element[sprite_id].speed_x;
			if (spr_element[sprite_id].pos_x <= MIN_POS_X)
			{
				spr_element[sprite_id].pos_x = MAX_POS_X - 16;
			}
		}
		else if (spr_element[sprite_id].order_x == RIGHT)
		{
			spr_element[sprite_id].pos_x += spr_element[sprite_id].speed_x;
			if ((spr_element[sprite_id].pos_x + spr_element[sprite_id].width) >= MAX_POS_X)
			{
				spr_element[sprite_id].pos_x = MIN_POS_X;
			}
		}
	}
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
			player[player_id].anim = ANIM_JUMP;
			player[player_id].is_full_anim = FALSE;
			player[player_id].frame = 4;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}
		if (joystick[player_id].btn_left)
		{
			player[player_id].order_x = LEFT;
			player[player_id].last_order_x = LEFT;
			player[player_id].state = STATE_WALK;
			player[player_id].anim = ANIM_WALK;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		else if (joystick[player_id].btn_right)
		{
			player[player_id].order_x = RIGHT;
			player[player_id].last_order_x = RIGHT;
			player[player_id].state = STATE_WALK;
			player[player_id].anim = ANIM_WALK;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
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

		if (joystick[player_id].btn_x /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (joystick[player_id].btn_y /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_DEFEND;
			player[player_id].anim = ANIM_DEFEND;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}
		break;

	case STATE_WALK:
		player[player_id].max_speed_x = (player[player_id].impulse_x / 2);
		if (player[player_id].order_y == DOWN)
		{
			player[player_id].state = STATE_FALL;
			player[player_id].anim = ANIM_JUMP;
			player[player_id].is_full_anim = FALSE;
			player[player_id].frame = 4;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
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
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}
		if (joystick[player_id].btn_c)
		{
			player[player_id].max_speed_x = player[player_id].impulse_x;
			player[player_id].state = STATE_RUN;
			player[player_id].anim = ANIM_RUN;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}

		if (joystick[player_id].btn_x /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (joystick[player_id].btn_y /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_DEFEND;
			player[player_id].anim = ANIM_DEFEND;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
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
			player[player_id].anim = ANIM_JUMP;
			player[player_id].is_full_anim = FALSE;
			player[player_id].frame = 4;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
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
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (joystick[player_id].btn_x /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			player[player_id].is_attacking = TRUE;
			// player[player_id].has_stamina = FALSE;
			anim_duration[player_id] = TIME_ATTACK_1;
			player[player_id].state = STATE_ATTACK_1;
			player[player_id].anim = ANIM_ATTACK_1;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (joystick[player_id].btn_y /*  && player[player_id].has_stamina */)
		{
			player[player_id].order_x = NEUTRAL;
			// player[player_id].has_stamina = FALSE;
			player[player_id].state = STATE_DEFEND;
			player[player_id].anim = ANIM_DEFEND;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
		}

		if (!joystick[player_id].btn_c)
		{
			// player[player_id].speed_x = (player[player_id].impulse_x / 2);
			player[player_id].anim = ANIM_WALK;
			player[player_id].state = STATE_WALK;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}

		if (!(joystick[player_id].btn_left) && !(joystick[player_id].btn_right))
		{
			player[player_id].anim = ANIM_WALK;
			player[player_id].state = STATE_WALK;
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
			player[player_id].anim = ANIM_JUMP;
			player[player_id].is_full_anim = FALSE;
			player[player_id].frame = 4;
			SPR_setAnimationLoop(player[player_id].sprite, FALSE);
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
				player[player_id].anim = ANIM_JUMP;
				player[player_id].is_full_anim = FALSE;
				player[player_id].frame = 4;
				SPR_setAnimationLoop(player[player_id].sprite, FALSE);
			}
		}
		else if (player[player_id].order_y == NEUTRAL)
		{
			if (player[player_id].speed_x > 0)
			{
				player[player_id].anim = ANIM_RUN;
				player[player_id].state = STATE_RUN;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
			else
			{
				player[player_id].state = STATE_STANDING;
				player[player_id].anim = ANIM_STANDING;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
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
				SPR_setAnimationLoop(player[player_id].sprite, FALSE);
			}
			else
			{
				player[player_id].state = STATE_STANDING;
				player[player_id].anim = ANIM_STANDING;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_2:
		if (!player[player_id].is_attacking)
		{
			if (joystick[player_id].btn_x)
			{
				player[player_id].order_x = NEUTRAL;
				player[player_id].is_attacking = TRUE;
				// player[player_id].has_stamina = FALSE;
				anim_duration[player_id] = TIME_ATTACK_3;
				player[player_id].state = STATE_ATTACK_3;
				player[player_id].anim = ANIM_ATTACK_3;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, FALSE);
			}
			else
			{
				player[player_id].state = STATE_STANDING;
				player[player_id].anim = ANIM_STANDING;
				player[player_id].is_full_anim = TRUE;
				SPR_setAnimationLoop(player[player_id].sprite, TRUE);
			}
		}
		break;
	case STATE_ATTACK_3:
		if (!player[player_id].is_attacking)
		{
			player[player_id].state = STATE_STANDING;
			player[player_id].anim = ANIM_STANDING;
			player[player_id].is_full_anim = TRUE;
			SPR_setAnimationLoop(player[player_id].sprite, TRUE);
		}
		break;

	case STATE_DEFEND:
		if (!joystick[player_id].btn_y)
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
		if (player[player_id].last_order_x == LEFT || player[player_id].order_x == LEFT)
		{
			if (player[player_id].pos_x > min_x_coord[player_id])
			{
				player[player_id].pos_x -= player[player_id].speed_x;
			}
		}
		else if (player[player_id].last_order_x == RIGHT || player[player_id].order_x == RIGHT)
		{
			if (player[player_id].pos_x < max_x_coord[player_id])
			{
				player[player_id].pos_x += player[player_id].speed_x;
			}
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
		}
		else if (player[player_id].order_y == DOWN)
		{
			if (player[player_id].pos_y < max_y_coord[player_id])
			{
				player[player_id].pos_y += player[player_id].speed_y;
			}
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

// static void controlMapBoundaries(int player_id)
//{
//	if ((player[player_id].pos_x + BOX_LEFT_OFFSET) == MIN_POS_X)
//	{
//		player[player_id].pos_x = (MIN_POS_X - BOX_LEFT_OFFSET);
//	}
//	else if (((player[player_id].pos_x + player[player_id].width) - BOX_RIGHT_OFFSET) > MAX_POS_X)
//	{
//		player[player_id].pos_x = (MAX_POS_X - player[player_id].width) + BOX_RIGHT_OFFSET;
//	}
//
//	if ((player[player_id].pos_y + BOX_TOP_OFFSET) == MIN_POS_Y)
//	{
//		player[player_id].pos_y = (MIN_POS_Y - BOX_TOP_OFFSET);
//	}
//	else if (((player[player_id].pos_y + player[player_id].height) - BOX_BOTTON_OFFSET) > MAX_POS_Y)
//	{
//		player[player_id].pos_y = (MAX_POS_Y - player[player_id].height) + BOX_BOTTON_OFFSET;
//	}
// }

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
	// https://cse442--17f-github-io.translate.goog/Gilbert-Johnson-Keerthi-Distance-Algorithm/?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt&_x_tr_pto=tc
	// https://www.jeffreythompson.org/collision-detection/table_of_contents.php

	// define a caixa de colisão do personagem
	int left_edge = (player[player_id].pos_x + BOX_LEFT_OFFSET);
	int right_edge = (player[player_id].pos_x + player[player_id].width) - BOX_RIGHT_OFFSET;
	int top_edge = (player[player_id].pos_y + BOX_TOP_OFFSET);
	int botton_edge = (player[player_id].pos_y + player[player_id].height) - BOX_BOTTON_OFFSET;

	// arestas:
	// verifica com qual indice da matriz cada aresta está colidindo
	int left_edge_column = (left_edge / TILE_IN_PIXELS);
	int right_edge_column = (right_edge / TILE_IN_PIXELS);
	int top_edge_line = (top_edge / TILE_IN_PIXELS);
	int botton_edge_line = (botton_edge / TILE_IN_PIXELS);

	if (right_edge_column > MATRIX_MAX_COL_INDEX)
		right_edge_column = MATRIX_MAX_COL_INDEX;

	if (botton_edge_line > MATRIX_MAX_LIN_INDEX)
		botton_edge_line = MATRIX_MAX_LIN_INDEX;

	// vértices: (A,B,C,D)
	int A_vertex;
	int B_vertex;
	int C_vertex;
	int D_vertex;

	switch (player[player_id].last_order_x)
	{
	case LEFT:
		A_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][left_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][left_edge_column];
		if (A_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			min_x_coord[player_id] = ((left_edge_column * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_LEFT_OFFSET;
		}
		else
		{
			min_x_coord[player_id] = (MIN_POS_X - BOX_LEFT_OFFSET);
		}
		break;

	case RIGHT:
		B_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][right_edge_column];
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][right_edge_column];
		if (B_vertex == SOLID_TILE || C_vertex == SOLID_TILE)
		{
			max_x_coord[player_id] = ((right_edge_column * TILE_IN_PIXELS) - player[player_id].width) + BOX_RIGHT_OFFSET;
		}
		else
		{
			max_x_coord[player_id] = (MAX_POS_X - player[player_id].width) + BOX_RIGHT_OFFSET;
		}
		break;

	default:
		break;
	}

	switch (player[player_id].order_y)
	{
	case UP:
		A_vertex = BGA_COLLISION_MATRIX[top_edge_line][left_edge_column];
		B_vertex = BGA_COLLISION_MATRIX[top_edge_line][right_edge_column];
		if (A_vertex == SOLID_TILE || B_vertex == SOLID_TILE)
		{
			min_y_coord[player_id] = ((top_edge_line * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_TOP_OFFSET;
		}
		else
		{
			min_y_coord[player_id] = (MIN_POS_Y - BOX_TOP_OFFSET);
		}
		break;

	case DOWN:
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column];
		if (C_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			max_y_coord[player_id] = ((botton_edge_line * TILE_IN_PIXELS) - player[player_id].height) + BOX_BOTTON_OFFSET;
		}
		else
		{
			max_y_coord[player_id] = (MAX_POS_Y - player[player_id].height) + BOX_BOTTON_OFFSET;
		}
		break;

	case NEUTRAL:
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column];
		if (C_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			max_y_coord[player_id] = ((botton_edge_line * TILE_IN_PIXELS) - player[player_id].height) + BOX_BOTTON_OFFSET;
		}
		else
		{
			max_y_coord[player_id] = (MAX_POS_Y - player[player_id].height) + BOX_BOTTON_OFFSET;
		}
		break;

	default:
		break;
	}

	// sprintf(buffer_a, "posx:%d", spr_element[0].pos_x);
	// sprintf(buffer_b, "posx:%d", player[0].pos_x);
	//  sprintf(buffer_c, "posx:%d", player[0].pos_x);
	//  sprintf(buffer_d, "posx:%d", player[0].pos_x);

	// VDP_clearTextBG(BG_A, 28, 5, 10);
	// VDP_clearTextBG(BG_A, 28, 6, 10);
	//  VDP_clearTextBG(BG_A, 28, 7, 10);
	//  VDP_clearTextBG(BG_A, 28, 8, 10);

	// VDP_setTextPriority(TRUE);
	// PAL_setColor(15, RGB24_TO_VDPCOLOR(0xffff00));

	// VDP_drawTextBG(BG_A, buffer_a, 28, 5);
	// VDP_drawTextBG(BG_A, buffer_b, 28, 6);
	//  VDP_drawTextBG(BG_A, buffer_c, 28, 7);
	//  VDP_drawTextBG(BG_A, buffer_d, 28, 8);
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