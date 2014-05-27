#ifndef __SCENE_SETUP_H__
#define __SCENE_SETUP_H__


using namespace glm;

#include "singleMesh.h"
#include "globalState.h"

struct transformNode{
	PGR2Model* model;
	mat4 globalTransform; // global translate from the camera matrix.
	transformNode(PGR2Model* _model, mat4 transform): model(_model), globalTransform(transform)
	{ };
};

std::vector<PGR2Model*> models;
std::vector<PGR2Model*> modelsCastle;
std::vector<PGR2Model*> modelsVillage;
std::vector<PGR2Model*> modelsDesert;
std::vector<transformNode*> transforms;
int nTrans = 0;

// === floor mesh ===
#ifndef SS_FLOOR_SETUP
SingleMesh floorMesh;
vec3 floorScale = vec3(15.0, 0.0, 15.0);

GLfloat verticesFloor[4*3] = {
		0.0,	0.0,	0.0,
		0.0,	0.0,	150.0,
		150.0,	0.0,	0.0,
		150.0,	0.0,	150.0 };
GLfloat normalsFloor[4*3] = {
		0.0,	1.0,	0.0,
		0.0,	1.0,	0.0,
		0.0,	1.0,	0.0,
		0.0,	1.0,	0.0 
};
GLfloat texCoordsFloor[4*2] = {
		0.0,	0.0,
		0.0,	7.0,
		7.0,	0.0,
		7.0,	7.0 };

//GLushort indicesFloor[6] = { 0, 2, 1, 1, 2, 3 }; // front is the bottom side
GLushort indicesFloor[6] = { 0, 1, 2, 1, 3, 2 }; // front is the top side
#endif

#ifndef SS_SKYBOX_SETUP
#define WORLD_SIZE	200.0f
#define W_OFF	100.0f
GLfloat y = -10.0;

SingleMesh skybox;

GLfloat texCoordsSkybox[2*4*6] = {
	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,
	
	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,	

	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,
		
	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,

	1.0, 1.0,
	0.0, 1.0,
	0.0, 0.0,
	1.0, 0.0,

	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
	0.0, 1.0
};

GLfloat normalsSkybox[3*4*6] = {
	-1.0,	0.0,	0.0,	
	-1.0,	0.0,	0.0,
	-1.0,	0.0,	0.0,
	-1.0,	0.0,	0.0,	

	1.0,	0.0,	0.0,
	1.0,	0.0,	0.0,
	1.0,	0.0,	0.0,
	1.0,	0.0,	0.0,

	0.0,	0.0,	-1.0,	
	0.0,	0.0,	-1.0,	
	0.0,	0.0,	-1.0,	
	0.0,	0.0,	-1.0,	

	0.0,	0.0,	1.0,
	0.0,	0.0,	1.0,
	0.0,	0.0,	1.0,
	0.0,	0.0,	1.0,

	0.0,	-1.0,	0.0,
	0.0,	-1.0,	0.0,
	0.0,	-1.0,	0.0,
	0.0,	-1.0,	0.0,

	0.0,	1.0,	0.0,
	0.0,	1.0,	0.0,
	0.0,	1.0,	0.0,
	0.0,	1.0,	0.0,

};

GLfloat verticesSkybox[3*4*6] = {
	WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE, WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	WORLD_SIZE, WORLD_SIZE+W_OFF,

	-WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,

	-WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE,	WORLD_SIZE+W_OFF,

	WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	WORLD_SIZE, WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,

	WORLD_SIZE+W_OFF,	WORLD_SIZE, WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE, WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	WORLD_SIZE, -WORLD_SIZE+W_OFF,

	-WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	y,			WORLD_SIZE+W_OFF,
	WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
	-WORLD_SIZE+W_OFF,	y,			-WORLD_SIZE+W_OFF,
};

#endif


void configureFloor(){
	floorMesh.nIndices = 6;
	floorMesh.nVertices = 4;

	floorMesh.indices  = indicesFloor;
	floorMesh.vertices = verticesFloor;
	floorMesh.normals  = normalsFloor;
	floorMesh.texCoords = texCoordsFloor;

	floorMesh.init_VBOindex();
}


void configureSkybox(){
	skybox.nVertices = 6*4;

	skybox.vertices  = verticesSkybox;
	skybox.normals   = normalsSkybox;
 	skybox.texCoords = texCoordsSkybox;

	skybox.init_VBOnoIndex();

}


PGR2Model* TW_CALL loadNewModel(const std::string fileName){
    PGR2Model* modelPointer = NULL;
	if (!fileName.empty()){
        modelPointer = PGR2Model::loadFromFile(fileName.c_str());
    }
	modelPointer->initVBO();
	return modelPointer;
} 


