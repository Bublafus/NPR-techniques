#version 330 compatibility

// ### Variables ###
// === Uniform ===
// --- Vectors ---
uniform vec3 u_LightPos;
// --- Matrices ---
uniform mat4 u_ModelViewMatrix;
uniform mat4 u_ProjectionMatrix;
//uniform mat3 u_NormalMatrix;
uniform mat4 u_DepthMatrix; // shadow transformation (shadow -> model)
// --- Other ---
// Mode determines which fragment shader rutine to run!
// 1 = only edge
// 2 = only toon
uniform int u_Mode;

// === Varying ===
// --- In ---
layout (location = 0) in vec4 a_Vertex;
layout (location = 3) in vec4 a_Normal;
layout (location = 6) in vec2 a_MultiTexCoord0;
// --- Out ---

// === Local ===
out block{
	vec4 v_DepthCoord;
	vec4 v_LocalPosition;
	vec4 v_ViewPosition;  // in view space .w is not important (compatibility)
	vec4 v_WorldPosition; //    - || -
	vec3 v_Normal;
	vec2 v_TexCoord;
} Out;

out float vertCoord; // hatching


void main(){
	
	Out.v_DepthCoord = u_DepthMatrix * a_Vertex; // shadow

	Out.v_Normal = normalize(mat3(u_ModelViewMatrix) * a_Normal.xyz); // TODO: normal matrix
	
	Out.v_LocalPosition = a_Vertex;
	Out.v_ViewPosition  = u_ModelViewMatrix * a_Vertex;
	Out.v_WorldPosition = u_ProjectionMatrix * u_ModelViewMatrix * a_Vertex;
	gl_Position  = Out.v_WorldPosition; // if inactive geometry shader
	
    vertCoord = a_MultiTexCoord0.x; // non to interpolate by both values (send only one which is needed)
	Out.v_TexCoord = a_MultiTexCoord0.xy;

}
