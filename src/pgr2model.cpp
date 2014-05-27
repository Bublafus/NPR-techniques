//-----------------------------------------------------------------------------
//  [PGR2] 3D model class
//  14/02/2011
//-----------------------------------------------------------------------------
#include "pgr2model.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string>

//TODO: introduce to some file?!
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

// Data file structure ------------------------------------------------->

/*
   PGR2 file structure
   +-------------------------------+
   | PGR2Model_Header              |   -> main file header
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture 0 descriptor
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture 1 descriptor
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture N descriptor
   +-------------------------------+      (N = PGR2Model_Header.num_textures)
   | MaterialData                  |   -> material 0 data
   +-------------------------------+
   | MaterialData                  |   -> material 1 data
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | MaterialData                  |   -> material N data
   +-------------------------------+      (N = PGR2Model_Header.num_materials)
   | PGR2Model_MeshHeader          |   -> mesh 0 data
   +-------------------------------+
   | FaceData                      |   -> mesh 0 first face
   +-------------------------------+
   | FaceData                      |   -> mesh 0 second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData N                    |   -> mesn 0 N-th face
   +-------------------------------+      (N = PGR2Model_MeshHeader.num_faces)
   | PGR2Model_MeshHeader          |   -> mesh 1 data
   +-------------------------------+
   | FaceData                      |   -> mesh 1 first face
   +-------------------------------+
   | FaceData                      |   -> mesh 1 second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData                      |   -> mesh N N-th face
   +-------------------------------+      
   | ...                           |
   +-------------------------------+
   | PGR2Model_MeshHeader          |   -> mesh N data
   +-------------------------------+      (N = PGR2Model_Header.num_meshes)
   | FaceData                      |   -> mesh N first face
   +-------------------------------+
   | FaceData                      |   -> mesh N second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData                      |   -> mesh N N-th face
   +-------------------------------+
   | GLfloat[0..m_NumVertices]     |   -> vertices data (m_Vertex)
   +-------------------------------+
   | GLfloat[0..m_NumTexCoords]    |   -> texture coordinates (m_TexCoords)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> normal vectors (m_Normals)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> binormal vectors (m_Binormals)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> tangent vectors (m_Tangents)
   +-------------------------------+
*/

// Main file header
struct PGR2Model_Header
{
    unsigned short num_textures;
    unsigned short num_materials;
    unsigned short num_meshes;
    unsigned long  num_vertices;
    unsigned long  num_tex_coords;
    unsigned long  num_normals;
    unsigned long  num_binormals;
};

// Texture data header
struct PGR2Model_TextureHeader
{
    char file_name[TEXTURE_NAME_SIZE];
};
struct PGR2Texture_Header
{
    GLsizei        width;
    GLsizei        height;
    GLenum         type;
    unsigned int   size;
};


// Mesh data header
struct PGR2Model_MeshHeader
{
    unsigned short material_index;
    unsigned long  num_faces;
};

// Data file structure ------------------------------------------------->


