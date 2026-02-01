#include <genesis.h>
#include <resources.h>

// 320 X 224
#define HORIZONTAL_RESOLUTION 320
#define VERTICAL_RESOLUTION 224

// map
#define MAP_WIDTH 512
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

#define PLAYER_1_WIDTH 96
#define PLAYER_1_HEIGTH 64

#define PLAYER_2_WIDTH 96
#define PLAYER_2_HEIGTH 64

#define SOLID_TILE 1
#define TILE_IN_PIXELS 8

#define MATRIX_MAX_LIN_INDEX 27
#define MATRIX_MAX_COL_INDEX 63

const u8 BGA_COLLISION_MATRIX[28][64] =
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// arestas: a1, a2, a3, a4
s16 left_edge_column;
s16 right_edge_column;
s16 top_edge_line;
s16 botton_edge_line;

// vértices: A,B,C,D    //  A---a2----B
u8 top_left_vertex;		//  |         |
u8 top_right_vertex;	// a1         a3
u8 botton_right_vertex; //  |         |
u8 botton_left_vertex;	//  D---a4----C

s16 min_x_coord[2] = {(MIN_POS_X - BOX_LEFT_OFFSET), (MIN_POS_X - BOX_LEFT_OFFSET)};
s16 max_x_coord[2] = {(MAX_POS_X - PLAYER_1_WIDTH) + BOX_RIGHT_OFFSET, (MAX_POS_X - PLAYER_2_WIDTH) + BOX_RIGHT_OFFSET};
s16 min_y_coord[2] = {(MIN_POS_Y - BOX_TOP_OFFSET), (MIN_POS_Y - BOX_TOP_OFFSET)};
s16 max_y_coord[2] = {(MAX_POS_Y - PLAYER_1_HEIGTH) + BOX_BOTTON_OFFSET, (MAX_POS_Y - PLAYER_2_HEIGTH) + BOX_BOTTON_OFFSET};

typedef struct
{
	s16 cur_pos_x;
	s16 cur_pos_y;
	s16 new_pos_x;
	s16 new_pos_y;
	s16 entity_x_on_map;
	s16 entity_y_on_map;
	s16 entity_x_on_screen;
	s16 entity_y_on_screen;
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
	STATE_PARRY,
	STATE_DEFEND,
	STATE_HURT,
	STATE_DEATH
};

enum TimeOfAnimation
{
	TIME_ATTACK_1 = 48,
	TIME_ATTACK_2 = 40,
	TIME_ATTACK_3 = 48,
	TIME_HURT = 32,
	TIME_PARRY = 48
};

u16 attack_timer[2] = {0, 0};
u16 effect_timer[2] = {0, 0};

#define HERO 0
#define ENEMY 1
#define FIREBALL 2
#define POINTER 3

// sprite order
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1
#define NEUTRAL 0

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
Joystick joy_1, joy_2;

typedef struct
{
	s16 x;
	s16 y;
	s16 w;
	s16 h;
} CollBox;

typedef struct
{
	s16 player_id;
	Sprite *sprite;
	s16 *ptr_anim;
	s16 anim;
	s16 anim_duration; // animation speed * num of frames of animation
	bool is_full_anim;
	s16 frame;
	enum PlayerState state;
	s16 pos_x;
	s16 pos_y;
	s16 width;
	s16 height;
	bool flip_h;
	bool flip_v;
	s16 order_x;
	s16 order_y;
	s16 last_order_x;
	s16 last_order_y;
	s16 impulse_x;
	s16 impulse_y;
	s16 vel_x;
	s16 vel_y;
	s16 max_vel_x;
	s16 max_vel_y;
	bool is_attacking;
	bool has_stamina;
	bool is_visible;
	bool is_active;
	CollBox coll_box;
	Joystick *joy;
} Entity;
Entity player_1, player_2;
Entity *p1 = &player_1;
Entity *p2 = &player_2;
s16 gravity = 1;
s16 acceleration = 1;

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

u32 frame_counter = 1;
u16 ind;
u16 base_tile_index[3]; // BGA, BGB, WINDOW
Map *bga;
Map *bgb;

static void gameInputHandler(u16 joy, u16 changed, u16 state);

static void finiteStateMachine(Entity *p);
static void updateEntityPosition(Entity *p);

/*Perform full animation, blocking other animation starts until timer ends*/
static void controlAttackTimer(Entity *p);

static void controlXAcceleration(Entity *p);
static void controlYAcceleration(Entity *p);

// FLIP
static void controlHorizontalFlip(Entity *p);
static void controlVerticalFlip(Entity *p);

// COLLISION
static void calculateEntityMapCollision(Entity *p);
static void checkBottonCollision(Entity *p);
static void checkTopCollision(Entity *p);
static void checkLeftCollision(Entity *p);
static void checkRightCollision(Entity *p);

// CAMERA
static void updateCamera(Entity *p);

