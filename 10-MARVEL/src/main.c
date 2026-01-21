#include <genesis.h>
#include <resources.h>

// 320 X 224
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224

// map
#define MAP_WIDTH 768
#define MAP_HEIGTH 224

// screen
#define LEFT_SCREEN_LIMIT 0
#define RIGHT_SCREEN_LIMIT MAP_WIDTH - HORIZONTAL_RESOLUTION // VDP_getScreenWidth()
#define TOP_SCREEN_LIMIT 0
#define BOTTON_SCREEN_LIMIT MAP_HEIGTH - VERTICAL_RESOLUTION // VDP_getScreenHeight()

#define MIN_POS_X 0
#define MAX_POS_X MAP_WIDTH
#define MIN_POS_Y 0
#define MAX_POS_Y MAP_HEIGTH

//  How far to (left, right, up, down) before camera moves
#define HOW_FAR_TO_LEFT 64	  //(MAP_WIDTH / 2 - PLAYER_WIDTH)
#define HOW_FAR_TO_RIGHT 160  //(MAP_WIDTH / 2)
#define HOW_FAR_TO_TOP 48	  //(MAP_HEIGTH / 2 - PLAYER_HEIGTH)
#define HOW_FAR_TO_BOTTON 112 //(MAP_HEIGTH / 2)

// (*ptr_entity) collision box offset
#define BOX_LEFT_OFFSET 40
#define BOX_RIGHT_OFFSET 40
#define BOX_TOP_OFFSET 28
#define BOX_BOTTON_OFFSET 0

// Hero
#define PLAYER_1_WIDTH 96
#define PLAYER_1_HEIGTH 64

#define SOLID_TILE 1
#define TILE_IN_PIXELS 16
// #define MATRIX_MAX_LIN_INDEX 27
// #define MATRIX_MAX_COL_INDEX 39

#define MATRIX_MAX_LIN_INDEX 13
#define MATRIX_MAX_COL_INDEX 47

// const int BGA_COLLISION_MATRIX[28][40] =
//	{
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

const int BGA_COLLISION_MATRIX[14][48] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int min_x_coord[2] = {(MIN_POS_X - BOX_LEFT_OFFSET), (MIN_POS_X - BOX_LEFT_OFFSET)};
int max_x_coord[2] = {MAX_POS_X, MAX_POS_X};
int min_y_coord[2] = {(MIN_POS_Y - BOX_TOP_OFFSET), (MIN_POS_Y - BOX_TOP_OFFSET)};
int max_y_coord[2] = {(MAX_POS_Y - PLAYER_1_HEIGTH) + BOX_BOTTON_OFFSET, MAX_POS_Y};

typedef struct
{
	fix16 cur_pos_x;
	fix16 cur_pos_y;
	fix16 new_pos_x;
	fix16 new_pos_y;
	fix16 entity_x_on_map;
	fix16 entity_y_on_map;
	fix16 entity_x_on_screen;
	fix16 entity_y_on_screen;
} Camera;
Camera camera;

// players
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
	TIME_ATTACK_1 = 48,
	TIME_ATTACK_2 = 40,
	TIME_ATTACK_3 = 48,
	TIME_HURT = 32
};

u16 attack_timer[2] = {0, 0};
u16 effect_timer[2] = {0, 0};
u16 anim_duration[2] = {0, 0}; // animation _speed * num of frames of animation
// (*ptr_entity) sprite array entity_id

#define HERO 0
#define BOSS 1
#define FIREBALL 2
#define POINTER 3

// sprite order
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define NEUTRAL 0

#define MAX_SPRITES 20

typedef struct
{
	Sprite *sprite;
	fix16 *ptr_anim;
	fix16 anim;
	bool is_full_anim;
	fix16 frame;
	enum PlayerState state;
	fix16 pos_x;
	fix16 pos_y;
	fix16 width;
	fix16 height;
	bool flip_h;
	bool flip_v;
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
	bool is_visible;
	bool is_active;
} Entity;
Entity game_entities[MAX_SPRITES]; // max sprites
Entity (*ptr_entity)[MAX_SPRITES] = &game_entities;

// ponteiro para array
// considerando uma struct (Entity)
// e um array (Entity game_entities[10])

// forma 1: apontar para 1º elemento
// Entity *ptr_entities = ptr_entities
// Modificar o 1º elemento
// ptr_entities->flip_h = TRUE;
// então alterar o índice:
// ptr_entities++
// ptr_entities->flip_h = TRUE; <--esse é o 2º elemento do array

// forma 2: apontar para todo array
// Entity (*ptr_entities)[10] = &game_entities
// acessando os índices (*ptr_entities)[5].flip_h = TRUE;

#define MAX_PLAYERS 2

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
Joystick joysticks[MAX_PLAYERS];
Joystick (*ptr_joy)[MAX_PLAYERS] = &joysticks;
#define ONE 0
#define TWO 1

u32 frame_counter;
u16 ind;
u16 base_tile_index[3]; // BGA, BGB, WINDOW
Map *bga;
Map *bgb;

static void gameInputHandler(u16 joy, u16 changed, u16 state);

static void finiteStateMachine(int entity_id);
static void updateEntityPosition(int entity_id);

/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAttackTimer(int entity_id);

static void controlXAcceleration(int entity_id);
static void controlYAcceleration(int entity_id);

// FLIP
static void controlHorizontalFlip(int entity_id);
static void controlVerticalFlip(int entity_id);

// COLLISION
static void controlEntityMapCollision(int entity_id);
static void checkBottonCollision(int entity_id);
static void checkTopCollision(int entity_id);

// CAMERA
static void updateCamera(int entity_id);

// SPRITES
static void controlProjectile(int sprite_id);

static void processGameTitle();
static void processMainGame();
static void releaseMemory();

char buffer_a[100];
char buffer_b[100];
char buffer_c[100];
char buffer_d[100];
// para imprimir coordenadas durante o debug
static void debug();

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