// === Transformation matrices ===
#ifndef SS_MATRICES
// ### Wall & Towes - Castle ###
// C as Castle
// --- C 1 ---
mat4 objC01matrix = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        129.609, 9.750, 112.50, 1.000
);
mat4 objC01matrix2 = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        129.648, 9.750, 45.50, 1.000
);
// --- C 3 ---
mat4 objC03matrix = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        130.636, 7.050, 112.5, 1.000
);
mat4 objC03matrix2 = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        130.557, 7.050, 45.50, 1.000
);
mat4 objC03matrix3 = mat4(
        0.000, 0.000, 2.600, 0.000,
        0.000, 2.600, 0.000, 0.000,
        -2.600, 0.000, 0.000, 0.000,
        129.851, 17.800, 15.639, 1.000
);
// --- C 8 ---
mat4 objC08matrix = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        135.207, 0.000, 78.0, 1.000
);
// --- C 9 ---
mat4 objC09matrix = mat4(
        1.000, 0.000, 0.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        0.000, 0.000, 1.000, 0.000,
        73.477, 0.000, 139.968, 1.000
);
// --- C 14 ---
mat4 objC14matrix = mat4(
        -0.000, 0.000, -1.300, 0.000,
        0.000, 1.300, 0.000, 0.000,
        1.300, 0.000, -0.000, 0.000,
        135.935, -6.600, 139.947, 1.000
);
mat4 objC14matrix2 = mat4(
        1.300, 0.000, 0.000, 0.000,
        0.000, 1.300, 0.000, 0.000,
        0.000, 0.000, 1.300, 0.000,
        135.935, -6.600, 15.521, 1.000
);
// --- C 16 ---
mat4 objC16matrix = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        135.342, 0.000, 112.5, 1.000
);
mat4 objC16matrix2 = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        135.342, 0.000, 45.5, 1.000
);
// --- C 17 ---
mat4 objC17matrix = mat4(
        1.500, 0.000, 0.000, 0.000,
        0.000, 1.500, 0.000, 0.000,
        0.000, 0.000, 1.500, 0.000,
        11.086, -8.050, 140.0, 1.000
);
mat4 objC17matrix2 = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        7.600, 0.000, 74.459, 1.000
);
// --- C 20 ---
mat4 objC20matrix = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 96.370, 1.000
);
mat4 objC20matrix2 = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 111.87, 1.000
);
mat4 objC20matrix3 = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 127.37, 1.000
);
mat4 objC20matrix4 = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 59.60, 1.000
);
mat4 objC20matrix5 = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 44.10, 1.000
);
mat4 objC20matrix6 = mat4(
        0.000, 0.000, 0.800, 0.000,
        0.000, 0.800, 0.000, 0.000,
        -0.800, 0.000, 0.000, 0.000,
        136.0, 0.000, 28.60, 1.000
);
mat4 objC20matrix7 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        122.40, 0.000, 139.50, 1.000
);
mat4 objC20matrix8 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        106.90, 0.000, 139.50, 1.000
);
mat4 objC20matrix9 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        91.40, 0.000, 139.50, 1.000
);
mat4 objC20matrix10 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        54.90, 0.000, 139.50, 1.000
);
mat4 objC20matrix11 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        39.40, 0.000, 139.50, 1.000
);
mat4 objC20matrix12 = mat4(
        0.800, 0.000, 0.000, 0.000,
        0.000, 0.800, 0.000, 0.000,
        0.000, 0.000, 0.800, 0.000,
        23.90, 0.000, 139.50, 1.000
);
// --- C 22 ---
mat4 objC22matrix = mat4(
        0.000, 0.000, 0.900, 0.000,
        0.000, 0.900, 0.000, 0.000,
        -0.900, 0.000, 0.000, 0.000,
        5.639, 0.000, 128.50, 1.000
);
mat4 objC22matrix2 = mat4(
        0.000, 0.000, 0.900, 0.000,
        0.000, 0.900, 0.000, 0.000,
        -0.900, 0.000, 0.000, 0.000,
        5.639, 0.000, 87.50, 1.000
);
// --- C 23 ---
mat4 objC23matrix = mat4(
         0.000, 0.000, 0.900, 0.000,
        0.000, 0.900, 0.000, 0.000,
        -0.900, 0.000, 0.000, 0.000,
        5.639, 0.000, 108.00, 1.000
);

