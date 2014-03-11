// -------- Olympic-Themed OpenGL Scene for -------- //
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>

//Typedefs
typedef GLfloat point8[8]; //Data type to hold Vertex, Texture Co-ords, and Normals
typedef GLfloat point5[5]; //Data type to hold both Vertex and Texture Co-ords
typedef GLfloat point3[3]; //Data type to hold only Vertex Co-ords

// Function prototypes
void KeyPress(unsigned char key, int x, int y);
GLuint Raw_Texture_Load(const char *filename, int width, int height);
void InitDrawLists();
void InitVertArrays();
void Menu(int item);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);
void Reshape (int width, int height);
void RenderSphere(float x, float y, float z, float radius,int subdivisions,GLUquadricObj *quadric);
void RotateVector(float angle);
void IdleCall();

// Light Variables
GLfloat light0_pos[] = {0.0f,10.0f,0.0f,1.0f}; GLfloat light1_pos[] = {0.0f,10.0f,-12.0f,1.0f}; GLfloat light2_pos[] = {0.0f,10.0f,-6.0f,1.0f};
GLfloat light0_dir[] = {0.0f,-1.0f,0.0f};	   GLfloat light1_dir[] = {0.0f,-1.0f,0.0f};
GLfloat light0_ambi[] = {0.2f,0.2f,0.2f,1.0f};													GLfloat light2_ambi[] = {0.2f,0.2f,0.2f,1.0f};
GLfloat light0_spec[] = {1.0f,1.0f,1.0f,1.0f};													GLfloat light2_spec[] = {0.5f,0.5,0.5f,1.0f};

// Material Variables
GLfloat mat_spec_blue[] = {0.0f,0.0f,1.0f,0.6f};
GLfloat mat_ambi_blue[] = {0.2f,0.2f,0.2f,1.0f};
GLfloat mat_emis_blue[] = {0.0f,0.0f,1.0f,1.0f};
GLfloat mat_emis_none[] = {0.0f,0.0f,0.0f,0.0f};
GLfloat mat_spec_stonewhite[] = {0.2f,0.2f,0.2f};
GLfloat mat_ambi_stonewhite[] = {0.6f,0.6f,0.6f};
GLfloat mat_ambi_grassgreen[] = {0.4f,1.0f,0.0f};
GLfloat mat_spec_grassgreen[] = {0.0f,1.0f,0.0f};
GLfloat mat_emis_grassgreen[] = {0.4f, 1.0f, 0.0f};

// Camera Variables
float lookX = 0.0f, lookY = 1.0f, lookZ = 0.0f, centX = 0.0f, centY = 0.0f, centZ = lookZ - 3.0f;
float theta = 0.0f;
const float PI = 3.14159265358979;
float viewVector[4] = {0.0f, 1.0f, lookZ - 3.0f, 0};
float newViewVector[4] = {0.0f, 0.0f, 0.0f, 0.0f};
float changeX, changeZ;

// Mouse Variables
float xClicked = 0.0;	float yClicked = 0.0;
bool leftClick = false;	bool middleClick = false;

// Robot Limb Variables (Rotation)
float UR_RotArm = 0.0f, LR_RotArm = 0.0f, UL_RotArm = 0.0f, LL_RotArm = 0.0f, UU_RotTor = 0.0f,
	UR_RotLeg = 0.0f, LR_RotLeg = 0.0f, UL_RotLeg = 0.0f, LL_RotLeg = 0.0f;

// Water Animation Variables
int wave_iter = 0;
float wave_form_l = 0.8;
float wave_form_r = 0.8;
float wave_dir_l = 0.00005f;
float wave_dir_r = -0.00005f;

// Switches to turn graphical routines on or off
bool textureSwitch = true; bool lightingSwitch = true; bool colMatSwitch = true;
bool blendSwitch = true; bool animSwitch = true; bool cullSwitch = true;
bool daySwitch = true; bool shadeSwitch = true; bool light0Switch = true;
bool light1Switch = true;	bool light2Switch = true;

// Pre-define texture2D textureIDs
GLuint tex_foot, tex_limb, tex_torso, tex_arena_grass, tex_arena_stand, tex_arena_fans, tex_arena_tile, tex_arena_logo;


