// Other libraries
#include "windows.h"
// Other includes
#include "shader.h"
#include "main.h"
#include <cstdlib>

void key_callback(GLFWwindow*, int, int, int, int);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void mouse_button_callback(GLFWwindow*, int, int, int);
void do_movement();
int initGLFWAndGLEW();
void init();
void show();

int main()
{
	if (initGLFWAndGLEW() == -1)
		return -1;
	init();
	mciSendStringW(TEXT("play sounds/background.mp3 repeat"), NULL, 0, NULL);

	// Start menu
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.623f, 0.443f, 0.349f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		Wrl::Mn::showElementMenu(Wrl::Mn::mainMenu.position, Wrl::Mn::mainMenu.sizeX, Wrl::Mn::mainMenu.sizeY, Wrl::Mn::mainMenu.texture);

		glfwSwapBuffers(window);
		Sleep(1);
		if (exitMainMenu)
			break;
	}

	// If the flag has changed, then we will not enter the game cycle
	if(exitProgram)
		glfwSetWindowShouldClose(window, GL_TRUE);
	else
	{
		// Hiding and fixing the mouse
		flagMouseBind = true;
		glfwSetCursorPos(window, WIDTH_SCREEN / 2, HEIGHT_SCREEN / 2);

		glEnable(GL_DEPTH_TEST);
		// Delete files if there is a new game or check if there are files if you continue the game
		Wrl::SLP::deleteWriteSaveFile(flagNewGame);
		Wrl::SLP::loadGame(flagNewGame);

		// Set the time
		Wrl::Fld::timerDirt.timer.lastTime = glfwGetTime();
		Wrl::Fld::timerDirt.trigger = RAND_SECOND_TRIGGER_DIRT;
		lastFrame = glfwGetTime();
	}

	// Game cycle
	while (!glfwWindowShouldClose(window))
	{
		// Calculating the time between displays
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Event checking, calculation of the sun angleand lightingand rendering
		glfwPollEvents();
		do_movement();

		Wrl::Mp::calculateAngleSun();

		//The sound of walking
		if (Wrl::keys[GLFW_KEY_D] || Wrl::keys[GLFW_KEY_A] || Wrl::keys[GLFW_KEY_S] || Wrl::keys[GLFW_KEY_W])
		{
			if (Wrl::player.OnGround && !playingSound)
			{
				playingSound = true;
				mciSendStringW(TEXT("play sounds/walk.mp3 repeat"), NULL, 0, NULL);
			}
				
		}
		else
		{
			mciSendStringW(TEXT("stop sounds/walk.mp3"), NULL, 0, NULL);
			playingSound = false;
		}
			
		// Checking the timer to dust the block
		Wrl::Fld::timerDirt.timer.nowTime = glfwGetTime();
		if (Wrl::Fld::timerDirt.timer.nowTime - Wrl::Fld::timerDirt.timer.lastTime >= Wrl::Fld::timerDirt.trigger)
		{
			Wrl::Fld::timerDirt.timer.lastTime = Wrl::Fld::timerDirt.timer.nowTime;
			Wrl::Fld::timerDirt.trigger = RAND_SECOND_TRIGGER_DIRT;
			if (Wrl::Fld::numberClearBattery)
				Wrl::Fld::dirtBlock();
		}

		// Generate ice at night
		if (secondDay == 600)
			Wrl::SLP::generateIce(Wrl::SLP::img, Wrl::SLP::cnt);

		// Updating player parameters
		Wrl::player.Update(deltaTime);

		// If the number of lives is not left, then exit the game
		if (Wrl::player.heart < 1)
			glfwSetWindowShouldClose(window, GL_TRUE);

		show();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Clearing the memory on the video card
	glDeleteVertexArrays(1, &Wrl::Mp::VAOCube);
	glDeleteBuffers(1, &Wrl::Mp::VBOCube);
	glDeleteVertexArrays(1, &Wrl::Mn::VAOQuad);
	glDeleteBuffers(1, &Wrl::Mn::VBOQuad);
	glDeleteVertexArrays(1, &Wrl::Mn::Txt::VBOFont);
	glDeleteBuffers(1, &Wrl::Mn::Txt::VAOFont);

	// Completing GLFW, clearing all resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

//************************************************************************************************************************************************

int initGLFWAndGLEW()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;

	glfwInit();														// Initializing GLFW
	// Configuring GLFW
	// Specifies the minimum required OpenGL version.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// Major 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// Minor
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Setting the profile for which the context is being created
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);						// Disabling the ability to resize the window

	// Creating a GLFW window object that we can use for GLFW functions
	window = glfwCreateWindow(WIDTH_SCREEN, HEIGHT_SCREEN, "MARS", glfwGetPrimaryMonitor(), nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Installing the necessary callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// Setting the value to true so that GLEW knows how to use the modern approach to extracting function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initializing GLEW to configure OpenGL function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Determine the size of the viewport
	glViewport(0, 0, WIDTH_SCREEN, HEIGHT_SCREEN);
	// Configuring OpenGL parameters
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	return 0;
}

