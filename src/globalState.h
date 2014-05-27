#ifndef __GLOBAL_STATE_H__
#define __GLOBAL_STATE_H__


using namespace glm;

// ### GLOBAL DEFINES ###
#define OASIS_PATH(modelName)	 "../models/DesertOasis/"modelName".pgr2"
#define MEDIEVAL_PATH(modelName) "../models/MedievalVillage/"modelName".pgr2"
#define CASTLE_PATH(modelName)   "../models/Walls&Towers/"modelName".pgr2"

// Shader defines should be the same!
#define DISCARD -1

#define FIXED_FS 0
#define DIFFUSE_FS 1
#define UNIF_COLOR_FS 2
#define EDGE_FS 4
#define TOON_FS 8
#define SHADOW_FS 16
#define HATCH_FS 32
#define OIL_FS 64
#define NOISY_FS 128

// ### GLOBAL VARIABLES ### 
// === Window ===
GLint	gWindowWidth  = 800;    // Window width
GLint   gWindowHeight = 600;    // Window height

// GLSL variables
GLuint g_ProgramId = 0;

bool gWireMode		= false;  // Wire mode enabled/disabled
bool gFaceCulling	= true;  // Face culling enabled/disabled

bool gUseShaders        = true;  // Programmable pipeline on/off
bool gUseVertexShader   = true;  // Use vertex shader
bool gUseGeometryShader = false;  // Use geometry shader
bool gUseFragmentShader = true;  // Use fragment shader
bool gUseFixedFragmentShader = false;
bool gDisplayBuffTextures = true;

// Objects construction and positioning
//vec3	curObjPos;
//GLfloat	curObjRotateAngle	= 0.0f;
//int		curObj				= 0;
//bool	curObjMove			= false;
//GLfloat	curObjScale			= 1.0f;
//vec3 cameraObjOffset = vec3(40.0, 0.0, 40.0);

// GEOMETRY
enum eGeometry{	
    SCENE2 = 0, 
    CUBE_GEOMETRY,
	SCENE,
    NUM_GEOMETRY_TYPES
};
int g_GeometryType = SCENE; // Geometry type

enum eEffect{	
	E_FIXED = 0,
    E_EDGE, 
    E_TOON,
	E_EDGE_TOON,
	E_SHADOW_TOON,
	E_HATCH,
	E_NOISY,
	E_NOISY_SHADOW,
	E_OIL,
    NUM_EFFECTS
};

int gCurEffect = E_FIXED;


// === Camera ====
GLfloat angleRotScene_X;
GLfloat angleRotScene_Y;

const vec3 cameraForwardDefault = vec3(0.0f, 0.0f, 1.0f);
const vec3 cameraUp				= vec3(0.0f, 1.0f, 0.0f);
vec3 cameraForward (cameraForwardDefault); // copy
vec3 cameraRight				= vec3(-1.0f, 0.0f, 0.0f); // cross product of cameraForward anc cameraUp
vec3 cameraPosition				= vec3(-60.0f, -2.5f, -60.0f);


// === Light ===
//vec3 lightPos = vec3(0.0f, 0.5f, 12.0f);
vec3 lightPosition = vec3(30.0, 60.0, 30.0);
GLfloat angleRotLight_X = 212.0f;
GLfloat angleRotLight_Y = 306.0f;


vec3 lightLookAt = vec3(46.2, 55.0, 42.2);
// === Shadows ===
mat4 shadowTransform;
mat4 g_LightViewMatrix;
mat4 g_LightProjectionMatrix;

float lightProjNear = 1.0f;
float lightProjFar = 1000.0f;
int gLightAnchor = 0;

glm::mat4 shadowScale = glm::mat4(
	0.5f,0.0f,0.0f,0.0f,
	0.0f,0.5f,0.0f,0.0f,
	0.0f,0.0f,0.5f,0.0f,
	0.5f,0.5f,0.5f,1.0f);

// Textures