// SPRITES
static void controlProjectile(Entity *p);

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
Sprite *ptr_start;
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

	sprintf(buffer_a, "cb.x:%d", (p1->pos_x + BOX_LEFT_OFFSET));
	sprintf(buffer_b, "cb.w:%d", ((p1->pos_x + p1->width) - BOX_RIGHT_OFFSET));
	sprintf(buffer_c, "cb.y:%d", (p1->pos_y + BOX_TOP_OFFSET));
	sprintf(buffer_d, "cb.h:%d", ((p1->pos_y + p1->height) - BOX_BOTTON_OFFSET));

	VDP_clearTextBG(BG_A, 28, 5, 10);
	VDP_clearTextBG(BG_A, 28, 6, 10);
	VDP_clearTextBG(BG_A, 28, 7, 10);
	VDP_clearTextBG(BG_A, 28, 8, 10);

	VDP_drawTextBG(BG_A, buffer_a, 28, 5);
	VDP_drawTextBG(BG_A, buffer_b, 28, 6);
	VDP_drawTextBG(BG_A, buffer_c, 28, 7);
	VDP_drawTextBG(BG_A, buffer_d, 28, 8);
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
	VDP_loadTileSet(&title_bgb_tileset, base_tile_index[BG_B], DMA);
	PAL_setPalette(PAL0, title_bgb_palette.data, DMA);
	bgb = MAP_create(&title_bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
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
	// ptr_cursor = SPR_addSprite(&fire, 0, 0, TILE_ATTR(PAL2, FALSE, FALSE, FALSE));
	// PAL_setPalette(PAL2, fire.palette->data, DMA);

	ptr_start = SPR_addSprite(&start, 116, 180, TILE_ATTR(PAL3, FALSE, FALSE, FALSE));
	PAL_setPalette(PAL3, start.palette->data, DMA);
	bool is_visible = TRUE;

	// Draw options
	// VDP_setTextPriority(TRUE);
	// PAL_setColor(15, RGB24_TO_VDPCOLOR(0xffff00));
	// VDP_drawTextBG(BG_A, options[0].label, options[0].x, options[0].y);
	// VDP_drawTextBG(BG_A, options[1].label, options[1].x, options[1].y);
	// VDP_drawTextBG(BG_A, options[2].label, options[2].x, options[2].y);

	while (current_game_state == TITLE)
	{
		frame_counter++;
		if (frame_counter >= 30)
		{
			if (is_visible)
			{
				SPR_setVisibility(ptr_start, HIDDEN);
				is_visible = FALSE;
			}
			else
			{
				SPR_setVisibility(ptr_start, VISIBLE);
				is_visible = TRUE;
			}
			frame_counter = 1;
		}
		// updateCursorPosition();
		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	// VDP_clearTextBG(BG_A, options[0].x, options[0].y, 20);
	// VDP_clearTextBG(BG_A, options[1].x, options[1].y, 20);
	// VDP_clearTextBG(BG_A, options[2].x, options[2].y, 20);
	// MEM_free(bga);
	MEM_free(bgb);
	// MEM_free(ptr_cursor);
	MEM_free(ptr_start);
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

	s16 bgb_x = 0;
	s16 bgb_y = 0;

	VDP_loadTileSet(&bgb_tileset, base_tile_index[BG_B], DMA);
	PAL_setPalette(PAL0, bgb_palette.data, DMA);
	bgb = MAP_create(&bgb_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, base_tile_index[BG_B]));
	// MAP_scrollTo(bgb, camera.cur_pos_x, camera.cur_pos_y);
	MAP_scrollTo(bgb, bgb_x, bgb_y);

	// ind += bgb_tileset.numTile;
	// base_tile_index[WINDOW] = ind;

	// PLAYER 1
	p1->joy = &joy_1;
	p1->player_id = HERO;
	p1->state = STATE_STANDING;
	p1->width = PLAYER_1_WIDTH;
	p1->height = PLAYER_1_HEIGTH;
	p1->anim = ANIM_STANDING;
	p1->is_full_anim = TRUE;
	p1->flip_v = FALSE;
	p1->flip_h = FALSE;
	p1->impulse_x = 4;
	p1->impulse_y = 4;
	p1->max_vel_x = 2;
	p1->max_vel_y = 2;
	p1->order_x = NEUTRAL;
	p1->order_y = NEUTRAL;
	p1->last_order_x = RIGHT;
	p1->last_order_y = NEUTRAL;
	p1->pos_x = HOW_FAR_TO_LEFT;
	p1->pos_y = HOW_FAR_TO_TOP;
	p1->vel_x = 0;
	p1->vel_y = 0;
	p1->is_attacking = FALSE;
	p1->has_stamina = TRUE;
	p1->sprite = SPR_addSprite(&knight, p1->pos_x, p1->pos_y, TILE_ATTR(PAL2, FALSE, p1->flip_v, p1->flip_h));
	PAL_setPalette(PAL2, knight.palette->data, DMA);
	SPR_setAnim(p1->sprite, p1->anim);

	Entity box;
	box.state = STATE_STANDING;
	box.width = 96;
	box.height = 64;
	box.anim = ANIM_STANDING;
	box.is_full_anim = TRUE;
	box.flip_v = FALSE;
	box.flip_h = TRUE;
	box.impulse_x = 4;
	box.impulse_y = 4;
	box.max_vel_x = 2;
	box.max_vel_y = 2;
	box.order_x = NEUTRAL;
	box.order_y = NEUTRAL;
	box.last_order_x = RIGHT;
	box.last_order_y = NEUTRAL;
	box.pos_x = HOW_FAR_TO_LEFT;
	box.pos_y = HOW_FAR_TO_TOP;
	box.vel_x = 0;
	box.vel_y = 0;
	box.is_attacking = FALSE;
	box.has_stamina = TRUE;
	box.sprite = SPR_addSprite(&col_box, box.pos_x, box.pos_y, TILE_ATTR(PAL3, FALSE, box.flip_v, box.flip_h));
	PAL_setPalette(PAL3, col_box.palette->data, DMA);
	SPR_setAnim(box.sprite, box.anim);
	Entity *p_box = &box;

	while (current_game_state == GAME)
	{

		frame_counter++;
		if (frame_counter >= 60)
		{
			frame_counter = 1;
		}

		finiteStateMachine(p1);
		updateCamera(p1);
		updateEntityPosition(p1);
		calculateEntityMapCollision(p1);
		checkBottonCollision(p1);
		checkLeftCollision(p1);
		checkRightCollision(p1);
		checkTopCollision(p1);
		controlHorizontalFlip(p1);
		controlXAcceleration(p1);
		controlYAcceleration(p1);
		controlAttackTimer(p1);
		// debug();

		if (p1->is_full_anim)
		{
			SPR_setAnim(p1->sprite, p1->anim);
		}
		else
		{
			SPR_setAnimAndFrame(p1->sprite, p1->anim, p1->frame);
			// SPR_setAnim(p1->sprite, p1->anim);
			// SPR_setFrame(p1->sprite, p1->frame);
		}

		if (p_box->is_full_anim)
		{
			SPR_setAnim(p_box->sprite, p_box->anim);
		}
		else
		{
			SPR_setAnimAndFrame(p_box->sprite, p_box->anim, p_box->frame);
			// SPR_setAnim(e_p->sprite, e_p->anim);
			// SPR_setFrame(e_p->sprite, e_p->frame);
		}

		SPR_setHFlip(p1->sprite, p1->flip_h);
		SPR_setPosition(p1->sprite, (p1->pos_x - camera.cur_pos_x), (p1->pos_y - camera.cur_pos_y));

		SPR_setHFlip(p_box->sprite, p_box->flip_h);
		SPR_setPosition(p_box->sprite, (p1->pos_x - camera.cur_pos_x), (p1->pos_y - camera.cur_pos_y));

		MAP_scrollTo(bga, camera.cur_pos_x, camera.cur_pos_y);
		bgb_x = F16_mul(camera.cur_pos_x, FIX16(0.1));
		MAP_scrollTo(bgb, bgb_x, bgb_y);

		SPR_update();
		SYS_doVBlankProcessEx(ON_VBLANK_START);
	}
	MEM_free(bga);
	// MEM_free(bgb);
	MEM_free(p1->sprite);
	MEM_free(p_box->sprite);
	MEM_free(p1);
	MEM_free(p_box);
	releaseMemory();
}