//************************************************************************************************************************************************

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		// Opening and closing the menu inside the game
		if (key == GLFW_KEY_ESCAPE)
		{
			Wrl::Mn::ClM::flagGameMenu = !Wrl::Mn::ClM::flagGameMenu;
			if (!Wrl::Mn::ClM::flagGameMenu)
				flagMouseBind = true;
			else
				flagMouseBind = false;
			Wrl::Mn::ClM::bake = false;
			Wrl::Mn::ClM::generator = false;
			Wrl::Mn::ClM::craft = false;
			Wrl::Mn::ClM::box = false;
			return;
		}
		// The selected item is digit management
		else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
		{
			CALC_SELECT_ITEM(key);
			return;
		}
		else if (key==GLFW_KEY_RIGHT || key == GLFW_KEY_LEFT)
		{
			key == GLFW_KEY_RIGHT ? ++Wrl::player.selectElement : --Wrl::player.selectElement;
			if (Wrl::player.selectElement >= BAG_SIZE - 1)
				Wrl::player.selectElement = BAG_SIZE - 1;
			if (Wrl::player.selectElement < 0)
				Wrl::player.selectElement = 0;
			return;
		}
		// Fixing and displaying the mouse
		else if (key == GLFW_KEY_B)
		{
			flagMouseBind = !flagMouseBind;
			return;
		}
		// Opening the crafting menu
		else if (key == GLFW_KEY_C && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::box && !Wrl::Mn::ClM::flagGameMenu)
		{
			Wrl::Mn::ClM::craft = true;
			flagMouseBind = false;
			return;
		}
		// Drop the item from the bag
		else if (key == GLFW_KEY_P)
		{
			Wrl::player.bag[Wrl::player.selectElement].type = -1;
			Wrl::player.bag[Wrl::player.selectElement].count = 0;
			return;
		}
		else if (key == GLFW_KEY_U && (Wrl::player.bag[Wrl::player.selectElement].type == POTATO || Wrl::player.bag[Wrl::player.selectElement].type == POTATO_BAKED || Wrl::player.bag[Wrl::player.selectElement].type == OXYGEN_FULL))
		{
			if (Wrl::player.bag[Wrl::player.selectElement].type == POTATO)
			{
				Wrl::player.hunger = Wrl::player.hunger + 1 > MAX_NUM_LIFE_INDEX ? MAX_NUM_LIFE_INDEX : Wrl::player.hunger + 1;
				mciSendStringW(TEXT("play sounds/eat.mp3"), NULL, 0, NULL);
			}
				
			if (Wrl::player.bag[Wrl::player.selectElement].type == POTATO_BAKED)
			{
				Wrl::player.hunger = Wrl::player.hunger + MAX_NUM_LIFE_INDEX / 2 > MAX_NUM_LIFE_INDEX ? MAX_NUM_LIFE_INDEX : Wrl::player.hunger + MAX_NUM_LIFE_INDEX / 2;
				mciSendStringW(TEXT("play sounds/eat.mp3"), NULL, 0, NULL);
			}
				
			// Use an oxygen cylinder if there is space in the bag for an empty cylinder
			if (Wrl::player.bag[Wrl::player.selectElement].type == OXYGEN_FULL)
			{
				freeLocBag = -1;
				for (j = 0; j < BAG_SIZE; ++j)
					if (Wrl::player.bag[j].type == -1 || Wrl::player.bag[j].type == OXYGEN_EMPTY)
					{
						freeLocBag = j;
						break;
					}

				if (freeLocBag > -1)
				{
					if (Wrl::player.bag[freeLocBag].type == -1)
					{
						Wrl::player.bag[freeLocBag].type = OXYGEN_EMPTY;
						Wrl::player.bag[freeLocBag].count = 1;
					}
					else
					{
						Wrl::player.bag[freeLocBag].count += 1;
					}
					Wrl::player.oxygen = Wrl::player.oxygen + MAX_NUM_LIFE_INDEX / 2 > MAX_NUM_LIFE_INDEX ? MAX_NUM_LIFE_INDEX : Wrl::player.oxygen + MAX_NUM_LIFE_INDEX / 2;
					Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
				}
				return;
			}
			Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
		}

	}

	// If a key is pressed, the true value with this key is written to the array
	if (key >= 0 && key < NUM_KEY_IN_ARRAY)
	{
		if (action == GLFW_PRESS)
			Wrl::keys[key] = true;
		else if (action == GLFW_RELEASE)
			Wrl::keys[key] = false;
	}
	
}

