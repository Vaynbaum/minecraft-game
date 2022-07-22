#pragma once
// OpenGL Libraries
#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType Library
#include <ft2build.h>
#include FT_FREETYPE_H
// Other libraries
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <fstream>
#include "variable.h"
#include <shlobj.h> //SHLDialog
#include <stdio.h>
#pragma comment (lib, "winmm.lib")

namespace Wrl
	//World
{
	//************************************************************************************************************************************************

	void loadTexture(const GLchar* fileName, GLuint* texture)
	{
		// Allocate memory on the video card for the texture and bind it
		glGenTextures(1, texture);
		glBindTexture(GL_TEXTURE_2D, *texture);

		// Repeats the texture along the S and T axes
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Configuring Texture Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Loading and creating a mipmaps texture
		GLFWimage image; int count;
		image.pixels = stbi_load(fileName, &image.width, &image.height, &count, 0);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, count == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image.pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, (GLuint)0);

		stbi_image_free(image.pixels);
	}

	//************************************************************************************************************************************************

	class block
	{
	public:
		GLuint* type;
		GLuint icon;
		bool destroy;
		bool build;
		std::string name;
		void init(const GLchar* pathIcon, bool destroyFlag, bool buildFlag, const GLchar* Paramname)
		{
			loadTexture(pathIcon, &icon);
			destroy = destroyFlag;
			build = buildFlag;
			name = Paramname;
		}
		void init(const GLchar* pathSide1, const GLchar* pathSide2, const GLchar* pathSide3, const GLchar* pathTop, const GLchar* pathBottom, const GLchar* pathIcon, bool destroyFlag, bool buildFlag, const GLchar* Paramname)
		{
			type = new GLuint[5];
			loadTexture(pathSide1, type);
			loadTexture(pathSide2, type + 1);
			loadTexture(pathSide3, type + 2);
			loadTexture(pathBottom, type + 3);
			loadTexture(pathTop, type + 4);
			loadTexture(pathIcon, &icon);
			destroy = destroyFlag;
			build = buildFlag;
			name = Paramname;
		}
		void init(const GLchar* pathSide, const GLchar* pathTop1, const GLchar* pathTop2, const GLchar* pathBottom, const GLchar* pathIcon)
		{
			type = new GLuint[4];
			loadTexture(pathSide, type);
			loadTexture(pathTop1, type + 2);
			loadTexture(pathTop2, type + 3);
			loadTexture(pathBottom, type + 1);
			loadTexture(pathIcon, &icon);
		}
		void init(const GLchar* pathSide, const GLchar* pathTop, const GLchar* pathBottom, const GLchar* pathIcon, bool destroyFlag, bool buildFlag, const GLchar* Paramname)
		{
			type = new GLuint[3];
			loadTexture(pathSide, type);
			loadTexture(pathBottom, type + 1);
			loadTexture(pathTop, type + 2);
			loadTexture(pathIcon, &icon);
			destroy = destroyFlag;
			build = buildFlag;
			name = Paramname;
		}
		void init(const GLchar* pathSide, const GLchar* pathBottomAndTop, const GLchar* pathIcon, bool destroyFlag, bool buildFlag)
		{
			type = new GLuint[2];
			loadTexture(pathSide, type);
			loadTexture(pathBottomAndTop, type + 1);
			loadTexture(pathIcon, &icon);
			destroy = destroyFlag;
			build = buildFlag;
		}
		void init(bool destroyFlag, bool buildFlag, const GLchar* path)
		{
			type = new GLuint;
			loadTexture(path, type);
			destroy = destroyFlag;
			build = buildFlag;
		}
		void init(const GLchar* path, const GLchar* pathIcon, bool destroyFlag, bool buildFlag, const GLchar* Paramname)
		{
			type = new GLuint;
			loadTexture(path, type);
			loadTexture(pathIcon, &icon);
			destroy = destroyFlag;
			build = buildFlag;
			name = Paramname;
		}
	};
	typedef struct
	{
		bool exist;
		int cube;
		float light;
	}blockMap;
	// A cell for a thing
	typedef struct
	{
		GLfloat sizeX;
		GLfloat sizeY;
		glm::vec2 position;
		int type;
		unsigned char count;
	}item;
	blockMap map[MAP_LENGTH_AND_WIDTH][MAP_HEIGHT][MAP_LENGTH_AND_WIDTH];
	// Array of different blocks
	block arrayBlocks[NUM_BLOCKS_GAME];
	GLfloat sizeCube = 1.0f;
	// Array of pressed keys
	bool keys[NUM_KEY_IN_ARRAY];

	namespace Fld
		//Space on the map for potatoes and solar panels
	{
		typedef struct
		{
			double timer;
			bool grown;
		}parametersPotato;
		typedef struct
		{
			bool dust;
		}parametersBattery;
		parametersPotato arrayPotato[FIELD_POTATO_WIDTH][FIELD_POTATO_LEN];
		parametersBattery arrayBattery[FIELD_BATTERY_WIDTH][FIELD_BATTERY_LEN];

		// Dust the battery timer
		typedef struct
		{
			timerDouble timer;
			int trigger;
		}dirt;
		dirt timerDirt = { 0,0,0 };

		int numberClearBattery = FIELD_BATTERY_WIDTH * FIELD_BATTERY_LEN;
		int maxNumBat = numberClearBattery;
		int timeGrowPotato = 0;

		//************************************************************************************************************************************************
		// When the potatoes were planted
		void plant(int x, int z)
		{
			arrayPotato[x][z].timer = 0.0f;
			arrayPotato[x][z].grown = false;
		}

		//************************************************************************************************************************************************

		void dirtBlock()
		{
			// Search until a clean solar battery is found and dust it
			int randX = rand() % FIELD_BATTERY_WIDTH, randZ = rand() % FIELD_BATTERY_LEN;
			while (numberClearBattery)
			{
				if (!arrayBattery[randX][randZ].dust)
				{
					arrayBattery[randX][randZ].dust = true;
					break;
				}
				else
				{
					randX = rand() % FIELD_BATTERY_WIDTH;
					randZ = rand() % FIELD_BATTERY_LEN;
				}
			}
			--numberClearBattery;
		}

		//************************************************************************************************************************************************
	}

	class Player
	{
	private:
		//************************************************************************************************************************************************
		
		// Collision detection and resolving
		void Collision(GLfloat Dx, GLfloat Dy, GLfloat Dz, GLfloat sizeCube)
		{
			// Pass through the cubes that surround the hero
			for (int X = (Position.x - Width) / sizeCube; X < (Position.x + Width) / sizeCube; ++X)
				for (int Y = (Position.y - Height) / sizeCube; Y < (Position.y + Height) / sizeCube; ++Y)
					for (int Z = (Position.z - Length) / sizeCube; Z < (Position.z + Length) / sizeCube; ++Z)
						// Checking whether there is such a cube on the map
						if (Check(X, Y, Z) && map[X][Y][Z].cube != POTATO)
						{
							// Against the direction of the x axis
							if (Dx < 0)
								Position.x = X + sizeCube + Width;
							// In the direction of the x axis
							if (Dx > 0)
								Position.x = X - Width;
							if (Dy < 0)
							{
								OnGround = true;
								Jump = 0;
								Position.y = Y + sizeCube + Height;
								if (map[X][Y][Z].cube == POTATO_GROUND)
									map[X][Y][Z].cube = UNTILLED_GROUND;
								if (map[X][Y + 1][Z].cube == POTATO && map[X][Y + 1][Z].exist)
								{
									map[X][Y + 1][Z].exist = false;
									Fld::arrayPotato[IND_FIELD_FUNC_X(X, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(Z, FIELD_POTATO_LEN)].grown = true;
								}

								int damageHeight = oldY - Position.y;
								if (damageHeight > MIN_BLOCKS_DAMAGE)
								{
									heart -= (damageHeight - MIN_BLOCKS_DAMAGE);
									mciSendStringW(TEXT("play sounds/damage.mp3"), NULL, 0, NULL);
								}
									
								oldY = Position.y;
							}
							if (Dy > 0)
							{
								Jump = 0;
								Position.y = Y - Height;
							}
							if (Dz < 0)
								Position.z = Z + sizeCube + Length;
							if (Dz > 0)
								Position.z = Z - Length;
						}
		}

		//************************************************************************************************************************************************
		
		// Updating vectors
		void UpdateCameraVectors()
		{
			// Change the direction vector depending on the angles
			glm::vec3 front = glm::vec3(1.0f);
			front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
			front.y = sin(glm::radians(Pitch));
			front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));
			Front = glm::normalize(front);

			Right = glm::normalize(glm::cross(Front, WorldUp));
			Up = glm::normalize(glm::cross(Right, Front));
		}

		//************************************************************************************************************************************************

		timerLong oxygenTimer;
		timerLong heartTimer;
		timerLong hungerTimer;

	public:
		// Camera Attributes
		glm::vec3 Position, Front, WorldUp, Right, Up;
		// Euler angles
		GLfloat Yaw, Pitch;
		// Player Parameters
		GLfloat MovementSpeed, MouseSensitivity, Zoom;
		GLfloat Width, Length, Height;
		bool OnGround;
		// The speed of movement in the directions
		GLfloat Jump, Dx, Dz;
		// The number of hearts, oxygen and hunger
		int heart, oxygen, hunger;
		// Previous height on the ground
		int oldY;
		item bag[BAG_SIZE];
		// The selected item in the bag
		char selectElement;
		// Map visibility radiuses
		int radiusHoris, radiusFront;
		item handItem;

		//************************************************************************************************************************************************

		Player(glm::vec3 position)
		{
			Position = position;
			WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
			Up = glm::normalize(glm::cross(Right, Front));

			Yaw = -90.0f;  Pitch = 0.0f;

			MovementSpeed = 5.0f; MouseSensitivity = 0.15f; Zoom = MAX_ZOOM_CAMERA;
			OnGround = false;
			Height = 1.0f; Width = 0.25f; Length = 0.25f;

			oldY = position.y / sizeCube;
			Jump = 0; Dx = 0; Dz = 0;
			selectElement = 0;
			radiusFront = 25, radiusHoris = 17;
			handItem.count = 0;
			handItem.type = -1;

			UpdateCameraVectors();

			oxygenTimer.lastTime = oxygenTimer.nowTime = glfwGetTime();
			hungerTimer.lastTime = hungerTimer.nowTime = glfwGetTime();
		}

		//************************************************************************************************************************************************
		
		// Using vectors
		glm::mat4 GetViewMatrix()
		{
			return glm::lookAt(glm::vec3(Position.x, Position.y + Height / 2, Position.z),
				glm::vec3(Position.x + Front.x, Position.y + Height / 2 + Front.y, Position.z + Front.z), Up);
		}

		//************************************************************************************************************************************************

		// Moving the player
		void ProcessKeyboard(int direction, GLfloat deltaTime)
		{
			if (direction == GLFW_KEY_SPACE)
				if (OnGround)
				{
					OnGround = false;
					Jump = 2.7f;
				}

			if (direction == GLFW_KEY_W)
			{
				Dx = Front.x * MovementSpeed;
				Dz = Front.z * MovementSpeed;
			}
			if (direction == GLFW_KEY_S)
			{
				Dx = -Front.x * MovementSpeed;
				Dz = -Front.z * MovementSpeed;
			}
			if (direction == GLFW_KEY_A)
			{
				Dx = -Right.x * MovementSpeed;
				Dz = -Right.z * MovementSpeed;
			}
			if (direction == GLFW_KEY_D)
			{
				Dx = Right.x * MovementSpeed;
				Dz = Right.z * MovementSpeed;
			}

			// Do not let the hero go outside the map
			Position.x += Dx * deltaTime;
			Collision(Dx, 0, 0, sizeCube);
			Position.z += Dz * deltaTime;
			Collision(0, 0, Dz, sizeCube);
			if (Position.x < Width)
			{
				Position.x = Width;
			}
			if (Position.x > MAP_LENGTH_AND_WIDTH - Width)
			{
				Position.x = MAP_LENGTH_AND_WIDTH - Width;
			}
			if (Position.z < Length)
			{
				Position.z = Length;
			}
			if (Position.z > MAP_LENGTH_AND_WIDTH - Length)
			{
				Position.z = MAP_LENGTH_AND_WIDTH - Length;
			}
			Dx = Dz = 0;
		}
		//************************************************************************************************************************************************

		// Changing angles
		void ProcessMouseMovement(float xpos, float ypos, int centerX, int centerY)
		{
			if (flagMouseBind)
			{
				GLfloat xoffset = xpos - centerX;
				GLfloat yoffset = centerY - ypos; // Reverse the order of subtraction because the window Y-coordinates increase from top to bottom 

				xoffset *= MouseSensitivity;
				yoffset *= MouseSensitivity;

				Yaw += xoffset;
				Pitch += yoffset;

				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;

				UpdateCameraVectors();
				// Return the mouse to the center of the screen
				glfwSetCursorPos(window, centerX, centerY);
			}
		}

		//************************************************************************************************************************************************
	
		// Changing the zoom
		void ProcessMouseScroll(GLfloat yoffset)
		{
			if (Zoom >= MIN_ZOOM_CAMERA && Zoom <= MAX_ZOOM_CAMERA)
				Zoom -= yoffset / 5.0f;
			if (Zoom <= MIN_ZOOM_CAMERA)
				Zoom = MIN_ZOOM_CAMERA;
			if (Zoom >= MAX_ZOOM_CAMERA)
				Zoom = MAX_ZOOM_CAMERA;
		}
		
		//************************************************************************************************************************************************

		void Update(GLfloat deltaTime)
		{
			// The jump height decreases if not on the ground
			if (!OnGround) Jump -= 2.7 * deltaTime;
			OnGround = false;

			// Add an offset along the axis and check for a collision
			Position.y += Jump * deltaTime;
			Collision(0, Jump, 0, sizeCube);
			if (Jump < 0)
			{
				mciSendStringW(TEXT("stop sounds/walk.mp3"), NULL, 0, NULL);
				playingSound = false;
			}
				

			hungerTimer.nowTime = glfwGetTime();
			oxygenTimer.nowTime = glfwGetTime();
			heartTimer.nowTime = glfwGetTime();

			// Timers for reducing the life parameters
			if ((hungerTimer.nowTime - hungerTimer.lastTime) >= 80) {
				hungerTimer.lastTime = hungerTimer.nowTime;
				--hunger;
				if (hunger < 1) hunger = 0;
			}

			if ((oxygenTimer.nowTime - oxygenTimer.lastTime) >= 60) {
				oxygenTimer.lastTime = oxygenTimer.nowTime;
				--oxygen;
				if (oxygen < 1) oxygen = 0;
				if (hunger >= MAX_NUM_LIFE_INDEX / 2)
					++heart;
				if (heart > MAX_NUM_LIFE_INDEX)
					heart = MAX_NUM_LIFE_INDEX;
			}

			// In the greenhouse, the player's oxygen increases
			if ((Position.x >= GREENHOUSE_MIN_X && Position.x <= GREENHOUSE_MAX_X) &&
				(Position.y >= GREENHOUSE_MIN_Y && Position.y <= GREENHOUSE_MAX_Y) &&
				(Position.z >= GREENHOUSE_MIN_Z && Position.z <= GREENHOUSE_MAX_Z) &&
				oxygen < MAX_NUM_LIFE_INDEX && Fld::numberClearBattery > Fld::maxNumBat / 2)
				oxygen = MAX_NUM_LIFE_INDEX;

			if ((oxygen < 1 || hunger < 1) && ((heartTimer.nowTime - heartTimer.lastTime) >= 3))
			{
				heartTimer.lastTime = heartTimer.nowTime;
				--heart;
			}
		}

		//************************************************************************************************************************************************

		// Is there a block in these coordinates
		bool Check(int x, int y, int z)
		{
			if ((x < 0) || (x >= MAP_LENGTH_AND_WIDTH) ||
				(y < 0) || (y >= MAP_HEIGHT) ||
				(z < 0) || (z >= MAP_LENGTH_AND_WIDTH)) return false;

			return map[x][y][z].exist;
		}

		//************************************************************************************************************************************************
	};
	Player player(glm::vec3(400.0f, 14.0f, 400.0f));

	namespace Mn
		//Menu
	{
		// Parameters for the location of the menu item
		typedef struct
		{
			GLfloat sizeX;
			GLfloat sizeY;
			glm::vec2 position;
			GLuint texture;
		}elementMenu;

		elementMenu cursor = { SIZE_ELEM_MENU, SIZE_ELEM_MENU, glm::vec2(-0.05f, -0.05f), 0 };
		elementMenu heart = { SIZE_ELEM_MENU, SIZE_ELEM_MENU, glm::vec2(-1.0f, 0.9f), 0 };
		elementMenu oxygen = { SIZE_ELEM_MENU, SIZE_ELEM_MENU, glm::vec2(-1.0f, 0.8f), 0 };
		elementMenu inventory = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y,glm::vec2(-0.65f, -1.5f), 0 };
		elementMenu hunger = { SIZE_ELEM_MENU, SIZE_ELEM_MENU, glm::vec2(-1.0f, 0.7f), 0 };
		elementMenu elementBag = { SIZE_ELEM_MENU, SIZE_ITEM_Y - 0.02, glm::vec2(-0.592f, -0.8f), 0 };
		elementMenu frame = { SIZE_ITEM_X + 0.01, SIZE_ITEM_Y + 0.02, glm::vec2(-0.606,-0.823),0 };
		elementMenu bakeInterface = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y, glm::vec2(-0.65,-0.65),0 };
		elementMenu generatorInterface = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y, glm::vec2(-0.65,-0.65),0 };
		elementMenu craftInterface = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y, glm::vec2(-0.65,-0.65),0 };
		elementMenu boxInterface = { BIG_SIZE_ELEM_MENU_X - 0.1, BIG_SIZE_ELEM_MENU_Y - 0.1, glm::vec2(-0.6,-0.68),0 };
		elementMenu gameMenu = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y, glm::vec2(-0.65,-0.65),0 };
		elementMenu mainMenu = { BIG_SIZE_ELEM_MENU_X, BIG_SIZE_ELEM_MENU_Y, glm::vec2(-0.65,-0.65),0 };
		// ID for the loaded texture
		GLuint textureHeartNull;

		// Vertex buffer, an array of vertices and indexes of the square buffer
		GLuint VBOQuad, VAOQuad, shaderProgramQuad;
		// Vertices and texture coordinates of the square
		GLfloat verticesQuadPosTexCoord[] = {
			// Position		// Texture coordinates
			1.0f, 1.0f,		1.0f, 1.0f,
			0.0f, 0.0f,		0.0f, 0.0f,
			1.0f, 0.0f,		1.0f, 0.0f,
			1.0f, 1.0f,		1.0f, 1.0f,
			0.0f, 1.0f,		0.0f, 1.0f,
			0.0f, 0.0f,		0.0f, 0.0f,
		};

		namespace Txt
			//Text
		{
			// Structure for the text output symbol in the menu
			typedef struct Character {
				GLuint TextureID;		// Glyph texture ID
				glm::ivec2 Size;		// Glyph dimensions
				glm::ivec2 Bearing;		// Offset of the upper-left point of the glyph
				GLuint Advance;			// Horizontal offset to the beginning of the next glyph
			};

			// An array of characters and an element for displaying text in the menu
			Character Characters[NUM_CHAR_TEXT_RENDER];
			Character ch;
			// Object vertex buffer, an array of object vertices, and a shader program descriptor text
			GLuint VBOFont, VAOFont, shaderProgramFont;
			// Auxiliary variables for text output
			GLfloat xpos, ypos, w, h, vertices[6][4];
			// A string for displaying text on the screen
			std::string str;

			//************************************************************************************************************************************************

			void initFreeType(const GLchar* pathVert, const GLchar* pathFrag)
			{
				FT_Library ft;
				shaderBuildProgram(pathVert, pathFrag, &shaderProgramFont);

				if (FT_Init_FreeType(&ft))
					std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
				FT_Face face;
				if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
					std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

				FT_Set_Pixel_Sizes(face, 0, 48);

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disabling byte alignment constraint

				GLuint texture;
				for (GLubyte c = 0; c < NUM_CHAR_TEXT_RENDER; ++c)
				{
					// Loading a glyph of a character
					if (FT_Load_Char(face, c, FT_LOAD_RENDER))
					{
						std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
						continue;
					}
					// Creating a texture
					glGenTextures(1, &texture);
					glBindTexture(GL_TEXTURE_2D, texture);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
					// Setting the texture parameterb
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					// Saving a symbol for later use
					Character character = { texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),face->glyph->advance.x };
					// Inserting a character into an array
					Characters[c] = character;
				}

				glBindTexture(GL_TEXTURE_2D, 0);
				// After completion, we clear the memory
				FT_Done_Face(face);
				FT_Done_FreeType(ft);

				// Setting up VAO/VBO for texture squares
				glGenVertexArrays(1, &VAOFont);
				glGenBuffers(1, &VBOFont);
				glBindVertexArray(VAOFont);
				glBindBuffer(GL_ARRAY_BUFFER, VBOFont);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}

			//************************************************************************************************************************************************

			void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
			{
				// Activate the appropriate settings for displaying
				Use(shaderProgramFont);
				glUniformMatrix4fv(glGetUniformLocation(shaderProgramFont, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
				glUniform3f(glGetUniformLocation(shaderProgramFont, "textColor"), color.x, color.y, color.z);
				glActiveTexture(GL_TEXTURE0);
				glBindVertexArray(VAOFont);

				// Pass through all characters of the string
				for (int i = 0; i < text.size(); ++i)
				{
					ch = Characters[text[i]];

					xpos = x + ch.Bearing.x * scale;
					ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

					w = ch.Size.x * scale;
					h = ch.Size.y * scale;
					// Updating the VBA for each character
					vertices[0][0] = xpos;		vertices[0][1] = ypos + h;	vertices[0][2] = 0.0;	vertices[0][3] = 0.0;
					vertices[1][0] = xpos;		vertices[1][1] = ypos;		vertices[1][2] = 0.0;	vertices[1][3] = 1.0;
					vertices[2][0] = xpos + w;	vertices[2][1] = ypos;		vertices[2][2] = 1.0;	vertices[2][3] = 1.0;
					vertices[3][0] = xpos;		vertices[3][1] = ypos + h;	vertices[3][2] = 0.0;	vertices[3][3] = 0.0;
					vertices[4][0] = xpos + w;	vertices[4][1] = ypos;		vertices[4][2] = 1.0;	vertices[4][3] = 1.0;
					vertices[5][0] = xpos + w;	vertices[5][1] = ypos + h;	vertices[5][2] = 1.0;	vertices[5][3] = 0.0;

					// Displaying a glyph texture on top of a square
					glBindTexture(GL_TEXTURE_2D, ch.TextureID);
					// Updating the contents of the VBO memory
					glBindBuffer(GL_ARRAY_BUFFER, VBOFont);
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
					// Displaying a square
					glDrawArrays(GL_TRIANGLES, 0, 6);
					// Move the cursor for the next glyph
					x += (ch.Advance >> 6) * scale;
				}
				glBindVertexArray(0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			//************************************************************************************************************************************************
		}

		namespace ClM
			//ClickMenu
		{
			// Crafting recipe
			typedef struct
			{
				int inp[NUM_ITEM_CRAFT_INP][NUM_ITEM_CRAFT_INP];
				int out;
			}Trecipe;

			// Menu modes
			bool bake = false, generator = false, craft = false, box = false, flagGameMenu = false;

			// Input menu items for things
			item bakeInpBucket = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(-0.286,-0.105),-1,0 };
			item bakeInpItem = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(-0.29,0.165),-1,0 };
			item bakeOutItem =  { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(0.16,0.032),-1,0 };
			item generatorInpWater = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(-0.29,0.165),-1,0 };
			item generatorInpBall = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(-0.286,-0.105),-1,0 };
			item generatorOut = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(0.16,0.032),-1,0 };
			item craftMenuInp[NUM_ITEM_CRAFT_INP][NUM_ITEM_CRAFT_INP];
			item craftOut = { SIZE_ITEM_X, SIZE_ITEM_Y, glm::vec2(0.244,0.03),-1,0 };
			item boxMenu[BAG_SIZE - 1][BAG_SIZE - 1];
			// A temporary cell for exchanging things
			item tmp;

			// Array of crafting recipes
			Trecipe recipe[RECIPE_COUNT];
			double coordScreenX, coordScreenY, coordOpenGLX, coordOpenGLY;

			//************************************************************************************************************************************************

			void calcuateCoordOpenGL()
			{
				glfwGetCursorPos(window, &coordScreenX, &coordScreenY);
				coordOpenGLX = coordScreenX / WIDTH_SCREEN * ((WIDTH_SCREEN / HEIGHT_SCREEN) - (-WIDTH_SCREEN / HEIGHT_SCREEN)) + (-WIDTH_SCREEN / HEIGHT_SCREEN);
				coordOpenGLY = coordScreenY / HEIGHT_SCREEN * (-1.0f - 1.0f) + 1.0f;
			}

			//************************************************************************************************************************************************

			void addRecipe(int out, unsigned char count, const char* pathImg)
			{
				int cnt;
				GLFWimage img;
				// Loading the image
				img.pixels = stbi_load(pathImg, &img.width, &img.height, &cnt, 0);
				for (int x = 0; x < NUM_ITEM_CRAFT_INP; ++x)
					for (int z = 0; z < NUM_ITEM_CRAFT_INP; ++z)
					{
						// Take a pointer to the current pixel
						unsigned char* pixelOffset = img.pixels + (x + img.height * z) * cnt;
						// Take the component R of the pixel
						unsigned char r = pixelOffset[0];

						if (r < 30)
							recipe[count].inp[x][z] = r;
						else
							recipe[count].inp[x][z] = -1;
					}
				// The item must be crafted
				recipe[count].out = out;
				stbi_image_free(img.pixels);
			}

			//************************************************************************************************************************************************

			void initCraftMenu()
			{
				// Initialization of crafting menu input cells
				for (i = 0; i < NUM_ITEM_CRAFT_INP; ++i)
					for (j = 0; j < NUM_ITEM_CRAFT_INP; ++j)
					{
						craftMenuInp[i][j].count = 0;
						craftMenuInp[i][j].sizeX = SIZE_ITEM_X;
						craftMenuInp[i][j].sizeY = SIZE_ITEM_Y;
						craftMenuInp[i][j].type = -1;
						craftMenuInp[i][j].position = glm::vec2(-0.404f + i * 0.136f, -0.12f + j * 0.15f);
					}
			}

			//************************************************************************************************************************************************

			void initBoxMenu()
			{
				// Initializing the cells of the chest menu
				for (i = 0; i < BAG_SIZE - 1; ++i)
					for (j = 0; j < BAG_SIZE - 1; ++j)
					{
						boxMenu[i][j].count = 0;
						boxMenu[i][j].sizeX = SIZE_ITEM_X;
						boxMenu[i][j].sizeY = SIZE_ITEM_Y;
						boxMenu[i][j].type = -1;
						boxMenu[i][j].position = glm::vec2(-0.595f + i * 0.15f, -0.63f + j * 0.168f);
					}
			}

			//************************************************************************************************************************************************

			void takeElement(item& out)
			{
				// "In the hand" to take a thing
				player.handItem.type = out.type;
				player.handItem.count = out.count;
				// The output cell is reset to zero
				out.count = 0;
				out.type = -1;
			}

			//************************************************************************************************************************************************

			void decreaseCount(item& obj, unsigned char num)
			{
				obj.count -= num;
				if (obj.count < 1)
				{
					obj.type = -1;
					obj.count = 0;
				}
					
			}

			//************************************************************************************************************************************************

			void swapHandItem(item& obj)
			{
				// If the items are different or they are tools, swap the elements in the hand and cell
				if (player.handItem.type != obj.type || (player.handItem.type >= BRUSH && player.handItem.type <= SHOVEL) ||
					(obj.type >= BRUSH && obj.type <= SHOVEL))
				{
					tmp = player.handItem;
					player.handItem.count = obj.count;
					player.handItem.type = obj.type;
					obj.count = tmp.count;
					obj.type = tmp.type;
				}
				// If the items are the same, then checking the sum of their quantity
				else
				{
					if (obj.count + player.handItem.count < MAX_NUM_ELEM_ITEM)
					{
						obj.count += player.handItem.count;
						player.handItem.count = 0;
						player.handItem.type = -1;
					}
					else
					{
						player.handItem.count -= (MAX_NUM_ELEM_ITEM - 1) - obj.count;
						obj.count = (MAX_NUM_ELEM_ITEM - 1);
					}
				}
			}

			//************************************************************************************************************************************************

			bool hitItem(double mouseX, double mouseY, item obj)
			{
				return mouseY > obj.position.y && mouseY < obj.position.y + obj.sizeY && mouseX > obj.position.x && mouseX < obj.position.x + obj.sizeX;
			}

			//************************************************************************************************************************************************

			bool hit(double mouseX, double mouseY, double minCoordX, double minCoordY, double maxCoordX, double maxCoordY)
			{
				return mouseY > minCoordY && mouseY < maxCoordY&& mouseX > minCoordX && mouseX < maxCoordX;
			}

			//************************************************************************************************************************************************

			void mainMenuClick(double mouseX, double mouseY)
			{
				// If you click "Exit", then exit the cycle of the start menu and do not go into the game cycle
				if (hit(mouseX, mouseY, -0.554, -0.1725, 0.554, -0.035))
				{
					exitProgram = true;
					exitMainMenu = true;
				}
				// If you click "Continue playing"
				if (hit(mouseX, mouseY, -0.554, 0.03, 0.554, 0.1675))
				{
					flagNewGame = false;
					exitMainMenu = true;
				}
				// If you click "New Game"
				if (hit(mouseX, mouseY, -0.554, 0.2375, 0.554, 0.375))
				{
					flagNewGame = true;
					exitMainMenu = true;
				}
			}

			//************************************************************************************************************************************************

			void boxClick(double mouseX, double mouseY, int button)
			{
				// Go through the cells of the chest and if you hit the cell with the right button, 
				// then exchange the items in the chest and "in the hand"
				for (i = 0; i < BAG_SIZE - 1; ++i)
					for (j = 0; j < BAG_SIZE - 1; ++j)
						if (hitItem(mouseX, mouseY, boxMenu[i][j]) && button == GLFW_MOUSE_BUTTON_RIGHT)
						{
							swapHandItem(boxMenu[i][j]);
							i = j = BAG_SIZE;
						}
							
				// If you hit the cross button with the left button, then close the menu
				if (hit(mouseX, mouseY, 0.534, 0.62, 0.592, 0.6875) && button == GLFW_MOUSE_BUTTON_LEFT)
				{
					flagMouseBind = true;
					box = false;
				}
			}

			//************************************************************************************************************************************************

			void craftClick(double mouseX, double mouseY, int button)
			{
				// Go through the crafting input cells and check the items "in the hand" and the cell
				for (i = 0; i < NUM_ITEM_CRAFT_INP; ++i)
					for (j = 0; j < NUM_ITEM_CRAFT_INP; ++j)
					{
						if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, craftMenuInp[i][j]) &&
							(player.handItem.type == IRON_BULLION || player.handItem.type == GOLD_BULLION || player.handItem.type == -1))
						{

							if (player.handItem.type >= 0 && craftMenuInp[i][j].type == -1)
							{
								craftMenuInp[i][j].type = player.handItem.type;
								craftMenuInp[i][j].count = 1;
								decreaseCount(player.handItem, 1);
							}

							else if (craftMenuInp[i][j].type >= 0)
								if (craftMenuInp[i][j].type == player.handItem.type && player.handItem.count + 1 < MAX_NUM_ELEM_ITEM)
								{
									++player.handItem.count;
									decreaseCount(craftMenuInp[i][j], 1);
								}
								else
									swapHandItem(craftMenuInp[i][j]);
						}
					}
			
				// Checking whether the items in the cells correspond to some crafting recipe
				for (int k = 0; k < RECIPE_COUNT; ++k)
				{
					bool checkOk = true;
					for (i = 0; i < NUM_ITEM_CRAFT_INP; ++i)
						for (j = 0; j < NUM_ITEM_CRAFT_INP; ++j)
							if (craftMenuInp[i][j].type != recipe[k].inp[i][j])
							{
								checkOk = false;
								i = j = NUM_ITEM_CRAFT_INP;
							}

					if (checkOk)
					{
						craftOut.type = recipe[k].out;
						if (craftOut.type > BOX && craftOut.type < IRON_BULLION)
							craftOut.count = SOUNDNESS_TOOL;
						else if (craftOut.type == MONEY)
							craftOut.count = 9;
						else
							craftOut.count = 1;
						break;
					}
					else
						craftOut.type = -1;
				}
				
				// Take an item from the output cell
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, craftOut) && player.handItem.type == -1 &&
					craftOut.type >= 0)
				{
					// Take the thing "in hand" and reset the input cells
					takeElement(craftOut);
					for (i = 0; i < NUM_ITEM_CRAFT_INP; ++i)
						for (j = 0; j < NUM_ITEM_CRAFT_INP; ++j)
							decreaseCount(craftMenuInp[i][j], 1);
				}

				// Exit from crafting
				if (hit(mouseX, mouseY, 0.424, 0.2975, 0.475, 0.3575))
				{
					flagMouseBind = true;
					craft = false;
				}
			}

			//************************************************************************************************************************************************

			void bagClick(double mouseX, double mouseY, int button)
			{
				// Search for a cell to hit and swap things in it and "in hand"
				for (i = 0; i < BAG_SIZE; ++i)
				{
					if (hit(mouseX, mouseY, Mn::elementBag.position.x + i * OFFSET_ICON_BAG_RENDER, Mn::elementBag.position.y,
						Mn::elementBag.position.x + (i + 1) * OFFSET_ICON_BAG_RENDER, Mn::elementBag.position.y + Mn::elementBag.sizeY) && button == GLFW_MOUSE_BUTTON_RIGHT)
					{
						swapHandItem(player.bag[i]);
						i = BAG_SIZE;
					}
				}
			}

			//************************************************************************************************************************************************

			void generatorClick(double mouseX, double mouseY, int button)
			{
				// Checking the hit on the cell and exchanging things between the cell and the "hand"
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, generatorInpWater) && (player.handItem.type == BUCKET_WATER || player.handItem.type == -1))
					swapHandItem(generatorInpWater);
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, generatorInpBall) && (player.handItem.type == OXYGEN_EMPTY || player.handItem.type == -1))
					swapHandItem(generatorInpBall);
				// If there is a bucket of water and a balloon in the cells, then get a balloon with oxygen
				if (generatorOut.type < 0)
				{
					if (generatorInpWater.type == BUCKET_WATER && generatorInpBall.type == OXYGEN_EMPTY && Fld::numberClearBattery > Fld::maxNumBat / 2)
					{
						generatorOut.type = OXYGEN_FULL;
						generatorOut.count = generatorInpWater.count < generatorInpBall.count ? generatorInpWater.count : generatorInpBall.count;
					}
					else
					{
						generatorOut.type = -1;
						generatorOut.count = 0;
					}
				}
				
				// Take the oxygen cylinder and bucket from the output cell
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, generatorOut) && player.handItem.type == -1 && generatorOut.type>=0)
				{
					if (generatorOut.type == OXYGEN_FULL)
					{
						decreaseCount(generatorInpBall, generatorOut.count);
						decreaseCount(generatorInpWater, generatorOut.count);

						// From the output cell, take the thing "in hand"
						takeElement(generatorOut);
						generatorOut.type = BUCKET;
						generatorOut.count = player.handItem.count;
					}
					else
						takeElement(generatorOut);
				}

				// Exit from the generator
				if (hit(mouseX, mouseY, 0.424, 0.3, 0.474, 0.3575))
				{
					flagMouseBind = true;
					generator = false;
				}
			}

			//************************************************************************************************************************************************

			void bakeClick(double mouseX, double mouseY, int button)
			{
				// Checking the hit on the cell and exchanging things between the cell and the "hand"
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, bakeInpItem) && (player.handItem.type == POTATO ||
					player.handItem.type == IRON_ORE || player.handItem.type == GOLD_ORE || player.handItem.type == ICE || player.handItem.type == -1))
					swapHandItem(bakeInpItem);

				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, bakeInpBucket) && 
					(player.handItem.type == BUCKET || player.handItem.type == -1))
					swapHandItem(bakeInpBucket);

					// Checking the input cells and displaying the necessary thing
					if (Fld::numberClearBattery > Fld::maxNumBat / 2)

						switch (bakeInpItem.type)
						{
						case POTATO:
							bakeOutItem.type = POTATO_BAKED;
							bakeOutItem.count = bakeInpItem.count;
							break;
						case IRON_ORE:
							bakeOutItem.type = IRON_BULLION;
							bakeOutItem.count = bakeInpItem.count;
							break;
						case GOLD_ORE:
							bakeOutItem.type = GOLD_BULLION;
							bakeOutItem.count = bakeInpItem.count;
							break; 
						default:
							bakeOutItem.type = -1;
							bakeOutItem.count = 0;

							if (bakeInpItem.type == ICE && bakeInpBucket.type == BUCKET)
							{
								bakeOutItem.type = BUCKET_WATER;
								bakeOutItem.count = bakeInpItem.count < bakeInpBucket.count ? bakeInpItem.count : bakeInpBucket.count;
							}
							break;
						}
					else
					{
						bakeOutItem.type = -1;
						bakeOutItem.count = 0;
					}

				// Take the received item, if there is one
				if (button == GLFW_MOUSE_BUTTON_RIGHT && hitItem(mouseX, mouseY, bakeOutItem) && player.handItem.type == -1)
				{
					if (bakeInpItem.type != ICE && bakeInpItem.type >= 0)
					{
						bakeInpItem.count = 0;
						bakeInpItem.type = -1;
					}
					else
					{
						decreaseCount(bakeInpItem, bakeOutItem.count);
						decreaseCount(bakeInpBucket, bakeOutItem.count);
					}

					takeElement(bakeOutItem);
				}

				if (hit(mouseX, mouseY, 0.424, 0.3, 0.474, 0.3575))
				{
					flagMouseBind = true;
					bake = false;
				}
			}
		}

		void initMenu(const GLchar* pathVert, const GLchar* pathFrag)
		{
			// Creating a shader program
			shaderBuildProgram(pathVert, pathFrag, &shaderProgramQuad);
			// Allocate memory on the video card
			glGenVertexArrays(1, &VAOQuad);
			glGenBuffers(1, &VBOQuad);
			// Linking the VAO
			glBindVertexArray(VAOQuad);
			// Bind a VBO and write to it from an array of vertices
			glBindBuffer(GL_ARRAY_BUFFER, VBOQuad);
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesQuadPosTexCoord), verticesQuadPosTexCoord, GL_STATIC_DRAW);
			// Position Attribute
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			// Untie the VAO
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************

		void showElementMenu(glm::vec2 position, GLfloat sizeX, GLfloat sizeY, int texture)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(position, 0.0));
			model = glm::scale(model, glm::vec3(sizeX, sizeY, 1.0f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgramQuad, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindTexture(GL_TEXTURE_2D, texture);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//************************************************************************************************************************************************

		void showItemInterface(float positionX, float positionY, float sizeX, float sizeY, GLuint icon, bool renderText, bool text, int count)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(positionX, positionY, 0.0));
			model = glm::scale(model, glm::vec3(sizeX, sizeY, 1.0f));

			glUniformMatrix4fv(glGetUniformLocation(shaderProgramQuad, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glBindTexture(GL_TEXTURE_2D, icon);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			if (renderText)
			{
				// For a thing, output the quantity
				if (text)
				{
					Txt::str = std::to_string(count);
					Txt::RenderText(Txt::str, positionX + sizeX * 0.8, positionY, 0.0007, glm::vec3(1.0f, 1.0f, 1.0f));
				}
				// For the gun, the amount of strength / the maximum amount of strength
				else
				{
					Txt::str = std::to_string(count);
					Txt::str += "/";
					Txt::str += std::to_string(SOUNDNESS_TOOL);
					Txt::RenderText(Txt::str, positionX, positionY, 0.00065, glm::vec3(1.0f, 1.0f, 1.0f));
				}

				// Returning the original parameters
				Use(shaderProgramQuad);
				glBindVertexArray(VAOQuad);
			}
		}

		//************************************************************************************************************************************************

		void showMenu()
		{
			glUniform1i(glGetUniformLocation(shaderProgramQuad, "TexCoord"), 0);
			// Use a shader program
			Use(shaderProgramQuad);
			// Linking the VAO
			glBindVertexArray(VAOQuad);
			// Get the location of uniform matrices and set the matrices
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramQuad, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			// Checking which menu to display
			if (ClM::flagGameMenu)
				// Menu for saving and exiting the game
				showElementMenu(gameMenu.position, gameMenu.sizeX, gameMenu.sizeY, gameMenu.texture);
			else
			{
				showElementMenu(cursor.position, cursor.sizeX, cursor.sizeY, cursor.texture);
				// Displaying the bag cells
				showElementMenu(inventory.position, inventory.sizeX, inventory.sizeY, inventory.texture);
				// Displays the object that is currently selected
				showElementMenu(glm::vec2(frame.position.x + player.selectElement * OFFSET_ICON_BAG_RENDER, frame.position.y), frame.sizeX, frame.sizeY, frame.texture);

				// Отображение элементов в сумке
				for (i = 0; i < BAG_SIZE; ++i)
					if (player.bag[i].type >= 0)
						showItemInterface(elementBag.position.x + OFFSET_ICON_BAG_RENDER * i, elementBag.position.y, elementBag.sizeX, elementBag.sizeY,
							arrayBlocks[player.bag[i].type].icon, true, player.bag[i].type < BRUSH || player.bag[i].type > SHOVEL, player.bag[i].count);
				
				// Output of the name of the item that is selected
				if (player.bag[player.selectElement].type >= 0)
				{
					Txt::str = arrayBlocks[player.bag[player.selectElement].type].name;
					Txt::RenderText(Txt::str, -0.165f, -0.6f, 0.001, glm::vec3(1.0f, 1.0f, 1.0f));
					Use(shaderProgramQuad);
					glBindVertexArray(VAOQuad);
				}

				// Display of the menu and input and output cells of the stove elements
				if (ClM::bake && !ClM::generator && !ClM::craft && !ClM::box)
				{
					showElementMenu(bakeInterface.position, bakeInterface.sizeX, bakeInterface.sizeY, bakeInterface.texture);

					if (ClM::bakeInpItem.type >= 0)
						showItemInterface(ClM::bakeInpItem.position.x, ClM::bakeInpItem.position.y, ClM::bakeInpItem.sizeX, ClM::bakeInpItem.sizeY,
							arrayBlocks[ClM::bakeInpItem.type].icon, true, true, ClM::bakeInpItem.count);
					if (ClM::bakeInpBucket.type >= 0)
						showItemInterface(ClM::bakeInpBucket.position.x, ClM::bakeInpBucket.position.y, ClM::bakeInpBucket.sizeX, ClM::bakeInpItem.sizeY,
							arrayBlocks[ClM::bakeInpBucket.type].icon, true, true, ClM::bakeInpBucket.count);
					if (ClM::bakeOutItem.type >= 0)
						showItemInterface(ClM::bakeOutItem.position.x, ClM::bakeOutItem.position.y, ClM::bakeOutItem.sizeX, ClM::bakeOutItem.sizeY,
							arrayBlocks[ClM::bakeOutItem.type].icon, true, true, ClM::bakeOutItem.count);
				}

				// Displaying the menu and input and output cells of the generator elements
				if (ClM::generator && !ClM::bake && !ClM::craft && !ClM::box)
				{
					showElementMenu(generatorInterface.position, generatorInterface.sizeX, generatorInterface.sizeY, generatorInterface.texture);

					if (ClM::generatorInpWater.type >= 0)
						showItemInterface(ClM::generatorInpWater.position.x, ClM::generatorInpWater.position.y, ClM::generatorInpWater.sizeX, ClM::generatorInpWater.sizeY,
							arrayBlocks[ClM::generatorInpWater.type].icon, true, true, ClM::generatorInpWater.count);
					if (ClM::generatorInpBall.type >= 0)
						showItemInterface(ClM::generatorInpBall.position.x, ClM::generatorInpBall.position.y, ClM::generatorInpBall.sizeX, ClM::generatorInpBall.sizeY,
							arrayBlocks[ClM::generatorInpBall.type].icon, true, true, ClM::generatorInpBall.count);
					if (ClM::generatorOut.type >= 0)
						showItemInterface(ClM::generatorOut.position.x, ClM::generatorOut.position.y, ClM::generatorOut.sizeX, ClM::generatorOut.sizeY,
							arrayBlocks[ClM::generatorOut.type].icon, true, true, ClM::generatorOut.count);
				}

				// Displaying the menu and input and output cells of crafting elements
				if (ClM::craft && !ClM::bake && !ClM::generator && !ClM::box)
				{
					showElementMenu(craftInterface.position, craftInterface.sizeX, craftInterface.sizeY, craftInterface.texture);

					for (i = 0; i < NUM_ITEM_CRAFT_INP; ++i)
						for (j = 0; j < NUM_ITEM_CRAFT_INP; ++j)
							if (ClM::craftMenuInp[i][j].type >= 0)
								showItemInterface(ClM::craftMenuInp[i][j].position.x, ClM::craftMenuInp[i][j].position.y, ClM::craftMenuInp[i][j].sizeX,
									ClM::craftMenuInp[i][j].sizeY, arrayBlocks[ClM::craftMenuInp[i][j].type].icon, true, true, ClM::craftMenuInp[i][j].count);

					if (ClM::craftOut.type >= 0)
						showItemInterface(ClM::craftOut.position.x, ClM::craftOut.position.y, ClM::craftOut.sizeX, ClM::craftOut.sizeY,
							arrayBlocks[ClM::craftOut.type].icon, ClM::craftOut.type < BRUSH || ClM::craftOut.type > SHOVEL, ClM::craftOut.type < BRUSH || ClM::craftOut.type > SHOVEL, ClM::craftOut.count);
				}

				// Displaying the menu and chest cells
				if (ClM::box && !ClM::bake && !ClM::generator && !ClM::craft)
				{
					showElementMenu(boxInterface.position, boxInterface.sizeX, boxInterface.sizeY, boxInterface.texture);

					for (i = 0; i < BAG_SIZE - 1; ++i)
						for (j = 0; j < BAG_SIZE - 1; ++j)
							if (ClM::boxMenu[i][j].type >= 0)
								showItemInterface(ClM::boxMenu[i][j].position.x, ClM::boxMenu[i][j].position.y, ClM::boxMenu[i][j].sizeX,
									ClM::boxMenu[i][j].sizeY, arrayBlocks[ClM::boxMenu[i][j].type].icon, true, ClM::boxMenu[i][j].type < BRUSH || ClM::boxMenu[i][j].type > SHOVEL, ClM::boxMenu[i][j].count);
				}

				// Displaying the item that is currently in your hand, if the mouse is visible
				if (player.handItem.type >= 0 && !flagMouseBind)
				{
					ClM::calcuateCoordOpenGL();
					showItemInterface(ClM::coordOpenGLX - elementBag.sizeX / 2, ClM::coordOpenGLY - elementBag.sizeY / 2, elementBag.sizeX, elementBag.sizeY,
						arrayBlocks[player.handItem.type].icon, false, false, player.handItem.count);
				}

				// Displaying player parameters
				for (i = 0; i < MAX_NUM_LIFE_INDEX; ++i)
				{
					if (i < player.heart)
						showItemInterface(heart.position.x + heart.sizeX * i, heart.position.y, heart.sizeX, heart.sizeY,
							heart.texture, false, false, 0);
					else
						showItemInterface(heart.position.x + heart.sizeX * i, heart.position.y, heart.sizeX, heart.sizeY,
							textureHeartNull, false, false, 0);

					if (i < player.oxygen)
						showItemInterface(oxygen.position.x + oxygen.sizeX * i, oxygen.position.y, oxygen.sizeX, oxygen.sizeY,
							oxygen.texture, false, false, 0);

					if (i < player.hunger)
						showItemInterface(hunger.position.x + hunger.sizeX * i, hunger.position.y, hunger.sizeX, hunger.sizeY,
							hunger.texture, false, false, 0);
				}
			}
			// Untie the VAO
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************
	}

	namespace Mp
	{
		// An array of vertices and a handle of the sun shader program
		GLuint VAOLight, lightingShader;
		// Object vertex buffer, an array of object vertices and a shader program descriptor
		GLuint VBOCube, VAOCube, shaderProgramCube;
		// Cube vertices and texture coordinates
		GLfloat verticesCubePosTexCoord[] = {
			// Position			  // Texture coordinates
			// Back face
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom left
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top right
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom right
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top right
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top left

			// Front face
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom left
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom right
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top right
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top right
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top left
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom left

			// Left side
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top right
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top left
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom left
			-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom left
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom right
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top right

			// Right side
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top left
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom right
			 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top right     
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom right
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top left
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom left   

			// Bottom edge
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top right
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top left
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom left   
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom left
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom right
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top right

			// Top face
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top left
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom right
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top right
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom right
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top left
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom left
		};
		// Sun Parameters
		glm::vec3 lightPos(MAP_LENGTH_AND_WIDTH / 2, 500.0f, 0); GLfloat sizeLight = 6.5f, alfa = 0;
		// Variables for calculating the lighting of blocks
		float light = 0.0, lightBlue = 0.0, electricity = 1.0f, naturalLight = 0.0f, naturalLightBlue = 0.0f;
		float coefSunset = 0.0f, coefIllum = 1.0f, coefBlue = 0.0f;

		//************************************************************************************************************************************************

		void initLight(const GLchar* pathVert, const GLchar* pathFrag)
		{
			// Creating a shader program
			shaderBuildProgram(pathVert, pathFrag, &lightingShader);
			// Allocate memory on the video card
			glGenVertexArrays(1, &VAOLight);
			// Linking the VAO
			glBindVertexArray(VAOLight);
			// Linking the VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
			// Position Attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			//Untie the VAO
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************

		void initCubeVAOAndVBO(const GLchar* pathVert, const GLchar* pathFrag)
		{
			// Creating a shader program
			shaderBuildProgram(pathVert, pathFrag, &shaderProgramCube);
			// Allocate memory on the video card for VBO и VAO
			glGenBuffers(1, &VBOCube);
			glGenVertexArrays(1, &VAOCube);
			//  Linking the VAO
			glBindVertexArray(VAOCube);
			// Linking the VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
			// Copying an array with vertices to the OpenGL buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubePosTexCoord), verticesCubePosTexCoord, GL_STATIC_DRAW);
			// Setting pointers to vertex attributes with coordinates
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			// Setting the pointer to the coordinates of the textures
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
			// Untie the VAO
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************

		void showLight()
		{
			// Use a shader program
			Use(lightingShader);
			// Find the identifier of the uniform view and projection and pass the matrices to shader programs
			glUniformMatrix4fv(glGetUniformLocation(lightingShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightingShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			// Changing the model matrix
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(alfa), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(lightPos.x, lightPos.y + ((alfa < 0 ? 0 : alfa) / 90) * MAP_LENGTH_AND_WIDTH, lightPos.z));
			model = glm::scale(model, glm::vec3(sizeLight));
			// Find the uniform model identifier and pass the matrix to shader programs
			glUniformMatrix4fv(glGetUniformLocation(lightingShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
			// Bind VAO draw a cube and untie VAO
			glBindVertexArray(VAOLight);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************
		
		void calculateAngleSun()
		{
			// Changing the angle of rotation of the sun, lighting, sky color
			alfa = 360 * (secondDay) / MAX_SECONDS_IN_DAY;
			secondDay += deltaTime;
			if (secondDay > MAX_SECONDS_IN_DAY)
				secondDay -= MAX_SECONDS_IN_DAY;

			if (alfa >= 180)
				alfa -= 360;

			coefIllum = (1 - (abs(alfa) / 180));
			coefIllum = coefIllum > 0.1 ? coefIllum : 0.1;

			coefSunset = 90 - abs(alfa);
			coefSunset = (ANGLE_SUNSET - abs(coefSunset));
			coefSunset = coefSunset < 0 ? 0 : coefSunset / ANGLE_SUNSET;
			coefBlue = coefSunset > 0 ? 1.01 * (1 + coefSunset) : 1;
		}

		//************************************************************************************************************************************************
		void showMap()
		{
			// Calculate the light from electricity and from the sun
			electricity = Fld::numberClearBattery / (float)Fld::maxNumBat;
			naturalLight = 1.0f * coefIllum;
			naturalLightBlue = naturalLight * coefBlue;

			glUniform1i(glGetUniformLocation(shaderProgramCube, "TexCoord"), 0);
			// Use a shader program
			Use(shaderProgramCube);
			// Getting the ID of the variables in the shader program
			objectColorLoc = glGetUniformLocation(shaderProgramCube, "objectColor");
			lightColorLoc = glGetUniformLocation(shaderProgramCube, "lightColor");

			// Get the location of uniform matrices and set the matrices
			modelLoc = glGetUniformLocation(shaderProgramCube, "model");
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramCube, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(shaderProgramCube, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
			// Linking the VAO
			glBindVertexArray(VAOCube);

			// Get the player's position in integers
			X = player.Position.x / sizeCube;
			Z = player.Position.z / sizeCube;
			// Depending on where the character is looking, we draw the map
			// If the rotation angle is closer to the axis Ox
			if (abs(player.Front.x) >= abs(player.Front.z))
			{
				// Blocks on the x-axis on the previous 2 and up to the player's position + visibility radius
				startX = player.Front.x > 0 ? (X - 2 > 0 ? X - 2 : 0) : (X + 2 < MAP_LENGTH_AND_WIDTH ? X + 2 : MAP_LENGTH_AND_WIDTH - 1);
				// Blocks on the Ox axis from the player's position - the radius of visibility to the player's position + the radius of visibility
				startZ = Z - player.radiusHoris > 0 ? Z - player.radiusHoris : 0;
				borderX = player.Front.x > 0 ? (X + player.radiusFront < MAP_LENGTH_AND_WIDTH ? X + player.radiusFront : MAP_LENGTH_AND_WIDTH) : (X - player.radiusFront > 0 ? X - player.radiusFront : -1);
				borderZ = Z + player.radiusHoris < MAP_LENGTH_AND_WIDTH ? Z + player.radiusHoris : MAP_LENGTH_AND_WIDTH;
			}

			// If the rotation angle is closer to the axis Oz
			else
			{
				// Blocks on the z-axis on the previous 2 and up to the player's position + visibility radius
				startZ = player.Front.z > 0 ? (Z - 2 > 0 ? Z - 2 : 0) : (Z + 2 < MAP_LENGTH_AND_WIDTH ? Z + 2 : MAP_LENGTH_AND_WIDTH - 1);
				//  Blocks on the Ox axis from the player's position - the radius of visibility to the player's position + the radius of visibility
				startX = X - player.radiusHoris > 0 ? X - player.radiusHoris : 0;
				borderZ = player.Front.z > 0 ? (Z + player.radiusHoris < MAP_LENGTH_AND_WIDTH ? Z + player.radiusHoris : MAP_LENGTH_AND_WIDTH) : (Z - player.radiusHoris > 0 ? Z - player.radiusHoris : -1);
				borderX = X + player.radiusHoris < MAP_LENGTH_AND_WIDTH ? X + player.radiusHoris : MAP_LENGTH_AND_WIDTH;
			}

			// Depending on which axis is closer to the rotation angle is greater or less than x and z to the borders and increase or decrease them
			for (x = startX; (abs(player.Front.x) >= abs(player.Front.z)) ? (player.Front.x > 0 ? x < borderX : x > borderX) : (x < borderX); (abs(player.Front.x) >= abs(player.Front.z)) ? (player.Front.x > 0 ? ++x : --x) : ++x)
				for (y = 0; y < MAP_HEIGHT; ++y)
					for (z = startZ; (abs(player.Front.x) >= abs(player.Front.z)) ? (z < borderZ) : (player.Front.z > 0 ? z < borderZ : z > borderZ); (abs(player.Front.x) >= abs(player.Front.z)) ? ++z : (player.Front.z > 0 ? ++z : --z))
					{
						// If there is no block on the map, then skip it
						if (!map[x][y][z].exist)
							continue;

						// If the map block is closed by blocks on the OXZ plane and if the player is above the cube and it is closed from above or the player is below the cube and the cube is closed from below, then skip
						if (((y + 1 < MAP_HEIGHT && (int)player.Position.y > y && map[x][y + 1][z].exist && map[x][y + 1][z].cube != GLASS) !=
							(y - 1 >= 0 && player.Position.y < y && map[x][y - 1][z].exist && map[x][y - 1][z].cube != GLASS)) &&
							(map[x + 1][y][z].exist && map[x + 1][y][z].cube != GLASS) && (map[x - 1][y][z].exist && map[x - 1][y][z].cube != GLASS) &&
							(map[x][y][z + 1].exist && map[x][y][z + 1].cube != GLASS) && (map[x][y][z - 1].exist && map[x][y][z - 1].cube != GLASS))
							continue;

						// Setting the color of the object and its lighting
						glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
						light = (electricity * map[x][y][z].light > naturalLight) ? (electricity * map[x][y][z].light > 1.0f ? 1.0f : electricity * map[x][y][z].light) : naturalLight;
						lightBlue = (electricity * map[x][y][z].light > naturalLightBlue) ? (electricity * map[x][y][z].light > 1.0f ? 1.0f : electricity * map[x][y][z].light) : naturalLightBlue;
						glUniform3f(lightColorLoc, light, light, lightBlue); // зададим цвет источника света
						
						// Calculating the model matrix for an object and passing it to the shader before drawing
						model = glm::mat4(1.0f);
						model = glm::scale(model, glm::vec3(sizeCube, sizeCube, sizeCube));
						model = glm::translate(model, glm::vec3(x + sizeCube / 2, y + sizeCube / 2, z + sizeCube / 2));
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

						if (map[x][y][z].cube == POTATO)
						{
							// Displaying all the faces
							glDisable(GL_CULL_FACE);
							// If the potatoes have not grown, then depending on how much time has passed from sowing, choose the texture
							if (!Fld::arrayPotato[IND_FIELD_FUNC_X(x, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(z, FIELD_POTATO_LEN)].grown)
							{
								Fld::timeGrowPotato = Fld::arrayPotato[IND_FIELD_FUNC_X(x, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(z, FIELD_POTATO_LEN)].timer;
								if (Fld::timeGrowPotato >= HALF_TIME_GROWN_POTATO && Fld::timeGrowPotato < TIME_GROWN_POTATO)
									glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[1]);
								else if (Fld::timeGrowPotato >= TIME_GROWN_POTATO)
								{
									Fld::arrayPotato[IND_FIELD_FUNC_X(x, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(z, FIELD_POTATO_LEN)].grown = true;
									glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[2]);
								}
								else
									glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[0]);
							}
							else
								// If the potatoes have grown, then the last texture
								glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[2]);

							// Render the side faces and empty upper and lower faces of the potato cube
							glDrawArrays(GL_TRIANGLES, 0, FACES_CUBE * 4);

							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[3]);
							glDrawArrays(GL_TRIANGLES, FACES_CUBE * 4, FACES_CUBE);

							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[4]);
							glDrawArrays(GL_TRIANGLES, FACES_CUBE * 5, FACES_CUBE);
							glEnable(GL_CULL_FACE);
						}

						else if (map[x][y][z].cube == GROUND || map[x][y][z].cube == BATTERY)
						{
							// Выводим боковые и нижнюю грани
							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[0]);
							glDrawArrays(GL_TRIANGLES, 0, FACES_CUBE * 4);
							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[1]);
							glDrawArrays(GL_TRIANGLES, FACES_CUBE * 4, FACES_CUBE);

							// Render the upper face, if the stone ground, then just the upper texture
							if (map[x][y][z].cube == GROUND)
								glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[2]);
							// If there is a solar battery, then render the upper texture depending on whether the block is dirty or not
							else if (map[x][y][z].cube == BATTERY && Fld::arrayBattery[IND_FIELD_FUNC_X(x, FIELD_BATTERY_WIDTH)][IND_FIELD_FUNC_Z(z, FIELD_BATTERY_LEN)].dust)
								glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[3]);
							else
								glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[2]);
							glDrawArrays(GL_TRIANGLES, FACES_CUBE * 5, FACES_CUBE);
						}

						else if (map[x][y][z].cube == BOX)
						{
							// Render the side faces, then the bottom and top
							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[0]);
							glDrawArrays(GL_TRIANGLES, 0, FACES_CUBE * 4);
							glBindTexture(GL_TEXTURE_2D, arrayBlocks[map[x][y][z].cube].type[1]);
							glDrawArrays(GL_TRIANGLES, FACES_CUBE * 4, FACES_CUBE * 2);
						}

						else
						{
							glBindTexture(GL_TEXTURE_2D, *(arrayBlocks[map[x][y][z].cube].type));
							glDrawArrays(GL_TRIANGLES, 0, FACES_CUBE * 6);
						}
					}

			// If the potato block grows, then increase the growth time
			for (i = 0; i < FIELD_POTATO_WIDTH; ++i)
				for (j = 0; j < FIELD_POTATO_LEN; ++j)
					if (!Fld::arrayPotato[i][j].grown)
						Fld::arrayPotato[i][j].timer += deltaTime;
			glBindVertexArray(0);
		}

		//************************************************************************************************************************************************
	}

	namespace SLP
		
		//Save, generate and load parametrs
	{
		int cnt;
		GLFWimage img;

		//************************************************************************************************************************************************

		void deleteWriteSaveFile(bool& newGame)
		{
			if (newGame)
			{
				std::remove("save/bag.bin");
				std::remove("save/param.bin");
				std::remove("save/field.bin");
				std::remove("save/saveMap.bin");
			}
			else
			{
				std::ifstream fileBag("save/bag.bin", std::ios::binary), 
					fileField("save/field.bin", std::ios::binary),
					fileParam("save/field.bin", std::ios::binary), 
					fileMap("save/saveMap.bin", std::ios::binary);

				// Checking whether all files can be opened
				if (fileBag.is_open() && fileField.is_open() && fileParam.is_open() && fileMap.is_open())
					newGame = false;
				else
					newGame = true;
				if (fileBag.is_open()) fileBag.close();
				if (fileField.is_open()) fileField.close();
				if (fileParam.is_open()) fileParam.close();
				if (fileMap.is_open()) fileMap.close();
			}
		}

		//************************************************************************************************************************************************

		void loadField(const char* path, bool newGame)
		{
			// Download information from a file or set default values
			if (!newGame)
			{
				std::ifstream file(path, std::ios::binary);
				if (file.is_open())
				{
					file.read((char*)Fld::arrayPotato, sizeof(Fld::parametersPotato)* FIELD_POTATO_WIDTH * FIELD_POTATO_LEN);
					file.read((char*)Fld::arrayBattery, sizeof(Fld::parametersBattery) * FIELD_BATTERY_WIDTH * FIELD_BATTERY_LEN);
					file.read((char*)&Fld::numberClearBattery, sizeof(int));
					file.close();
				}
				else
					std::cout << "File: " << path << " not found!" << std::endl;
			}
			else
			{
				for (int x = 0; x < FIELD_POTATO_WIDTH; ++x)
					for (int z = 0; z < FIELD_POTATO_LEN; ++z)
					{
						Fld::arrayPotato[x][z].timer = 0.0f;
						Fld::arrayPotato[x][z].grown = true;
					}

				for (int x = 0; x < FIELD_BATTERY_WIDTH; ++x)
					for (int z = 0; z < FIELD_BATTERY_LEN; ++z)
						Fld::arrayBattery[x][z].dust = false;

				// Plant two blocks of potatoes
				Fld::arrayPotato[IND_FIELD_FUNC_X(340, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(332, FIELD_POTATO_LEN)].grown = false;
				Fld::arrayPotato[IND_FIELD_FUNC_X(340, FIELD_POTATO_WIDTH)][IND_FIELD_FUNC_Z(333, FIELD_POTATO_LEN)].grown = false;
			}
		}

		//************************************************************************************************************************************************

		void loadBag(const char* path, bool newGame)
		{
			// Download information from a file or set default values
			if (!newGame)
			{
				std::ifstream file(path, std::ios::binary);
				if (file.is_open())
				{
					file.read((char*)player.bag, sizeof(item) * BAG_SIZE);
					file.read((char*)&player.handItem, sizeof(item));
					file.read((char*)Wrl::Mn::ClM::boxMenu, sizeof(item) * (BAG_SIZE - 1) * (BAG_SIZE - 1)); 
					file.read((char*)&Wrl::Mn::ClM::bakeInpItem, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::bakeInpBucket, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::bakeOutItem, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::generatorInpWater, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::generatorInpBall, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::generatorOut, sizeof(item));
					file.read((char*)&Wrl::Mn::ClM::craftOut, sizeof(item));
					file.read((char*)Wrl::Mn::ClM::craftMenuInp, sizeof(item) * NUM_ITEM_CRAFT_INP * NUM_ITEM_CRAFT_INP);
					file.close();
				}
				else
					std::cout << "File: " << path << " not found!" << std::endl;
			}
			// Reset the cells of the bag
			else
			{
				for (i = 0; i < BAG_SIZE; ++i)
				{
					player.bag[i].type = -1;
					player.bag[i].count = 0;
				}

				player.bag[0].type = POTATO;
				player.bag[0].count = 2;
				player.bag[1].type = PICKAXE;
				player.bag[1].count = SOUNDNESS_TOOL;

				// Initialize the cells for entering into the crafting menu and the chest
				Wrl::Mn::ClM::initCraftMenu();
				Wrl::Mn::ClM::initBoxMenu();
			}
		}

		//************************************************************************************************************************************************

		void loadParametrs(const char* path, bool newGame)
		{
			// Download information from a file or set default values
			if (newGame)
			{
				player.heart = player.oxygen = player.hunger = MAX_NUM_LIFE_INDEX;
			}
			else
			{
				std::ifstream file(path, std::ios::binary);
				if (file.is_open())
				{
					file.read((char*)&player.heart, sizeof(int));
					file.read((char*)&player.hunger, sizeof(int));
					file.read((char*)&player.oxygen, sizeof(int));
					file.read((char*)&player.Position, sizeof(glm::vec3));
					file.read((char*)&secondDay, sizeof(double));
					file.read((char*)&player.oldY, sizeof(int));
					file.close();
				}
				else
					std::cout << "File: " << path << " not found!" << std::endl;
			}
		}

		//************************************************************************************************************************************************

		void loadAllMap(const char* path)
		{
			std::ifstream file(path, std::ios::binary);
			if (file.is_open())
			{
				file.read((char*)map, sizeof(blockMap) * MAP_LENGTH_AND_WIDTH * MAP_HEIGHT * MAP_LENGTH_AND_WIDTH);
				file.close();
			}
			else
				std::cout << "File: " << path << " not found!" << std::endl;
		}

		//************************************************************************************************************************************************

		void loadBase(const char* pathHome)
		{
			std::ifstream file(pathHome, std::ios::binary);
			if (file.is_open())
			{
				blockMap tmp;
				for (x = MIN_COORD_X_BASE; x < MAX_COORD_X_BASE; ++x)
					for (y = MIN_COORD_Y_BASE; y < 19; ++y)
						for (z = MIN_COORD_Z_BASE; z < MAX_COORD_Z_BASE; ++z)
						{
							file.read((char*)&tmp, sizeof(blockMap));
							if (tmp.exist)
							{
								map[x][y][z].cube = tmp.cube;
								map[x][y][z].exist = true;
							}
						}
				file.close();
			}
			else
				std::cout << "File: " << pathHome << " not found!" << std::endl;
		}

		//************************************************************************************************************************************************

		void generateIce(GLFWimage img, int cnt)
		{
			// Go all over the map and fill it with ice
			for (int x = 0; x < MAP_LENGTH_AND_WIDTH; ++x)
				for (int z = 0; z < MAP_LENGTH_AND_WIDTH; ++z)
				{
					unsigned char* pixelOffset = img.pixels + (x + img.height * z) * cnt;
					unsigned char r = pixelOffset[0] / DIV_R_IN_RGB;

					if (!map[x][r][z].exist &&
						(map[x + 1][r][z].exist || map[x + 2][r][z].exist || map[x + 3][r][z].exist || map[x + 4][r][z].exist || map[x + 5][r][z].exist) &&
						(map[x - 1][r][z].exist || map[x - 2][r][z].exist || map[x - 3][r][z].exist || map[x + 4][r][z].exist || map[x + 5][r][z].exist) &&
						(map[x][r][z + 1].exist || map[x][r][z + 2].exist || map[x][r][z + 3].exist || map[x][r][z + 4].exist || map[x][r][z + 5].exist) &&
						(map[x][r][z - 1].exist || map[x][r][z - 2].exist || map[x][r][z - 3].exist || map[x][r][z - 4].exist || map[x][r][z - 5].exist))
					{
						SET_BLOCK_PROPERTIES(x, r, z, true, ICE, 0.0f);
					}
				}
		}

		//************************************************************************************************************************************************

		void calculateAllLamp()
		{
			// Go through each block of the map
			for (int x = 0; x < MAP_LENGTH_AND_WIDTH; ++x)
				for (int y = 0; y < MAP_HEIGHT; ++y)
					for (int z = 0; z < MAP_LENGTH_AND_WIDTH; ++z)
					{
						// From the lamp for the nearest blocks calculate the lighting from it
						if (map[x][y][z].cube == LAMP)
						{
							for (int i = x - RADIUS_BLOCKS_LIGHTING > 0 ? x - RADIUS_BLOCKS_LIGHTING : 0; i <= (x + RADIUS_BLOCKS_LIGHTING < MAP_LENGTH_AND_WIDTH ? x + RADIUS_BLOCKS_LIGHTING : MAP_LENGTH_AND_WIDTH - 1); ++i)
								for (int j = y - RADIUS_BLOCKS_LIGHTING > 0 ? y - RADIUS_BLOCKS_LIGHTING : 0; j <= (y + RADIUS_BLOCKS_LIGHTING < MAP_HEIGHT ? y + RADIUS_BLOCKS_LIGHTING : MAP_HEIGHT); ++j)
									for (int k = z - RADIUS_BLOCKS_LIGHTING; k <= (z + RADIUS_BLOCKS_LIGHTING < MAP_LENGTH_AND_WIDTH ? z + RADIUS_BLOCKS_LIGHTING : MAP_LENGTH_AND_WIDTH - 1); ++k)
									{
										float light = 1.0f - abs(x - i) * STEP_REDUCING_LIGHTING - abs(z - k) * STEP_REDUCING_LIGHTING - abs(y - j) * STEP_REDUCING_LIGHTING;
										map[i][j][k].light = light > map[i][j][k].light ? light : map[i][j][k].light;
									}
							}
					}

		}

		//************************************************************************************************************************************************

		void generateMap(bool newGame)
		{
			// Loading the heightmap
			img.pixels = stbi_load("textures/other_Picture/heightmap.png", &img.width, &img.height, &cnt, 0);
			// The entire map is loaded otherwise only the database is loaded and the map is generated
			if (newGame)
			{
				// Going through the heightmap pixels
				for (int x = 0; x < MAP_LENGTH_AND_WIDTH; ++x)
					for (int z = 0; z < MAP_LENGTH_AND_WIDTH; ++z)
					{
						//Find the desired pixel and take 1/13 of the red RGB component
						unsigned char* pixelOffset = img.pixels + (x + img.height * z) * cnt;
						unsigned char r = pixelOffset[0] / DIV_R_IN_RGB;

						// The first two layers are filled with BEDROCK and GROUND by default
						SET_BLOCK_PROPERTIES(x, 0, z, true, BEDROCK, 0.0f);
						SET_BLOCK_PROPERTIES(x, 1, z, true, GROUND, 0.0f);

						// The larger the r, the higher the blocks will be
						for (int y = MIN_COORD_Y_BASE; y < r; ++y)
						{
							// The top 4 blocks up to r are filled with ground
							if (y > r - 4)
							{
								SET_BLOCK_PROPERTIES(x, y, z, true, GROUND, 0.0f);
							}
							// The lower ones are randomly filled with gold iron and ground
							else if (!map[x][y][z].exist && y < r - 5 && y > MIN_COORD_Y_BASE - 1)
							{
								int p = rand() % 1000;

								if (p < RAND_IRON_GENERATE || p == RAND_GOLD_GENERATE)
								{
									SET_BLOCK_PROPERTIES(x, y, z, true, p == RAND_GOLD_GENERATE ? GOLD_ORE : (p < RAND_IRON_GENERATE ? IRON_ORE : 0), 0.0f);

									for (int k = 0; k < (p == RAND_GOLD_GENERATE ? 2 : (p < RAND_IRON_GENERATE ? 5 : 0)); ++k)
									{
										int i = x + rand() % 4 - 2, j = z + rand() % 4 - 2;
										if (!map[i][y][j].exist && (i > 0 && i < MAP_LENGTH_AND_WIDTH && y > 0 &&
											y < MAP_HEIGHT && j > 0 && j < MAP_LENGTH_AND_WIDTH))
										{
											SET_BLOCK_PROPERTIES(i, y, j, true, p == RAND_GOLD_GENERATE ? GOLD_ORE : (p < RAND_IRON_GENERATE ? IRON_ORE : 0), 0.0f);

										}
									}
								}
								else if (p > 150)
								{
									SET_BLOCK_PROPERTIES(x, y, z, true, GROUND, 0.0f);
								}

							}
						}
					}

				loadBase("save/saveHome/saveHome.bin");

				generateIce(img,cnt);
				calculateAllLamp();
			}
			else
			{
				loadAllMap("save/saveMap.bin");
			}
		}

		//************************************************************************************************************************************************

		void loadGame(bool newGame)
		{
			generateMap(newGame);
			loadBag("save/bag.bin", newGame);
			loadParametrs("save/param.bin", newGame);
			loadField("save/field.bin", newGame);
		}

		//************************************************************************************************************************************************

		void saveMap(const char* path)
		{
			std::ofstream file(path, std::ios::binary);
			if (file.is_open())
			{
				file.write((char*)map, sizeof(blockMap) * MAP_LENGTH_AND_WIDTH * MAP_HEIGHT * MAP_LENGTH_AND_WIDTH);
				file.close();
			}
			else
				std::cout << "File: " << path << " not found!" << std::endl;

			stbi_image_free(img.pixels);
		}

		//************************************************************************************************************************************************

		void saveField(const char* path)
		{
			std::ofstream file(path, std::ios::binary);
			if (file.is_open())
			{
				file.write((char*)Fld::arrayPotato, sizeof(Fld::parametersPotato)* FIELD_POTATO_WIDTH * FIELD_POTATO_LEN);
				file.write((char*)Fld::arrayBattery, sizeof(Fld::parametersBattery)* FIELD_BATTERY_WIDTH * FIELD_BATTERY_LEN);
				file.write((char*)&Fld::numberClearBattery, sizeof(int));
				file.close();
			}
			else
				std::cout << "File: " << path << " not found!" << std::endl;
		}

		//************************************************************************************************************************************************

		void saveParametrs(const char* path)
		{
			std::ofstream file(path, std::ios::binary);
			if (file.is_open())
			{
				file.write((char*)&player.heart, sizeof(int));
				file.write((char*)&player.hunger, sizeof(int));
				file.write((char*)&player.oxygen, sizeof(int));
				file.write((char*)&player.Position, sizeof(glm::vec3));
				file.write((char*)&secondDay, sizeof(double));
				file.write((char*)&player.oldY, sizeof(int));
				file.close();
			}
			else
				std::cout << "File: " << path << " not found!" << std::endl;
		}

		//************************************************************************************************************************************************

		void saveBag(const char* path)
		{
			std::ofstream file(path, std::ios::binary);
			if (file.is_open())
			{
				file.write((char*)player.bag, sizeof(item)* BAG_SIZE);
				file.write((char*)&player.handItem, sizeof(item));
				file.write((char*)Wrl::Mn::ClM::boxMenu, sizeof(item)* (BAG_SIZE - 1) * (BAG_SIZE - 1));
				file.write((char*)&Wrl::Mn::ClM::bakeInpItem, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::bakeInpBucket, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::bakeOutItem, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::generatorInpWater, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::generatorInpBall, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::generatorOut, sizeof(item));
				file.write((char*)&Wrl::Mn::ClM::craftOut, sizeof(item));
				file.write((char*)Wrl::Mn::ClM::craftMenuInp, sizeof(item)* NUM_ITEM_CRAFT_INP * NUM_ITEM_CRAFT_INP);
				file.close();
			}
			else
				std::cout << "File: " << path << " not found!" << std::endl;
		}

		//************************************************************************************************************************************************

		void saveGame()
		{
			saveMap("save/saveMap.bin");
			saveBag("save/bag.bin");
			saveParametrs("save/param.bin");
			saveField("save/field.bin");
		}

		//************************************************************************************************************************************************

		void gameMenuClick(double mouseX, double mouseY)
		{
			if (Mn::ClM::hit(mouseX, mouseY, -0.554, 0.43, 0.554, 0.565))
			{
				Mn::ClM::flagGameMenu = false;
				flagMouseBind = true;
			}
			if (Mn::ClM::hit(mouseX, mouseY, -0.554, 0.225, 0.554, 0.3575))
			{
				glfwSetWindowShouldClose(window, GL_TRUE);
				saveGame();
			}
		}

		//************************************************************************************************************************************************

	}
}