/*------------- Array Initilisation -------------*/
//Point8: U,V, Nx,Ny,Nz, X,Y,Z
//Define faces in terms of Texture/Vertices - for limb-component
point8 limb_TexVert[24] = { /*0*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f}, /*1*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f},
							/*2*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f}, /*3*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,-1.0f},

							/*7*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,-1.0f},/*6*/{1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,-4.0f,-1.0f}, 
							/*5*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,-4.0f,1.0f},/*4*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,1.0f},							

							/*8*/{0.0f,0.0f, 0.0f,0.0f,-1.0f, -1.0f,0.0f,-1.0f}, /*9*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f},
							/*10*/{1.0f,1.0f, 0.0f,0.0f,-1.0f, 1.0f,-4.0f,-1.0f}, /*11*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,-1.0f},

							/*12*/{0.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f}, /*13*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f},
							/*14*/{1.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,-4.0f,1.0f}, /*15*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,-4.0f,-1.0f},

							/*16*/{0.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f}, /*17*/{0.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f},
							/*18*/{1.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,1.0f}, /*19*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,-4.0f,1.0f},

							/*20*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f}, /*21*/{0.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,-1.0f},
							/*22*/{1.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,-1.0f}, /*23*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,-4.0f,1.0f} };
//Connect the vertices of the limb to create a 3D model
GLubyte limb_indices[24] = {0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23};

//Define faces in terms of Texture/Vertices - for torso-component
point8 torso_TexVert[56] = { {0.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,-1.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,1.0f},
								{0.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,2.0f},
								{0.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,-1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,1.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,2.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,-1.0f}, {1.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,1.0f},
								{0.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,2.0f},
								{0.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,-1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,1.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,2.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,-1.0f},{0.0f,1.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,1.0f},{1.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,-1.0f},							 

								{0.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,1.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,1.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,2.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,1.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,2.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,1.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,-1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,-1.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,1.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, 3.0f,0.0f,-1.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,-2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 3.0f,-7.0f,-1.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,-2.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,-2.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, 2.0f,-7.0f,-2.0f},

								{0.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,-2.0f}, {0.0f,1.0f, 0.0f,0.0f,0.0f, -3.0f,0.0f,-1.0f},
								{1.0f,1.0f, 0.0f,0.0f,0.0f, -3.0f,-7.0f,-1.0f}, {1.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,-7.0f,-2.0f} };
//Connect the vertices of the torso to create a 3D model
GLubyte torso_indices[56] = {3,2,1,0, 7,6,5,4, 11,10,9,8, 15,14,13,12, 19,18,17,16, 23,22,21,20, 27,26,25,24,
	31,30,29,28, 35,34,33,32, 39,38,37,36, 43,42,41,40, 47,46,45,44, 51,50,49,48, 55,54,53,52};

//Define every instance once, of a vertice in a foot-component
point8 foot_TexVert[] = { /*0*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f}, /*1*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f},
							/*2*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f}, /*3*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,-1.0f},

							/*4*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,1.5f}, /*5*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.5f,-1.0f,1.5f},
							/*6*/{1.0f,1.0f, 0.0f,0.0f,0.0f, 1.5f,-1.0f,-1.5f}, /*7*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,-1.5f},

							/*8*/{0.0f,0.0f, 0.0f,0.0f,-1.0f, -1.0f,0.0f,-1.0f}, /*9*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f},
							/*10*/{1.0f,1.0f, 0.0f,0.0f,-1.0f, 1.5f,-1.0f,-1.5f}, /*11*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,-1.5f},

							/*12*/{0.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,-1.0f}, /*13*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f},
							/*14*/{1.0f,1.0f, 0.0f,0.0f,0.0f, 1.5f,-1.0f,1.5f}, /*15*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.5f,-1.0f,-1.5f},

							/*16*/{0.0f,0.0f, 0.0f,0.0f,0.0f, 1.0f,0.0f,1.0f}, /*17*/{0.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f},
							/*18*/{1.0f,1.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,1.5f}, /*19*/{1.0f,0.0f, 0.0f,0.0f,0.0f, 1.5f,-1.0f,1.5f},

							/*20*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,1.0f}, /*21*/{0.0f,1.0f, 0.0f,0.0f,0.0f, -1.0f,0.0f,-1.0f},
							/*22*/{1.0f,1.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,-1.5f}, /*23*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -1.5f,-1.0f,1.5f} };
//Connect the vertices of the foot to create a 3D model
GLubyte foot_indices[] = {0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23};


//Define every instance once, of a vertice in an arena-component
point8 arena_TexVert[] = { /*0*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,3.0f},/*1*/{0.0f,6.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,3.0f},
						/*2*/{12.0f,6.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,-3.0f},/*3*/{12.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,-3.0f},

						/*4*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,3.0f},/*5*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,3.0f},
						/*6*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,0.0f,3.0f},/*7*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,0.0f,3.0f},							

						/*8*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,0.0f,3.0f},/*9*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,2.0f},
						/*10*/{1.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,2.0f},/*11*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,3.0f}, 


						/*12*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,2.0f},/*13*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,-2.0f},
						/*14*/{1.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,-2.0f},/*15*/{1.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,2.0f},													

						/*16*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,0.0f,-2.0f},/*17*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,0.0f,-3.0f},
						/*18*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-3.0f},/*19*/{1.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,-2.0f},														

						/*20*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-3.0f},/*21*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-3.0f},
						/*22*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,0.0f,-3.0f},/*23*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,0.0f,-3.0f},							

						/*24*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,0.0f,-3.0f},/*25*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,-2.0f},	
						/*26*/{1.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,-2.0f},/*27*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-3.0f},												

						/*28*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,-2.0f},/*29*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,2.0f},
						/*30*/{1.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,2.0f},/*31*/{1.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,-2.0f},														

						/*32*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,0.0f,2.0f},/*33*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,0.0f,3.0f},
						/*34*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,3.0f},/*35*/{1.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,2.0f},													

						/*36*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,4.0f},/*37*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,4.0f},
						/*38*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,3.0f},/*39*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,3.0f},							

						/*40*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,4.0f},/*41*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,2.0f}, 
						/*42*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,2.0f},/*43*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,3.0f},

						/*44*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,2.0f},/*45*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,-2.0f},
						/*46*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,-2.0f},/*47*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,2.0f},							

						/*48*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,-2.0f},/*49*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-4.0f},
						/*50*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-3.0f},/*51*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,-2.0f},							

						/*52*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-4.0f},/*53*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-4.0f},
						/*54*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-3.0f},/*55*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-3.0f},							

						/*56*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-4.0f},/*57*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,-2.0f},
						/*58*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,-2.0f},/*59*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-3.0f},							

						/*60*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,-2.0f},/*61*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,2.0f}, 
						/*62*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,2.0f},/*63*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,-2.0f},							 

						/*64*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,2.0f},/*65*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,4.0f},
						/*66*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,3.0f},/*67*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,2.0f},

						/*68*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,4.0f},/*69*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,4.0f},
						/*70*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,4.0f},/*71*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,4.0f},							

						/*72*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,4.0f},/*73*/{0.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,2.0f},
						/*74*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,2.0f},/*75*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,4.0f},														

						/*76*/{0.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,2.0f},/*77*/{0.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,-2.0f},
						/*78*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,-2.0f},/*79*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,2.0f},																										 

						/*80*/{0.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,1.0f,-2.0f},/*81*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-4.0f},
						/*82*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,-4.0f}, /*83*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,-2.0f},														

						/*84*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,-4.0f},/*85*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,-4.0f},
						/*86*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-4.0f},/*87*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-4.0f},							  

						/*88*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-4.0f},/*89*/{0.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,-2.0f},
						/*90*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,-2.0f},/*91*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,-4.0f},														

						/*92*/{0.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,-2.0f},/*93*/{0.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,2.0f},
						/*94*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,2.0f},/*95*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,-2.0f},														

						/*96*/{0.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,1.0f,2.0f},/*97*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,4.0f},
						/*98*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,4.0f},/*99*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,2.0f},

						/*100*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,2.0f,6.0f},/*101*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,2.0f,6.0f},	
						/*102*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,4.0f},/*103*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,4.0f},													

						/*104*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,2.0f,6.0f},/*105*/{0.0f,1.0f, 0.0f,0.0f,0.0, 8.0f,2.0f,4.0f},
						/*106*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,2.0f},/*107*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,4.0f}, 

						/*108*/{0.0f,0.0f, 0.0f,0.0f,0.0, 8.0f,2.0f,4.0f},/*109*/{0.0f,1.0f, 0.0f,0.0f,0.0, 8.0f,2.0f,-4.0f},
						/*110*/{1.0f,0.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,-2.0f},/*111*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,2.0f}, 

						/*112*/{0.0f,0.0f, 0.0f,0.0f,0.0, 8.0f,2.0f,-4.0f},/*113*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,2.0f,-6.0f},
						/*114*/{1.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,-4.0f},/*115*/{1.0f,1.0f, 0.0f,0.0f,0.0, 7.0f,2.0f,-2.0f}, 

						/*116*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,2.0f,-6.0f},/*117*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,2.0f,-6.0f},
						/*118*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,-4.0f},/*119*/{1.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,2.0f,-4.0f}, 

						/*120*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,2.0f,-6.0f},/*121*/{0.0f,1.0f, 0.0f,0.0f,0.0, -8.0f,2.0f,-4.0f},
						/*122*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,-2.0f},/*123*/{1.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,-4.0f}, 

						/*124*/{0.0f,0.0f, 0.0f,0.0f,0.0, -8.0f,2.0f,-4.0f},/*125*/{0.0f,1.0f, 0.0f,0.0f,0.0, -8.0f,2.0f,4.0f},
						/*126*/{1.0f,0.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,2.0f},/*127*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,-2.0f}, 

						/*128*/{0.0f,0.0f, 0.0f,0.0f,0.0, -8.0f,2.0f,4.0f},/*129*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,2.0f,6.0f},
						/*130*/{1.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,2.0f,4.0f},/*131*/{1.0f,1.0f, 0.0f,0.0f,0.0, -7.0f,2.0f,2.0f},

						/*132*/{4.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.5f,2.0f},/*133*/{0.0f,0.0f, 0.0f,0.0f,0.0, 6.0f,1.5f,-2.0f},
						/*134*/{0.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,-2.0f},/*135*/{4.0f,1.0f, 0.0f,0.0f,0.0, 6.0f,1.0f,2.0f},

						/*136*/{0.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,2.0f},/*137*/{4.0f,1.0f, 0.0f,0.0f,0.0, -6.0f,1.0f,-2.0f},
						/*138*/{4.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.5f,-2.0f},/*139*/{0.0f,0.0f, 0.0f,0.0f,0.0, -6.0f,1.5f,2.0f},

						/*140*/{0.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,3.0f},/*141*/{10.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,3.0f},
						/*142*/{10.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.5f,3.0f},/*142*/{0.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.5f,3.0f},

						/*144*/{0.0f,1.0f, 0.0f,0.0f,0.0, -5.0f,1.0f,-3.0f},/*145*/{10.0f,1.0f, 0.0f,0.0f,0.0, 5.0f,1.0f,-3.0f},
						/*146*/{10.0f,0.0f, 0.0f,0.0f,0.0, 5.0f,1.5f,-3.0f},/*147*/{0.0f,0.0f, 0.0f,0.0f,0.0, -5.0f,1.5f,-3.0f} };