//************************************************************************************************************************************************

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(!Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::box && !Wrl::Mn::ClM::flagGameMenu)
		// Camera rotation
		Wrl::player.ProcessMouseMovement(xpos, ypos, WIDTH_SCREEN / 2, HEIGHT_SCREEN / 2);
}

//************************************************************************************************************************************************

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::box && !Wrl::Mn::ClM::flagGameMenu)
		// Changing the camera zoom
		Wrl::player.ProcessMouseScroll(yoffset);
}

//************************************************************************************************************************************************

void do_movement()
{
	// Camera control
	if (!Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft && 
		!Wrl::Mn::ClM::box && !Wrl::Mn::ClM::flagGameMenu)
	{
		// Jump
		if (Wrl::keys[GLFW_KEY_SPACE])
		{
			Wrl::player.ProcessKeyboard(GLFW_KEY_SPACE, deltaTime);
			mciSendStringW(TEXT("stop sounds/walk.mp3"), NULL, 0, NULL); 
			playingSound = false;
		}

		// Go forward
		if (Wrl::keys[GLFW_KEY_W])
			Wrl::player.ProcessKeyboard(GLFW_KEY_W, deltaTime);
		// Go back
		if (Wrl::keys[GLFW_KEY_S])
			Wrl::player.ProcessKeyboard(GLFW_KEY_S, deltaTime);
		// Go left
		if (Wrl::keys[GLFW_KEY_A])
			Wrl::player.ProcessKeyboard(GLFW_KEY_A, deltaTime);
		// Go right
		if (Wrl::keys[GLFW_KEY_D])
			Wrl::player.ProcessKeyboard(GLFW_KEY_D, deltaTime);
	}
	
}

//************************************************************************************************************************************************

