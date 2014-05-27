// Do whateever you want with this source files. 
// Do not use 3d models in other application without writen permission.
//
//
// <author>Daniel Mikeš</author>
// <email>bublafus@gmail.com</email>
//
// Sorry - code is not structured well (you know how it works in school projects and submission deadlines)
//


#define USE_ANTTWEAKBAR

#include "../common/common.h"
#include "../common/models/elephant.h"
#include "../common/models/cube.h"

#include <iostream>
#include <vector>

#include "pgr2model.h"
#include "globalState.h"
#include "GUI.h"
#include "singleMesh.h"
#include "sceneSetup.h"
#include "shaderConfig.h"


using namespace glm;

// ### DEFINES ###
#define PI 3.1415926535897
#define DEG_TO_RAD PI/180.0


// ### GLOBAL CONSTANTS ###
const char* VS_FILE_NAME     = "../shaders/npr.vert";  // Vertex shader source file
const char* GS_FILE_NAME     = "../shaders/npr.geom";  // Geometry shader source file
const char* FS_FILE_NAME     = "../shaders/npr.frag";  // Fragment shader source file
const char* FIXES_FS_FILE    = "../shaders/fixed.frag";  // Fragment shader source file


// === Mouse ===
bool firstChangeAfterPress = false;
bool g_MouseRotationEnabled = false;

int press_screenX;
int press_screenY;
int lastScreenX;
int lastScreenY; 
#define MOUSE_SENCE_X 0.7
#define MOUSE_SENCE_Y 0.7

mat4 tempMatrix;


void printVec(glm::vec3 data){
	printf("(%f, %f, %f)\n",  data.x, data.y, data.z);
}

void printMatrix(glm::mat4 data){
	printf("mat4 xxx = mat4(\n	%.3f, %.3f, %.3f, %.3f,\n	%.3f, %.3f, %.3f, %.3f,\n	%.3f, %.3f, %.3f, %.3f,\n	%.3f, %.3f, %.3f, %.3f \n);\n",  
		data[0][0], data[0][1], data[0][2], data[0][3],
		data[1][0], data[1][1], data[1][2], data[1][3],
		data[2][0], data[2][1], data[2][2], data[2][3],
		data[3][0], data[3][1], data[3][2], data[3][3]);
}


void setupCamera(){
    MVmatrix = identityMatrix;

	MVmatrix = glm::rotate(MVmatrix, angleRotScene_X, X_AXIS);
	MVmatrix = glm::rotate(MVmatrix, angleRotScene_Y, Y_AXIS);
	MVmatrix = glm::translate(MVmatrix, cameraPosition);
}

void setupLightCamera(){
	g_LightViewMatrix = glm::lookAt(lightPosition, lightLookAt, Y_AXIS);	
	g_LightProjectionMatrix = glm::frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightProjNear, lightProjFar);
}


void drawScene(int floorShader, int skyboxShader, int modelShader){
	glEnableVertexAttribArray(gShaderPositionTag);
	glEnableVertexAttribArray(gShaderNormalTag);
	glEnableVertexAttribArray(gShaderTexCoord0Tag);

	
	switch (g_GeometryType){
		case SCENE : 
			drawScene_1(floorShader, skyboxShader, modelShader);
			break;
		case SCENE2 :	
			drawScene_2(modelShader, skyboxShader, modelShader); 
			//pgr2DrawElephant();
			break;
		case CUBE_GEOMETRY     :	pgr2DrawCube();     break;
	}

	glDisableVertexAttribArray(gShaderPositionTag);
	glDisableVertexAttribArray(gShaderNormalTag);
	glDisableVertexAttribArray(gShaderTexCoord0Tag);
}




bool skip1stPass = true;
bool skip2ndPass = true;