// ### VILLAGE - Medieval ###
// V as Village
// --- V 4 ---
mat4 objV04matrix = mat4(
        1.034, 0.000, 0.376, 0.000,
        0.000, 1.100, 0.000, 0.000,
        -0.376, 0.000, 1.034, 0.000,
        38.458, 0.000, 35.027, 1.000
);
// --- V 6 ---
mat4 objV06matrix = mat4(
        -1.212, 0.000, 0.700, 0.000,
        0.000, 1.400, 0.000, 0.000,
        -0.700, 0.000, -1.212, 0.000,
        57.741, 0.000, 80.355, 1.000
);
// --- V 7 ---
mat4 objV07matrix = mat4(
        -0.547, 0.000, 1.504, 0.000,
        0.000, 1.600, 0.000, 0.000,
        -1.504, 0.000, -0.547, 0.000,
        58.778, 0.000, 78.507, 1.000
);
mat4 objV07matrix2 = mat4(
        -1.034, 0.000, -0.376, 0.000,
        0.000, 1.100, 0.000, 0.000,
        0.376, 0.000, -1.034, 0.000,
        60.648, 0.000, 78.075, 1.000
);
// --- V 8 ---
mat4 objV08matrix = mat4(
        -1.212, 0.000, 0.700, 0.000,
        0.000, 1.400, 0.000, 0.000,
        -0.700, 0.000, -1.212, 0.000,
        83.276, 0.000, 57.475, 1.000
);
mat4 objV08matrix2 = mat4(
        -0.059, 0.000, -3.399, 0.000,
        0.000, 3.400, 0.000, 0.000,
        3.399, 0.000, -0.059, 0.000,
        68.964, 0.000, 55.839, 1.000
);
mat4 objV08matrix3 = mat4(
        1.600, 0.000, 0.000, 0.000,
        0.000, 1.600, 0.000, 0.000,
        0.000, 0.000, 1.600, 0.000,
        55.215, 0.000, 119.685, 1.000
);
mat4 objV08matrix4 = mat4(
        1.515, 0.000, 0.772, 0.000,
        0.000, 1.700, 0.000, 0.000,
        -0.772, 0.000, 1.515, 0.000,
        125.353, 0.000, 18.013, 1.000
);
mat4 objV08matrix5 = mat4(
        -1.117, 0.000, 1.537, 0.000,
        0.000, 1.900, 0.000, 0.000,
        -1.537, 0.000, -1.117, 0.000,
        119.307, 0.000, 18.570, 1.000
);
// --- V 10 ---
mat4 objV10matrix = mat4(
        1.400, 0.000, 0.000, 0.000,
        0.000, 1.400, 0.000, 0.000,
        0.000, 0.000, 1.400, 0.000,
        56.540, 0.000, 114.0, 1.000
);
mat4 objV10matrix2 = mat4(
        1.300, 0.000, 0.000, 0.000,
        0.000, 1.300, 0.000, 0.000,
        0.000, 0.000, 1.300, 0.000,
        97.209, 0.050, 29.784, 1.000
);
// --- V 13 ---
mat4 objV13matrix = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        126.043, -0.000, 9.0, 1.000
);
mat4 objV13matrix2 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
		107.00, 0.000, 9.0, 1.000
);
mat4 objV13matrix3 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        102.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix4 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        97.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix5 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        92.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix6 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        87.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix7 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        82.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix8 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        77.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix9 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        72.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix10 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        67.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix11 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        62.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix12 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        57.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix13 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        52.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix14 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        47.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix15 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        42.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix16 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        37.0, 0.100, 9.0, 1.000
);
mat4 objV13matrix17 = mat4(
        0.600, 0.000, 0.000, 0.000,
        0.000, 0.600, 0.000, 0.000,
        0.000, 0.000, 0.600, 0.000,
        32.0, 0.100, 9.0, 1.000
);
// --- V 17 ---
mat4 objV17matrix = mat4(
        -1.071, 0.000, 1.189, 0.000,
        0.000, 1.600, 0.000, 0.000,
        -1.189, 0.000, -1.071, 0.000,
        59.712, 0.000, 55.463, 1.000
);
mat4 objV17matrix2 = mat4(
        -0.764, 0.000, 1.052, 0.000,
        0.000, 1.300, 0.000, 0.000,
        -1.052, 0.000, -0.764, 0.000,
        121.582, 0.000, 22.757, 1.000
);
// --- V 20 ---
mat4 objV20matrix = mat4(
        0.000, 0.000, 1.000, 0.000,
        0.000, 1.000, 0.000, 0.000,
        -1.000, 0.000, 0.000, 0.000,
        55.0, -0.000, 92.0, 1.000
);
// --- V 22 ---
mat4 objV22matrix = mat4(
        -0.000, 0.000, -1.200, 0.000,
        0.000, 1.200, 0.000, 0.000,
        1.200, 0.000, -0.000, 0.000,
        58.682, 0.000, 72.00, 1.000
);
// --- V 25 ---
mat4 objV25matrix = mat4(
        1.300, 0.000, 0.000, 0.000,
        0.000, 1.300, 0.000, 0.000,
        0.000, 0.000, 1.300, 0.000,
        76.480, 0.050, 54.726, 1.000
);
// --- V 28 ---
mat4 objV28matrix = mat4(
        0.000, 0.000, 1.200, 0.000,
        0.000, 1.200, 0.000, 0.000,
        -1.200, 0.000, 0.000, 0.000,
        91.028, 0.000, 119.185, 1.000
);
mat4 objV28matrix2 = mat4(
        -0.000, 0.000, -1.300, 0.000,
        0.000, 1.300, 0.000, 0.000,
        1.300, 0.000, -0.000, 0.000,
        97.236, 0.000, 54.210, 1.000
);
mat4 objV28matrix3 = mat4(
        -1.400, 0.000, 0.000, 0.000,
        0.000, 1.400, 0.000, 0.000,
        -0.000, 0.000, -1.400, 0.000,
        10.804, 0.000, 57.731, 1.000
);
// --- V 31 ---
mat4 objV31matrix = mat4(
        1.645, 0.000, -0.950, 0.000,
        0.000, 1.900, 0.000, 0.000,
        0.950, 0.000, 1.645, 0.000,
        59.859, 0.000, 119.852, 1.000
);
mat4 objV31matrix2 = mat4(
        -1.462, 0.000, 0.337, 0.000,
        0.000, 1.500, 0.000, 0.000,
        -0.337, 0.000, -1.462, 0.000,
        55.154, 0.000, 121.252, 1.000
);
mat4 objV31matrix3 = mat4(
        0.337, 0.000, 1.462, 0.000,
        0.000, 1.500, 0.000, 0.000,
        -1.462, 0.000, 0.337, 0.000,
        60.533, 1.050, 54.413, 1.000
);
mat4 objV31matrix4 = mat4(
        1.198, 0.000, -0.063, 0.000,
        0.000, 1.200, 0.000, 0.000,
        0.063, 0.000, 1.198, 0.000,
        59.709, 0.900, 54.407, 1.000
);
// --- V 33 ---
mat4 objV33matrix = mat4(
		1.700, 0.000, 0.000, 0.000,
		0.000, 1.700, 0.000, 0.000,
		0.000, 0.000, 1.700, 0.000,
		119.365, 0.100, 9.755, 1.000
);
mat4 objV33matrix2 = mat4(
        -0.000, 0.000, -1.400, 0.000,
        0.000, 1.400, 0.000, 0.000,
        1.400, 0.000, -0.000, 0.000,
        27.349, 0.000, 9.117, 1.000
);

