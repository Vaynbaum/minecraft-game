#pragma once
#define WIDTH_SCREEN 1920
#define HEIGHT_SCREEN 1080
#define MIN_COORD_X_BASE 300 
#define MAX_COORD_X_BASE 384 
#define MIN_COORD_Z_BASE 289 
#define MAX_COORD_Z_BASE 360
#define MIN_COORD_Y_BASE 2
#define MAP_LENGTH_AND_WIDTH 511
#define MAP_HEIGHT  19
#define MIN_ZOOM_CAMERA 44.0f 
#define MAX_ZOOM_CAMERA 44.7f
#define RADIUS_BLOCKS_LIGHTING 15
#define STEP_REDUCING_LIGHTING 0.066
#define DIV_R_IN_RGB 13
#define RAND_IRON_GENERATE 10
#define RAND_GOLD_GENERATE 61
#define GREENHOUSE_MIN_X 339
#define GREENHOUSE_MAX_X 343
#define GREENHOUSE_MIN_Y 4
#define GREENHOUSE_MAX_Y 5
#define GREENHOUSE_MIN_Z 331
#define GREENHOUSE_MAX_Z 346
#define ANGLE_SUNSET 35.0
#define MAX_SECONDS_IN_DAY 1200
#define NUM_KEY_IN_ARRAY 100
#define MIN_BLOCKS_DAMAGE 5
#define FACES_CUBE 6
#define BAG_SIZE 9
#define OFFSET_ICON_BAG_RENDER 0.1345f
#define NUM_CHAR_TEXT_RENDER 128
// Block indexes
#define GROUND 0
#define ICE 1
#define BEDROCK 2
#define IRON 3
#define GLASS 4
#define BATTERY 5
#define IRON_ORE 6
#define GOLD_ORE 7
#define BAKE 8
#define FLOOR 9
#define GENERATOR 10
#define LAMP 11
#define GREY 12
#define SOMETHING 13
#define POTATO_GROUND 14
#define POTATO 15
#define BOX 16
#define UNTILLED_GROUND  17
// Indexes of elements
#define BRUSH 18
#define HOE 19
#define PICKAXE 20
#define SHOVEL 21
#define IRON_BULLION 22
#define GOLD_BULLION 23
#define OXYGEN_FULL 24
#define OXYGEN_EMPTY 25
#define POTATO_BAKED 26
#define MONEY 27
#define BUCKET 28
#define BUCKET_WATER 29
#define FIELD_POTATO_WIDTH 5
#define FIELD_POTATO_LEN 13
#define FIELD_BATTERY_WIDTH 10
#define FIELD_BATTERY_LEN 16
#define SOUNDNESS_TOOL 200
#define MAX_NUM_ELEM_ITEM 100
#define NUM_ITEM_CRAFT_INP 3
#define IND_FIELD_FUNC_X(x,width) (x)%(width)
#define IND_FIELD_FUNC_Z(z,length) (z)%(length)
#define RAND_NUM_POTATO rand() % 5 + 1
#define RAND_SECOND_TRIGGER_DIRT rand() % 30 + 120
#define TIME_GROWN_POTATO MAX_SECONDS_IN_DAY
#define HALF_TIME_GROWN_POTATO TIME_GROWN_POTATO/2
#define NUM_BLOCKS_GAME 30
#define MAX_NUM_LIFE_INDEX 10
// Sizes of textures for displaying the menu
#define SIZE_ITEM_X 0.12f
#define SIZE_ITEM_Y 0.14f
#define SIZE_ELEM_MENU 0.1f
#define BIG_SIZE_ELEM_MENU_X 1.3f
#define BIG_SIZE_ELEM_MENU_Y 1.5f
#define RECIPE_COUNT 7
#define CALC_SELECT_ITEM(KEY) Wrl::player.selectElement = KEY - GLFW_KEY_1;
#define SET_BLOCK_PROPERTIES(PARAM_X, PARAM_Y, PARAM_Z, PARAM_EXIST, PARAM_CUBE, PARAM_LIGHT) map[PARAM_X][PARAM_Y][PARAM_Z].exist = PARAM_EXIST;\
map[PARAM_X][PARAM_Y][PARAM_Z].cube = PARAM_CUBE; map[PARAM_X][PARAM_Y][PARAM_Z].light = PARAM_LIGHT;
// Integer and real timers
typedef struct
{
	double lastTime;
	double nowTime;
}timerDouble;
typedef struct
{
	unsigned long lastTime;
	unsigned long nowTime;
}timerLong;
// Pointer to the application window
GLFWwindow* window;
// Variable IDs in the shader program
GLint objectColorLoc, lightColorLoc, modelLoc;
// Matrices for displaying objects on the screen
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
// Auxiliary variables for loops
int i = 0, j = 0, x = 0, y = 0, z = 0, X = 0, Y = 0, Z = 0;
// Border and initial coordinates for loops when displaying the map
int borderX = 0, borderZ = 0, startX = 0, startZ = 0;
int freeLocBag = -1;
// Variables for calculating the time elapsed between the last and current frame
double deltaTime = 0.0f, lastFrame = 0.0f;
// Game day time
double secondDay = 0.0f;
bool flagMouseBind = false;
bool exitMainMenu = false, exitProgram = false, flagNewGame = true, playingSound = false;