GLubyte arena_indices[] = { 0,1,2,3 };
GLubyte arena_stand_indices[] = { 4,5,6,7, 11,10,9,8, 15,14,13,12, 19,18,17,16, 20,21,22,23, 27,26,25,24, 31,30,29,28, 35,34,33,32,
	36,37,38,39, 40,41,42,43, 44,45,46,47, 48,49,50,51, 52,53,54,55, 56,57,58,59, 60,61,62,63, 64,65,66,67,
	68,69,70,71, 75,74,73,72, 79,78,77,76, 83,82,81,80, 84,85,86,87, 91,90,89,88, 95,94,93,92, 99,98,97,96,
	100,101,102,103, 104,105,106,107, 108,109,110,111, 112,113,114,115, 116,117,118,119, 120,121,122,123,
	124,125,126,127, 128,129,130,131 };
GLubyte arena_fans_indices[] = { 132,133,134,135, 136,137,138,139, 140,141,142,143, 144,145,146,147 };

point8 logo_TexVert[] = { /*0*/{0.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,2.0f},/*1*/{0.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,2.0f},
	/*2*/{1.0f,1.0f, 0.0f,0.0f,0.0f, 2.0f,0.0f,-2.0f},/*3*/{1.0f,0.0f, 0.0f,0.0f,0.0f, -2.0f,0.0f,-2.0f} };