/// ### Desert Oasis ###
// D as Desert
// --- D 0 ---
mat4 objD00matrix = mat4(
        -0.000, 0.000, -3.100, 0.000,
        0.000, 3.100, 0.000, 0.000,
        3.100, 0.000, -0.000, 0.000,
        -1.573, 0.000, 18.463, 1.000
);
// --- D 3 ---
mat4 objD03matrix = mat4(
        3.800, 0.000, 0.000, 0.000,
        0.000, 3.800, 0.000, 0.000,
        0.000, 0.000, 3.800, 0.000,
        15.942, 0.000, 9.746, 1.000
);
mat4 objD03matrix2 = mat4(
        -0.000, 0.000, -4.600, 0.000,
        0.000, 4.600, 0.000, 0.000,
        4.600, 0.000, -0.000, 0.000,
        20.354, 0.000, 118.758, 1.000
);
mat4 objD03matrix3 = mat4(
        -0.000, 0.000, -4.600, 0.000,
        0.000, 4.600, 0.000, 0.000,
        4.600, 0.000, -0.000, 0.000,
        95.298, 0.000, -1.755, 1.000
);
mat4 objD03matrix4 = mat4(
        -0.000, 0.000, -4.600, 0.000,
        0.000, 4.600, 0.000, 0.000,
        4.600, 0.000, -0.000, 0.000,
        -3.429, 0.000, 97.964, 1.000
);
// --- D 4 ---
mat4 objD04matrix = mat4(
        -0.000, 0.000, -4.300, 0.000,
        0.000, 4.300, 0.000, 0.000,
        4.300, 0.000, -0.000, 0.000,
        8.401, 0.000, 35.514, 1.000
);
// --- D 34 ---
mat4 objD34matrix = mat4(
        1.100, 0.000, 0.000, 0.000,
        0.000, 1.100, 0.000, 0.000,
        0.000, 0.000, 1.100, 0.000,
        92.366, 0.000, 89.125, 1.000
);

#endif