/**
CallBack display function
*/
void cbDisplay(){
	int floorShader;
	int skyboxShader;
	int modelShader;

	// Turn on programmable pipeline
    if(gUseShaders){
        glUseProgram(g_ProgramId);    // Active shader program
    }

	setupLightCamera();
	MVmatrix = g_LightViewMatrix; // MV matrix is updated in scene - global var :(

	glUniform3fv(glGetUniformLocation(g_ProgramId, "u_LightPos"), 1, &lightPosition[0]);
	glEnable(GL_DEPTH_TEST);
	// < 1. PASS : SHADOWS >
		switch(gCurEffect){
			case E_SHADOW_TOON:
			case E_NOISY_SHADOW:
				skip1stPass = false;
				break;
			default:
				skip1stPass = true; 
				break;
		}

		if(!skip1stPass){
			glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &g_LightViewMatrix[0][0]);	
			glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &g_LightProjectionMatrix[0][0]);	
		
			glBindFramebuffer(GL_FRAMEBUFFER, gShadowFBOId);
			GLenum buffer[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
			glDrawBuffers(2, buffer);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT); // !!
			 
				drawScene(FIXED_FS, FIXED_FS, FIXED_FS);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	// </ 1. PASS >

	
	
	// if before 1. shadow pass - must stored before using lightViewMatrix
	setupCamera();
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_TextureLOD"), gTextureLOD);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_RenderTexture"), 0); // unbind for 2nd pass
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &MVmatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ProjectionMatrix"), 1, GL_FALSE, &Pmatrix[0][0]);
	glUniform2fv(glGetUniformLocation(g_ProgramId, "u_InvScreenSize"), 1, &invScreenSize[0]);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // !!
	
	// < 2. PASS : PRE-RENDER >
		switch(gCurEffect){
			case E_EDGE:
				skyboxShader = DISCARD;
				modelShader  = TOON_FS;
				break;
			case E_EDGE_TOON:
				skyboxShader = DISCARD;
				modelShader  = TOON_FS;
				break;
			case E_NOISY:	
				skyboxShader = FIXED_FS;
				modelShader  = DIFFUSE_FS;
				break;
			case E_NOISY_SHADOW:
				skyboxShader = FIXED_FS;
				modelShader  = SHADOW_FS;
				break;
			case E_OIL:
				skyboxShader = FIXED_FS;
				modelShader  = DIFFUSE_FS;
				break;
			default: // E_FIXED, E_TOON, E_SHADOW_TOON
				skip2ndPass = true; 
				break;
		}

		if(!skip2ndPass){
			glBindFramebuffer(GL_FRAMEBUFFER, gSobelFBOId);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
				drawScene(modelShader, skyboxShader, modelShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		skip2ndPass = false;
	// </ 2. PASS >
	setupLightCamera();
	// < 3. PASS : FINAL SCENE>
		switch(gCurEffect){
			case E_FIXED : 
				skyboxShader = FIXED_FS;
				modelShader  = DIFFUSE_FS;
				break;
			case E_EDGE:
				skyboxShader = DISCARD;
				modelShader  = EDGE_FS | UNIF_COLOR_FS;
				break;
			case E_TOON:
				skyboxShader = FIXED_FS;
				modelShader  = TOON_FS;
				break;
			case E_EDGE_TOON:
				skyboxShader = FIXED_FS;
				modelShader  = TOON_FS | EDGE_FS;
				break;
			case E_SHADOW_TOON:
				skyboxShader = FIXED_FS;
				modelShader  = TOON_FS | SHADOW_FS;
				break;
			case E_NOISY_SHADOW:
			case E_NOISY :	
				skyboxShader = NOISY_FS;
				modelShader  = NOISY_FS;
				break;
			case E_HATCH:
				glClearColor(0.4f, 0.4f, 0.7f, 0);
				skyboxShader = DISCARD;
				modelShader  = HATCH_FS;
				break;
			case E_OIL:
				skyboxShader = OIL_FS;
				modelShader  = OIL_FS;
				break;
		}
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, gWireMode ? GL_LINE : GL_FILL);
		//gFaceCulling ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);	

		glUniform1i(glGetUniformLocation(g_ProgramId, "u_RenderTexture"), gFBPreRender_DiffuseColorTex.textureUnit);
		glUniform1i(glGetUniformLocation(g_ProgramId, "u_DepthTexture"), gFBShadow_DepthColorTex.textureUnit);
		glUniform1i(glGetUniformLocation(g_ProgramId, "u_NoiseTexture"), gNoiseTex_1.textureUnit);

		glUniform1i(glGetUniformLocation(g_ProgramId, "u_ToonShadeTexture"), gToonGreyTex.textureUnit);

		drawScene(modelShader, skyboxShader, modelShader);

		glClearColor(1.0f, 1.0f, 1.0f, 0.0);
	// </ 3. PASS >

	glPointSize(30);
	glBegin(GL_POINTS);
		glVertex3f(lightPosition.x, lightPosition.y, lightPosition.z);
	glEnd();
    // Turn off programmable pipeline
    glUseProgram(0);

	// these show texture examples for only with GL_TEXTURE0. It could be repaired but is only for debugging!
	glActiveTexture(GL_TEXTURE0);
	
	if(gDisplayBuffTextures){
		pgr2ShowTexture(gFBPreRender_DiffuseColorTex.texID, gWindowWidth - 200, gWindowHeight - 200, 200, 200);	
		pgr2ShowTexture(gFBShadow_DiffuseColorTex.texID, gWindowWidth - 200, gWindowHeight - 400, 200, 200);	
		
		//pgr2ShowTexture(gToonGreyTex.texID, gWindowWidth - 200, gWindowHeight - 600, 200, 200);
		pgr2ShowTexture(gFBShadow_DepthColorTex.texID, gWindowWidth - 200, gWindowHeight - 600, 200, 200);
		//pgr2ShowTexture(gNoiseTex_1.texID, gWindowWidth - 200, gWindowHeight - 600, 200, 200);
	}
}