GLubyte logo_indices[] = { 0,1,2,3 };
/*-----------End Array Initilisation ------------*/

// Function to convert Degrees to Radions
float deg2rad(float angle)
{
	return PI*angle/180;
}
// Function to rotate camera on the Y-Axis
void RotateY()
{
	float newX, newZ;
	float tempLookX, tempLookZ, tempX, tempZ;

	tempLookX = lookX - lookX;
	tempLookZ = lookZ - lookZ;
	tempX = viewVector[0] - lookX;
	tempZ = viewVector[2] - lookZ;

	newX = (tempX * cos(theta)) - (tempZ * sin(theta));
	newZ = (tempX * sin(theta)) + (tempZ * cos(theta));

	viewVector[0] = newX + lookX;
	viewVector[2] = newZ + lookZ;
}
//Progress the camera into or out of the scene in our facing vector direction
void TraverseLine()
{
	changeX = viewVector[0] - lookX;
	changeZ = viewVector[2] - lookZ;
}

// Subdivision of a plane
int n; //Number of subdivision steps
point3 v[4] = { {-3.0f,0.8,-3.0f}, {3.0f,0.8,-3.0f}, {3.0f,0.8,3.0f}, {-3.0f,0.8,3.0f} }; //Initial quad for subdivision
// Draws a QUAD at the given co-ordinates
void DrawSquare(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d)
{
	glBegin(GL_QUADS);
		glVertex3fv(a);
		glVertex3fv(d);
		glVertex3fv(c);
		glVertex3fv(b);
	glEnd();
}
// Implemented algarithm to recursively subdivide a QUAD to a give number of iterations (m)
void divide_square(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int m)
{
	point3 v0, v1, v2, v3, mid;
	int j;

	if (m > 0)
	{
		for(j=0; j<3; j++) v0[j]=(a[j]+b[j])/2;
		for(j=0; j<3; j++) v1[j]=(a[j]+d[j])/2;
		for(j=0; j<3; j++) v2[j]=(d[j]+c[j])/2;
		for(j=0; j<3; j++) v3[j]=(b[j]+c[j])/2;

		mid[0]=((v0[0]+v2[0])/2);
		mid[1]=(0.8);
		mid[2]=((v1[2]+v3[2])/2);
		divide_square(a, v0, mid, v1, m-1);
		divide_square(v0, b, v3, mid, m-1);
		divide_square(v1, mid, v2, d, m-1);
		divide_square(mid, v3, c, v2, m-1);
	}
	else
	{
		//Apply brute-force deformation to animate
		//Incredibly ineffecient soloution, needs revision
		wave_iter++;

		// '/'
		if(wave_iter == 1 || wave_iter == 3 || wave_iter == 9 || wave_iter == 11
			|| wave_iter == 33 || wave_iter == 35 || wave_iter == 41 || wave_iter == 43)
		{
			b[1] = wave_form_l;
			c[1] = wave_form_l;
		}
		if(wave_iter == 17 || wave_iter == 19 || wave_iter == 25 || wave_iter == 27
			|| wave_iter == 49 || wave_iter == 51 || wave_iter == 57 || wave_iter == 59)
		{
			b[1] = wave_form_r;
			c[1] = wave_form_r;
		}
		// '-'
		if(wave_iter == 2 || wave_iter == 4 || wave_iter == 10 || wave_iter == 12
			|| wave_iter == 34 || wave_iter == 36 || wave_iter == 42 || wave_iter == 44)
		{
			a[1] = wave_form_l;
			b[1] = wave_form_l;
			c[1] = wave_form_l;
			d[1] = wave_form_l;
		}
		if(wave_iter == 18 || wave_iter == 20 || wave_iter == 26 || wave_iter == 28
			|| wave_iter == 50 || wave_iter == 52 || wave_iter == 58 || wave_iter == 60)
		{
			a[1] = wave_form_r;
			b[1] = wave_form_r;
			c[1] = wave_form_r;
			d[1] = wave_form_r;
		}
		// '\'
		if(wave_iter == 5 || wave_iter == 7 || wave_iter == 13 || wave_iter == 15
			|| wave_iter == 37 || wave_iter == 39 || wave_iter == 45 || wave_iter == 47)
		{
			a[1] = wave_form_l;
			d[1] = wave_form_l;
		}
		if(wave_iter == 21 || wave_iter == 23 || wave_iter == 29 || wave_iter == 31
			|| wave_iter == 53 || wave_iter == 55 || wave_iter == 61 || wave_iter == 63)
		{
			a[1] = wave_form_r;
			d[1] = wave_form_r;
		}
		DrawSquare(a,b,c,d);
	}
}