void initScene(){
	configureFloor();
	configureSkybox();
	
	modelsCastle.resize(30);
	//modelsCastle[0] = loadNewModel(CASTLE_PATH("balcony_1"));
	modelsCastle[1] =  loadNewModel(CASTLE_PATH("balcony_2"));
	//modelsCastle[2] =  loadNewModel(CASTLE_PATH("balustrade_1"));
	modelsCastle[3] =  loadNewModel(CASTLE_PATH("banner"));
	//modelsCastle[4] =  loadNewModel(CASTLE_PATH("banner_LOD"));
	//modelsCastle[5] =  loadNewModel(CASTLE_PATH("bridge"));
	//modelsCastle[6] =  loadNewModel(CASTLE_PATH("bridge_2"));
	//modelsCastle[7] =  loadNewModel(CASTLE_PATH("bridge_LOD"));
	modelsCastle[8] =  loadNewModel(CASTLE_PATH("castle_gate_1"));
	modelsCastle[9] =  loadNewModel(CASTLE_PATH("castle_gate_LOD2"));
	//modelsCastle[10] =  loadNewModel(CASTLE_PATH("kanone"));
	//modelsCastle[11] =  loadNewModel(CASTLE_PATH("ladder_1"));
	//modelsCastle[12] =  loadNewModel(CASTLE_PATH("torbogen"));
	//modelsCastle[13] =  loadNewModel(CASTLE_PATH("torbogen_LOD"));
	modelsCastle[14] =  loadNewModel(CASTLE_PATH("tower"));
	//modelsCastle[15] =  loadNewModel(CASTLE_PATH("tower_1_LOD"));
	modelsCastle[16] =  loadNewModel(CASTLE_PATH("tower_2"));
	modelsCastle[17] =  loadNewModel(CASTLE_PATH("tower_3_LOD"));
	//modelsCastle[18] =  loadNewModel(CASTLE_PATH("tower_4"));
	//modelsCastle[19] =  loadNewModel(CASTLE_PATH("tower_4_LOD"));
	modelsCastle[20] =  loadNewModel(CASTLE_PATH("wall_1"));
	//modelsCastle[21] =  loadNewModel(CASTLE_PATH("wall_2"));
	modelsCastle[22] =  loadNewModel(CASTLE_PATH("wall_3"));
	modelsCastle[23] =  loadNewModel(CASTLE_PATH("wall_3_broken"));
	//modelsCastle[24] =  loadNewModel(CASTLE_PATH("wall_3_broken_lod"));
	//modelsCastle[25] =  loadNewModel(CASTLE_PATH("wall_3_lod"));
	//modelsCastle[26] =  loadNewModel(CASTLE_PATH("wall_4"));
	//modelsCastle[27] =  loadNewModel(CASTLE_PATH("wall_corner_lod2"));
	//modelsCastle[28] =  loadNewModel(CASTLE_PATH("wall1_lod2"));
	//modelsCastle[29] =  loadNewModel(CASTLE_PATH("watch2"));

	modelsVillage.resize(37);
	//modelsVillage[0] =  loadNewModel(MEDIEVAL_PATH("balcony1"));
	//modelsVillage[1] =  loadNewModel(MEDIEVAL_PATH("balcony1_LOD"));
	//modelsVillage[2] =  loadNewModel(MEDIEVAL_PATH("balcony2"));
	//modelsVillage[3] =  loadNewModel(MEDIEVAL_PATH("balcony2_lod"));
	modelsVillage[4] =  loadNewModel(MEDIEVAL_PATH("barn2"));
	//modelsVillage[5] =  loadNewModel(MEDIEVAL_PATH("barn2_lod"));
	modelsVillage[6] =  loadNewModel(MEDIEVAL_PATH("birdcage"));
	modelsVillage[7] =  loadNewModel(MEDIEVAL_PATH("birdcage_lod"));
	modelsVillage[8] =  loadNewModel(MEDIEVAL_PATH("egg_box"));
	//modelsVillage[9] =  loadNewModel(MEDIEVAL_PATH("egg_box_lod"));
	modelsVillage[10] =  loadNewModel(MEDIEVAL_PATH("fantasy_house4"));
	//modelsVillage[11] =  loadNewModel(MEDIEVAL_PATH("fantasy_house4_lod"));
	//modelsVillage[12] =  loadNewModel(MEDIEVAL_PATH("fantasy_house4_lod2"));
	modelsVillage[13] =  loadNewModel(MEDIEVAL_PATH("fence"));
	//modelsVillage[14] =  loadNewModel(MEDIEVAL_PATH("fence_door"));
	//modelsVillage[15] =  loadNewModel(MEDIEVAL_PATH("fence_door_lod"));
	//modelsVillage[16] =  loadNewModel(MEDIEVAL_PATH("fence_lod"));
	modelsVillage[17] =  loadNewModel(MEDIEVAL_PATH("haywagon"));
	//modelsVillage[18] =  loadNewModel(MEDIEVAL_PATH("haywagon_lod"));
	//modelsVillage[19] =  loadNewModel(MEDIEVAL_PATH("house1"));
	modelsVillage[20] =  loadNewModel(MEDIEVAL_PATH("house1_lod1"));
	//modelsVillage[21] =  loadNewModel(MEDIEVAL_PATH("house1_lod2"));
	modelsVillage[22] =  loadNewModel(MEDIEVAL_PATH("house2"));
	//modelsVillage[23] =  loadNewModel(MEDIEVAL_PATH("house2_lod1"));
	//modelsVillage[24] =  loadNewModel(MEDIEVAL_PATH("house2_lod2"));
	modelsVillage[25] =  loadNewModel(MEDIEVAL_PATH("house3"));
	//modelsVillage[26] =  loadNewModel(MEDIEVAL_PATH("house3_lod1"));
	//modelsVillage[27] =  loadNewModel(MEDIEVAL_PATH("house3_lod2"));
	modelsVillage[28] =  loadNewModel(MEDIEVAL_PATH("house4"));
	//modelsVillage[29] =  loadNewModel(MEDIEVAL_PATH("house4_lod1"));
	//modelsVillage[30] =  loadNewModel(MEDIEVAL_PATH("house4_lod2"));
	modelsVillage[31] =  loadNewModel(MEDIEVAL_PATH("sack01"));
	//modelsVillage[32] =  loadNewModel(MEDIEVAL_PATH("sack01_lod"));
	modelsVillage[33] =  loadNewModel(MEDIEVAL_PATH("stab"));
	//modelsVillage[34] =  loadNewModel(MEDIEVAL_PATH("stab_lod"));
	//modelsVillage[35] =  loadNewModel(MEDIEVAL_PATH("stairs_balcony"));
	//modelsVillage[36] =  loadNewModel(MEDIEVAL_PATH("stairs_balcony_lod"));

	modelsDesert.resize(36);
	modelsDesert[0] =  loadNewModel(OASIS_PATH("3"));
	//modelsDesert[1] =  loadNewModel(OASIS_PATH("3_lod1"));
	//modelsDesert[2] =  loadNewModel(OASIS_PATH("3_lod2"));
	modelsDesert[3] =  loadNewModel(OASIS_PATH("7"));
	modelsDesert[4] =  loadNewModel(OASIS_PATH("7_lod1"));
	//modelsDesert[5] =  loadNewModel(OASIS_PATH("7_lod2"));
	//modelsDesert[6] =  loadNewModel(OASIS_PATH("columns2F"));
	//modelsDesert[7] =  loadNewModel(OASIS_PATH("columns2F_lod1"));
	//modelsDesert[8] =  loadNewModel(OASIS_PATH("columns2F_lod2"));
	//modelsDesert[9] =  loadNewModel(OASIS_PATH("columnsF"));
	//modelsDesert[10] =  loadNewModel(OASIS_PATH("columnsF_lod"));
	//modelsDesert[11] =  loadNewModel(OASIS_PATH("H1F"));
	//modelsDesert[12] =  loadNewModel(OASIS_PATH("H1F_lod1"));
	//modelsDesert[13] =  loadNewModel(OASIS_PATH("H1F_lod2"));
	//modelsDesert[14] =  loadNewModel(OASIS_PATH("H3"));
	//modelsDesert[15] =  loadNewModel(OASIS_PATH("H3_lod1"));
	//modelsDesert[16] =  loadNewModel(OASIS_PATH("H3_lod2"));
	//modelsDesert[17] =  loadNewModel(OASIS_PATH("H3_lod3"));
	//modelsDesert[18] =  loadNewModel(OASIS_PATH("H4F"));
	//modelsDesert[19] =  loadNewModel(OASIS_PATH("H4F_lod1"));
	//modelsDesert[20] =  loadNewModel(OASIS_PATH("H5F"));
	//modelsDesert[21] =  loadNewModel(OASIS_PATH("H5F_lod"));
	//modelsDesert[22] =  loadNewModel(OASIS_PATH("H6F"));
	//modelsDesert[23] =  loadNewModel(OASIS_PATH("H6F_lod"));
	//modelsDesert[24] =  loadNewModel(OASIS_PATH("H7F"));
	//modelsDesert[25] =  loadNewModel(OASIS_PATH("H7F_lod"));
	//modelsDesert[26] =  loadNewModel(OASIS_PATH("wall"));
	//modelsDesert[27] =  loadNewModel(OASIS_PATH("wall_lod1"));
	//modelsDesert[28] =  loadNewModel(OASIS_PATH("wall_lod2"));
	//modelsDesert[29] =  loadNewModel(OASIS_PATH("wall_lod3"));
	//modelsDesert[30] =  loadNewModel(OASIS_PATH("wall2"));
	//modelsDesert[31] =  loadNewModel(OASIS_PATH("wall2_lod1"));
	//modelsDesert[32] =  loadNewModel(OASIS_PATH("wall3"));
	//modelsDesert[33] =  loadNewModel(OASIS_PATH("wall3_lod1"));
	modelsDesert[34] =  loadNewModel(OASIS_PATH("well2"));
	//modelsDesert[35] =  loadNewModel(OASIS_PATH("well2_lod"));
	
	// kdyz na to prijde, tak nahradit models[1] na model01 -> nemusim loadovat vsechny objekty
	transforms.push_back(new transformNode(modelsCastle[1], objC01matrix));
	transforms.push_back(new transformNode(modelsCastle[1], objC01matrix2));

	transforms.push_back(new transformNode(modelsCastle[3], objC03matrix));
	transforms.push_back(new transformNode(modelsCastle[3], objC03matrix2));
	transforms.push_back(new transformNode(modelsCastle[3], objC03matrix3));

	transforms.push_back(new transformNode(modelsCastle[8], objC08matrix));

	transforms.push_back(new transformNode(modelsCastle[9], objC09matrix));

	transforms.push_back(new transformNode(modelsCastle[14], objC14matrix));
	transforms.push_back(new transformNode(modelsCastle[14], objC14matrix2));

	transforms.push_back(new transformNode(modelsCastle[16], objC16matrix));
	transforms.push_back(new transformNode(modelsCastle[16], objC16matrix2));

	transforms.push_back(new transformNode(modelsCastle[17], objC17matrix));
	transforms.push_back(new transformNode(modelsCastle[17], objC17matrix2));

	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix2));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix3));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix4));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix5));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix6));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix7));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix8));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix9));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix10));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix11));
	transforms.push_back(new transformNode(modelsCastle[20], objC20matrix12));

	transforms.push_back(new transformNode(modelsCastle[22], objC22matrix));
	transforms.push_back(new transformNode(modelsCastle[22], objC22matrix2));

	transforms.push_back(new transformNode(modelsCastle[23], objC23matrix));

	//--- Village Models transform ---
	transforms.push_back(new transformNode(modelsVillage[4], objV04matrix));

	transforms.push_back(new transformNode(modelsVillage[6], objV06matrix));

	transforms.push_back(new transformNode(modelsVillage[7], objV07matrix));
	transforms.push_back(new transformNode(modelsVillage[7], objV07matrix2));
	
	transforms.push_back(new transformNode(modelsVillage[8], objV08matrix));
	transforms.push_back(new transformNode(modelsVillage[8], objV08matrix2));
	transforms.push_back(new transformNode(modelsVillage[8], objV08matrix3));
	transforms.push_back(new transformNode(modelsVillage[8], objV08matrix4));
	transforms.push_back(new transformNode(modelsVillage[8], objV08matrix5));

	transforms.push_back(new transformNode(modelsVillage[10], objV10matrix));
	transforms.push_back(new transformNode(modelsVillage[10], objV10matrix2));

	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix2));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix3));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix4));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix5));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix6));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix7));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix8));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix9));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix10));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix11));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix12));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix13));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix14));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix15));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix16));
	transforms.push_back(new transformNode(modelsVillage[13], objV13matrix17));


	transforms.push_back(new transformNode(modelsVillage[17], objV17matrix));
	transforms.push_back(new transformNode(modelsVillage[17], objV17matrix2));
	
	transforms.push_back(new transformNode(modelsVillage[20], objV20matrix));

	transforms.push_back(new transformNode(modelsVillage[22], objV22matrix));

	transforms.push_back(new transformNode(modelsVillage[25], objV25matrix));

	transforms.push_back(new transformNode(modelsVillage[28], objV28matrix));
	transforms.push_back(new transformNode(modelsVillage[28], objV28matrix2));
	transforms.push_back(new transformNode(modelsVillage[28], objV28matrix3));

	transforms.push_back(new transformNode(modelsVillage[31], objV31matrix));
	transforms.push_back(new transformNode(modelsVillage[31], objV31matrix2));
	transforms.push_back(new transformNode(modelsVillage[31], objV31matrix3));
	transforms.push_back(new transformNode(modelsVillage[31], objV31matrix4));

	transforms.push_back(new transformNode(modelsVillage[33], objV33matrix));
	transforms.push_back(new transformNode(modelsVillage[33], objV33matrix2));

	// --- Desert ---
	transforms.push_back(new transformNode(modelsDesert[0], objD00matrix));
	
	transforms.push_back(new transformNode(modelsDesert[3], objD03matrix));
	transforms.push_back(new transformNode(modelsDesert[3], objD03matrix2));
	transforms.push_back(new transformNode(modelsDesert[3], objD03matrix3));
	transforms.push_back(new transformNode(modelsDesert[3], objD03matrix4));

	transforms.push_back(new transformNode(modelsDesert[4], objD04matrix));

	transforms.push_back(new transformNode(modelsDesert[34], objD34matrix));
	
	//
	nTrans = transforms.size();


	/*int count = 0;
	for(int i = 0; i < nTrans; ++i){
		count += transforms[i]->model->m_NumIndices;
	}
	printf("nVert: %d\n",count);*/
}