static void debug()
{
	VDP_setTextPriority(TRUE);
	// PAL_setColor(15, RGB24_TO_VDPCOLOR(0xffff00));

	// u32 tick = getTick() / 300;
	sprintf(buffer_a, "pos_x:%d", (*ptr_entity)[BOSS].pos_x);
	//   sprintf(buffer_b, "posx:%d", (*ptr_entity)[0].pos_x);
	//    sprintf(buffer_c, "posx:%d", (*ptr_entity)[0].pos_x);
	//    sprintf(buffer_d, "posx:%d", (*ptr_entity)[0].pos_x);

	VDP_clearTextBG(BG_A, 28, 5, 10);
	//   VDP_clearTextBG(BG_A, 28, 6, 10);
	//    VDP_clearTextBG(BG_A, 28, 7, 10);
	//    VDP_clearTextBG(BG_A, 28, 8, 10);

	VDP_drawTextBG(BG_A, buffer_a, 28, 5);
	//   VDP_drawTextBG(BG_A, buffer_b, 28, 6);
	//    VDP_drawTextBG(BG_A, buffer_c, 28, 7);
	//    VDP_drawTextBG(BG_A, buffer_d, 28, 8);
}

static void updateCursorPosition()
{
	SPR_setPosition(ptr_cursor, (options[currentIndex].x * 8 - 32), (options[currentIndex].y * 8 - 40));
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
	VDP_loadTileSet(&bgb_tileset, base_tile_index[BG_B], DMA);
	PAL_setPalette(PAL0, bgb_palette.data, DMA);
	bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
	MAP_scrollTo(bgb, 0, 0);

	// ind += title_bgb_tileset.numTile;
	// base_tile_index[BG_A] = ind;
	//
	// VDP_loadTileSet(&title_bga_tileset, base_tile_index[BG_A], DMA);
	// PAL_setPalette(PAL1, title_bga_palette.data, DMA);
	// bga = MAP_create(&title_bga_map, BG_A, TILE_ATTR_FULL(PAL1, FALSE, FALSE, FALSE, ind));
	// MAP_scrollTo(bga, 0, 0);
	// ind += title_bga_tileset.numTile;

	// MENU
	ptr_cursor = SPR_addSprite(&fire, 0, 0, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	PAL_setPalette(PAL2, fire.palette->data, DMA);

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
	MEM_free(bga);
	MEM_free(bgb);
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

	(*ptr_entity)[HERO].state = STATE_STANDING;
	(*ptr_entity)[HERO].width = PLAYER_1_WIDTH;
	(*ptr_entity)[HERO].height = PLAYER_1_HEIGTH;
	(*ptr_entity)[HERO].anim = ANIM_STANDING;
	(*ptr_entity)[HERO].is_full_anim = TRUE;
	(*ptr_entity)[HERO].flip_v = FALSE;
	(*ptr_entity)[HERO].flip_h = FALSE;
	(*ptr_entity)[HERO].impulse_x = 4;
	(*ptr_entity)[HERO].impulse_y = 4;
	(*ptr_entity)[HERO].max_speed_x = 2;
	(*ptr_entity)[HERO].max_speed_y = 4;
	(*ptr_entity)[HERO].order_x = NEUTRAL;
	(*ptr_entity)[HERO].order_y = NEUTRAL;
	(*ptr_entity)[HERO].last_order_x = RIGHT;
	(*ptr_entity)[HERO].last_order_y = NEUTRAL;
	(*ptr_entity)[HERO].pos_x = HOW_FAR_TO_LEFT;
	(*ptr_entity)[HERO].pos_y = MIN_POS_Y - PLAYER_1_HEIGTH;
	(*ptr_entity)[HERO].speed_x = 0;
	(*ptr_entity)[HERO].speed_y = 0;
	(*ptr_entity)[HERO].is_attacking = FALSE;
	(*ptr_entity)[HERO].has_stamina = TRUE;
	(*ptr_entity)[HERO].sprite = SPR_addSprite(&knight, (*ptr_entity)[HERO].pos_x, (*ptr_entity)[HERO].pos_y, TILE_ATTR(PAL2, FALSE, (*ptr_entity)[HERO].flip_v, (*ptr_entity)[HERO].flip_h));
	PAL_setPalette(PAL2, knight.palette->data, DMA);
	SPR_setAnim((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].anim);

	(*ptr_entity)[BOSS].state = STATE_STANDING;
	(*ptr_entity)[BOSS].width = 80;
	(*ptr_entity)[BOSS].height = 80;
	(*ptr_entity)[BOSS].anim = ANIM_STANDING;
	(*ptr_entity)[BOSS].is_full_anim = TRUE;
	(*ptr_entity)[BOSS].flip_v = FALSE;
	(*ptr_entity)[BOSS].flip_h = TRUE;
	(*ptr_entity)[BOSS].impulse_x = 4;
	(*ptr_entity)[BOSS].impulse_y = 4;
	(*ptr_entity)[BOSS].max_speed_x = 2;
	(*ptr_entity)[BOSS].max_speed_y = 4;
	(*ptr_entity)[BOSS].order_x = NEUTRAL;
	(*ptr_entity)[BOSS].order_y = NEUTRAL;
	(*ptr_entity)[BOSS].last_order_x = RIGHT;
	(*ptr_entity)[BOSS].last_order_y = NEUTRAL;
	(*ptr_entity)[BOSS].pos_x = 320;
	(*ptr_entity)[BOSS].pos_y = MAX_POS_Y - 120;
	(*ptr_entity)[BOSS].speed_x = 0;
	(*ptr_entity)[BOSS].speed_y = 0;
	(*ptr_entity)[BOSS].is_attacking = FALSE;
	(*ptr_entity)[BOSS].has_stamina = TRUE;
	(*ptr_entity)[BOSS].sprite = SPR_addSprite(&dead, (*ptr_entity)[BOSS].pos_x, (*ptr_entity)[BOSS].pos_y, TILE_ATTR(PAL3, FALSE, (*ptr_entity)[BOSS].flip_v, (*ptr_entity)[BOSS].flip_h));
	PAL_setPalette(PAL3, dead.palette->data, DMA);
	SPR_setAnim((*ptr_entity)[BOSS].sprite, (*ptr_entity)[BOSS].anim);

	// ponteiro para array
	// considerando uma struct (Entity)
	// e um array (Entity game_entities[10])

	// forma 1: apontar para 1º elemento
	// Entity *ptr_entities = ptr_entities
	// Modificar o 1º elemento
	// ptr_entities->flip_h = TRUE;
	// então alterar o índice:
	// ptr_entities++
	// ptr_entities->flip_h = TRUE; <--esse é o 2º elemento do array

	// forma 2: apontar para todo array
	// Entity (*ptr_entities)[10] = &game_entities
	// acessando os índices (*ptr_entities)[5].flip_h = TRUE;

	// Other sprites...
	//(*ptr_entity)[FIREBALL].is_active = TRUE;
	//(*ptr_entity)[FIREBALL].is_visible = TRUE;
	//(*ptr_entity)[FIREBALL].pos_x = RIGHT_SCREEN_LIMIT;
	//(*ptr_entity)[FIREBALL].pos_y = 16;
	//(*ptr_entity)[FIREBALL].width = 40;
	//(*ptr_entity)[FIREBALL].height = 16;
	//(*ptr_entity)[FIREBALL].flip_v = FALSE;
	//(*ptr_entity)[FIREBALL].flip_h = TRUE;
	//(*ptr_entity)[FIREBALL].sprite = SPR_addSprite(&ball, (*ptr_entity)[FIREBALL].pos_x, (*ptr_entity)[FIREBALL].pos_y, TILE_ATTR(PAL3, FALSE, (*ptr_entity)[FIREBALL].flip_v, (*ptr_entity)[FIREBALL].flip_h));
	//(*ptr_entity)[FIREBALL].order_x = LEFT;
	//(*ptr_entity)[FIREBALL].order_y = NEUTRAL;
	//(*ptr_entity)[FIREBALL].speed_x = 3;
	//(*ptr_entity)[FIREBALL].speed_y = 1;
	// PAL_setPalette(PAL3, ball.palette->data, DMA);
	// SPR_setVisibility((*ptr_entity)[FIREBALL].sprite, HIDDEN);

	while (current_game_state == GAME)
	{

		frame_counter++;
		if (frame_counter >= 59)
		{
			frame_counter = 0;
		}

		finiteStateMachine(HERO);		
		updateCamera(HERO);		

		updateEntityPosition(HERO);
		controlEntityMapCollision(HERO);
		checkBottonCollision(HERO);
		checkTopCollision(HERO);		

		controlHorizontalFlip(HERO);		
		controlXAcceleration(HERO);
		controlYAcceleration(HERO);
		controlAttackTimer(HERO);

		//debug();
		
		// controlProjectile(FIREBALL);

		if ((*ptr_entity)[HERO].is_full_anim)
		{
			SPR_setAnim((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].anim);
		}
		else
		{
			SPR_setAnimAndFrame((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].anim, (*ptr_entity)[HERO].frame);
			// SPR_setAnim((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].anim);
			// SPR_setFrame((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].frame);
		}

		if ((*ptr_entity)[BOSS].is_full_anim)
		{
			SPR_setAnim((*ptr_entity)[BOSS].sprite, (*ptr_entity)[BOSS].anim);
		}
		else
		{
			SPR_setAnimAndFrame((*ptr_entity)[BOSS].sprite, (*ptr_entity)[BOSS].anim, (*ptr_entity)[BOSS].frame);
			// SPR_setAnim((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].anim);
			// SPR_setFrame((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].frame);
		}

		SPR_setHFlip((*ptr_entity)[HERO].sprite, (*ptr_entity)[HERO].flip_h);
		SPR_setPosition((*ptr_entity)[HERO].sprite, ((*ptr_entity)[HERO].pos_x - camera.cur_pos_x), ((*ptr_entity)[HERO].pos_y - camera.cur_pos_y));

		SPR_setHFlip((*ptr_entity)[BOSS].sprite, (*ptr_entity)[BOSS].flip_h);
		SPR_setPosition((*ptr_entity)[BOSS].sprite, ((*ptr_entity)[BOSS].pos_x - camera.cur_pos_x), ((*ptr_entity)[BOSS].pos_y - camera.cur_pos_y));

		MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
		MAP_scrollTo(bgb, 0, 0);
		// MAP_scrollTo(bgb, camera.cur_pos_x, camera.cur_pos_y);

		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	MEM_free(bga);
	MEM_free(bgb);
	MEM_free((*ptr_entity)[HERO].sprite);
	MEM_free((*ptr_entity)[BOSS].sprite);
	releaseMemory();
}

static void controlProjectile(int index)
{
	if ((*ptr_entity)[index].is_active)
	{
		if ((*ptr_entity)[index].order_x == LEFT)
		{
			(*ptr_entity)[index].pos_x -= (*ptr_entity)[index].speed_x;
			if ((*ptr_entity)[index].pos_x <= (camera.cur_pos_x - (*ptr_entity)[index].width))
			{
				(*ptr_entity)[index].pos_x = (camera.cur_pos_x + HORIZONTAL_RESOLUTION);
			}
		}
		else if ((*ptr_entity)[index].order_x == RIGHT)
		{
			(*ptr_entity)[index].pos_x += (*ptr_entity)[index].speed_x;
			if ((*ptr_entity)[index].pos_x >= (camera.cur_pos_x + HORIZONTAL_RESOLUTION))
			{
				(*ptr_entity)[index].pos_x = (camera.cur_pos_x - (*ptr_entity)[index].width);
			}
		}
	}
}

static void gameInputHandler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_LEFT)
			(*ptr_joy)[ONE].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			(*ptr_joy)[ONE].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			(*ptr_joy)[ONE].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			(*ptr_joy)[ONE].btn_right = FALSE;

		if (state & BUTTON_UP)
		{
			(*ptr_joy)[ONE].btn_up = TRUE;
			if (current_game_state == TITLE)
			{
				moveUp();
			}
		}
		else if (changed & BUTTON_UP)
			(*ptr_joy)[ONE].btn_up = FALSE;

		if (state & BUTTON_DOWN)
		{
			(*ptr_joy)[ONE].btn_down = TRUE;
			if (current_game_state == TITLE)
			{
				moveDown();
			}
		}
		else if (changed & BUTTON_DOWN)
			(*ptr_joy)[ONE].btn_down = FALSE;

		if (state & BUTTON_A)
			(*ptr_joy)[ONE].btn_a = TRUE;
		else if (changed & BUTTON_A)
			(*ptr_joy)[ONE].btn_a = FALSE;

		if (state & BUTTON_B)
			(*ptr_joy)[ONE].btn_b = TRUE;
		else if (changed & BUTTON_B)
			(*ptr_joy)[ONE].btn_b = FALSE;

		if (state & BUTTON_C)
			(*ptr_joy)[ONE].btn_c = TRUE;
		else if (changed & BUTTON_C)
			(*ptr_joy)[ONE].btn_c = FALSE;

		if (state & BUTTON_X)
			(*ptr_joy)[ONE].btn_x = TRUE;
		else if (changed & BUTTON_X)
			(*ptr_joy)[ONE].btn_x = FALSE;

		if (state & BUTTON_Y)
			(*ptr_joy)[ONE].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			(*ptr_joy)[ONE].btn_y = FALSE;

		if (state & BUTTON_Z)
			(*ptr_joy)[ONE].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			(*ptr_joy)[ONE].btn_z = FALSE;

		if (state & BUTTON_START)
		{
			(*ptr_joy)[ONE].btn_start = TRUE;
			if (current_game_state == TITLE)
			{
				select(currentIndex);
			}
		}
		else if (changed & BUTTON_START)
			(*ptr_joy)[ONE].btn_start = FALSE;

		if (state & BUTTON_MODE)
			(*ptr_joy)[ONE].btn_mode = TRUE;

		else if (changed & BUTTON_MODE)
			(*ptr_joy)[ONE].btn_mode = FALSE;
	}

	if (joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
			(*ptr_joy)[TWO].btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			(*ptr_joy)[TWO].btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			(*ptr_joy)[TWO].btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			(*ptr_joy)[TWO].btn_right = FALSE;

		if (state & BUTTON_UP)
			(*ptr_joy)[TWO].btn_up = TRUE;
		else if (changed & BUTTON_UP)
			(*ptr_joy)[TWO].btn_up = FALSE;

		if (state & BUTTON_DOWN)
			(*ptr_joy)[TWO].btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			(*ptr_joy)[TWO].btn_down = FALSE;

		if (state & BUTTON_A)
			(*ptr_joy)[TWO].btn_a = TRUE;
		else if (changed & BUTTON_A)
			(*ptr_joy)[TWO].btn_a = FALSE;

		if (state & BUTTON_B)
			(*ptr_joy)[TWO].btn_b = TRUE;
		else if (changed & BUTTON_B)
			(*ptr_joy)[TWO].btn_b = FALSE;

		if (state & BUTTON_C)
			(*ptr_joy)[TWO].btn_c = TRUE;
		else if (changed & BUTTON_C)
			(*ptr_joy)[TWO].btn_c = FALSE;

		if (state & BUTTON_X)
			(*ptr_joy)[TWO].btn_x = TRUE;
		else if (changed & BUTTON_X)
			(*ptr_joy)[TWO].btn_x = FALSE;

		if (state & BUTTON_Y)
			(*ptr_joy)[TWO].btn_y = TRUE;
		else if (changed & BUTTON_Y)
			(*ptr_joy)[TWO].btn_y = FALSE;

		if (state & BUTTON_Z)
			(*ptr_joy)[TWO].btn_z = TRUE;
		else if (changed & BUTTON_Z)
			(*ptr_joy)[TWO].btn_z = FALSE;

		if (state & BUTTON_START)
			(*ptr_joy)[TWO].btn_start = TRUE;
		else if (changed & BUTTON_START)
			(*ptr_joy)[TWO].btn_start = FALSE;

		if (state & BUTTON_MODE)
			(*ptr_joy)[TWO].btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			(*ptr_joy)[TWO].btn_mode = FALSE;
	}
}