// Apply cross-product of U and V to a face to define nomrals for lighting
// Works on every 4 Vertices - IE: A quad face
void CalculateNormals(point8 toCalc[], int toCalcLength)
{
	GLfloat N[3], U[3], V[3];//N will be our final calculated normal, U and V will be the subjects of cross-product
	float length;

	for (int i = 0; i < toCalcLength; i+=4)
	{
		U[0] = toCalc[i+1][5] - toCalc[i][5]; U[1] = toCalc[i+1][6] - toCalc[i][6]; U[2] = toCalc[i+1][7] - toCalc[i][7];
		V[0] = toCalc[i+3][5] - toCalc[i][5]; V[1] = toCalc[i+3][6] - toCalc[i][6]; V[2] = toCalc[i+3][7] - toCalc[i][7];

		N[0] = (U[1]*V[2]) - (U[2] * V[1]);
		N[1] = (U[2]*V[0]) - (U[0] * V[2]);
		N[2] = (U[0]*V[1]) - (U[1] * V[0]);

		//Calculate length for normalising
		length = (float)sqrt((pow(N[0],2)) + (pow(N[1],2)) + (pow(N[2],2)));

		//Normalise
		for (int a = 0; a < 3; a++)
		{
			N[a]/=length;
		}
		//Apply to every vertice in this quad
		for (int j = 0; j < 4; j++)
		{
			toCalc[i+j][2] = N[0]; toCalc[i+j][3] = N[1]; toCalc[i+j][4] = N[2];
		}
	}
}