//---------------------------------------------------------------------------
//	Description: Gets file path from full file name
//
//---------------------------------------------------------------------------
inline std::string getFilePath(const std::string& file_name)
{
    std::string file_path         = std::string(file_name);
    std::string::size_type offset = file_path.find_last_of('\\');
    if (offset != std::string::npos)
    {
        file_path = file_path.substr(0, ++offset);
    }
    else
    {
        offset = file_path.find_last_of('/');
        if (offset != std::string::npos)
        {
            file_path = file_path.substr(0, ++offset);
        }
    }

    return file_path;
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model::PGR2Model() : 
    m_NumTextures(0), m_NumMaterials(0), m_NumMeshes(0), 
    m_NumVertices(0), m_NumTexCoords(0), 
	m_NumNormals(0), m_NumBinormals(0), m_NumTangents(0),
    m_Textures(NULL), m_Materials(NULL), m_Meshes(NULL),
    m_Vertices(NULL), m_TexCoords(NULL), 
    m_Normals(NULL), m_Binormals(NULL), m_Tangents(NULL)
{
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model::~PGR2Model()
{
    if (m_Textures)   delete [] m_Textures;
    if (m_Materials)  delete [] m_Materials;
    if (m_Meshes)
    {
        for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
        {
            delete [] m_Meshes[iMesh].faces;
        }
        delete [] m_Meshes;
    }
    if (m_Vertices)   delete [] m_Vertices;
    if (m_TexCoords)  delete [] m_TexCoords;
    if (m_Normals)    delete [] m_Normals;
    if (m_Binormals)  delete [] m_Binormals;
    if (m_Tangents)   delete [] m_Tangents;
}


GLint loader_PositionTag = 0;
GLint loader_NormalTag	 = 3;
GLint loader_TexCoord0Tag = 6;


// NOTE: NonIndexed -> Indexed geometry drawing
// For indexed drawing (glDrawElements) is necessary to build index array. It can be built from indices in the
// cycle where in this moment are built vertex, normal and texcoord arrays.
// If this arrray would be bouild vertices need to be stored directly from m_Vertices (dont use indicies)
// Note: g_pModel->m_NumVertices is the real vertices count (the sum of the mesh.num_faces*3 is the number of indices)
// Problem #1: TexCoords has different indices
// Problem #2: Indidices are global for the whole model (i think) -
//		so it has to be stored in VBO each arrays after the other one (vertex, normals, coords)


//  Non index (It duplicates vertices)
void PGR2Model::initVBO(){
	curModel.vertexIndex = new int[m_NumMeshes];
	curModel.vertexCount = new int[m_NumMeshes];

	curModel.vertexBytes	= new int[m_NumMeshes];
	curModel.texCoordBytes	= new int[m_NumMeshes];

	// Count indicies (virtual vertices) of the whole model
	this->m_NumIndices = 0;
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++){
        const MeshData& mesh = m_Meshes[iMesh];

		// Prefix scan :)
		// ( 3 * nVertex + 3 * nVertex + 2 * nVertex     ) * sizeof(GLfloat) = 8 * sizeof(GLfloat)
		// ( [vertices]  + [normals]   + [textureCoords] ) * [floats]        = 32
		curModel.vertexIndex[iMesh] = m_NumIndices * 8 * sizeof(GLfloat);

		m_NumIndices += mesh.num_faces * 3; // Three vertices for each face
	}

	glGenBuffers(1, &curModel.idVBO);
	glBindBuffer(GL_ARRAY_BUFFER, curModel.idVBO);
		glBufferData(GL_ARRAY_BUFFER, m_NumIndices * 8 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Init each par of VBO (for each mesh). Its dependent on vertexIndex values.
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++){
		prepareMesh(iMesh);
	}
	
}
void PGR2Model::prepareMesh(int iMesh){
	const MeshData& mesh = m_Meshes[iMesh];

	// Face has 3 vertices.
	curModel.vertexCount[iMesh] = mesh.num_faces * 3;

	curModel.vertexBytes[iMesh]		= 3 * curModel.vertexCount[iMesh] * sizeof(GLfloat); // 3 float per vertex and normal
	curModel.texCoordBytes[iMesh]	= 2 * curModel.vertexCount[iMesh] * sizeof(GLfloat); // 2 float per texture coordinates

	GLfloat* vertexArray		= new GLfloat[3 * curModel.vertexCount[iMesh]];
	GLfloat* normalArray		= new GLfloat[3 * curModel.vertexCount[iMesh]];
	GLfloat* textureCoordArray  = new GLfloat[2 * curModel.vertexCount[iMesh]];
	
	// Load data
	int indexVert = 0, indexTex = 0;

	for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++){
        const FaceData& face = mesh.faces[iFace];

        for (int iVertex = 0; iVertex < 3; iVertex++){			
			vertexArray[indexVert  ]  = m_Vertices[ 3*face.ivertex[iVertex]];
			vertexArray[indexVert+1]  = m_Vertices[(3*face.ivertex[iVertex])+1];
			vertexArray[indexVert+2]  = m_Vertices[(3*face.ivertex[iVertex])+2];

			normalArray[indexVert  ]	= m_Normals[ 3*face.inormal[iVertex]];
			normalArray[indexVert+1]	= m_Normals[(3*face.inormal[iVertex])+1];
			normalArray[indexVert+2]	= m_Normals[(3*face.inormal[iVertex])+2];

			textureCoordArray[indexTex]		= m_TexCoords[ 2*face.itexture[iVertex]];
			textureCoordArray[indexTex+1]	= m_TexCoords[(2*face.itexture[iVertex])+1];

			indexVert += 3;
			indexTex += 2;
        }
    }

	// Copy to GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, curModel.idVBO);
		glBufferSubData(GL_ARRAY_BUFFER, curModel.vertexIndex[iMesh]							    ,	curModel.vertexBytes[iMesh], vertexArray);
		glBufferSubData(GL_ARRAY_BUFFER, curModel.vertexIndex[iMesh] +   curModel.vertexBytes[iMesh],   curModel.vertexBytes[iMesh], normalArray);
		glBufferSubData(GL_ARRAY_BUFFER, curModel.vertexIndex[iMesh] + 2*curModel.vertexBytes[iMesh], curModel.texCoordBytes[iMesh], textureCoordArray);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void PGR2Model::renderMesh(int iMesh) {
	// <Material>
		const MeshData& mesh = m_Meshes[iMesh];
		// Ignore transparent meshes!!
		const MaterialData& material = m_Materials[mesh.material_index];
		//if (material.alpha_tex_index >= 0) 
		//	return;

		/*glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, material.ambient);
		glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, material.diffuse);
		glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, material.specular);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.shininess * 128.0f);*/

		// Use diffuse texture if exists
		if (material.diffuse_tex_index >= 0){
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_Textures[material.diffuse_tex_index].id);
		} 
	// </Material>
	
	glBindBuffer(GL_ARRAY_BUFFER, curModel.idVBO);

	// IMPORTANT: Uncoment when not already enabled!!
	/*glEnableVertexAttribArray(gShaderPositionTag);
	glEnableVertexAttribArray(gShaderNormalTag);
	glEnableVertexAttribArray(gShaderTexCoord0Tag);*/

	// to preserve without shader (fixed pipeline)
	// <non-shader>
		//glEnableClientState(GL_NORMAL_ARRAY);
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		//glNormalPointer(     GL_FLOAT, 0, BUFFER_OFFSET(curModel.vertexIndex[iMesh] +   curModel.vertexBytes[iMesh]));
		//glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(curModel.vertexIndex[iMesh] + 2*curModel.vertexBytes[iMesh]));
	// </non-shader>

	glVertexAttribPointer(loader_PositionTag,  3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(curModel.vertexIndex[iMesh]));
	glVertexAttribPointer(loader_NormalTag,    3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(curModel.vertexIndex[iMesh] +   curModel.vertexBytes[iMesh]));
	glVertexAttribPointer(loader_TexCoord0Tag, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(curModel.vertexIndex[iMesh] + 2*curModel.vertexBytes[iMesh]));
	
	glDrawArrays(GL_TRIANGLES, 0, curModel.vertexCount[iMesh]);
	
	/*glDisableVertexAttribArray(loader_PositionTag);
	glDisableVertexAttribArray(loader_NormalTag);
	glDisableVertexAttribArray(loader_TexCoord0Tag);*/
	
	// <non-shader>
		//glDisableClientState(GL_NORMAL_ARRAY);
		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	// </non-shader>

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PGR2Model::renderVBO(){
	// For non indexed geometry
	//if(model.loader_VertexVBOID == 0){
	//	initVBO(0);
	//}

	for(int iMesh = 0; iMesh < m_NumMeshes; iMesh++){
		renderMesh(iMesh);
	}
}

void PGR2Model::renderImmediate(bool transparent_meshes){
    // 1st pass - render meshes with no visibility information (mesh.material.alpha_tex_id == 0)
	for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++){
        unsigned short num_textures_used = 0;

        const MeshData& mesh = m_Meshes[iMesh];
        if (mesh.material_index >= 0){
            const MaterialData& material = m_Materials[mesh.material_index];

            // Render non-transparent meshes only -> skip meshes with alpha texture
            if (material.alpha_tex_index >= 0) continue;

            glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, material.ambient);
            glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, material.diffuse);
            glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, material.specular);
            glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.shininess * 128.0f);

            // Use diffuse texture if exists
            if (material.diffuse_tex_index >= 0) {
                glActiveTexture(GL_TEXTURE0 + num_textures_used);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, m_Textures[material.diffuse_tex_index].id);
                num_textures_used++;
            }

            // Use specular texture if exists
            if (material.specular_tex_index >= 0) {
//              glActiveTexture(GL_TEXTURE0 + num_textures_used);
//              glEnable(GL_TEXTURE_2D);
//              glBindTexture(GL_TEXTURE_2D, m_Textures[material.specular_tex_index].id);
//              num_textures_used++;
            }
            // Use bump texture if exists
            if (material.bump_tex_index >= 0) {
//              glActiveTexture(GL_TEXTURE0 + num_textures_used);
//              glEnable(GL_TEXTURE_2D);
//              glBindTexture(GL_TEXTURE_2D, m_Textures[material.bump_tex_index].id);
//              num_textures_used++;
            }
            // Use bump texture if exists
            if (material.height_tex_index >= 0) {
//              glActiveTexture(GL_TEXTURE0 + num_textures_used);
//              glEnable(GL_TEXTURE_2D);
//              glBindTexture(GL_TEXTURE_2D, m_Textures[material.height_tex_index].id);
//              num_textures_used++;
            }   
        }