static void finiteStateMachine(int entity_id)
{
	switch ((*ptr_entity)[entity_id].state)
	{
	case STATE_STANDING:
		if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			(*ptr_entity)[entity_id].state = STATE_FALL;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].is_full_anim = FALSE;
			(*ptr_entity)[entity_id].frame = 4;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_left)
		{
			(*ptr_entity)[entity_id].order_x = LEFT;
			(*ptr_entity)[entity_id].last_order_x = LEFT;
			(*ptr_entity)[entity_id].state = STATE_WALK;
			(*ptr_entity)[entity_id].anim = ANIM_WALK;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		else if ((*ptr_joy)[entity_id].btn_right)
		{
			(*ptr_entity)[entity_id].order_x = RIGHT;
			(*ptr_entity)[entity_id].last_order_x = RIGHT;
			(*ptr_entity)[entity_id].state = STATE_WALK;
			(*ptr_entity)[entity_id].anim = ANIM_WALK;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		if ((*ptr_joy)[entity_id].btn_z /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_JUMP;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].order_y = UP;
			if ((*ptr_entity)[entity_id].speed_y == 0)
			{
				(*ptr_entity)[entity_id].speed_y += (*ptr_entity)[entity_id].impulse_y;
			}
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_mode /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].state = STATE_DEATH;
			(*ptr_entity)[entity_id].anim = ANIM_DEATH;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_start /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].is_attacking = TRUE;
			anim_duration[entity_id] = TIME_HURT;
			(*ptr_entity)[entity_id].state = STATE_HURT;
			(*ptr_entity)[entity_id].anim = ANIM_HURT;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_x /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].is_attacking = TRUE;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			anim_duration[entity_id] = TIME_ATTACK_1;
			(*ptr_entity)[entity_id].state = STATE_ATTACK_1;
			(*ptr_entity)[entity_id].anim = ANIM_ATTACK_1;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if ((*ptr_joy)[entity_id].btn_y /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_DEFEND;
			(*ptr_entity)[entity_id].anim = ANIM_DEFEND;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		break;

	case STATE_WALK:
		(*ptr_entity)[entity_id].max_speed_x = ((*ptr_entity)[entity_id].impulse_x / 2);
		if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			(*ptr_entity)[entity_id].state = STATE_FALL;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].is_full_anim = FALSE;
			(*ptr_entity)[entity_id].frame = 4;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_z /* && (*ptr_entity)[entity_id].has_stamina */)
		{
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_JUMP;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].order_y = UP;
			if ((*ptr_entity)[entity_id].speed_y == 0)
			{
				(*ptr_entity)[entity_id].speed_y += (*ptr_entity)[entity_id].impulse_y;
			}
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_c)
		{
			(*ptr_entity)[entity_id].max_speed_x = (*ptr_entity)[entity_id].impulse_x;
			(*ptr_entity)[entity_id].state = STATE_RUN;
			(*ptr_entity)[entity_id].anim = ANIM_RUN;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}

		if ((*ptr_joy)[entity_id].btn_x /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].is_attacking = TRUE;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			anim_duration[entity_id] = TIME_ATTACK_1;
			(*ptr_entity)[entity_id].state = STATE_ATTACK_1;
			(*ptr_entity)[entity_id].anim = ANIM_ATTACK_1;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if ((*ptr_joy)[entity_id].btn_y /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_DEFEND;
			(*ptr_entity)[entity_id].anim = ANIM_DEFEND;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].state = STATE_STANDING;
			(*ptr_entity)[entity_id].anim = ANIM_STANDING;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	case STATE_RUN:
		if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			(*ptr_entity)[entity_id].state = STATE_FALL;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].is_full_anim = FALSE;
			(*ptr_entity)[entity_id].frame = 4;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		if ((*ptr_joy)[entity_id].btn_z /* && (*ptr_entity)[entity_id].has_stamina */)
		{
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_JUMP;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].order_y = UP;
			if ((*ptr_entity)[entity_id].speed_y == 0)
			{
				(*ptr_entity)[entity_id].speed_y += (*ptr_entity)[entity_id].impulse_y;
			}
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if ((*ptr_joy)[entity_id].btn_x /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].is_attacking = TRUE;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			anim_duration[entity_id] = TIME_ATTACK_1;
			(*ptr_entity)[entity_id].state = STATE_ATTACK_1;
			(*ptr_entity)[entity_id].anim = ANIM_ATTACK_1;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if ((*ptr_joy)[entity_id].btn_y /*  && (*ptr_entity)[entity_id].has_stamina */)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			// (*ptr_entity)[entity_id].has_stamina = FALSE;
			(*ptr_entity)[entity_id].state = STATE_DEFEND;
			(*ptr_entity)[entity_id].anim = ANIM_DEFEND;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}

		if (!(*ptr_joy)[entity_id].btn_c)
		{
			// (*ptr_entity)[entity_id].speed_x = ((*ptr_entity)[entity_id].impulse_x / 2);
			(*ptr_entity)[entity_id].anim = ANIM_WALK;
			(*ptr_entity)[entity_id].state = STATE_WALK;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}

		if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
		{
			(*ptr_entity)[entity_id].anim = ANIM_WALK;
			(*ptr_entity)[entity_id].state = STATE_WALK;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	case STATE_JUMP:
		if ((*ptr_entity)[entity_id].order_y == UP)
		{
			if ((*ptr_joy)[entity_id].btn_left)
			{
				(*ptr_entity)[entity_id].order_x = LEFT;
				(*ptr_entity)[entity_id].last_order_x = LEFT;
			}
			else if ((*ptr_joy)[entity_id].btn_right)
			{
				(*ptr_entity)[entity_id].order_x = RIGHT;
				(*ptr_entity)[entity_id].last_order_x = RIGHT;
			}
			else if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
			{
				(*ptr_entity)[entity_id].order_x = NEUTRAL;
			}
		}
		else if ((*ptr_entity)[entity_id].order_y == NEUTRAL)
		{
			if ((*ptr_joy)[entity_id].btn_left)
			{
				(*ptr_entity)[entity_id].order_x = LEFT;
				(*ptr_entity)[entity_id].last_order_x = LEFT;
			}
			else if ((*ptr_joy)[entity_id].btn_right)
			{
				(*ptr_entity)[entity_id].order_x = RIGHT;
				(*ptr_entity)[entity_id].last_order_x = RIGHT;
			}
			else if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
			{
				(*ptr_entity)[entity_id].order_x = NEUTRAL;
			}
		}
		else if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			(*ptr_entity)[entity_id].state = STATE_FALL;
			(*ptr_entity)[entity_id].anim = ANIM_JUMP;
			(*ptr_entity)[entity_id].is_full_anim = FALSE;
			(*ptr_entity)[entity_id].frame = 4;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
		}
		break;

	case STATE_FALL:
		if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			if ((*ptr_entity)[entity_id].speed_y < (*ptr_entity)[entity_id].max_speed_y)
			{
				if ((*ptr_joy)[entity_id].btn_left)
				{
					(*ptr_entity)[entity_id].order_x = LEFT;
					(*ptr_entity)[entity_id].last_order_x = LEFT;
				}
				else if ((*ptr_joy)[entity_id].btn_right)
				{
					(*ptr_entity)[entity_id].order_x = RIGHT;
					(*ptr_entity)[entity_id].last_order_x = RIGHT;
				}
				else if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
				{
					(*ptr_entity)[entity_id].order_x = NEUTRAL;
				}
			}
			else if ((*ptr_entity)[entity_id].speed_y == (*ptr_entity)[entity_id].max_speed_y)
			{
				(*ptr_entity)[entity_id].state = STATE_FALL_LOOP;
				(*ptr_entity)[entity_id].anim = ANIM_JUMP;
				(*ptr_entity)[entity_id].is_full_anim = FALSE;
				(*ptr_entity)[entity_id].frame = 4;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
			}
		}
		else if ((*ptr_entity)[entity_id].order_y == NEUTRAL)
		{
			if ((*ptr_entity)[entity_id].speed_x > 0)
			{
				(*ptr_entity)[entity_id].anim = ANIM_RUN;
				(*ptr_entity)[entity_id].state = STATE_RUN;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
			}
			else
			{
				(*ptr_entity)[entity_id].state = STATE_STANDING;
				(*ptr_entity)[entity_id].anim = ANIM_STANDING;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
			}
		}
		break;

	case STATE_FALL_LOOP:
		if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			if ((*ptr_joy)[entity_id].btn_left)
			{
				(*ptr_entity)[entity_id].order_x = LEFT;
				(*ptr_entity)[entity_id].last_order_x = LEFT;
			}
			else if ((*ptr_joy)[entity_id].btn_right)
			{
				(*ptr_entity)[entity_id].order_x = RIGHT;
				(*ptr_entity)[entity_id].last_order_x = RIGHT;
			}
			else if (!((*ptr_joy)[entity_id].btn_left) && !((*ptr_joy)[entity_id].btn_right))
			{
				(*ptr_entity)[entity_id].order_x = NEUTRAL;
			}
		}
		else if ((*ptr_entity)[entity_id].order_y == NEUTRAL)
		{
			(*ptr_entity)[entity_id].order_x = NEUTRAL;
			(*ptr_entity)[entity_id].state = STATE_STANDING;
			(*ptr_entity)[entity_id].anim = ANIM_STANDING;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	case STATE_ATTACK_1:
		if (!(*ptr_entity)[entity_id].is_attacking)
		{
			if ((*ptr_joy)[entity_id].btn_x)
			{
				(*ptr_entity)[entity_id].order_x = NEUTRAL;
				(*ptr_entity)[entity_id].is_attacking = TRUE;
				// (*ptr_entity)[entity_id].has_stamina = FALSE;
				anim_duration[entity_id] = TIME_ATTACK_2;
				(*ptr_entity)[entity_id].state = STATE_ATTACK_2;
				(*ptr_entity)[entity_id].anim = ANIM_ATTACK_2;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
			}
			else
			{
				(*ptr_entity)[entity_id].state = STATE_STANDING;
				(*ptr_entity)[entity_id].anim = ANIM_STANDING;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_2:
		if (!(*ptr_entity)[entity_id].is_attacking)
		{
			if ((*ptr_joy)[entity_id].btn_x)
			{
				(*ptr_entity)[entity_id].order_x = NEUTRAL;
				(*ptr_entity)[entity_id].is_attacking = TRUE;
				// (*ptr_entity)[entity_id].has_stamina = FALSE;
				anim_duration[entity_id] = TIME_ATTACK_3;
				(*ptr_entity)[entity_id].state = STATE_ATTACK_3;
				(*ptr_entity)[entity_id].anim = ANIM_ATTACK_3;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, FALSE);
			}
			else
			{
				(*ptr_entity)[entity_id].state = STATE_STANDING;
				(*ptr_entity)[entity_id].anim = ANIM_STANDING;
				(*ptr_entity)[entity_id].is_full_anim = TRUE;
				SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
			}
		}
		break;
	case STATE_ATTACK_3:
		if (!(*ptr_entity)[entity_id].is_attacking)
		{
			(*ptr_entity)[entity_id].state = STATE_STANDING;
			(*ptr_entity)[entity_id].anim = ANIM_STANDING;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	case STATE_DEFEND:
		if (!(*ptr_joy)[entity_id].btn_y)
		{
			(*ptr_entity)[entity_id].state = STATE_STANDING;
			(*ptr_entity)[entity_id].anim = ANIM_STANDING;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	case STATE_HURT:
		if (!(*ptr_entity)[entity_id].is_attacking)
		{
			(*ptr_entity)[entity_id].state = STATE_STANDING;
			(*ptr_entity)[entity_id].anim = ANIM_STANDING;
			(*ptr_entity)[entity_id].is_full_anim = TRUE;
			SPR_setAnimationLoop((*ptr_entity)[entity_id].sprite, TRUE);
		}
		break;

	default:
		break;
	}
}

static void controlXAcceleration(int entity_id)
{
	if ((*ptr_entity)[entity_id].order_x != NEUTRAL)
	{
		if ((*ptr_entity)[entity_id].speed_x < (*ptr_entity)[entity_id].max_speed_x)
		{
			if (frame_counter % 5 == 0)
			{
				(*ptr_entity)[entity_id].speed_x += 1;
			}
		}
		else if ((*ptr_entity)[entity_id].speed_x > (*ptr_entity)[entity_id].max_speed_x)
		{
			if (frame_counter % 5 == 0)
			{
				(*ptr_entity)[entity_id].speed_x -= 1;
			}
		}
	}
	else if ((*ptr_entity)[entity_id].order_x == NEUTRAL)
	{
		if ((*ptr_entity)[entity_id].speed_x > 0)
		{
			if (frame_counter % 3 == 0)
			{
				(*ptr_entity)[entity_id].speed_x -= 1;
			}
		}
	}
}

static void controlYAcceleration(int entity_id)
{
	if ((*ptr_entity)[entity_id].order_y == UP)
	{
		if ((*ptr_entity)[entity_id].speed_y > 0)
		{
			if (frame_counter % 5 == 0)
			{
				(*ptr_entity)[entity_id].speed_y -= 1;
			}
		}
		else if ((*ptr_entity)[entity_id].speed_y == 0)
		{
			(*ptr_entity)[entity_id].order_y = NEUTRAL;
		}
	}
	else if ((*ptr_entity)[entity_id].order_y == DOWN)
	{
		if ((*ptr_entity)[entity_id].speed_y < (*ptr_entity)[entity_id].max_speed_y)
		{
			if (frame_counter % 5 == 0)
			{
				(*ptr_entity)[entity_id].speed_y += 1;
			}
		}
	}
	else if ((*ptr_entity)[entity_id].order_y == NEUTRAL)
	{
		(*ptr_entity)[entity_id].speed_y = 0;
	}
}

static void updateEntityPosition(int entity_id)
{
	if ((*ptr_entity)[entity_id].speed_x > 0)
	{
		if ((*ptr_entity)[entity_id].last_order_x == LEFT || (*ptr_entity)[entity_id].order_x == LEFT)
		{
			if ((*ptr_entity)[entity_id].pos_x > min_x_coord[entity_id])
			{
				(*ptr_entity)[entity_id].pos_x -= (*ptr_entity)[entity_id].speed_x;
			}
		}
		else if ((*ptr_entity)[entity_id].last_order_x == RIGHT || (*ptr_entity)[entity_id].order_x == RIGHT)
		{
			if ((*ptr_entity)[entity_id].pos_x < max_x_coord[entity_id])
			{
				(*ptr_entity)[entity_id].pos_x += (*ptr_entity)[entity_id].speed_x;
			}
		}
	}
	if ((*ptr_entity)[entity_id].speed_y > 0)
	{
		if ((*ptr_entity)[entity_id].order_y == UP)
		{
			if ((*ptr_entity)[entity_id].pos_y > min_y_coord[entity_id])
			{
				(*ptr_entity)[entity_id].pos_y -= (*ptr_entity)[entity_id].speed_y;
			}
		}
		else if ((*ptr_entity)[entity_id].order_y == DOWN)
		{
			if ((*ptr_entity)[entity_id].pos_y < max_y_coord[entity_id])
			{
				(*ptr_entity)[entity_id].pos_y += (*ptr_entity)[entity_id].speed_y;
			}
		}
	}
}

static void controlHorizontalFlip(int entity_id)
{
	if ((*ptr_entity)[entity_id].order_x == RIGHT)
	{
		(*ptr_entity)[entity_id].flip_h = FALSE;
	}
	else if ((*ptr_entity)[entity_id].order_x == LEFT)
	{
		(*ptr_entity)[entity_id].flip_h = TRUE;
	}
}

static void controlVerticalFlip(int entity_id)
{
	if ((*ptr_entity)[entity_id].order_y == UP)
	{
		(*ptr_entity)[entity_id].flip_v = FALSE;
	}
	else if ((*ptr_entity)[entity_id].order_y == DOWN)
	{
		(*ptr_entity)[entity_id].flip_v = TRUE;
	}
	else
	{
		(*ptr_entity)[entity_id].flip_v = (*ptr_entity)[entity_id].flip_v;
	}
}

static void controlAttackTimer(int entity_id)
{
	if ((*ptr_entity)[entity_id].is_attacking)
	{
		if (attack_timer[entity_id] < anim_duration[entity_id])
			attack_timer[entity_id]++;
		else
		{
			attack_timer[entity_id] = 0;
			(*ptr_entity)[entity_id].is_attacking = FALSE;
		}
	}
}

static void updateCamera(int entity_id)
{
	camera.entity_x_on_map = (*ptr_entity)[entity_id].pos_x;
	camera.entity_y_on_map = (*ptr_entity)[entity_id].pos_y;
	camera.entity_x_on_screen = (camera.entity_x_on_map - camera.cur_pos_x);
	camera.entity_y_on_screen = (camera.entity_y_on_map - camera.cur_pos_y);

	// camera horizontal movement
	if (camera.entity_x_on_screen > HOW_FAR_TO_RIGHT)
	{
		camera.new_pos_x = (camera.entity_x_on_map - HOW_FAR_TO_RIGHT);
	}
	else if (camera.entity_x_on_screen < HOW_FAR_TO_LEFT)
	{
		camera.new_pos_x = (camera.entity_x_on_map - HOW_FAR_TO_LEFT);
	}
	else
	{
		camera.new_pos_x = camera.cur_pos_x;
	}

	// camera vertical movement
	if (camera.entity_y_on_screen > HOW_FAR_TO_BOTTON)
	{
		camera.new_pos_y = (camera.entity_y_on_map - HOW_FAR_TO_BOTTON);
	}
	else if (camera.entity_y_on_screen < HOW_FAR_TO_TOP)
	{
		camera.new_pos_y = (camera.entity_y_on_map - HOW_FAR_TO_TOP);
	}
	else
	{
		camera.new_pos_y = camera.cur_pos_y;
	}

	// stop camera from going beyond map boundaries
	if (camera.new_pos_x < LEFT_SCREEN_LIMIT)
	{
		camera.new_pos_x = LEFT_SCREEN_LIMIT;
	}
	else if (camera.new_pos_x > RIGHT_SCREEN_LIMIT)
	{
		camera.new_pos_x = RIGHT_SCREEN_LIMIT;
	}

	if (camera.new_pos_y < TOP_SCREEN_LIMIT)
	{
		camera.new_pos_y = TOP_SCREEN_LIMIT;
	}
	else if (camera.new_pos_y > BOTTON_SCREEN_LIMIT)
	{
		camera.new_pos_y = BOTTON_SCREEN_LIMIT;
	}

	// update current camera X & Y
	if ((camera.cur_pos_x != camera.new_pos_x) || (camera.cur_pos_y != camera.new_pos_y))
	{
		camera.cur_pos_x = camera.new_pos_x;
		camera.cur_pos_y = camera.new_pos_y;
	}
};

static void controlEntityMapCollision(int entity_id)
{
	// https://cse442--17f-github-io.translate.goog/Gilbert-Johnson-Keerthi-Distance-Algorithm/?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt&_x_tr_pto=tc
	// https://www.jeffreythompson.org/collision-detection/table_of_contents.php

	// define a caixa de colisão do personagem
	int left_edge = ((*ptr_entity)[entity_id].pos_x + BOX_LEFT_OFFSET);
	int right_edge = ((*ptr_entity)[entity_id].pos_x + (*ptr_entity)[entity_id].width) - BOX_RIGHT_OFFSET;
	int top_edge = ((*ptr_entity)[entity_id].pos_y + BOX_TOP_OFFSET);
	int botton_edge = ((*ptr_entity)[entity_id].pos_y + (*ptr_entity)[entity_id].height) - BOX_BOTTON_OFFSET;

	// arestas:
	// verifica com qual indice da matriz cada aresta está colidindo
	int left_edge_column = (left_edge / TILE_IN_PIXELS);
	int right_edge_column = (right_edge / TILE_IN_PIXELS);
	int top_edge_line = (top_edge / TILE_IN_PIXELS);
	int botton_edge_line = (botton_edge / TILE_IN_PIXELS);

	// isso corrige um glitch
	if (right_edge_column > MATRIX_MAX_COL_INDEX)
		right_edge_column = MATRIX_MAX_COL_INDEX;
	// isso corrige um glitch
	if (botton_edge_line > MATRIX_MAX_LIN_INDEX)
		botton_edge_line = MATRIX_MAX_LIN_INDEX;

	// vértices: (A,B,C,D)
	int A_vertex;
	int B_vertex;
	int C_vertex;
	int D_vertex;

	switch ((*ptr_entity)[entity_id].last_order_x)
	{
	case LEFT:
		A_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][left_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][left_edge_column];
		if (A_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			min_x_coord[entity_id] = ((left_edge_column * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_LEFT_OFFSET;
		}
		else
		{
			min_x_coord[entity_id] = (MIN_POS_X - BOX_LEFT_OFFSET);
			//min_x_coord[entity_id] = (camera.cur_pos_x - BOX_LEFT_OFFSET);
		}
		break;

	case RIGHT:
		B_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][right_edge_column];
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][right_edge_column];
		if (B_vertex == SOLID_TILE || C_vertex == SOLID_TILE)
		{
			max_x_coord[entity_id] = ((right_edge_column * TILE_IN_PIXELS) - (*ptr_entity)[entity_id].width) + BOX_RIGHT_OFFSET;
		}
		else
		{
			max_x_coord[entity_id] = (MAX_POS_X - (*ptr_entity)[entity_id].width) + BOX_RIGHT_OFFSET;
			//max_x_coord[entity_id] = ((camera.cur_pos_x + HORIZONTAL_RESOLUTION) - (*ptr_entity)[entity_id].width) + BOX_RIGHT_OFFSET;
		}
		break;

	default:
		break;
	}

	switch ((*ptr_entity)[entity_id].order_y)
	{
	case UP:
		A_vertex = BGA_COLLISION_MATRIX[top_edge_line][left_edge_column];
		B_vertex = BGA_COLLISION_MATRIX[top_edge_line][right_edge_column];
		if (A_vertex == SOLID_TILE || B_vertex == SOLID_TILE)
		{
			min_y_coord[entity_id] = ((top_edge_line * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_TOP_OFFSET;
		}
		else
		{
			min_y_coord[entity_id] = (MIN_POS_Y - BOX_TOP_OFFSET);
		}
		break;

	case DOWN:
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column];
		if (C_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			max_y_coord[entity_id] = ((botton_edge_line * TILE_IN_PIXELS) - (*ptr_entity)[entity_id].height) + BOX_BOTTON_OFFSET;
		}
		else
		{
			max_y_coord[entity_id] = (MAX_POS_Y - (*ptr_entity)[entity_id].height) + BOX_BOTTON_OFFSET;
		}
		break;

	case NEUTRAL:
		C_vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column];
		D_vertex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column];
		if (C_vertex == SOLID_TILE || D_vertex == SOLID_TILE)
		{
			max_y_coord[entity_id] = ((botton_edge_line * TILE_IN_PIXELS) - (*ptr_entity)[entity_id].height) + BOX_BOTTON_OFFSET;
		}
		else
		{
			max_y_coord[entity_id] = (MAX_POS_Y - (*ptr_entity)[entity_id].height) + BOX_BOTTON_OFFSET;
		}
		break;

	default:
		break;
	}
}

