#ifndef __SINGLE_MESH_H__
#define __SINGLE_MESH_H__

#include "shaderConfig.h"

class SingleMesh{
public:
	GLuint nVertices;
	GLushort nIndices;

	GLfloat* vertices;
	GLfloat* normals;
	GLfloat* texCoords;

	GLushort* indices;

	GLuint modelVBO;
	GLuint modelIBO;

	SingleMesh(): modelVBO(0), modelIBO(0)
	{	}

	//SingleMesh(GLuint nVertex, GLushort nIndex, GLfloat* vertices, GLfloat* normals, GLfloat* texCoords) : 
	//	g_ModelVBO(0), g_ModelIBO(0)
	//{
	//	
	//}

	void init_VBOnoIndex(){
		glGenBuffers(1, &modelVBO);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVertices*8, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0,								sizeof(GLfloat)*nVertices*3, vertices);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVertices*3,   sizeof(GLfloat)*nVertices*3, normals);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nVertices*3*2, sizeof(GLfloat)*nVertices*2, texCoords);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}


	void init_VBOindex(){
		init_VBOnoIndex();

		glGenBuffers(1, &modelIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nIndices, indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	

	void render_VBOindex(){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIBO);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO);

		// IMPORTANT: Uncoment when not already enabled!!
		//glEnableVertexAttribArray(gShaderPositionTag);
		//glEnableVertexAttribArray(gShaderNormalTag);
		//glEnableVertexAttribArray(gShaderTexCoord0Tag);

		//glEnableClientState(GL_NORMAL_ARRAY);
		// to preserve normals without shader
		//glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(sizeof(GLfloat)*NUM_ELEPHANT_VERTICES*3));

		glVertexAttribPointer(gShaderPositionTag,  3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(gShaderNormalTag,    3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*nVertices*3));
		glVertexAttribPointer(gShaderTexCoord0Tag, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*nVertices*3*2));

		// Indexed
		glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
		// Non-indexed
		//glDrawArrays(GL_TRIANGLES, 0, nVertices);

		//glDisableVertexAttribArray(gShaderPositionTag);
		//glDisableVertexAttribArray(gShaderNormalTag);
		//glDisableVertexAttribArray(gShaderTexCoord0Tag);
	
		//glDisableClientState(GL_NORMAL_ARRAY);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void render_VBOnoIndex(){
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO);

		// IMPORTANT: Uncoment when not already enabled!!
		//glEnableVertexAttribArray(gShaderPositionTag);
		//glEnableVertexAttribArray(gShaderNormalTag);
		//glEnableVertexAttribArray(gShaderTexCoord0Tag);

		glVertexAttribPointer(gShaderPositionTag,  3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		glVertexAttribPointer(gShaderNormalTag,    3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*nVertices*3));
		glVertexAttribPointer(gShaderTexCoord0Tag, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(GLfloat)*nVertices*3*2));

		// Indexed
		//glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
		// Non-indexed
		glDrawArrays(GL_TRIANGLES, 0, nVertices);

		//glDisableVertexAttribArray(gShaderPositionTag);
		//glDisableVertexAttribArray(gShaderNormalTag);
		//glDisableVertexAttribArray(gShaderTexCoord0Tag);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

};


#endif