struct textureClient{
	GLuint texID;
	int textureUnit;
	const char* fileName;

	textureClient(int texUnit, const char* file):textureUnit(texUnit), fileName(file), texID(0)
	{ }
	textureClient(int texUnit):textureUnit(texUnit), texID(0)
	{ }
};

// TEXTURE_0 unit is often used (is deafult) and is overBindend
textureClient gFBShadow_DiffuseColorTex = textureClient(1);
textureClient gFBShadow_DepthColorTex = textureClient(2);
// In the end i have used RBO
//textureClient gFBShadow_DepthComponent = textureClient(0); // Tex unit is not used -> = 0.

textureClient gFBPreRender_DiffuseColorTex = textureClient(3);
// In the end i have used RBO
//textureClient gFBPreRender_DepthComponent = textureClient(0); // Tex unit is not used -> = 0.

textureClient gToonGreyTex = textureClient(5, "../textures/ToonGrey2D_2.raw");

textureClient gGrassTex = textureClient(7, "../textures/GrassGreenTexture0003_1024.raw");

textureClient gNoiseTex_1 = textureClient(10, "../textures/noise1_256.raw");
textureClient gNoiseTex_2 = textureClient(11, "../textures/noise_cloud1_256.raw");
textureClient gNoiseTex_3 = textureClient(12, "../textures/noise_cloud2_256.raw");

textureClient gSkyboxTex_down		= textureClient(15, "../textures/skybox/calm_down.raw");
textureClient gSkyboxTex_up			= textureClient(15, "../textures/skybox/calm_up.raw");
textureClient gSkyboxTex_front		= textureClient(15, "../textures/skybox/calm_north.raw");
textureClient gSkyboxTex_right		= textureClient(15, "../textures/skybox/calm_east.raw");
textureClient gSkyboxTex_back		= textureClient(15, "../textures/skybox/calm_south.raw");
textureClient gSkyboxTex_left		= textureClient(15, "../textures/skybox/calm_west.raw");

const char*	  TOON_GREY_TEX_FILE = "../textures/pokus1D.raw"; // OMG JA TU TEXTURU CETL ZE SPATNY SLOZKY!!

GLuint  gSobelFBOId  = 0;
//GLuint  gFrameBuffColorTexId_1 = 0;
GLuint	gFrameBuffDepthTexId_1 = 0;

GLuint  gShadowFBOId  = 0;
//GLuint  gFrameBuffColorTexId_2 = 0;
GLuint	gFrameBuffDepthTexId_2 = 0;

// Not used in the end!
/*GLint uniform_LightPos			= -1;
GLint uniform_Mode				= -1;
GLint uniform_Texture			= -1;
GLint uniform_RenderTexture		= -1;
GLint uniform_DepthTexture		= -1;
GLint uniform_ToonShadeTexture	= -1;
GLint uniform_ModelViewMatrix	= -1;*/

const vec3 X_AXIS = vec3(1.0f, 0.0f, 0.0f);
const vec3 Y_AXIS = vec3(0.0f, 1.0f, 0.0f);
const vec3 Z_AXIS = vec3(0.0f, 0.0f, 1.0f);

const vec3 WHITE_COLOR = vec3(1.0f, 1.0f, 1.0f);


const mat4 identityMatrix = mat4();
//	1.0, 0.0, 0.0, 0.0,
//	0.0, 1.0, 0.0, 0.0,
//	0.0, 0.0, 1.0, 0.0,
//	0.0, 0.0, 0.0, 1.0
//);
mat4 MVcameraMatrix;
mat4 MVmatrix;
mat4 Pmatrix;

GLfloat invScreenSize[2];
GLuint gTextureLOD = 3;

// ### TOOLS ###
void commitMVMatrix(){
	// TODO: update when shaders are recompiled!
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &MVmatrix[0][0]);
	//glUniformMatrix4fv(uniform_ModelViewMatrix, 1, GL_FALSE, &MVmatrix[0][0]);
}

#endif