static void controlProjectile(Entity *p)
{
	if (p->is_active)
	{
		if (p->order_x == LEFT)
		{
			p->pos_x -= p->vel_x;
			if (p->pos_x <= (camera.cur_pos_x - p->width))
			{
				p->pos_x = (camera.cur_pos_x + HORIZONTAL_RESOLUTION);
			}
		}
		else if (p->order_x == RIGHT)
		{
			p->pos_x += p->vel_x;
			if (p->pos_x >= (camera.cur_pos_x + HORIZONTAL_RESOLUTION))
			{
				p->pos_x = (camera.cur_pos_x - p->width);
			}
		}
	}
}

static void gameInputHandler(u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		if (state & BUTTON_LEFT)
			joy_1.btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joy_1.btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joy_1.btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joy_1.btn_right = FALSE;

		if (state & BUTTON_UP)
		{
			joy_1.btn_up = TRUE;
			if (current_game_state == TITLE)
			{
				moveUp();
			}
		}
		else if (changed & BUTTON_UP)
			joy_1.btn_up = FALSE;

		if (state & BUTTON_DOWN)
		{
			joy_1.btn_down = TRUE;
			if (current_game_state == TITLE)
			{
				moveDown();
			}
		}
		else if (changed & BUTTON_DOWN)
			joy_1.btn_down = FALSE;

		if (state & BUTTON_A)
			joy_1.btn_a = TRUE;
		else if (changed & BUTTON_A)
			joy_1.btn_a = FALSE;

		if (state & BUTTON_B)
			joy_1.btn_b = TRUE;
		else if (changed & BUTTON_B)
			joy_1.btn_b = FALSE;

		if (state & BUTTON_C)
			joy_1.btn_c = TRUE;
		else if (changed & BUTTON_C)
			joy_1.btn_c = FALSE;

		if (state & BUTTON_X)
			joy_1.btn_x = TRUE;
		else if (changed & BUTTON_X)
			joy_1.btn_x = FALSE;

		if (state & BUTTON_Y)
			joy_1.btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joy_1.btn_y = FALSE;

		if (state & BUTTON_Z)
			joy_1.btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joy_1.btn_z = FALSE;

		if (state & BUTTON_START)
		{
			joy_1.btn_start = TRUE;
			if (current_game_state == TITLE)
			{
				select(currentIndex);
			}
		}
		else if (changed & BUTTON_START)
			joy_1.btn_start = FALSE;

		if (state & BUTTON_MODE)
			joy_1.btn_mode = TRUE;

		else if (changed & BUTTON_MODE)
			joy_1.btn_mode = FALSE;
	}

	if (joy == JOY_2)
	{
		if (state & BUTTON_LEFT)
			joy_2.btn_left = TRUE;
		else if (changed & BUTTON_LEFT)
			joy_2.btn_left = FALSE;

		if (state & BUTTON_RIGHT)
			joy_2.btn_right = TRUE;
		else if (changed & BUTTON_RIGHT)
			joy_2.btn_right = FALSE;

		if (state & BUTTON_UP)
			joy_2.btn_up = TRUE;
		else if (changed & BUTTON_UP)
			joy_2.btn_up = FALSE;

		if (state & BUTTON_DOWN)
			joy_2.btn_down = TRUE;
		else if (changed & BUTTON_DOWN)
			joy_2.btn_down = FALSE;

		if (state & BUTTON_A)
			joy_2.btn_a = TRUE;
		else if (changed & BUTTON_A)
			joy_2.btn_a = FALSE;

		if (state & BUTTON_B)
			joy_2.btn_b = TRUE;
		else if (changed & BUTTON_B)
			joy_2.btn_b = FALSE;

		if (state & BUTTON_C)
			joy_2.btn_c = TRUE;
		else if (changed & BUTTON_C)
			joy_2.btn_c = FALSE;

		if (state & BUTTON_X)
			joy_2.btn_x = TRUE;
		else if (changed & BUTTON_X)
			joy_2.btn_x = FALSE;

		if (state & BUTTON_Y)
			joy_2.btn_y = TRUE;
		else if (changed & BUTTON_Y)
			joy_2.btn_y = FALSE;

		if (state & BUTTON_Z)
			joy_2.btn_z = TRUE;
		else if (changed & BUTTON_Z)
			joy_2.btn_z = FALSE;

		if (state & BUTTON_START)
			joy_2.btn_start = TRUE;
		else if (changed & BUTTON_START)
			joy_2.btn_start = FALSE;

		if (state & BUTTON_MODE)
			joy_2.btn_mode = TRUE;
		else if (changed & BUTTON_MODE)
			joy_2.btn_mode = FALSE;
	}
}