void init(void)
{
	glClearColor(0,0,0,1);	// Background color (RGB)- Black
	glLineWidth(3);			// Width of the drawing line

	glShadeModel (GL_SMOOTH); // Shading model to provide more realism - more costly

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set the blending mode to allow for Alpha Transparency

	glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Let glColor commands change material properties

	glMatrixMode(GL_MODELVIEW); // Work within the ModelView Matrix
	glLoadIdentity(); 
	gluPerspective(60, 1, .1, 100); //Set up the camera boundaries

	glEnable(GL_NORMALIZE); //Performance pitfall - consider scaling vertices co-ords by hand?

	glEnable(GL_DEPTH_TEST); // Allow components to be drawn in the correct depth order

	// Define and set any VertexArray data (ran outside of display for optimisation).
	InitDrawLists();

	// Load textures
	tex_foot = Raw_Texture_Load("Content/Textures/footTex", 256, 256);
	tex_limb = Raw_Texture_Load("Content/Textures/limbTex", 256, 256);
	tex_torso = Raw_Texture_Load("Content/Textures/torsoTex", 256, 256);

	tex_arena_grass = Raw_Texture_Load("Content/Textures/arenaGrassTex", 512, 512);
	tex_arena_stand = Raw_Texture_Load("Content/Textures/arenaStandTex", 256, 256);
	tex_arena_fans = Raw_Texture_Load("Content/Textures/arenaFansTex", 256, 256);
	tex_arena_tile = Raw_Texture_Load("Content/Textures/arenaTileTex", 256, 256);
	tex_arena_logo = Raw_Texture_Load("Content/Textures/arenaLogoTex", 256, 256);

	// Setup Menus - Descriptions and Case
	glutCreateMenu(Menu);
	glutAddMenuEntry("Wireframe Mode", 1);
	glutAddMenuEntry("Polygon Mode", 2);
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Enable/Disable Texturing", 3);
	glutAddMenuEntry("Enable/Disable Lighting", 4);
	glutAddMenuEntry("Enable/Disable Color Materials", 5);
	glutAddMenuEntry("Enable/Disable Alpha Blending", 6);
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Enable/Disable Animation", 7);
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Enable/Disable Backface Culling", 8);
	glutAddMenuEntry("Flat/Smooth Shading", 9);
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Day/Night Mode", 10);

	// Associate the right mouse button with Menu
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
// Menu is passed Case from Init
void Menu(int item)
{
	switch (item)
	{
	case 1:
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		break;
	case 2:
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glPolygonMode( GL_FRONT, GL_FILL );
		break;

	case 3:
		textureSwitch = !textureSwitch;
		break;
	case 4:
		lightingSwitch = !lightingSwitch;
		break;
	case 5:
		colMatSwitch = !colMatSwitch;
		break;
	case 6:
		blendSwitch = !blendSwitch;
		break;

	case 7:
		animSwitch = !animSwitch;
		break;
	case 8:
		cullSwitch = !cullSwitch;
		break;

	case 9:
		shadeSwitch = !shadeSwitch;
		break;

	case 10:
		daySwitch = !daySwitch;
		break;
	default:
		break;
	}

	glutPostRedisplay();

	return;
}


// Quadric setup for the rendering of a sphere
void RenderSphere(float x, float y, float z, float radius,int subdivisions,GLUquadricObj *quadric)
{
	glPushMatrix();
		glTranslatef(x,y,z);
		gluSphere(quadric, radius, subdivisions,subdivisions);
	glPopMatrix();
}


// Main drawing and drawing subroutines
void display()
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Set textures to have RGB effected by glColorf

	// Switch between flat and smooth shading
	if (shadeSwitch)
		glShadeModel (GL_SMOOTH);
	else
		glShadeModel (GL_FLAT);
	// Face culling, halting the render of back-faces
	if (cullSwitch)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	// Allow the idleFunc to be turned off, stopping animation/idle process
	if (animSwitch)
		glutIdleFunc(IdleCall);
	else
		glutIdleFunc(NULL);

	// Render/Camera setup
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();    
	gluLookAt(lookX,lookY,lookZ,viewVector[0],viewVector[1],viewVector[2],0,1,0);	

	// Enable lighting model
	if (lightingSwitch)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if (light0Switch)
		glEnable(GL_LIGHT0);
	if (light1Switch)
		glEnable(GL_LIGHT1);
	if (light2Switch)
		glEnable(GL_LIGHT2);

	// Re-apply lighting parameters, should they have changed
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);			glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);			glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambi);			glLightfv(GL_LIGHT1, GL_AMBIENT, light0_ambi);			glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambi);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_dir);	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_dir);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 15.0f);				glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0f);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_spec);			glLightfv(GL_LIGHT1, GL_SPECULAR, light0_spec);			glLightfv(GL_LIGHT2, GL_SPECULAR, light2_spec);

	if (colMatSwitch)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);

	if (blendSwitch)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	if (daySwitch)
	{
		glColor4f(0.0f,0.4f,1.0f,1.0f);
		light0_ambi[0] = 0.8f; light0_ambi[1] = 0.8f; light0_ambi[2] = 0.8f;
		light2_ambi[0] = 0.2f; light2_ambi[1] = 0.2f; light2_ambi[2] = 0.2f;
		light0_spec[0] = 1.0f; light0_spec[1] = 1.0f; light0_spec[2] = 1.0f;
		light2_spec[0] = 0.5f; light2_spec[1] = 0.5f; light2_spec[2] = 0.5f;
		mat_emis_grassgreen[0] = 0.4f; mat_emis_grassgreen[1] = 1.0f; mat_emis_grassgreen[2] = 0.0f;

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light0_ambi);
	}
	else
	{
		glColor4f(0.0f,0.0f,0.3f,1.0f);
		light0_ambi[0] = 0.3f; light0_ambi[1] = 0.3f; light0_ambi[2] = 0.3f;
		light2_ambi[0] = 0.0f; light2_ambi[1] = 0.0f; light2_ambi[2] = 0.0f;
		light0_spec[0] = 0.4f; light0_spec[1] = 0.4f; light0_spec[2] = 0.4f;
		light2_spec[0] = 0.1f; light2_spec[1] = 0.1f; light2_spec[2] = 0.1f;
		mat_emis_grassgreen[0] = 0.0f; mat_emis_grassgreen[1] = 0.3f; mat_emis_grassgreen[2] = 0.0f;

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light0_ambi);
	}
	// Render skysphere
	glPushMatrix();
		glScalef(-1,-1,-1); // Invert backfaces/View from inside
		RenderSphere(0,0,0,20,10,gluNewQuadric());
	glPopMatrix();

	if (textureSwitch)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
	
	//Draw torso and child components
	glColor4f(0.6f,0.6f,0.6f,1.0f); // Effect lighting to 'cloudy' ambient
	glPushMatrix();
		glTranslatef(0,0.4,0);
		glRotatef(UU_RotTor,1,0,0);
		glScalef(0.025f,0.025f,0.025f);
		//Draw following objects with texture ID'd tex_torso
		glBindTexture(GL_TEXTURE_2D, tex_torso);
		//Draw torso
		glCallList(3);

		//Draw head
		glPushMatrix();
			//Draw head
			glScalef(2.0f,2.0f,2.0f);
			glTranslatef(0,1,0);
			glCallList(1);
		glPopMatrix();

		//Draw following objects with texture ID'd tex_limb
		glBindTexture(GL_TEXTURE_2D, tex_limb);
		//Draw right arm
		glPushMatrix();
			//Draw UR_Arm
			glTranslatef(4, 0, 0);
			glRotatef(UR_RotArm,1,0,0);
			glCallList(2);

			glPushMatrix();
				//Draw LR_Arm
				glTranslatef(0, -4, 0);
				glRotatef(LR_RotArm,1,0,0);
				glCallList(2);
			glPopMatrix();
		glPopMatrix();

		//Draw left arm
		glPushMatrix();
		//Draw UL_Arm
			glTranslatef(-4, 0, 0);
			glRotatef(UL_RotArm,1,0,0);
			glCallList(2);

			glPushMatrix();
				//Draw LL_Arm
				glTranslatef(0, -4, 0);
				glRotatef(LL_RotArm,1,0,0);
				glCallList(2);
			glPopMatrix();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, tex_limb);
		//Draw right leg
		glPushMatrix();
			//Draw UR_Leg
			glTranslatef(-2, -7, 0);
			glRotatef(UR_RotLeg,1,0,0);
			glCallList(2);

			glPushMatrix();
				//Draw LR_Leg
				glTranslatef(0, -4, 0);
				glRotatef(LR_RotLeg,1,0,0);
				glCallList(2);
				glTranslatef(0, -4, 0);
				glBindTexture(GL_TEXTURE_2D, tex_foot);
				glCallList(1);
			glPopMatrix();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, tex_limb);
		//Draw left leg
		glPushMatrix();
			//Draw UL_Leg
			glTranslatef(2, -7, 0);
			glRotatef(UL_RotLeg,1,0,0);
			glBindTexture(GL_TEXTURE_2D, tex_limb);
			glCallList(2);

			glPushMatrix();
				//Draw LL_Leg
				glTranslatef(0, -4, 0);
				glRotatef(LL_RotLeg,1,0,0);
				glCallList(2);
				glTranslatef(0, -4, 0);
				glBindTexture(GL_TEXTURE_2D, tex_foot);
				glCallList(1);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	

	//First Stadium
	glBindTexture(GL_TEXTURE_2D, tex_arena_grass);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec_grassgreen);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_spec_grassgreen);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emis_grassgreen);
	glCallList(4);
	glPushMatrix();
		glTranslatef(0.0f,-0.1f,0.0f);
		glScalef(5.0f,1.0f,5.0f);
		glCallList(4);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, tex_arena_stand);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emis_none);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec_stonewhite);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambi_stonewhite);
	glCallList(5);

	if (blendSwitch)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glBindTexture(GL_TEXTURE_2D, tex_arena_fans);
	glCallList(6);

	//Second Stadium
	glTranslatef(0.0f,0.0f,-12.0f);
	glBindTexture(GL_TEXTURE_2D, tex_arena_tile);
	glCallList(4);

	glBindTexture(GL_TEXTURE_2D, tex_arena_stand);
	glCallList(5);

	glBindTexture(GL_TEXTURE_2D, tex_arena_fans);
	glCallList(6);

	glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, tex_arena_logo);
		glTranslatef(0.0f,0.1f,0.0f);
		glCallList(7);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	if (colMatSwitch)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_spec_blue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambi_blue);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emis_blue);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 40.0f);
	glColor4f(0.2f,0.2f,1.0f,0.4f);
	// Call subdivision and render water
	glTranslatef(3.0f,-0.1f,0.0f);
	glBegin(GL_QUADS);
		wave_iter = 0;
		divide_square(v[0], v[1], v[2], v[3], 3);
	glEnd();
	glTranslatef(-6.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		wave_iter = 0;
		divide_square(v[0], v[1], v[2], v[3], 3);
	glEnd();
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emis_none);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);

	glFlush();
}