void resetFBO_Sobel(int width, int height){
	if(gFBPreRender_DiffuseColorTex.texID) glDeleteTextures(1, &gFBPreRender_DiffuseColorTex.texID);
    if(gFrameBuffDepthTexId_1) glDeleteTextures(1, &gFrameBuffDepthTexId_1);
	if(gSobelFBOId) glDeleteFramebuffers(1, &gSobelFBOId);
	
	
	glActiveTexture(GL_TEXTURE0 + gFBPreRender_DiffuseColorTex.textureUnit);
	glGenTextures(1, &gFBPreRender_DiffuseColorTex.texID);
	glBindTexture(GL_TEXTURE_2D, gFBPreRender_DiffuseColorTex.texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//glTexImage2D(GL_TEXTURE_2D,0 ,GL_RGBA,g_FBODimension,g_FBODimension,0, GL_RGBA, GL_UNSIGNED_BYTE,NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	// init frame buffer depth texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &gFrameBuffDepthTexId_1);
	glBindTexture(GL_TEXTURE_2D, gFrameBuffDepthTexId_1);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// init render buffer object for depth buffer used in multirender sampling
	//GLuint rbo = 0; // there is no need to have global index (it works automatically)
	//glGenRenderbuffers(1, &rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Generate FBO
	glGenFramebuffers(1, &gSobelFBOId);
	assert(gSobelFBOId > 0);
	glBindFramebuffer(GL_FRAMEBUFFER, gSobelFBOId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFBPreRender_DiffuseColorTex.texID, 0);
		// use depth buffer as a texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, gFrameBuffDepthTexId_1, 0);
		// use depth buffer as a render buffer of frame buffer
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void resetFBO_Shadow(int width, int height){
	if (gFBShadow_DiffuseColorTex.texID) glDeleteTextures(1, &gFBShadow_DiffuseColorTex.texID);
    if (gFBShadow_DepthColorTex.texID)	glDeleteTextures(1, &gFBShadow_DepthColorTex.texID);
    if (gFrameBuffDepthTexId_2) glDeleteTextures(1, &gFrameBuffDepthTexId_2);
	if (gShadowFBOId)	glDeleteFramebuffers(1, &gShadowFBOId);


	// --- setting up texture ---
	glActiveTexture(GL_TEXTURE0 + gFBShadow_DiffuseColorTex.textureUnit);
	glGenTextures(1, &gFBShadow_DiffuseColorTex.texID);
	glBindTexture(GL_TEXTURE_2D, gFBShadow_DiffuseColorTex.texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glActiveTexture(GL_TEXTURE0 + gFBShadow_DepthColorTex.textureUnit);
	glGenTextures(1, &gFBShadow_DepthColorTex.texID);
	glBindTexture(GL_TEXTURE_2D, gFBShadow_DepthColorTex.texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	//gFBShadow_DepthColorTex
	// init frame buffer depth texture
	/*glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &gFrameBuffDepthTexId_2);
	glBindTexture(GL_TEXTURE_2D, gFrameBuffDepthTexId_2);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);*/

	// init render buffer object for depth buffer used in multirender sampling
	GLuint rbo = 0; // there is no need to have global index (it works automatically)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Create frame buffer for Shadows generation
	glGenFramebuffers(1, &gShadowFBOId);
	assert(gShadowFBOId > 0);
	glBindFramebuffer(GL_FRAMEBUFFER, gShadowFBOId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gFBShadow_DiffuseColorTex.texID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gFBShadow_DepthColorTex.texID, 0);
		// use depth buffer as a texture
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,  GL_TEXTURE_2D, gFrameBuffDepthTexId_2, 0);
		// use depth buffer as a render buffer of frame buffer
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void cbInitGL() {	
	// Init app GUI
    initGUI();

    // Set OpenGL state variables
    glClearColor(1.0f, 1.0f, 1.0f, 0.0);

	// === Textures ===
    // Load normalmap texture from raw-file
	//g_ToonGreyTexId = prg2Load8bitTextureFromRawFile(TOON_GREY_TEX_FILE);
	
	glActiveTexture(GL_TEXTURE0 + gGrassTex.textureUnit);
	gGrassTex.texID = prg2LoadRGBTextureFromRawFile(gGrassTex.fileName);

	glActiveTexture(GL_TEXTURE0 + gNoiseTex_1.textureUnit);
    gNoiseTex_1.texID = prg2LoadRGBTextureFromRawFile(gNoiseTex_1.fileName);

	glActiveTexture(GL_TEXTURE0 + gNoiseTex_2.textureUnit);
    gNoiseTex_2.texID = prg2LoadRGBTextureFromRawFile(gNoiseTex_2.fileName);

	glActiveTexture(GL_TEXTURE0 + gNoiseTex_3.textureUnit);
	gNoiseTex_3.texID = prg2LoadRGBTextureFromRawFile(gNoiseTex_3.fileName);


	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_down.textureUnit);
	gSkyboxTex_down.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_down.fileName);
	
	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_up.textureUnit);
	gSkyboxTex_up.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_up.fileName);

	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_front.textureUnit);
	gSkyboxTex_front.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_front.fileName);

	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_right.textureUnit);
	gSkyboxTex_right.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_right.fileName);

	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_back.textureUnit);
	gSkyboxTex_back.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_back.fileName);

	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_left.textureUnit);
	gSkyboxTex_left.texID = prg2LoadRGBTextureFromRawFile(gSkyboxTex_left.fileName);

	// loaded as a normal texture (texture is in that format)
	glActiveTexture(GL_TEXTURE0 + gToonGreyTex.textureUnit);
	gToonGreyTex.texID = prg2LoadRGBTextureFromRawFile(gToonGreyTex.fileName); 

	
	assert((gGrassTex.texID > 0));
	assert((gNoiseTex_1.texID > 0));
	assert((gSkyboxTex_down.texID > 0));
	assert((gSkyboxTex_up.texID > 0));
	assert((gSkyboxTex_front.texID > 0));
	assert((gSkyboxTex_right.texID > 0));
	assert((gSkyboxTex_back.texID > 0));
	assert((gSkyboxTex_left.texID > 0));
	assert((gToonGreyTex.texID > 0));

	

	glActiveTexture(GL_TEXTURE0 + gGrassTex.textureUnit);
    glBindTexture(GL_TEXTURE_2D, gGrassTex.texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // setup to mipmap filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glActiveTexture(GL_TEXTURE0 + gToonGreyTex.textureUnit);
    glBindTexture(GL_TEXTURE_2D, gToonGreyTex.texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // not necesarry


	// === FBOs ===
	// already in window resize callback function
	//resetFBO_Shadow(gWindowWidth, gWindowHeight);
	//resetFBO_Sobel(gWindowWidth, gWindowHeight);

	// Check FBO status and OGL error state
	//GLenum error  = glGetError();
	//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);


	// === Scene init ===
	//prepareElephant();
	initScene();

	// Compile shaders
    cbCompileShaderProgram(NULL);
}


void TW_CALL cbCompileShaderProgram(void *clientData)
{
    // Delete shader program if exists
    if (g_ProgramId){
        glDeleteProgram(g_ProgramId);
    }

    // Create new shader program object
    g_ProgramId = glCreateProgram();
	

    if (gUseVertexShader){
        // Create shader objects for vertex shader
        GLuint id = pgr2CreateShaderFromFile(GL_VERTEX_SHADER, VS_FILE_NAME);
        glAttachShader(g_ProgramId, id);
        glDeleteShader(id);
    }
    if (gUseGeometryShader){
        // Create shader objects for geometry shader
        GLuint id = pgr2CreateShaderFromFile(GL_GEOMETRY_SHADER, GS_FILE_NAME);
        glAttachShader(g_ProgramId, id);
        glDeleteShader(id);
        //glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_VERTICES_OUT_EXT, 6);
        //glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
        //glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
		//glProgramParameteriEXT(g_ProgramId, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLES_ADJACENCY);
    }
    if(gUseFragmentShader){
        // Create shader objects for fragment shader
		GLuint id;
		if(gUseFixedFragmentShader){
			id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FIXES_FS_FILE);
		}else{
			id = pgr2CreateShaderFromFile(GL_FRAGMENT_SHADER, FS_FILE_NAME);
		}
        glAttachShader(g_ProgramId, id);
        glDeleteShader(id);
	}

    // Link shader program
    glLinkProgram(g_ProgramId);
    if (!pgr2CheckProgramLinkStatus(g_ProgramId)){
        pgr2CheckProgramInfoLog(g_ProgramId);
        printf("Shader program creation failed.\n\n");
        glDeleteProgram(g_ProgramId);
        g_ProgramId  = 0;
        gUseShaders = false;
        return;
    }
    else{
        printf("Shader program compiled successfully.\n\n");
    }
}