//      glActiveTexture(GL_TEXTURE0);
        glBegin(GL_TRIANGLES);
        for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++) {
            const FaceData& face = mesh.faces[iFace];

            for (int iVertex = 0; iVertex < 3; iVertex++) {
                for (int iTex = 0; iTex < num_textures_used; iTex++) {
                    glMultiTexCoord2fv(GL_TEXTURE0 + iTex, &m_TexCoords[2*face.itexture[iVertex]]);
                }
                glNormal3fv(&m_Normals[3*face.inormal[iVertex]]);
                glVertex3fv(&m_Vertices[3*face.ivertex[iVertex]]);
            }
        }
        glEnd();

        // Disable texturing
        for (int iTex = num_textures_used-1; iTex >= 0; iTex--) {
            glActiveTexture(GL_TEXTURE0 + iTex);
            glDisable(GL_TEXTURE_2D);
        }
    }

	
	// transparent meshes display
    if (transparent_meshes) {
        // 2nd pass - render meshes with visibility information (mesh.material.alpha_tex_index > 0)
        glEnable(GL_BLEND);
        glEnable(GL_ALPHA_TEST);
        glDisable(GL_LIGHTING);

        for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
        {
            int num_textures_used = 0;
           
            const MeshData& mesh = m_Meshes[iMesh];
            if (mesh.material_index >= 0){
                const MaterialData& material = m_Materials[mesh.material_index];

                // Render meshes with alpha texture only
                if (material.alpha_tex_index < 0) continue;

                glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, material.ambient);
                glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, material.diffuse);
                glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, material.specular);
                glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.shininess * 128.0f);

                // Use diffuse texture if exists
                if (material.diffuse_tex_index >= 0) {
                   glActiveTexture(GL_TEXTURE0 + num_textures_used);
                   glEnable(GL_TEXTURE_2D);
                   glBindTexture(GL_TEXTURE_2D, m_Textures[material.diffuse_tex_index].id);
                   num_textures_used++;
                }
                // Use specular texture if exists
                if (material.specular_tex_index >= 0) {
//                  glActiveTexture(GL_TEXTURE0 + num_textures_used);
//                  glEnable(GL_TEXTURE_2D);
//                  glBindTexture(GL_TEXTURE_2D, m_Textures[material.specular_tex_index].id);
//                  num_textures_used++;
                }
                // Use bump texture if exists
                if (material.bump_tex_index >= 0) {
//                  glActiveTexture(GL_TEXTURE0 + num_textures_used);
//                  glEnable(GL_TEXTURE_2D);
//                  glBindTexture(GL_TEXTURE_2D, m_Textures[material.bump_tex_index].id);
//                  num_textures_used++;
                }
                // Use height texture if exists
                if (material.height_tex_index >= 0) {
//                  glActiveTexture(GL_TEXTURE0 + num_textures_used);
//                  glEnable(GL_TEXTURE_2D);
//                  glBindTexture(GL_TEXTURE_2D, m_Textures[material.height_tex_index].id);
//                  num_textures_used++;
                }
                // Setup alpha texture
                glActiveTexture(GL_TEXTURE0 + num_textures_used);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, m_Textures[material.alpha_tex_index].id);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                num_textures_used++;
            }