void prepareScene_1(){

}
void prepareScene_2(){
}

void drawSkybox(){
	glBindBuffer(GL_ARRAY_BUFFER, skybox.modelVBO);

	glVertexAttribPointer(gShaderPositionTag,  3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(gShaderNormalTag,    3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*skybox.nVertices*3));
	glVertexAttribPointer(gShaderTexCoord0Tag, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*skybox.nVertices*3*2));

	glActiveTexture(GL_TEXTURE0 + gSkyboxTex_back.textureUnit);
	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_back.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_back.textureUnit);
	glDrawArrays(GL_QUADS, 0, 4);

	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_front.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_front.textureUnit);
	glDrawArrays(GL_QUADS, 4, 4);

	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_right.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_right.textureUnit);
	glDrawArrays(GL_QUADS, 8, 4);

	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_left.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_left.textureUnit);
	glDrawArrays(GL_QUADS, 12, 4);

	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_up.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_up.textureUnit);
	glDrawArrays(GL_QUADS, 16, 4);

	glBindTexture(GL_TEXTURE_2D, gSkyboxTex_down.texID);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gSkyboxTex_down.textureUnit);
	glDrawArrays(GL_QUADS, 20, 4);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), NOISY_FS);
	//skybox.render_VBOnoIndex();
}