////////////////////////////////////////////// 
int main(int argc, char** argv) 
{
	glutInit(&argc, argv);	// GLUT Initialization 
	glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE); // Initializing the Display mode
	glutInitWindowSize(600,400);	// Define the window size
	glutCreateWindow("Robolympics!");	// Create the window, with caption.
	init();	// All OpenGL initialization

	// Callback functions
	glutDisplayFunc(display);
	glutKeyboardFunc(KeyPress);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);

	glutReshapeFunc(Reshape);

	glutMainLoop();	// Loop waiting for event 
}


// Idle processes and animation
void IdleCall()
{
	if (wave_form_l >= 0.9f)
		wave_dir_l = -0.00005f;
	if (wave_form_l <= 0.7f)
		wave_dir_l = 0.00005f;
	if (wave_form_r >= 0.9f)
		wave_dir_r = -0.00005f;
	if (wave_form_r <= 0.7f)
		wave_dir_r = 0.00005f;

	wave_form_l += wave_dir_l;
	wave_form_r += wave_dir_r;

	glutForceJoystickFunc();
	glutPostRedisplay();
}

// Track input from keyboard
void KeyPress(unsigned char key, int x, int y) //Various keyboard controls to change scene
{
	switch(key)
	{
	case '1':
		light0Switch = !light0Switch;
	break;
	case '2':
		light1Switch = !light1Switch;
	break;
	case '3':
		light2Switch = !light2Switch;
	break;

	case 'w':
	case 'W':
		TraverseLine();
		lookX += changeX/2;
		lookZ += changeZ/2;
		viewVector[0] += changeX/2;
		viewVector[2] += changeZ/2;
		break;
	case 's':
	case 'S':
		TraverseLine();
		lookX -= changeX/2;
		lookZ -= changeZ/2;
		viewVector[0] -= changeX/2;
		viewVector[2] -= changeZ/2;
		break;
	case 'q':
	case 'Q':
		lookY += 0.25;
		viewVector[1] += 0.25;
		break;
	case 'e':
	case 'E':
		lookY -= 0.25;
		viewVector[1] -= 0.25;
		break;

	case 'y':
	case 'Y':
		UR_RotLeg += 2.0f;
		break;
	case 'h':
	case 'H':
		UR_RotLeg -= 2.0f;
		break;
	case 'u':
	case 'U':
		LR_RotLeg += 2.0f;
		break;
	case 'j':
	case 'J':
		LR_RotLeg -= 2.0f;
		break;

	case 'i':
	case 'I':
		UL_RotLeg += 2.0f;
		break;
	case 'k':
	case 'K':
		UL_RotLeg -= 2.0f;
		break;
	case 'o':
	case 'O':
		LL_RotLeg += 2.0f;
		break;
	case 'l':
	case 'L':
		LL_RotLeg -= 2.0f;
		break;

	case 'r':
	case 'R':
		theta = deg2rad(1.0f);
		RotateY();
		break;
	case 't':
	case 'T':
		theta = deg2rad(-1.0f);
		RotateY();
		break;
	}

	glutPostRedisplay();
}