static void finiteStateMachine(Entity *p)
{
	switch (p->state)
	{
	case STATE_STANDING:
		// if (p->order_y == DOWN)
		//{
		//	p->state = STATE_FALL;
		//	p->anim = ANIM_JUMP;
		//	p->is_full_anim = FALSE;
		//	p->frame = 4;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }
		if (p->joy->btn_left)
		{
			p->order_x = LEFT;
			p->last_order_x = LEFT;
			p->state = STATE_WALK;
			p->anim = ANIM_WALK;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}
		else if (p->joy->btn_right)
		{
			p->order_x = RIGHT;
			p->last_order_x = RIGHT;
			p->state = STATE_WALK;
			p->anim = ANIM_WALK;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}

		if (p->joy->btn_up)
		{
			p->order_y = UP;
			p->last_order_y = UP;
			p->state = STATE_WALK;
			p->anim = ANIM_JUMP;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}
		else if (p->joy->btn_down)
		{
			p->order_y = DOWN;
			p->last_order_y = DOWN;
			p->state = STATE_WALK;
			p->anim = ANIM_JUMP;
			p->is_full_anim = FALSE;
			p->frame = 4;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		// if (p->joy->btn_z /*  && p->has_stamina */)
		//{
		//	// p->has_stamina = FALSE;
		//	frame_counter = 1;
		//	p->state = STATE_JUMP;
		//	p->anim = ANIM_JUMP;
		//	p->order_y = UP;
		//	if (p->vel_y == 0)
		//	{
		//		p->vel_y += p->impulse_y;
		//	}
		//	p->is_full_anim = TRUE;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }
		if (p->joy->btn_mode /*  && p->has_stamina */)
		{
			// p->has_stamina = FALSE;
			p->order_x = NEUTRAL;
			p->state = STATE_DEATH;
			p->anim = ANIM_DEATH;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}
		if (p->joy->btn_start /*  && p->has_stamina */)
		{
			// p->has_stamina = FALSE;
			p->order_x = NEUTRAL;
			p->is_attacking = TRUE;
			p->anim_duration = TIME_HURT;
			p->state = STATE_HURT;
			p->anim = ANIM_HURT;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}
		if (p->joy->btn_x /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_ATTACK_1;
			p->state = STATE_ATTACK_1;
			p->anim = ANIM_ATTACK_1;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		if (p->joy->btn_y /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_PARRY;
			p->state = STATE_PARRY;
			p->anim = ANIM_DEFEND;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}
		break;

	case STATE_WALK:
		// (p->impulse_x / 2);
		p->max_vel_x = F16_div(p->impulse_x, FIX16(2));
		p->max_vel_y = F16_div(p->impulse_y, FIX16(2));
		// if (p->order_y == DOWN)
		//{
		//	p->state = STATE_FALL;
		//	p->anim = ANIM_JUMP;
		//	p->is_full_anim = FALSE;
		//	p->frame = 4;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }

		// if (p->joy->btn_z /* && p->has_stamina */)
		//{
		//	// p->has_stamina = FALSE;
		//	frame_counter = 1;
		//	p->state = STATE_JUMP;
		//	p->anim = ANIM_JUMP;
		//	p->order_y = UP;
		//	if (p->vel_y == 0)
		//	{
		//		p->vel_y += p->impulse_y;
		//	}
		//	p->is_full_anim = TRUE;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }
		if (p->joy->btn_c)
		{
			p->max_vel_x = p->impulse_x;
			p->max_vel_y = p->impulse_x;
			p->state = STATE_RUN;
			p->anim = ANIM_RUN;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}

		if (p->joy->btn_x /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_ATTACK_1;
			p->state = STATE_ATTACK_1;
			p->anim = ANIM_ATTACK_1;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		if (p->joy->btn_y /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_PARRY;
			p->state = STATE_PARRY;
			p->anim = ANIM_DEFEND;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		if (!(p->joy->btn_left) && !(p->joy->btn_right) && !(p->joy->btn_up) && !(p->joy->btn_down))
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->state = STATE_STANDING;
			p->anim = ANIM_STANDING;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}
		break;

	case STATE_RUN:
		// if (p->order_y == DOWN)
		//{
		//	p->state = STATE_FALL;
		//	p->anim = ANIM_JUMP;
		//	p->is_full_anim = FALSE;
		//	p->frame = 4;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }

		// if (p->joy->btn_z /* && p->has_stamina */)
		//{
		//	// p->has_stamina = FALSE;
		//	frame_counter = 1;
		//	p->state = STATE_JUMP;
		//	p->anim = ANIM_JUMP;
		//	p->order_y = UP;
		//	if (p->vel_y == 0)
		//	{
		//		p->vel_y += p->impulse_y;
		//	}
		//	p->is_full_anim = TRUE;
		//	SPR_setAnimationLoop(p->sprite, FALSE);
		// }

		if (p->joy->btn_x /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_ATTACK_1;
			p->state = STATE_ATTACK_1;
			p->anim = ANIM_ATTACK_1;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		if (p->joy->btn_y /*  && p->has_stamina */)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->is_attacking = TRUE;
			// p->has_stamina = FALSE;
			p->anim_duration = TIME_PARRY;
			p->state = STATE_PARRY;
			p->anim = ANIM_DEFEND;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, FALSE);
		}

		if (!p->joy->btn_c)
		{
			// p->vel_x = (p->impulse_x / 2);
			p->anim = ANIM_WALK;
			p->state = STATE_WALK;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}
		if (!(p->joy->btn_left) && !(p->joy->btn_right) && !(p->joy->btn_up) && !(p->joy->btn_down))
		{
			p->anim = ANIM_WALK;
			p->state = STATE_WALK;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}

		break;

		// case STATE_JUMP:
		//	if (p->order_y == UP)
		//	{
		//		if (p->joy->btn_left)
		//		{
		//			p->order_x = LEFT;
		//			p->last_order_x = LEFT;
		//		}
		//		else if (p->joy->btn_right)
		//		{
		//			p->order_x = RIGHT;
		//			p->last_order_x = RIGHT;
		//		}
		//		else if (!(p->joy->btn_left) && !(p->joy->btn_right))
		//		{
		//			p->order_x = NEUTRAL;
		//		}
		//	}
		//	else if (p->order_y == NEUTRAL)
		//	{
		//		if (p->pos_y < max_y_coord[p->player_id])
		//		{
		//			if (p->joy->btn_left)
		//			{
		//				p->order_x = LEFT;
		//				p->last_order_x = LEFT;
		//			}
		//			else if (p->joy->btn_right)
		//			{
		//				p->order_x = RIGHT;
		//				p->last_order_x = RIGHT;
		//			}
		//			else if (!(p->joy->btn_left) && !(p->joy->btn_right))
		//			{
		//				p->order_x = NEUTRAL;
		//			}
		//		}
		//		else if (p->pos_y >= max_y_coord[p->player_id])
		//		{
		//			p->order_x = NEUTRAL;
		//			p->state = STATE_STANDING;
		//			p->anim = ANIM_STANDING;
		//			p->is_full_anim = TRUE;
		//			SPR_setAnimationLoop(p->sprite, TRUE);
		//		}
		//	}
		//	else if (p->order_y == DOWN)
		//	{
		//		p->state = STATE_FALL;
		//		p->anim = ANIM_JUMP;
		//		p->is_full_anim = FALSE;
		//		p->frame = 4;
		//		SPR_setAnimationLoop(p->sprite, FALSE);
		//	}
		//	break;
		//
		// case STATE_FALL:
		//	if (p->order_y == DOWN)
		//	{
		//		if (p->vel_y < p->max_vel_y)
		//		{
		//			if (p->joy->btn_left)
		//			{
		//				p->order_x = LEFT;
		//				p->last_order_x = LEFT;
		//			}
		//			else if (p->joy->btn_right)
		//			{
		//				p->order_x = RIGHT;
		//				p->last_order_x = RIGHT;
		//			}
		//			else if (!(p->joy->btn_left) && !(p->joy->btn_right))
		//			{
		//				p->order_x = NEUTRAL;
		//			}
		//		}
		//		else if (p->vel_y == p->max_vel_y)
		//		{
		//			p->state = STATE_FALL_LOOP;
		//			p->anim = ANIM_JUMP;
		//			p->is_full_anim = FALSE;
		//			p->frame = 4;
		//			SPR_setAnimationLoop(p->sprite, FALSE);
		//		}
		//	}
		//	else if (p->order_y == NEUTRAL)
		//	{
		//		p->order_x = NEUTRAL;
		//		p->state = STATE_STANDING;
		//		p->anim = ANIM_STANDING;
		//		p->is_full_anim = TRUE;
		//		SPR_setAnimationLoop(p->sprite, TRUE);
		//	}
		//
		//	break;
		//
		// case STATE_FALL_LOOP:
		//	if (p->order_y == DOWN)
		//	{
		//		if (p->joy->btn_left)
		//		{
		//			p->order_x = LEFT;
		//			p->last_order_x = LEFT;
		//		}
		//		else if (p->joy->btn_right)
		//		{
		//			p->order_x = RIGHT;
		//			p->last_order_x = RIGHT;
		//		}
		//		else if (!(p->joy->btn_left) && !(p->joy->btn_right))
		//		{
		//			p->order_x = NEUTRAL;
		//		}
		//	}
		//	else if (p->order_y == NEUTRAL)
		//	{
		//		p->order_x = NEUTRAL;
		//		p->state = STATE_STANDING;
		//		p->anim = ANIM_STANDING;
		//		p->is_full_anim = TRUE;
		//		SPR_setAnimationLoop(p->sprite, TRUE);
		//	}
		//	break;

	case STATE_ATTACK_1:
		if (!p->is_attacking)
		{
			if (p->joy->btn_x)
			{
				p->order_x = NEUTRAL;
				p->order_y = NEUTRAL;
				p->is_attacking = TRUE;
				// p->has_stamina = FALSE;
				p->anim_duration = TIME_ATTACK_2;
				p->state = STATE_ATTACK_2;
				p->anim = ANIM_ATTACK_2;
				p->is_full_anim = TRUE;
				SPR_setAnimationLoop(p->sprite, FALSE);
			}
			else
			{
				p->order_x = NEUTRAL;
				p->order_y = NEUTRAL;
				p->state = STATE_STANDING;
				p->anim = ANIM_STANDING;
				p->is_full_anim = TRUE;
				SPR_setAnimationLoop(p->sprite, TRUE);
			}
		}
		break;

	case STATE_ATTACK_2:
		if (!p->is_attacking)
		{
			if (p->joy->btn_x)
			{
				p->order_x = NEUTRAL;
				p->order_y = NEUTRAL;
				p->is_attacking = TRUE;
				// p->has_stamina = FALSE;
				p->anim_duration = TIME_ATTACK_3;
				p->state = STATE_ATTACK_3;
				p->anim = ANIM_ATTACK_3;
				p->is_full_anim = TRUE;
				SPR_setAnimationLoop(p->sprite, FALSE);
			}
			else
			{
				p->order_x = NEUTRAL;
				p->order_y = NEUTRAL;
				p->state = STATE_STANDING;
				p->anim = ANIM_STANDING;
				p->is_full_anim = TRUE;
				SPR_setAnimationLoop(p->sprite, TRUE);
			}
		}
		break;
	case STATE_ATTACK_3:
		if (!p->is_attacking)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->state = STATE_STANDING;
			p->anim = ANIM_STANDING;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}
		break;

	case STATE_PARRY:
		if (!p->is_attacking)
		{
			if (!p->joy->btn_y)
			{
				p->order_x = NEUTRAL;
				p->order_y = NEUTRAL;
				p->state = STATE_STANDING;
				p->anim = ANIM_STANDING;
				p->is_full_anim = TRUE;
				SPR_setAnimationLoop(p->sprite, TRUE);
			}
		}
		break;

	case STATE_HURT:
		if (!p->is_attacking)
		{
			p->order_x = NEUTRAL;
			p->order_y = NEUTRAL;
			p->state = STATE_STANDING;
			p->anim = ANIM_STANDING;
			p->is_full_anim = TRUE;
			SPR_setAnimationLoop(p->sprite, TRUE);
		}
		break;

	default:
		p->order_x = NEUTRAL;
		p->order_y = NEUTRAL;
		p->state = STATE_STANDING;
		p->anim = ANIM_STANDING;
		p->is_full_anim = TRUE;
		SPR_setAnimationLoop(p->sprite, TRUE);
		break;
	}
}

static void controlXAcceleration(Entity *p)
{
	if (p->order_x != NEUTRAL)
	{
		if (p->vel_x < p->max_vel_x)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_x += acceleration;
			}
		}
		else if (p->vel_x > p->max_vel_x)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_x -= acceleration;
			}
		}
	}
	else if (p->order_x == NEUTRAL)
	{
		if (p->vel_x > 0)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_x -= acceleration;
			}
		}
	}
}

