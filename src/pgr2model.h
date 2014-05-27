//-----------------------------------------------------------------------------
//  [PGR2] 3D model class
//  14/02/2011
//-----------------------------------------------------------------------------

#ifndef __PGR2_MODEL__
#define __PGR2_MODEL__


#include "../common/GLEW/glew.h" // must be first

#include <string>
#include "shaderConfig.h"


#define TEXTURE_NAME_SIZE 50

class PGR2Model{
public:
	struct MaterialData {
        GLfloat ambient[4];
        GLfloat diffuse[4];
        GLfloat specular[4];
        GLfloat shininess;
        GLfloat refraction;
        GLfloat visibility;
        bool    highlights;

        short diffuse_tex_index;
        short specular_tex_index;
        short bump_tex_index;
        short alpha_tex_index;
        short height_tex_index;
    };
    struct TextureData {
        GLuint id;
        char file_name[TEXTURE_NAME_SIZE];
    };
    struct FaceData {
        unsigned int ivertex[3];
        unsigned int inormal[3];
        unsigned int itexture[3];
        GLfloat      normal[3];
    };
    struct MeshData {
        unsigned short material_index;
        unsigned long  num_faces;
        FaceData*      faces;
    };

	struct ModelClientInfo{
		GLuint idVBO;

		int* vertexIndex;
		int* vertexCount;
		int* vertexBytes;
		int* texCoordBytes;

		ModelClientInfo():idVBO(0)
		{ }
	};

	ModelClientInfo curModel;
	std::string modelName;
	const char* modelName2;

	/// ### FIELDS ###
	TextureData*   m_Textures;
    MaterialData*  m_Materials;
    MeshData*      m_Meshes;

    // Array of 3D vertices {x0, y0, z0}, {x1, y1, z1}, ...
    GLfloat*       m_Vertices;
    // Array of vertices' 2D texture coordinates {s0, t0}, {s1, t1}, ...
    GLfloat*       m_TexCoords;
    // Array of vertices' normals {n0x, n0y, n0z}, {n1x, n1y, n1z}
    GLfloat*       m_Normals;
    // Array of vertices' binormals {b0x, b0y, b0z}, {b1x, b1y, b1z}
    GLfloat*       m_Binormals;
    // Array of vertices' tangents {t0x, t0y, t0z, t0w}, {t1x, t1y, t1z, t1w}
    GLfloat*       m_Tangents; // Warning! Tangents are 4D vectors!!!

    unsigned short m_NumTextures;
    unsigned short m_NumMaterials;
    unsigned short m_NumMeshes;
    unsigned long  m_NumVertices;
    unsigned long  m_NumTexCoords;
    unsigned long  m_NumNormals;
    unsigned long  m_NumBinormals;
	unsigned long  m_NumTangents;
	
	unsigned short m_NumIndices;
    
	/// ### METHODS ###
    PGR2Model();
    virtual	~PGR2Model();

	static	PGR2Model*	loadFromFile(const char* file_name);
    bool				saveToFile(const char* file_name);

	void	initVBO();
	void	renderMesh(int iMesh);
	void	prepareMesh(int iMesh);
	void	renderVBO();

    void	renderImmediate(bool transparent_meshes);
    void	renderVertexNormals(float scale);
    void	renderFaceNormals(float scale);
};


#endif // __PGR2_MODEL__