//          glActiveTexture(GL_TEXTURE0);
            glBegin(GL_TRIANGLES);
            for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++) {
                const FaceData& face = mesh.faces[iFace];

                for (int iVertex = 0; iVertex < 3; iVertex++) {
                    for (int iTex = 0; iTex < num_textures_used; iTex++) {
                        glMultiTexCoord2fv(GL_TEXTURE0 + iTex, &m_TexCoords[2*face.itexture[iVertex]]);
                    }
                    glNormal3fv(&m_Normals[3*face.inormal[iVertex]]);
                    glVertex3fv(&m_Vertices[3*face.ivertex[iVertex]]);
                }
            }
            glEnd();

            // Disable texturing
            for (int iTex = num_textures_used-1; iTex >= 0; iTex--){
                glActiveTexture(GL_TEXTURE0 + iTex);
                glDisable(GL_TEXTURE_2D);
            }
        }

        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
        glEnable(GL_LIGHTING);
    }
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
void PGR2Model::renderFaceNormals(float scale)
{
    const float normals_scale = 2*scale;

    glDisable(GL_LIGHTING);
    glBegin(GL_LINES);
    glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
    for (unsigned int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
    {
        const MeshData& mesh = m_Meshes[iMesh];
        for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++)
        {
            const FaceData& face = mesh.faces[iFace];
            const unsigned int v0i = 3*face.ivertex[0];
            const unsigned int v1i = 3*face.ivertex[1];
            const unsigned int v2i = 3*face.ivertex[2];

            const GLfloat center[3] = 
            {
                (m_Vertices[v0i + 0] + m_Vertices[v1i + 0] + m_Vertices[v2i + 0]) * 0.33f,
                (m_Vertices[v0i + 1] + m_Vertices[v1i + 1] + m_Vertices[v2i + 1]) * 0.33f,
                (m_Vertices[v0i + 2] + m_Vertices[v1i + 2] + m_Vertices[v2i + 2]) * 0.33f
            };
            const GLfloat end[3]    = 
            {
                center[0] + face.normal[0]*normals_scale,
                center[1] + face.normal[1]*normals_scale,
                center[2] + face.normal[2]*normals_scale
            };

            glVertex3fv(center);
            glVertex3fv(end);
        }
    }
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_LIGHTING);
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
void PGR2Model::renderVertexNormals(float scale)
{
    if ((m_Binormals != NULL) && (m_NumBinormals > 0))
    {
        const float normals_scale = 2*scale;

        glDisable(GL_LIGHTING);
        glBegin(GL_LINES);
        for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
        {
            const MeshData& mesh = m_Meshes[iMesh];
            for (int iFace = 0; iFace < mesh.num_faces; iFace++)
            {
                const FaceData& face = mesh.faces[iFace];
                for (int iVertex = 0; iVertex < 3; iVertex++)
                {
                    const GLfloat* pVertex     = m_Vertices + 3*face.ivertex[iVertex];
                    const GLfloat* pNormal     = m_Normals + 3*face.inormal[iVertex];
                    const GLfloat* pBinormal   = m_Binormals + 3*face.inormal[iVertex];
                    const GLfloat* pTangent    = m_Tangents + 4*face.inormal[iVertex];

                    const GLfloat end_normal[3]   = {  pVertex[0] + pNormal[0]*scale,   pVertex[1] + pNormal[1]*scale,    pVertex[2] + pNormal[2]*scale};
                    const GLfloat end_binormal[3] = {pVertex[0] + pBinormal[0]*scale, pVertex[1] + pBinormal[1]*scale,  pVertex[2] + pBinormal[2]*scale};
                    const GLfloat end_tangent[3]  = { pVertex[0] + pTangent[0]*scale,  pVertex[1] + pTangent[1]*scale,   pVertex[2] + pTangent[2]*scale};

                    // Normal
                    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                    glVertex3fv(pVertex);
                    glVertex3fv(end_normal);
                    // Binormal
                    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                    glVertex3fv(pVertex);
                    glVertex3fv(end_binormal);
                    // Tangent
                    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                    glVertex3fv(pVertex);
                    glVertex3fv(end_tangent);
                }
            }
        }
        glEnd();

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_LIGHTING);
    }
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model* PGR2Model::loadFromFile(const char* file_name){
	// start loading
    bool load_failed = true;

    FILE* fin = fopen(file_name, "rb");
    if (!fin){
        return NULL;
    }

    // Get file path
    std::string file_path = getFilePath(file_name);

    PGR2Model* pNewModel = new PGR2Model();
    if (!pNewModel) goto end_of_load;
	
	pNewModel->modelName = file_path;// added
	pNewModel->modelName2 = file_name;// added

    // Read model header ---------------------------------------------->
    PGR2Model_Header header = {0};
    if (fread(&header, sizeof(PGR2Model_Header), 1, fin) != 1) goto end_of_load;

    if ((header.num_materials == 0) || (header.num_meshes == 0) || (header.num_normals == 0) || (header.num_vertices == 0))
    {
        goto end_of_load;
    }

	// Read textures -------------------------------------------------->
    pNewModel->m_NumTextures = header.num_textures;
    if (pNewModel->m_NumTextures > 0)
    {
        // Allocate array for texture objects id
        pNewModel->m_Textures = new TextureData[header.num_textures];
        memset(pNewModel->m_Textures, 0, header.num_textures*sizeof(GLubyte));

        // Create texture objects
        for (int iTexture = 0; iTexture < header.num_textures; iTexture++)
        {
            // Read texture header from file
            PGR2Model_TextureHeader tex_header = {0};
            if (fread(&tex_header, sizeof(PGR2Model_TextureHeader), 1, fin) != 1) 
				goto end_of_load;

            // Read texture data from file ---------------------------->
            FILE* file_texture = fopen((file_path + tex_header.file_name).c_str(), "rb");
            if (file_texture == NULL)
				goto end_of_load;

            PGR2Texture_Header tex_data_header = {0};
            if (fread(&tex_data_header, sizeof(PGR2Texture_Header), 1, file_texture) != 1) 
				goto end_of_load;

            assert(tex_data_header.size != 0);
            GLubyte* pixels = new GLubyte[tex_data_header.size];
            if (fread(pixels, sizeof(GLubyte), tex_data_header.size, file_texture) != tex_data_header.size)
				goto end_of_load;

			// save texture file name
			//strcpy(pNewModel->m_Textures[iTexture].file_name, tex_header.file_name, TEXTURE_NAME_SIZE);

            // Create texture object
            GLenum error = glGetError();
			glActiveTexture(GL_TEXTURE0); // dm
            glGenTextures(1, &pNewModel->m_Textures[iTexture].id);
            glBindTexture(GL_TEXTURE_2D, pNewModel->m_Textures[iTexture].id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            gluBuild2DMipmaps(GL_TEXTURE_2D, tex_data_header.type, tex_data_header.width, tex_data_header.height, tex_data_header.type, GL_UNSIGNED_BYTE, pixels);
            delete [] pixels;
            fclose(file_texture);

            if ((error = glGetError()) != GL_NO_ERROR) 
				goto end_of_load;
        }
    }

	//  Read materials ------------------------------------------------->
    pNewModel->m_NumMaterials = header.num_materials;

    // Allocate array for materials
    pNewModel->m_Materials = new MaterialData[header.num_materials];
    memset(pNewModel->m_Materials, 0, header.num_materials*sizeof(MaterialData));

    // Load materials from file
    for (int iMaterial = 0; iMaterial < header.num_materials; iMaterial++)
    {
        if (fread(&pNewModel->m_Materials[iMaterial], sizeof(MaterialData), 1, fin) != 1)
            goto end_of_load;
    }

    // Read meshes ---------------------------------------------------->
    pNewModel->m_NumMeshes = header.num_meshes;

    // Allocate array for meshes
    pNewModel->m_Meshes = new MeshData[header.num_meshes];
    memset(pNewModel->m_Meshes, 0, header.num_meshes*sizeof(MeshData));

    for (int iMesh = 0; iMesh < header.num_meshes; iMesh++)
    {
        // Read mesh header from file
        PGR2Model_MeshHeader mes_header = {0};
        if (fread(&mes_header, sizeof(PGR2Model_MeshHeader), 1, fin) != 1) 
            goto end_of_load;

        pNewModel->m_Meshes[iMesh].material_index = mes_header.material_index;
        pNewModel->m_Meshes[iMesh].num_faces      = mes_header.num_faces;
        if (mes_header.num_faces > 0)
        {
            // Read mesh face data from file
            pNewModel->m_Meshes[iMesh].faces = new FaceData[mes_header.num_faces];
            if (fread(pNewModel->m_Meshes[iMesh].faces, sizeof(FaceData),
                mes_header.num_faces, fin) != mes_header.num_faces)
            goto end_of_load;
        }
        else
        {
            goto end_of_load;
        }
    }

	// Read model vertices -------------------------------------------->
    pNewModel->m_NumVertices = header.num_vertices;
    // Allocate array for vertices
    pNewModel->m_Vertices = new GLfloat[3*header.num_vertices];
    if (fread(pNewModel->m_Vertices, 3*sizeof(GLfloat), header.num_vertices, fin) != header.num_vertices)
        goto end_of_load;

    // Read model texture coordinates --------------------------------->
    pNewModel->m_NumTexCoords = header.num_tex_coords;
    if (pNewModel->m_NumTexCoords > 0)
    {
        // Allocate array for texture coordinates
        pNewModel->m_TexCoords = new GLfloat[2*header.num_tex_coords];
        if (fread(pNewModel->m_TexCoords, 2*sizeof(GLfloat), header.num_tex_coords, fin) != header.num_tex_coords)
            goto end_of_load;
    }

    // Read model normals --------------------------------------------->
    pNewModel->m_NumNormals = header.num_normals;
    // Allocate array for normals
    pNewModel->m_Normals = new GLfloat[3*header.num_normals];
    if (fread(pNewModel->m_Normals, 3*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
        goto end_of_load;

	// Read model binormals ------------------------------------------->
    pNewModel->m_NumBinormals = header.num_binormals;
	pNewModel->m_NumTangents  = header.num_binormals; // counts are the same
    if (pNewModel->m_NumBinormals > 0)
    {
        // Allocate array for binormals
        pNewModel->m_Binormals = new GLfloat[3*header.num_binormals];
        if (fread(pNewModel->m_Binormals, 3*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
            goto end_of_load;

        // Read model tangents -------------------------------------------->
        // Allocate array for tangents
        pNewModel->m_Tangents = new GLfloat[4*header.num_binormals];
        if (fread(pNewModel->m_Tangents, 4*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
            goto end_of_load;
    }

    // Model was loaded successfully
    load_failed = false;

end_of_load:
    if (load_failed){
        delete pNewModel;
        pNewModel = NULL;
    }
    fclose(fin);

    return pNewModel;
};


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
bool PGR2Model::saveToFile(const char* file_name)
{
    bool result = false;

    FILE* fout = fopen(file_name, "wb");

    if (!fout)
    {
        return false;
    }

    // Get file path
    std::string file_path = getFilePath(file_name);

    // File header start ---------------------------------------------->
    PGR2Model_Header header = 
    {
        m_NumTextures,
        m_NumMaterials,
        m_NumMeshes, 
        m_NumVertices,
        m_NumTexCoords,
        m_NumNormals,
        m_NumBinormals
    };

    if (fwrite(&header, sizeof(PGR2Model_Header), 1, fout) != 1)
    {
        goto end_of_save;
    }
    // File header end ------------------------------------------------>

    // Texture headers start ------------------------------------------>
    GLenum error = glGetError();
    for (int iTexture = 0; iTexture < m_NumTextures; iTexture++)
    {
        glBindTexture(GL_TEXTURE_2D, m_Textures[iTexture].id);
        assert(glGetError() == GL_NO_ERROR);
        GLint width      = 0;
        GLint height     = 0;
        GLint format     = 0;
        GLint compressed = 0;
        GLint red = 0, green = 0, blue = 0, alpha = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
        if (compressed > 0)
        {
            assert(0);  // Not supported now !!!
            goto end_of_save;
        }

        switch(format)
        {
        case GL_ALPHA: 
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
            break;
        case GL_RGBA :
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
        case GL_RGB  :
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE,   &red);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &green);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE,  &blue);
            break;
        default      :
            goto end_of_save;
            break;
        }

        GLint texel_size = (red + green + blue + alpha) / 8;
        if (((width <= 0) || (height <= 0)) || (texel_size <= 0))
        {
            goto end_of_save;
        }

        const unsigned int data_size = texel_size * width * height;
        GLubyte* texels = new GLubyte[data_size];
        memset(texels, 0, data_size);
        glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, texels);
        assert(glGetError() == GL_NO_ERROR);

        // Save texture to separate file --------------->
        FILE* file_texture = fopen((file_path + m_Textures[iTexture].file_name).c_str(), "wb");
        if (file_texture == NULL) 
            goto end_of_save;

        PGR2Texture_Header texture_header = 
        {
            width, height, format, data_size
        };
        // Write texture header
        if (fwrite(&texture_header, sizeof(PGR2Texture_Header), 1, file_texture) != 1)
        {
            goto end_of_save;
        }
        // Write texture data
        if (fwrite(texels, data_size, 1, file_texture) != 1)
        {
            goto end_of_save;
        }
        fclose(file_texture);

        glBindTexture(GL_TEXTURE_2D, 0);
        // Save texture to separate file --------------->

        // Save texture header to model file ----------->
        PGR2Model_TextureHeader model_tex_header;
        strcpy(model_tex_header.file_name, m_Textures[iTexture].file_name);
        if (fwrite(&model_tex_header, sizeof(PGR2Model_TextureHeader), 1, fout) != 1)
        {
            goto end_of_save;
        }
        // Save texture header to model file ----------->
    }
// Texture headers end -------------------------------------------->

// Material headers start ----------------------------------------->
    for (int iMaterial = 0; iMaterial < m_NumMaterials; iMaterial++)
    {
        // Write material header
        if (fwrite(&m_Materials[iMaterial], sizeof(MaterialData), 1, fout) != 1)
        {
            goto end_of_save;
        }
    }
// Material headers end ------------------------------------------->

// Mesh headers start --------------------------------------------->
    for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
    {
        const MeshData& mesh = m_Meshes[iMesh];

        PGR2Model_MeshHeader mesh_header = 
        {
            (unsigned short)mesh.material_index,
            mesh.num_faces
        };

        // Write face header
        if (fwrite(&mesh_header, sizeof(PGR2Model_MeshHeader), 1, fout) != 1)
        {
            goto end_of_save;
        }

        for (int iFace = 0; iFace < mesh.num_faces; iFace++)
        {
            // Write face
            if (fwrite(&mesh.faces[iFace], sizeof(FaceData), 1, fout) != 1)
            {
                goto end_of_save;
            }
        }
    }
// Mesh headers end ----------------------------------------------->

// Vertices ------------------------------------------------------->
    if (fwrite(m_Vertices, 3*sizeof(GLfloat), m_NumVertices, fout) != m_NumVertices)
    {
        goto end_of_save;
    }
// TexCoords ------------------------------------------------------>
    if (m_TexCoords != NULL)
    {
        if (fwrite(m_TexCoords, 2*sizeof(GLfloat), m_NumTexCoords, fout) != m_NumTexCoords)
        {
            goto end_of_save;
        }
    }
// Normals -------------------------------------------------------->
   if (fwrite(m_Normals, 3*sizeof(GLfloat), m_NumNormals, fout) != m_NumNormals)
   {
      goto end_of_save;
   }
// Binormals ------------------------------------------------------>
    if (m_Binormals != NULL)
    {
        if (fwrite(m_Binormals, 3*sizeof(GLfloat), m_NumNormals, fout) != m_NumNormals)
        {
            goto end_of_save;
        }
// Tangents ------------------------------------------------------->
        if (fwrite(m_Tangents, 4*sizeof(GLfloat), m_NumNormals, fout) !=  m_NumNormals)
        {
            goto end_of_save;
        }
    }

    result = true;

end_of_save:
    fclose(fout);

    return result;
}