void cbWindowSizeChanged(int width, int height) {
    glViewport(0, 0, width, height);
   
	Pmatrix = glm::perspective(55.0f, GLfloat(width)/height, 0.1f, 1000.0f);

    gWindowWidth  = width;
    gWindowHeight = height;

	resetFBO_Sobel(width, height);
	resetFBO_Shadow(width, height);

	invScreenSize[0] = 1.0f/width;
	invScreenSize[1] = 1.0f/height;	

}


void cbKeyboardChanged(int key, int action){
    switch (key) {
		case 's' : // backwards
			cameraPosition -= cameraForward;
			break;
		case 'w' : // forwards
			cameraPosition += cameraForward;
			break;
		case 'q' : cameraPosition.y += 0.5f; break;
		case 'e' : cameraPosition.y -= 0.5f; break;
		case 'd' :  // right
			cameraPosition += cameraRight;
			break;  
		case 'a' : // left
			cameraPosition -= cameraRight;
			break;
		case 'y': // change current obj
			/*--curObj;
			if(curObj < 0)
				curObj = models.size() - 1;*/
			break;
		case 'x': // change current obj
			//curObj = (++curObj) % models.size();
			break;
		case 'b': // place
			//curObjMove = !curObjMove;
			printVec(cameraPosition);
			printf("light rotation: x:%f y:%f | near: %f far: %f | \n", angleRotLight_X , angleRotLight_Y, lightProjNear, lightProjFar);
			printVec(lightPosition);
			printVec(lightLookAt);
			//printVec(curObjPos);
			break;
		case 'p': // rotate obj
			/*++curObjRotateAngle;
			if(curObjRotateAngle > 360)
				curObjRotateAngle -= 360;
			if(curObjRotateAngle < 0)
				curObjRotateAngle += 360;
			break;*/
			++angleRotLight_X;
			if(angleRotLight_X > 360)
				angleRotLight_X -= 360;
			break;
		case 'o': // rotate obj
			/*--curObjRotateAngle;
			if(curObjRotateAngle > 360)
				curObjRotateAngle -= 360;
			if(curObjRotateAngle < 0)
				curObjRotateAngle += 360;*/
			++angleRotLight_Y;
			if(angleRotLight_Y > 360)
				angleRotLight_Y -= 360;
			break;
		case 'i': // rotate obj
			/*curObjRotateAngle += 90;
			if(curObjRotateAngle > 360)
				curObjRotateAngle -= 360;
			if(curObjRotateAngle < 0)
				curObjRotateAngle += 360;*/
			
			//lightPosition.x += 0.5;
			lightPosition.z += 0.2;
			break;
		case 'u': // rotate obj
			/*curObjRotateAngle -= 90;
			if(curObjRotateAngle > 360)
				curObjRotateAngle -= 360;
			if(curObjRotateAngle < 0)
				curObjRotateAngle += 360;*/

			/*lightPosition.x -= 0.5f;
			lightPosition.z -= 0.5f;*/
			break;
		case 'l': 
			//curObjScale += 0.1f;
			lightPosition.x += 0.2;
			break;
		case 'k': 
			//curObjScale -= 0.1f;
			lightPosition.z -= 0.2;
			break;
		case 'j':
			lightPosition.x -= 0.2;
			break;
		case 'm':
			lightPosition.y += 0.1;
			break;
		case 'n':
			lightPosition.y -= 0.1;
			break;
		case 'f':
			//cameraObjOffset.x -= 0.5f;
			//cameraObjOffset.z -= 0.5f;
			lightProjFar += 10.0f;
			break;
		case 'h':
			//cameraObjOffset.x += 0.5f;
			//cameraObjOffset.z += 0.5f;
			lightProjFar -= 10.0f;
			break;
		case 'g':
			//cameraObjOffset.y -= 0.05f;
			lightProjNear += 0.01f;
			break;
		case 't':
			lightProjNear -= 0.01f;
			//cameraObjOffset.y += 0.05f;
			break;
		case 'v': 
			/*mat4 tmp; // v must be last
			tmp = glm::translate(tmp, curObjPos);
			tmp = glm::rotate(tmp, curObjRotateAngle, yAxis);
			tmp = glm::scale(tmp, vec3(curObjScale, curObjScale, curObjScale));
			printf("//-- matrix | object #%d : ", curObj, models[curObj]->modelName2);
			std::cout << models[curObj]->modelName  << " --" << std::endl;
			printMatrix(tmp);*/

			gLightAnchor = (gLightAnchor + 1) % 2;
			if(gLightAnchor == 0){
				//light rotation: x:212.000000 y:306.000000 | near: 1.000000 far: 1000.000000 |
				//(30.0, 60.0, 30.0)
				//(46.2, 55.000000, 42.200001)
				lightProjNear = 1.0;
				lightProjFar  = 1000.0;
				lightPosition = vec3(30.0, 60.0, 30.0);
				lightLookAt   = vec3(46.2, 55.0, 42.2);
				vec3(46.2, 55.0, 42.2);
			}else{
				lightProjNear = 0.98;
				lightProjFar  = 2480.0;
				lightLookAt   = vec3(60.0,1.0,55.0);
				lightPosition = vec3(55.5, 7.5, 47.2);
			}
			break;
    }
}


	void GLFWCALL cbMouseButtonChanged(int button, int action){
		//if(action == GLFW_PRESS){
		//	printf("press mouse\n");	
		//}

		if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
			firstChangeAfterPress = true;
			g_MouseRotationEnabled = true;
		} else{
			firstChangeAfterPress = false;
			g_MouseRotationEnabled = false;
		}
		
	}