///////////////////////////////////
GLuint Raw_Texture_Load(const char *filename, int width, int height)
{
	GLuint texture;
	unsigned char *data;
	FILE *file;

	// open texture data
	file = fopen(filename, "rb");
	if (file == NULL){printf("File did not load."); return 0;}

	// allocate buffer
	data = (unsigned char*) malloc(width * height * 4);

	// read texture data
	fread(data, width * height * 4, 1, file);
	fclose(file);

	// allocate a texture name
	glGenTextures(1, &texture);

	// select our current texture
	glBindTexture(GL_TEXTURE_2D, texture);

	// select modulate to mix texture with color for shading
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);

	// when texture area is small, bilinear filter the closest mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	// when texture area is large, bilinear filter the first mipmap
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// texture should tile
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// build our texture mipmaps
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// free buffer
	free(data);

	return texture;
}


///////////////////////////////////
void InitDrawLists()
{
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glGenLists(7);

	//Foot
	glTexCoordPointer(2,GL_FLOAT,8*sizeof(float),foot_TexVert[0]);
	CalculateNormals(foot_TexVert,24);
	glNormalPointer(GL_FLOAT,8*sizeof(float),foot_TexVert[0]+2);
	glVertexPointer(3,GL_FLOAT,8*sizeof(float),foot_TexVert[0]+5);
	glNewList(1, GL_COMPILE);
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, foot_indices);
	glEndList();
	//Limb
	glTexCoordPointer(2,GL_FLOAT,8*sizeof(float),limb_TexVert[0]);
	CalculateNormals(limb_TexVert,24);
	glNormalPointer(GL_FLOAT,8*sizeof(float),limb_TexVert[0]+2);
	glVertexPointer(3,GL_FLOAT,8*sizeof(float),limb_TexVert[0]+5);
	glNewList(2, GL_COMPILE);
		glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, limb_indices);
	glEndList();
	//Torso
	glTexCoordPointer(2,GL_FLOAT,8*sizeof(float),torso_TexVert[0]);
	CalculateNormals(torso_TexVert,56);
	glNormalPointer(GL_FLOAT,8*sizeof(float),torso_TexVert[0]+2);
	glVertexPointer(3,GL_FLOAT,8*sizeof(float),torso_TexVert[0]+5);
	glNewList(3, GL_COMPILE);
		glDrawElements(GL_QUADS, 56, GL_UNSIGNED_BYTE, torso_indices);
	glEndList();	

	//Arena Pieces
	glTexCoordPointer(2,GL_FLOAT,8*sizeof(float),arena_TexVert[0]);
	CalculateNormals(arena_TexVert,148);
	glNormalPointer(GL_FLOAT,8*sizeof(float),arena_TexVert[0]+2);
	glVertexPointer(3,GL_FLOAT,8*sizeof(float),arena_TexVert[0]+5);
	glNewList(4, GL_COMPILE);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, arena_indices);
	glEndList();

	glNewList(5, GL_COMPILE);
		glDrawElements(GL_QUADS, 128, GL_UNSIGNED_BYTE, arena_stand_indices);
	glEndList();

	glNewList(6, GL_COMPILE);
		glDrawElements(GL_QUADS, 16, GL_UNSIGNED_BYTE, arena_fans_indices);
	glEndList();

	//Logo
	glTexCoordPointer(2,GL_FLOAT,8*sizeof(float),logo_TexVert[0]);
	glNormalPointer(GL_FLOAT,8*sizeof(float),logo_TexVert[0]+2);
	CalculateNormals(logo_TexVert, 4);
	glVertexPointer(3,GL_FLOAT,8*sizeof(float),logo_TexVert[0]+5);
	glNewList(7, GL_COMPILE);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, logo_indices);
	glEndList();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// Track input from mouse buttons
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		leftClick = (state == GLUT_DOWN);
		xClicked = x;
	}
	if (button == GLUT_MIDDLE_BUTTON)
	{
		middleClick = (state == GLUT_DOWN);
		yClicked = y;
	}
}
// Track movement of the cursor
void MouseMotion(int x, int y)
{
	if (leftClick)
	{
		theta = deg2rad((x - xClicked));
		RotateY();
		xClicked = x;
	}
	if (middleClick)
	{
		lookY -= y - yClicked;
		viewVector[1] -= y - yClicked;
		yClicked = y;
	}
}

// Ensure no distorting upon resizing the window
void Reshape (int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height); // Set our viewport to the size of our window  
	glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed  
	glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)  
	gluPerspective(60, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes  
	glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly  
}