void drawScene_1(int floorMode, int skyboxMode, int modelMode){
	// before scene - MVmatrix is only a camery matrix. So i could store it her hence no invertions are needed
	MVcameraMatrix = MVmatrix;  // something like push matrix

	MVmatrix = MVcameraMatrix * identityMatrix;
	shadowTransform = ( shadowScale *  g_LightProjectionMatrix * g_LightViewMatrix);
	mat4 newDepthMatrix	= shadowTransform * identityMatrix;
		
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_DepthMatrix"), 1 , GL_FALSE, &newDepthMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_ModelViewMatrix"), 1, GL_FALSE, &MVmatrix[0][0]);
	
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Mode"), floorMode);
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gGrassTex.textureUnit);
	floorMesh.render_VBOindex();

	if(skyboxMode != -1){
		glUniform1i(glGetUniformLocation(g_ProgramId, "u_Mode"), skyboxMode);
		drawSkybox();
	}

	/*int size = models.size();
	for(int iModel = 0; iModel < size; ++iModel){
		if(iModel != curObj)
			models[iModel]->renderVBO();

		//glTranslatef(20.0f, 0.0f, 0.0f);
		MVmatrix = glm::translate(MVmatrix, vec3(20.0f, 0.0f, 0.0f));
		commitMVMatrix();
	}*/
	
	// MVmatrix = MVcameraMatrix;
	// --- moving object ---
	/*if(curObjMove){
		curObjPos = -cameraPosition - cameraForward * cameraObjOffset;
		curObjPos.y = cameraObjOffset.y;
	}

	MVmatrix = glm::translate(MVmatrix, curObjPos);
	MVmatrix = glm::rotate(MVmatrix, curObjRotateAngle, yAxis);
	MVmatrix = glm::scale(MVmatrix, vec3(curObjScale, curObjScale, curObjScale));
	commitMVMatrix();

	models[curObj]->renderVBO();*/
	// ---
	MVmatrix = MVcameraMatrix; // undo transforms
	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Mode"), modelMode);

	
	for(int iTrans = 0; iTrans < nTrans; ++iTrans ){
		MVmatrix			= MVcameraMatrix * transforms[iTrans]->globalTransform;
		mat4 newDepthMatrix	= shadowTransform * transforms[iTrans]->globalTransform;
		
		glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_DepthMatrix"), 1 , GL_FALSE, &newDepthMatrix[0][0]);
		commitMVMatrix();

		if((modelMode & UNIF_COLOR_FS) == UNIF_COLOR_FS){
			if(iTrans == 60){
				glUniform3fv(glGetUniformLocation(g_ProgramId, "u_UniformColor"), 1, &X_AXIS[0]);
			}else{
				glUniform3fv(glGetUniformLocation(g_ProgramId, "u_UniformColor"), 1, &WHITE_COLOR[0]);
			}
		}

		glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), 0);
		transforms[iTrans]->model->renderVBO();
	}

	/*for (int i = 0; i < usedModels.size(); i++){
		glm::mat4 newMVMatrix		= g_CurrentModelViewMatrix * usedModels.at(i).modelMat;
		glm::mat4 newDepthMatrix	= matTrans * usedModels.at(i).modelMat;

		glUniformMatrix4fv(glGetUniformLocation(program_id, "u_ModelViewMatrix"), 1, GL_FALSE, &newMVMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program_id, "u_DepthMatrix"), 1, GL_FALSE, &newDepthMatrix[0][0]);

		usedModels.at(i).data->renderVBO();
	}*/

	// --- end of current pass ---
	MVmatrix = MVcameraMatrix; // something like pop matrix
	commitMVMatrix();
}