static void controlYAcceleration(Entity *p)
{
	if (p->order_y != NEUTRAL)
	{
		if (p->vel_y < p->max_vel_y)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_y += gravity;
			}
		}
		else if (p->vel_y > p->max_vel_y)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_y -= gravity;
			}
		}
	}
	else if (p->order_y == NEUTRAL)
	{
		if (p->vel_y > 0)
		{
			if (frame_counter % 3 == 0)
			{
				p->vel_y -= gravity;
			}
		}
	}
	// if (p->order_y == UP)
	//{
	//	if (p->vel_y > 0)
	//	{
	//		if (frame_counter % 3 == 0)
	//		{
	//			p->vel_y -= gravity;
	//		}
	//	}
	//	else if (p->vel_y == 0)
	//	{
	//		p->order_y = NEUTRAL;
	//	}
	// }
	// else if (p->order_y == DOWN)
	//{
	//	if (p->vel_y < p->max_vel_y)
	//	{
	//		if (frame_counter % 3 == 0)
	//		{
	//			p->vel_y += gravity;
	//		}
	//	}
	// }
	// else
	//{
	//	if (p->order_y == NEUTRAL)
	//	{
	//		p->vel_y = 0;
	//	}
	// }
}

static void updateEntityPosition(Entity *p)
{
	if (p->vel_x > 0)
	{
		if (p->last_order_x == LEFT || p->order_x == LEFT)
		{
			if (p->pos_x > min_x_coord[p->player_id])
			{
				p->pos_x -= p->vel_x;
			}
		}
		else if (p->last_order_x == RIGHT || p->order_x == RIGHT)
		{
			if (p->pos_x < max_x_coord[p->player_id])
			{
				p->pos_x += p->vel_x;
			}
		}
	}

	if (p->vel_y > 0)
	{
		if (p->last_order_y == UP || p->order_y == UP)
		{
			if (p->pos_y > min_y_coord[p->player_id])
			{
				p->pos_y -= p->vel_y;
			}
		}
		else if (p->last_order_y == DOWN || p->order_y == DOWN)
		{
			if (p->pos_y < max_y_coord[p->player_id])
			{
				p->pos_y += p->vel_y;
			}
		}
	}
}