void init()
{
	srand(time(NULL));
	// Generating shader programs, initializing VBO, VAO, and loading a font for text output
	Wrl::Mp::initCubeVAOAndVBO("shaders/vertCubeTexture.txt", "shaders/fragCubeTexture.txt");
	Wrl::Mp::initLight("shaders/vertLight.txt", "shaders/fragLight.txt");
	Wrl::Mn::initMenu("shaders/vertQuadTexture.txt", "shaders/fragQuadTexture.txt");
	Wrl::Mn::Txt::initFreeType("shaders/vertFont.txt", "shaders/fragFont.txt");

	// Loading textures of menu items
	Wrl::loadTexture("textures/other_Picture/cursor.png", &Wrl::Mn::cursor.texture);
	Wrl::loadTexture("textures/other_Picture/heartFull.png", &Wrl::Mn::heart.texture);
	Wrl::loadTexture("textures/other_Picture/heartNull.png", &Wrl::Mn::textureHeartNull);
	Wrl::loadTexture("textures/other_Picture/oxygen.png", &Wrl::Mn::oxygen.texture);
	Wrl::loadTexture("textures/other_Picture/invenFast.png", &Wrl::Mn::inventory.texture);
	Wrl::loadTexture("textures/other_Picture/potato.png", &Wrl::Mn::hunger.texture);
	Wrl::loadTexture("textures/other_Picture/frame.png", &Wrl::Mn::frame.texture);
	Wrl::loadTexture("textures/other_Picture/GeneratorInterface.png", &Wrl::Mn::bakeInterface.texture);
	Wrl::loadTexture("textures/other_Picture/GeneratorInterface.png", &Wrl::Mn::generatorInterface.texture);
	Wrl::loadTexture("textures/other_Picture/Craft.png", &Wrl::Mn::craftInterface.texture);
	Wrl::loadTexture("textures/other_Picture/BigBox.png", &Wrl::Mn::boxInterface.texture);
	Wrl::loadTexture("textures/other_Picture/GameMenu.png", &Wrl::Mn::gameMenu.texture);
	Wrl::loadTexture("textures/other_Picture/MainMenu.png", &Wrl::Mn::mainMenu.texture);

	// Loading the app icon
	GLFWimage img;
	int count;
	img.pixels = stbi_load("textures/groundBox/groundIcon.png", &img.width, &img.height, &count, 0);
	glfwSetWindowIcon(window, 1, &img);
	stbi_image_free(img.pixels);

	// Initializing blocks
	Wrl::arrayBlocks[GROUND].init("textures/groundBox/side.jpg", "textures/groundBox/top.jpg", "textures/groundBox/bottom.jpg", "textures/groundBox/groundIcon.png", true, true,"         Ground");
	Wrl::arrayBlocks[ICE].init("textures/iceBox/ice.jpg", "textures/iceBox/iceIcon.png", true, true,"            Ice");
	Wrl::arrayBlocks[BEDROCK].init(false, false,"textures/bedrock.jpg");
	Wrl::arrayBlocks[IRON].init("textures/ironBox/iron.jpg", "textures/ironBox/ironIcon.png", false, false,"");
	Wrl::arrayBlocks[GLASS].init("textures/glassBox/glass.png", "textures/glassBox/glassIcon.png", false, false,"");
	Wrl::arrayBlocks[BATTERY].init("textures/solarBatteryBox/Side.jpg","textures/solarBatteryBox/TopClear.jpg", "textures/solarBatteryBox/TopDirty.jpg","textures/solarBatteryBox/Bottom.jpg", "textures/solarBatteryBox/solarBatteryIcon.png");
	Wrl::arrayBlocks[IRON_ORE].init("textures/ironBox/ironOre.jpg", "textures/ironBox/ironOreIcon.png", true, true,"       Iron ore");
	Wrl::arrayBlocks[GOLD_ORE].init("textures/goldOreBox/gold.jpg", "textures/goldOreBox/goldOreIcon.png", true, true,"       Gold ore");
	Wrl::arrayBlocks[BAKE].init("textures/bake.jpg", "textures/bake.jpg", false, false,"");
	Wrl::arrayBlocks[FLOOR].init("textures/floor.jpg", "textures/floor.jpg", false, false,"");
	Wrl::arrayBlocks[GENERATOR].init("textures/generatorOtsek.jpg", "textures/generatorOtsek.jpg", false, false,"");
	Wrl::arrayBlocks[LAMP].init("textures/lamp.jpg", "textures/lamp.jpg", false, false,"");
	Wrl::arrayBlocks[GREY].init("textures/concretereGrey.jpg", "textures/concretereGrey.jpg", false, false,"");
	Wrl::arrayBlocks[SOMETHING].init("textures/something.jpg", "textures/something.jpg", false, false,"");
	Wrl::arrayBlocks[POTATO_GROUND].init("textures/potato/PotatoGround.jpg", "textures/potato/PotatoGround.jpg", false, false,"");
	Wrl::arrayBlocks[BOX].init("textures/box/boxSide.jpg", "textures/box/BoxBottomAndTop.jpg", "textures/box/boxSide.jpg", false, false);
	Wrl::arrayBlocks[POTATO].init("textures/potato/potato1.png", "textures/potato/potato2.png", "textures/potato/potato3.png", "textures/potato/Vacuum.png", "textures/potato/PotatoGround.jpg", "textures/potato/potatoEat.png", true, true,"         Potato");
	Wrl::arrayBlocks[UNTILLED_GROUND].init("textures/potato/potatoGroundUntilled.jpg", "textures/potato/potatoGroundUntilled.jpg", true, true,"");
	// Initializing elements
	Wrl::arrayBlocks[BRUSH].init("textures/elements/Brush.png", false, false,"          Brush");
	Wrl::arrayBlocks[PICKAXE].init("textures/elements/Pickaxe.png", false, false,"        Pickaxe");
	Wrl::arrayBlocks[SHOVEL].init("textures/elements/Shovel.png", false, false,"         Shovel");
	Wrl::arrayBlocks[HOE].init("textures/elements/Hoe.png", false, false,"            Hoe");
	Wrl::arrayBlocks[IRON_BULLION].init("textures/elements/IronKusok.png", false, false,"     Iron Ingot");
	Wrl::arrayBlocks[GOLD_BULLION].init("textures/elements/GoldKusok.png", false, false,"       Gold Bar");
	Wrl::arrayBlocks[OXYGEN_FULL].init("textures/elements/OxygenFull.png", false, false,"    Oxygen tank");
	Wrl::arrayBlocks[OXYGEN_EMPTY].init("textures/elements/OxygenEmpty.png", false, false,"        Balloon");
	Wrl::arrayBlocks[POTATO_BAKED].init("textures/elements/PotatoBaked.png", false, false," Baked potatoes");
	Wrl::arrayBlocks[MONEY].init("textures/elements/Money.png", false, false,"           Coin");
	Wrl::arrayBlocks[BUCKET].init("textures/elements/Bucket.png", false, false, "         Bucket");
	Wrl::arrayBlocks[BUCKET_WATER].init("textures/elements/BucketWater.png", false, false, "Bucket of water");
	// Creating crafting Recipes
	i = 0;
	Wrl::Mn::ClM::addRecipe(SHOVEL, i++,"textures/elements/shovelCraft.png");
	Wrl::Mn::ClM::addRecipe(BRUSH, i++, "textures/elements/brushCraft.png");
	Wrl::Mn::ClM::addRecipe(PICKAXE, i++, "textures/elements/pickaxeCraft.png");
	Wrl::Mn::ClM::addRecipe(HOE, i++, "textures/elements/hoeCraft.png");
	Wrl::Mn::ClM::addRecipe(OXYGEN_EMPTY, i++, "textures/elements/balloonCraft.png");
	Wrl::Mn::ClM::addRecipe(MONEY, i++, "textures/elements/moneyCraft.png");
	Wrl::Mn::ClM::addRecipe(BUCKET, i++, "textures/elements/bucketCraft.png");

	// Setting the parameters for displaying the start menu
	glDisable(GL_DEPTH_TEST);
	projection = glm::ortho((GLfloat)(-(WIDTH_SCREEN / HEIGHT_SCREEN)), (GLfloat)(WIDTH_SCREEN / HEIGHT_SCREEN), -1.0f, 1.0f, -1.0f, 1.0f);
	glUniform1i(glGetUniformLocation(Wrl::Mn::shaderProgramQuad, "TexCoord"), 0);
	// Use a shader program
	Use(Wrl::Mn::shaderProgramQuad);
	// Linking the VAO
	glBindVertexArray(Wrl::Mn::VAOQuad);
	// Get the location of uniform matrices and set the matrices
	glUniformMatrix4fv(glGetUniformLocation(Wrl::Mn::shaderProgramQuad, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

//************************************************************************************************************************************************

void show()
{
	glClearColor(0.623f * Wrl::Mp::coefIllum, 0.443f * Wrl::Mp::coefIllum, 0.349f * Wrl::Mp::coefIllum * Wrl::Mp::coefBlue, 1.0f);
	// Clearing color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Show or hide the mouse
	if (!flagMouseBind)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, WIDTH_SCREEN / 2, HEIGHT_SCREEN / 2);
	}
		
	// Creating Transformations
	view = glm::mat4(1.0f);
	view = Wrl::player.GetViewMatrix();
	projection = glm::mat4(1.0f);
	projection = glm::perspective(Wrl::player.Zoom, (GLfloat)WIDTH_SCREEN / (GLfloat)HEIGHT_SCREEN, 0.1f, 1100.0f);

	// Displaying the sun and the map
	Wrl::Mp::showLight();
	Wrl::Mp::showMap();

	// Displaying the menu
	glDisable(GL_DEPTH_TEST);
		projection = glm::ortho((GLfloat)(-(WIDTH_SCREEN / HEIGHT_SCREEN)), (GLfloat)(WIDTH_SCREEN / HEIGHT_SCREEN), -1.0f, 1.0f, -1.0f, 1.0f);
		Wrl::Mn::showMenu();
	glEnable(GL_DEPTH_TEST);
}

//************************************************************************************************************************************************

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if((button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT) && action == GLFW_PRESS)

		if (flagMouseBind)
		{
			// Player's eye position
			float x = Wrl::player.Position.x;
			float y = Wrl::player.Position.y + Wrl::player.Height/2;
			float z = Wrl::player.Position.z;

			// The point the player is looking at
			int X = x / Wrl::sizeCube, Y = y / Wrl::sizeCube, Z = z / Wrl::sizeCube, oldX = X, oldY = Y, oldZ = Z;
			int dist = 0;

			while (dist < 5)
			{
				++dist;
				x += Wrl::player.Front.x; X = x / Wrl::sizeCube;
				y += Wrl::player.Front.y; Y = y / Wrl::sizeCube;
				z += Wrl::player.Front.z; Z = z / Wrl::sizeCube;

				if (Wrl::player.Check(X, Y, Z))
				{
					if (button == GLFW_MOUSE_BUTTON_LEFT)
					{
						// Interaction with the solar battery
						if (Wrl::map[X][Y][Z].cube == BATTERY && Wrl::player.bag[Wrl::player.selectElement].type == BRUSH)
						{
							if (Wrl::Fld::arrayBattery[IND_FIELD_FUNC_X(X, FIELD_BATTERY_WIDTH)][IND_FIELD_FUNC_Z(Z, FIELD_BATTERY_LEN)].dust)
							{
								Wrl::Fld::arrayBattery[IND_FIELD_FUNC_X(X, FIELD_BATTERY_WIDTH)][IND_FIELD_FUNC_Z(Z, FIELD_BATTERY_LEN)].dust = false;
								++Wrl::Fld::numberClearBattery;
								mciSendStringW(TEXT("play sounds/clean.mp3"), NULL, 0, NULL);
							}
							Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
							return;
						}

						// Interaction with the earth for potatoes
						if (Wrl::map[X][Y][Z].cube == UNTILLED_GROUND && Wrl::player.bag[Wrl::player.selectElement].type == HOE)
						{
							Wrl::map[X][Y][Z].cube = POTATO_GROUND;
							Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
							return;
						}

						// Interaction with the furnace, generator, chest blocks
						if (Wrl::map[X][Y][Z].cube == BAKE && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::box && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::flagGameMenu)
						{
							Wrl::Mn::ClM::bake = true;
							flagMouseBind = false;
							return;
						}
						if (Wrl::map[X][Y][Z].cube == GENERATOR && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::box && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::flagGameMenu)
						{
							Wrl::Mn::ClM::generator = true;
							flagMouseBind = false;
							return;
						}
						if (Wrl::map[X][Y][Z].cube == BOX && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::flagGameMenu)
						{
							Wrl::Mn::ClM::box = true;
							flagMouseBind = false;
							return;
						}

						// Try to destroy the block
						if (Wrl::arrayBlocks[Wrl::map[X][Y][Z].cube].destroy)
						{
							bool added = false, foundFreeLoc = false;
							int freeLoc = -1;
							for (int i = 0; i < BAG_SIZE; ++i)
							{
								// Add to the bag if there is space
								if (Wrl::player.bag[i].type == Wrl::map[X][Y][Z].cube && !added && Wrl::player.bag[i].count + 1 < MAX_NUM_ELEM_ITEM)
								{
									if ((Wrl::map[X][Y][Z].cube == POTATO && Wrl::player.bag[Wrl::player.selectElement].type == SHOVEL) || (Wrl::map[X][Y][Z].cube != POTATO && Wrl::player.bag[Wrl::player.selectElement].type == PICKAXE))
									{
										// If the potatoes are ripe, then dig up a few
										if (Wrl::map[X][Y][Z].cube == POTATO && Wrl::Fld::arrayPotato[IND_FIELD_FUNC_X(X, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(Z, FIELD_POTATO_LEN)].grown)
											Wrl::player.bag[i].count += RAND_NUM_POTATO;
										else
											++Wrl::player.bag[i].count;
										added = true;
										Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
										break;
									}

								}
								// Remember the empty space in the bag
								if (Wrl::player.bag[i].type == -1 && !added && !foundFreeLoc)
								{
									freeLoc = i;
									foundFreeLoc = true;
								}
							}

							// Add to an empty space in the bag
							if (!added && foundFreeLoc)
							{
								if ((Wrl::map[X][Y][Z].cube == POTATO && Wrl::player.bag[Wrl::player.selectElement].type == SHOVEL) || (Wrl::map[X][Y][Z].cube != POTATO && Wrl::player.bag[Wrl::player.selectElement].type == PICKAXE))
								{
									Wrl::player.bag[freeLoc].type = Wrl::map[X][Y][Z].cube;
									added = true;
									// If the potatoes are ripe, then dig up a few
									if (Wrl::map[X][Y][Z].cube == POTATO && Wrl::Fld::arrayPotato[IND_FIELD_FUNC_X(X, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_X(Z, FIELD_POTATO_LEN)].grown)
										Wrl::player.bag[freeLoc].count = RAND_NUM_POTATO;
									else
										Wrl::player.bag[freeLoc].count = 1;
									Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
								}
							}

							// Destroy the block
							if (added)
							{
								Wrl::map[X][Y][Z].exist = false;
								mciSendStringW(TEXT("play sounds/build.mp3"), NULL, 0, NULL);
								return;
							}
						}
					}
					
					// Put a block on the map or plant potatoes
					else if(Wrl::player.bag[Wrl::player.selectElement].type >= 0 && Wrl::arrayBlocks[Wrl::player.bag[Wrl::player.selectElement].type].build &&
						!Wrl::map[oldX][oldY][oldZ].exist && Wrl::map[oldX][oldY - 1][oldZ].cube != POTATO)
					{
						// Plant potatoes only in the ground
						if (Wrl::player.bag[Wrl::player.selectElement].type == POTATO && Wrl::map[oldX][oldY - 1][oldZ].cube != POTATO_GROUND)
							return;
						if (Wrl::map[oldX][oldY - 1][oldZ].cube == POTATO_GROUND)
							Wrl::map[oldX][oldY - 1][oldZ].cube = UNTILLED_GROUND;

						if (oldX != (int)Wrl::player.Position.x / Wrl::sizeCube || oldZ != (int)Wrl::player.Position.z / Wrl::sizeCube ||
							(oldY != (int)(Wrl::player.Position.y - Wrl::player.Height / 2) / Wrl::sizeCube && oldY != (int)(Wrl::player.Position.y) / Wrl::sizeCube))
						{
							if (Wrl::player.bag[Wrl::player.selectElement].type == POTATO)
								Wrl::Fld::plant(IND_FIELD_FUNC_X(oldX, FIELD_POTATO_WIDTH), IND_FIELD_FUNC_Z(oldZ, FIELD_POTATO_LEN));
							Wrl::map[oldX][oldY][oldZ].exist = true;
							mciSendStringW(TEXT("play sounds/build.mp3"), NULL, 0, NULL);
							Wrl::map[oldX][oldY][oldZ].cube = Wrl::player.bag[Wrl::player.selectElement].type;

							Wrl::Mn::ClM::decreaseCount(Wrl::player.bag[Wrl::player.selectElement], 1);
						}
						return;
					}
				}
			// Remember the coordinates of the current cube
				oldX = X; oldY = Y; oldZ = Z;
			}
		}

		// If the mouse is active and the mouse button is pressed, then translate the screen coordinates to OpenGL coordinates and call the functions
		else
		{
			Wrl::Mn::ClM::calcuateCoordOpenGL();

			if(!exitMainMenu)
				// Start menu
				Wrl::Mn::ClM::mainMenuClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY);
			else
			{
				if (Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::box)
					Wrl::Mn::ClM::bakeClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY, button);
			
				else if (Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::box)
					Wrl::Mn::ClM::generatorClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY, button);

				else if (Wrl::Mn::ClM::craft && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::box)
					Wrl::Mn::ClM::craftClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY, button);

				else if (Wrl::Mn::ClM::box && !Wrl::Mn::ClM::bake && !Wrl::Mn::ClM::generator && !Wrl::Mn::ClM::craft)
					Wrl::Mn::ClM::boxClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY, button);

				else if (Wrl::Mn::ClM::flagGameMenu)
					Wrl::SLP::gameMenuClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY);

				Wrl::Mn::ClM::bagClick(Wrl::Mn::ClM::coordOpenGLX, Wrl::Mn::ClM::coordOpenGLY, button);
			}
		}
}