void drawScene_2(int floorMode, int skyboxMode, int modelMode){
	// before scene - MVmatrix is only a camery matrix. So i could store it her hence no invertions are needed
	MVcameraMatrix = MVmatrix;  // something like push matrix
	//tempLight = g_LightViewMatrix;

	MVmatrix = MVcameraMatrix * identityMatrix;
	shadowTransform = ( shadowScale *  g_LightProjectionMatrix * g_LightViewMatrix);
	mat4 newDepthMatrix	= shadowTransform * identityMatrix;

	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_DepthMatrix"), 1 , GL_FALSE, &newDepthMatrix[0][0]);

	//glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_DepthMatrix"), 1 , GL_FALSE, &newDepthMatrix[0][0]);
	commitMVMatrix();
	
		glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), gGrassTex.textureUnit);

		floorMesh.render_VBOindex();

	MVmatrix = MVcameraMatrix; // undo transforms
	
	// -----------
		
	int iTrans = 60; 

	MVmatrix		= MVcameraMatrix * transforms[iTrans]->globalTransform;
	newDepthMatrix	= shadowTransform * transforms[iTrans]->globalTransform;
		
	glUniformMatrix4fv(glGetUniformLocation(g_ProgramId, "u_DepthMatrix"), 1 , GL_FALSE, &newDepthMatrix[0][0]);
	commitMVMatrix();

	glUniform1i(glGetUniformLocation(g_ProgramId, "u_Texture"), 0);
	transforms[iTrans]->model->renderVBO();
	

	// --- end of current pass ---
	MVmatrix = MVcameraMatrix; // something like pop matrix
	commitMVMatrix();

	//g_LightViewMatrix = tempLight;
}

#endif