static void controlHorizontalFlip(Entity *p)
{
	if (p->order_x == RIGHT)
	{
		p->flip_h = FALSE;
	}
	else if (p->order_x == LEFT)
	{
		p->flip_h = TRUE;
	}
}

static void controlVerticalFlip(Entity *p)
{
	if (p->order_y == UP)
	{
		p->flip_v = FALSE;
	}
	else if (p->order_y == DOWN)
	{
		p->flip_v = TRUE;
	}
	else
	{
		p->flip_v = p->flip_v;
	}
}

static void controlAttackTimer(Entity *p)
{
	if (p->is_attacking)
	{
		if (attack_timer[p->player_id] < p->anim_duration)
			attack_timer[p->player_id]++;
		else
		{
			attack_timer[p->player_id] = 0;
			p->is_attacking = FALSE;
		}
	}
}

static void updateCamera(Entity *p)
{
	camera.entity_x_on_map = p->pos_x;
	camera.entity_y_on_map = p->pos_y;
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

static void calculateEntityMapCollision(Entity *p)
{
	// https://cse442--17f-github-io.translate.goog/Gilbert-Johnson-Keerthi-Distance-Algorithm/?_x_tr_sl=en&_x_tr_tl=pt&_x_tr_hl=pt&_x_tr_pto=tc
	// https://www.jeffreythompson.org/collision-detection/table_of_contents.php

	// isso corrige um glitch
	// if (left_edge_column < 0)
	//	left_edge_column = 0;
	// if (top_edge_line < 0)
	//	top_edge_line = 0;
	// if (right_edge_column > MATRIX_MAX_COL_INDEX)
	//	right_edge_column = MATRIX_MAX_COL_INDEX;
	// if (botton_edge_line > MATRIX_MAX_LIN_INDEX)
	//	botton_edge_line = MATRIX_MAX_LIN_INDEX;

	switch (p->last_order_x)
	{
	case LEFT:		
		p->coll_box.x = (p->pos_x + BOX_LEFT_OFFSET);
		// p->coll_box.w = ((p->pos_x + p->width) - BOX_RIGHT_OFFSET);
		p->coll_box.y = (p->pos_y + BOX_TOP_OFFSET);
		p->coll_box.h = ((p->pos_y + p->height) - BOX_BOTTON_OFFSET);

		left_edge_column = (p->coll_box.x / TILE_IN_PIXELS);
		// right_edge_column = (p->coll_box.w / TILE_IN_PIXELS);
		top_edge_line = (p->coll_box.y / TILE_IN_PIXELS);
		botton_edge_line = (p->coll_box.h / TILE_IN_PIXELS);

		top_left_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][left_edge_column];
		botton_left_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][left_edge_column];
		if (top_left_vertex == SOLID_TILE || botton_left_vertex == SOLID_TILE)
		{
			min_x_coord[p->player_id] = ((left_edge_column * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_LEFT_OFFSET;
		}
		else
		{
			min_x_coord[p->player_id] = (MIN_POS_X - BOX_LEFT_OFFSET);
		}
		break;

	case RIGHT:		
		// p->coll_box.x = (p->pos_x + BOX_LEFT_OFFSET);
		p->coll_box.w = ((p->pos_x + p->width) - BOX_RIGHT_OFFSET);
		p->coll_box.y = (p->pos_y + BOX_TOP_OFFSET);
		p->coll_box.h = ((p->pos_y + p->height) - BOX_BOTTON_OFFSET);

		// left_edge_column = (p->coll_box.x / TILE_IN_PIXELS);
		right_edge_column = (p->coll_box.w / TILE_IN_PIXELS);
		top_edge_line = (p->coll_box.y / TILE_IN_PIXELS);
		botton_edge_line = (p->coll_box.h / TILE_IN_PIXELS);

		top_right_vertex = BGA_COLLISION_MATRIX[top_edge_line + 1][right_edge_column];
		botton_right_vertex = BGA_COLLISION_MATRIX[botton_edge_line - 1][right_edge_column];
		if (top_right_vertex == SOLID_TILE || botton_right_vertex == SOLID_TILE)
		{
			max_x_coord[p->player_id] = ((right_edge_column * TILE_IN_PIXELS) - p->width) + BOX_RIGHT_OFFSET;
		}
		else
		{
			max_x_coord[p->player_id] = (MAX_POS_X - p->width) + BOX_RIGHT_OFFSET;
		}
		break;

	default:
		break;
	}

	switch (p->last_order_y)
	{
	case UP:		
		p->coll_box.x = (p->pos_x + BOX_LEFT_OFFSET);
		p->coll_box.w = ((p->pos_x + p->width) - BOX_RIGHT_OFFSET);
		p->coll_box.y = (p->pos_y + BOX_TOP_OFFSET);
		// p->coll_box.h = ((p->pos_y + p->height) - BOX_BOTTON_OFFSET);

		left_edge_column = (p->coll_box.x / TILE_IN_PIXELS);
		right_edge_column = (p->coll_box.w / TILE_IN_PIXELS);
		top_edge_line = (p->coll_box.y / TILE_IN_PIXELS);
		// botton_edge_line = (p->coll_box.h / TILE_IN_PIXELS);

		top_left_vertex = BGA_COLLISION_MATRIX[top_edge_line][left_edge_column + 1];
		top_right_vertex = BGA_COLLISION_MATRIX[top_edge_line][right_edge_column - 1];
		if (top_left_vertex == SOLID_TILE || top_right_vertex == SOLID_TILE)
		{
			min_y_coord[p->player_id] = ((top_edge_line * TILE_IN_PIXELS) + TILE_IN_PIXELS) - BOX_TOP_OFFSET;
		}
		else
		{
			min_y_coord[p->player_id] = (MIN_POS_Y - BOX_TOP_OFFSET);
		}
		break;
	case DOWN:		
		p->coll_box.x = (p->pos_x + BOX_LEFT_OFFSET);
		p->coll_box.w = ((p->pos_x + p->width) - BOX_RIGHT_OFFSET);
		//p->coll_box.y = (p->pos_y + BOX_TOP_OFFSET);
		p->coll_box.h = ((p->pos_y + p->height) - BOX_BOTTON_OFFSET);

		left_edge_column = (p->coll_box.x / TILE_IN_PIXELS);
		right_edge_column = (p->coll_box.w / TILE_IN_PIXELS);
		//top_edge_line = (p->coll_box.y / TILE_IN_PIXELS);
		botton_edge_line = (p->coll_box.h / TILE_IN_PIXELS);

		botton_right_vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column - 1];
		botton_left_vertex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column + 1];
		if (botton_right_vertex == SOLID_TILE || botton_left_vertex == SOLID_TILE)
		{
			max_y_coord[p->player_id] = ((botton_edge_line * TILE_IN_PIXELS) - p->height) + BOX_BOTTON_OFFSET;
		}
		else
		{
			max_y_coord[p->player_id] = (MAX_POS_Y - p->height) + BOX_BOTTON_OFFSET;
		}
		break;
		// case NEUTRAL   :
		//	botton_right  _vertex = BGA_COLLISION_MATRIX[botton_edge_line][right_edge_column];
		//	botton_left_vetex = BGA_COLLISION_MATRIX[botton_edge_line][left_edge_column];
		//	if (botton_right_vertex == SOLID_TILE || botton_left_vertex == SOLID_TILE)
		//	{
		//		max_y_coord[p->player_id] = ((botton_edge_line * TILE_IN_PIXELS) - p->height) + BOX_BOTTON_OFFSET;
		//	}
		//	else
		//	{
		//		max_y_coord[p->player_id] = (MAX_POS_Y - p->height) + BOX_BOTTON_OFFSET;
		//	}
		//	break;

	default:
		break;
	}
}

static void checkTopCollision(Entity *p)
{
	if ((p->last_order_y == UP) && (p->pos_y <= min_y_coord[p->player_id]))
	{
		p->order_y = NEUTRAL;
		p->vel_y = 0;
		p->pos_y = min_y_coord[p->player_id];
	}
}

static void checkBottonCollision(Entity *p)
{
	if ((p->last_order_y == DOWN) && (p->pos_y >= max_y_coord[p->player_id]))
	{
		p->order_y = NEUTRAL;
		p->vel_y = 0;
		p->pos_y = max_y_coord[p->player_id];
	}
}

static void checkLeftCollision(Entity *p)
{
	if (p->last_order_x == LEFT && (p->pos_x <= min_x_coord[p->player_id]))
	{
		p->order_x = NEUTRAL;
		p->vel_x = 0;
		p->pos_x = min_x_coord[p->player_id];
	}
}

static void checkRightCollision(Entity *p)
{
	if (p->last_order_x == RIGHT && (p->pos_x >= max_x_coord[p->player_id]))
	{
		p->order_x = NEUTRAL;
		p->vel_x = 0;
		p->pos_x = max_x_coord[p->player_id];
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