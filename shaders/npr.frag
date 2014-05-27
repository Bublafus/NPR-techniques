#version 330 compatibility

// ### Defines ###
#define FIXED_FS 0
#define DIFFUSE_FS 1
#define UNIF_COLOR_FS 2
#define EDGE_FS 4
#define TOON_FS 8
#define SHADOW_FS 16
#define HATCH_FS 32
#define OIL_FS 64
#define NOISY_FS 128

#define MAX_FLOAT 1000.0
#define OIL_RADIUS 5
// ### Variables ###
// === Uniform ===
// --- Vectors ---
uniform vec3 u_LightPos;
uniform vec3 u_UniformColor;
uniform vec2 u_InvScreenSize;

// --- Samplers (textures) ---
uniform sampler2D u_Texture;
uniform sampler2D u_RenderTexture;
uniform sampler2D u_DepthTexture;
uniform sampler2D u_NoiseTexture;
uniform sampler2D u_ToonShadeTexture;
// --- Matrices ---
uniform mat4 u_DepthMatrix;
// --- Other ---
// Mode determines which fragment shader rutine to run!
uniform int u_Mode;
uniform int u_MipmapLowest;
uniform int u_TextureLOD;
// === Varying ===
// --- In ---
in block{
	vec4 v_DepthCoord;
	vec4 v_LocalPosition;
	vec4 v_ViewPosition;
	vec4 v_WorldPosition;
	vec3 v_Normal;
	vec2 v_TexCoord;
} In;

in float vertCoord;
// --- Out ---
layout (location = 0) out vec4 v_FragColor;	// for GLSL 1.2, use gl_FragColor instead
layout (location = 1) out vec4 v_FragColor1;

// === Local ===
// Edge detection
vec4 strokeColor = vec4(1.0, 1.0, 1.0, 1.0); 

mat3 sobelOperator = mat3(
	-3.0,	-10.0,	-3.0,
	0.0,	0.0,	0.0,
	3.0,	10.0,	3.0
);
float sobelTreshold = 0.6; // TODO: uniform

bool edgeShader = false;
bool toonShader = false;
float isContour;
vec4 edgeColor;
vec4 toonColor;

// ### Functions ###
float getGrey(vec3 color){
	return 0.3 * color.r + 0.59 * color.g + 0.11 * color.b;
}

float getGrey(vec4 color){
	return getGrey(color.xyz);
}


int findSobelCountour(vec2 fragCoord){
    //Find adjacent values
	mat3 adjacentValues;

	adjacentValues[0][0]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x + 1, fragCoord.y + 1) * u_InvScreenSize, 0) );
	adjacentValues[0][1]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x + 1, fragCoord.y    ) * u_InvScreenSize, 0) );
	adjacentValues[0][2]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x + 1, fragCoord.y - 1) * u_InvScreenSize, 0) );

	adjacentValues[1][0]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x    , fragCoord.y + 1) * u_InvScreenSize, 0) );
	adjacentValues[1][1]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x    , fragCoord.y    ) * u_InvScreenSize, 0) );
	adjacentValues[1][2]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x    , fragCoord.y - 1) * u_InvScreenSize, 0) );

	adjacentValues[2][0]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x - 1, fragCoord.y + 1) * u_InvScreenSize, 0) );
	adjacentValues[2][1]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x - 1, fragCoord.y    ) * u_InvScreenSize, 0) );
	adjacentValues[2][2]	= getGrey( texture(u_RenderTexture, vec2(fragCoord.x - 1, fragCoord.y - 1) * u_InvScreenSize, 0) );

	// Convolution
	// SumX = value00 * sobel00 + value01 * sobel01 + value02 * sobel02 + ... + value22 * sobel22
	// per vector multiplication (then sum it up)
	mat3 sumX;
	sumX[0] = adjacentValues[0] * sobelOperator[0];
	sumX[1] = adjacentValues[1] * sobelOperator[1];
	sumX[2] = adjacentValues[2] * sobelOperator[2];
	
	sumX[0] = sumX[0] + sumX[1] + sumX[2];
	sumX[0][0] = sumX[0][0] + sumX[0][1] + sumX[0][2];

	// Y requires transposing matrix. It works like this: 
	// SumY = value00 * sobel00 + value01 * sobel10 + value02 * sobel20 + ... + value22 * sobel22
	sobelOperator = transpose(sobelOperator);
	mat3 sumY;
	sumY[0] = adjacentValues[0] * sobelOperator[0];
	sumY[1] = adjacentValues[1] * sobelOperator[1];
	sumY[2] = adjacentValues[2] * sobelOperator[2];
	
	sumY[0] = sumY[0] + sumY[1] + sumY[2];
	sumY[0][0] = sumY[0][0] + sumY[0][1] + sumY[0][2];

	// sqrt(x^2 + y^2);
	// distance(sumX[0][0], sumY[0][0]); // works worse (some artifacts in the contour)
	float dist = sqrt(sumX[0][0] * sumX[0][0] + sumY[0][0] * sumY[0][0]);
	
	if(dist > sobelTreshold){
		return 0; // adjacent fragments do not differ that much
	}
	return 1; // adjacent fragments differ (there will be contour)
}