void cbMousePositionChanged(int x, int y){
		
	if(firstChangeAfterPress){
		// I dont know x and y coordinates when performing click (callback mousebutton changed)
		firstChangeAfterPress = false;
		lastScreenX = x;
		lastScreenY = y;
	}

	if (g_MouseRotationEnabled){
		// Delta X in screen coordinates changes Scene rotation in Y axis. And vice versa.
		angleRotScene_X += MOUSE_SENCE_X*(y - lastScreenY);
		if(angleRotScene_X < -90.0f) angleRotScene_X = -90.0f;
		if(angleRotScene_X >  90.0f) angleRotScene_X = 90.0f;

		angleRotScene_Y +=  MOUSE_SENCE_Y*(x - lastScreenX);
		if(angleRotScene_Y > 360.0f) angleRotScene_Y -= 360.0f;
		if(angleRotScene_Y < 0.0f)   angleRotScene_Y += 360.0f;

		//float cosA = cos(DEG_TO_RAD * angleRotScene_X);
		//float sinA = sin(DEG_TO_RAD * angleRotScene_X);
		float cosB = (float)cosf(DEG_TO_RAD * angleRotScene_Y);
		float sinB = (float)sinf(DEG_TO_RAD * angleRotScene_Y);

		// Rotation above Y axis
		cameraForward.x = cosB * cameraForwardDefault.x - sinB * cameraForwardDefault.z;
		//cameraForward.y = -cameraForwardDefault.y; // cameraForwardDefault.y = 0
		cameraForward.z = sinB * cameraForwardDefault.x + cosB * cameraForwardDefault.z;

		// normalize
		//cameraForward.normalize();
		glm::normalize(cameraForward);

		// both vectors are already normalized
		cameraRight = glm::cross(cameraForward, cameraUp);
		//cameraRight = crossProduct(&cameraForward, &cameraUp);

		// update screen coordinates
		lastScreenX = x;
		lastScreenY = y;
	}
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    common_main(gWindowWidth, gWindowHeight,
                       "[PGR2]",
                       cbInitGL,              // init GL callback function
                       cbDisplay,             // display callback function
                       cbWindowSizeChanged,   // window resize callback function
                       cbKeyboardChanged,     // keyboard callback function
                       cbMouseButtonChanged,  // mouse button callback function
                       cbMousePositionChanged // mouse motion callback function
                       );

	// delete
	return 0;
}