static void checkBottonCollision(int entity_id)
{
	if (((*ptr_entity)[entity_id].order_y == NEUTRAL) && ((*ptr_entity)[entity_id].pos_y < max_y_coord[entity_id]))
	{
		(*ptr_entity)[entity_id].order_y = DOWN;
	}
	else if (((*ptr_entity)[entity_id].order_y == DOWN) && ((*ptr_entity)[entity_id].pos_y >= max_y_coord[entity_id]))
	{
		(*ptr_entity)[entity_id].order_y = NEUTRAL;
		(*ptr_entity)[entity_id].pos_y = max_y_coord[entity_id];
	}
}

static void checkTopCollision(int entity_id)
{
	if (((*ptr_entity)[entity_id].order_y == UP) && ((*ptr_entity)[entity_id].pos_y <= min_y_coord[entity_id]))
	{
		(*ptr_entity)[entity_id].order_y = NEUTRAL;
	}
}

// make GDK=/path/to/sgdk -f /path/to/sgdk/makefile_wine.gen
// make GDK=${HOME}/SGDK -f ${HOME}/SGDK/makefile_wine.gen

// https://gameprogrammingpatterns.com/state.html
// https://www.youtube.com/watch?v=xldCreKST3s
// OHSAT Games Tutorials
// https://www.ohsat.com/tutorial/mdmisc/sor-style-text-crawl/
// PIGSY Retro Game Sega Genesis Tutorials
// https://www.youtube.com/watch?v=BnGqc5OTTY4&list=PL1xqkpO_SvY2_rSwHTBIBxXMqmek--GAb