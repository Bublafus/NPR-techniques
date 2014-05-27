#version 330 compatibility

// ### Defines ###

// ### Variables ###
// === Uniform ===
// --- Vectors ---
uniform vec3 u_LightPos;
// --- Samplers (textures) ---
uniform sampler2D u_Texture;
// === Varying ===
// --- In ---
in block{
	vec3 v_Color;
	vec4 v_Position;
	vec3 v_viewPos;
	vec3 v_Normal;
} In;
in vec2 v_TexCoord;
// --- Out ---
//out vec4 v_FragColor;
layout (location = 0) out vec4 v_FragColor;	// for GLSL 1.2, use gl_FragColor instead
layout (location = 1) out vec4 v_FragColor1;

// -- temp --
in vec4 v_DepthCoord;
uniform int u_Mode;
uniform sampler2D u_DepthTexture; // shadows
uniform mat4 u_DepthMatrix;


// ### FUNCTIONS ###
// === Main ===
void main(){
    vec3 normal = normalize(In.v_Normal);
    vec3 eye	= normalize(-In.v_viewPos);
    vec3 lightRay		= normalize(u_LightPos - In.v_viewPos);
    vec3 reflectedRay	= normalize(-reflect(lightRay, normal));
	
	float diffuse  = max( dot(normal, lightRay), 0.0); // must be positive
    float specular = pow( max( dot(reflectedRay, eye), 0.0), 32);

	vec4 tex_color = texture(u_Texture, v_TexCoord, 0);

	vec4 color = tex_color * diffuse;

		
	//v_FragColor = tex_color * diffuse;// + specular; // texture
	//color.r = shadow2D(u_DepthTexture, v_DepthCoord.xyz).x;
	v_FragColor = tex_color; //color;
	v_FragColor1 = vec4(gl_FragCoord.z, 0, 0, 0);
}