// === Main ===
void main(){
    vec3 normal = normalize(In.v_Normal);
    vec3 eye	= normalize(-In.v_ViewPosition.xyz);
    vec3 lightRay		= normalize(u_LightPos - In.v_ViewPosition.xyz);
    vec3 reflectedRay	= normalize(-reflect(lightRay, normal));
	
	float diffuse  = max( dot(normal, lightRay), 0.0); // must be positive
    //float specular = pow( max( dot(reflectedRay, eye), 0.0), 32);
	
	// Use texture with shading
	vec4 texColor = texture(u_Texture, In.v_TexCoord, 0);
	vec4 texColorLowest = texture(u_Texture, In.v_TexCoord, u_TextureLOD);

	vec2 FragCoord = gl_FragCoord.xy;
	
	// Color which go throug the whole effect pipeline. final color
	vec4 color = vec4(1.0, 1.0, 1.0, 1.0);


	if(u_Mode == FIXED_FS){
		color = texColor;
	}

	if(u_Mode == DIFFUSE_FS){ // TODO: bit &!
		color = texColor * diffuse;
	}
	
	if((u_Mode & EDGE_FS) == EDGE_FS){ // edge detection
		edgeShader = true;		
		
		isContour = findSobelCountour(FragCoord);

		edgeColor = (strokeColor * diffuse) * isContour;
	}

	if((u_Mode & TOON_FS) == TOON_FS){ // (toon) cel shader
		toonShader = true;

		float greyPointer = getGrey(texColorLowest * diffuse);
		//float number = texture(u_ToonShadeTexture, vec2(greyPointer, 0.0)).x;
		float number; // nicer results so far
		if(greyPointer < 0.1){
			number = 0.3;
		}else if(greyPointer < 0.2){
			number = 0.5;
		}else if(greyPointer < 0.3){
			number = 0.7;
		}else{
			number = 1.0;
		}
		toonColor = texColorLowest * number;
	}
	
	// There can be added a color which replaces toon color (uniform variable)
	if((u_Mode & UNIF_COLOR_FS) == UNIF_COLOR_FS){
		toonColor = vec4(u_UniformColor,1.0) * diffuse;
		toonShader = true; // lie :)
	}

	if(toonShader && edgeShader){
		color = mix(toonColor, edgeColor, (1-isContour)); 
	}else{
		if(toonShader){
			color = toonColor;
		}
		if(edgeShader){
			color = edgeColor;
		}	
	}
	

	if(u_Mode == HATCH_FS){
		// http://www.yaldex.com/open-gl/ch18lev1sec1.html
		// Line (Signal) generation
		float dp       = length(vec2(dFdx(vertCoord), dFdy(vertCoord))); // normalize according to 
	    float logdp    = -log2(dp * 8.0); // 
	    float ilogdp   = floor(logdp);
	    float stripes  = exp2(ilogdp);

	    float sawtooth = fract((vertCoord + texColor.x * 0.01) * stripes);
	    float triangle = abs(2.0 * sawtooth - 1.0);

	    // adjust line width
	    float transition = logdp - ilogdp;
	    // taper ends
	    triangle = abs((1.0 + transition) * triangle - transition);

	    const float edgew = 0.3;// width of smooth step

		// diffuse makes slightly more light!
	    float edge0  = clamp(0.5*diffuse - edgew, 0.0, 1.0);
	    float edge1  = clamp(0.5*diffuse, 0.0, 1.0);
	    float square = 1.0 - smoothstep(edge0, edge1, triangle);
	
		color = vec4(vec3(square), 1.0) + texColorLowest; // or max
	}


	if(u_Mode == OIL_FS){ 
		// Kuwahara filter
		float blockSize = (OIL_RADIUS+1.0) * (OIL_RADIUS+1.0);
		
		vec3 oilColorAdd;
		vec3 average[4];
		vec3 square[4];
	
		for(int k = 0; k < 4; ++k) {
			average[k] = vec3(0.0);
			square[k]  = vec3(0.0);
		}
		// Left lower quadrant
		for(int i = -OIL_RADIUS; i <= 0; ++i)  {
			for(int j = -OIL_RADIUS; j <= 0; ++j)  {
				oilColorAdd = texture(u_RenderTexture, vec2(FragCoord.x + j, FragCoord.y + i) * u_InvScreenSize, 0).rgb;
				average[0] += oilColorAdd;
				square[0]  += oilColorAdd * oilColorAdd;
			}
		}
		// Left upper quadrant
		for(int i = -OIL_RADIUS; i <= 0; ++i)  {
			for(int j = 0; j <= OIL_RADIUS; ++j)  {
				oilColorAdd = texture(u_RenderTexture, vec2(FragCoord.x + j, FragCoord.y + i) * u_InvScreenSize, 0).rgb;
				average[1] += oilColorAdd;
				square[1]  += oilColorAdd * oilColorAdd;
			}
		}
		// Right upper quadrant
		for(int i = 0; i <= OIL_RADIUS; ++i)  {
			for(int j = 0; j <= OIL_RADIUS; ++j)  {
				oilColorAdd = texture(u_RenderTexture, vec2(FragCoord.x + j, FragCoord.y + i) * u_InvScreenSize, 0).rgb;
				average[2] += oilColorAdd;
				square[2]  += oilColorAdd * oilColorAdd;
			}
		}
		// Right lower quadrant
		for(int i = 0; i <= OIL_RADIUS; ++i)  {
			for(int j = -OIL_RADIUS; j <= 0; ++j)  {
				oilColorAdd = texture(u_RenderTexture, vec2(FragCoord.x + j, FragCoord.y + i) * u_InvScreenSize, 0).rgb;
				average[3] += oilColorAdd;
				square[3]  += oilColorAdd * oilColorAdd;
			}
		}

		// Which quadrat has the lowest color variance (smallest spread of values)
		float minColorVariance = MAX_FLOAT;
		for(int k = 0; k < 4; ++k) {
			average[k] /= blockSize; // avarage it!
			square[k]   = abs(square[k] / blockSize - average[k] * average[k]);

			float colorVar = getGrey( square[k] );
			if (colorVar < minColorVariance) {
				minColorVariance = colorVar;
				color = vec4(average[k], 1.0);
			}
		}
	}

	if(u_Mode == NOISY_FS){
		vec4 noiseColor = texture(u_NoiseTexture, In.v_TexCoord.xy);
		vec2 shiftedCoord = FragCoord.xy * u_InvScreenSize + vec2( noiseColor.r * 0.059, noiseColor.r * 0.03 );
		color = texture(u_RenderTexture, shiftedCoord, 0);
	}

	if((u_Mode & SHADOW_FS) == SHADOW_FS){
		if(u_Mode == SHADOW_FS){ // refactor
			color = texColor * diffuse;
		}
		// Shadow test
		vec3 texCoord = In.v_DepthCoord.xyz / In.v_DepthCoord.w; // Linear W division ()
		float shadow = (texture(u_DepthTexture, texCoord.xy, 0).x + 0.00001 > texCoord.z) ? 1.0f : 0.2f;

		color.rgb *= shadow; // shadow color mult
    }

	//v_FragColor = texColor * diffuse + specular; // texture
	//v_FragColor = diffuseColor + specular;
	v_FragColor = color; 
	// If second render target is defined than draw the depth value
	v_FragColor1 = vec4(gl_FragCoord.z, 0, 0, 0);
}

