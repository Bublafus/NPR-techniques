#ifndef __GUI_H__
#define __GUI_H__

#include "globalState.h"

// ### FORWARD DECLARATIONS ###
void TW_CALL cbCompileShaderProgram(void *clientData);


void TW_CALL cbSetShaderStatus(const void *value, void *clientData){
	gUseShaders = *(bool*)(value);
	// Try to compile shader program
	if (gUseShaders){
		cbCompileShaderProgram(NULL);
	}
	//TwDefine((g_UseShaders) ? " Controls/Shaders readonly=false " : " Controls/Shaders readonly=true "); 
}

void TW_CALL cbGetShaderStatus(void *value, void *clientData){
	*(bool*)(value) = gUseShaders;
} 

//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: 
//-----------------------------------------------------------------------------
void initGUI() {
#ifdef USE_ANTTWEAKBAR
    // Initialize AntTweakBar GUI
    if (!TwInit(TW_OPENGL, NULL)){
        assert(0);
    }

    TwWindowSize(gWindowWidth, gWindowHeight);
    TwBar *controlBar = TwNewBar("Controls");
    TwDefine(" Controls position='10 10' size='200 320' refresh=0.1 ");

    TwAddVarCB(controlBar, "use_shaders", TW_TYPE_BOOLCPP, cbSetShaderStatus, cbGetShaderStatus, NULL, " label='shaders' help='Turn programmable pipeline on/off.' ");

    // Shader panel setup
    TwAddVarRW(controlBar, "vs", TW_TYPE_BOOLCPP, &gUseVertexShader, " group='Shaders' label='vertex' help='Toggle vertex shader.' ");
    TwAddVarRW(controlBar, "gs", TW_TYPE_BOOLCPP, &gUseGeometryShader, " group='Shaders' label='geometry' help='Toggle geometry shader.' ");
    TwAddVarRW(controlBar, "fs", TW_TYPE_BOOLCPP, &gUseFragmentShader, " group='Shaders' label='fragment' help='Toggle fragment shader.' ");
	TwAddVarRW(controlBar, "fs2", TW_TYPE_BOOLCPP, &gUseFixedFragmentShader, " group='Shaders' label='fixedFrag' help='Toggle fragment shader.' ");


    TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, " group='Shaders' label='build' help='Build shader program.' ");
	// TwDefine( " Controls/Shaders readonly=true "); 

    // Render panel setup
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &gWireMode, " group='Render' label='wire mode' help='Toggle wire mode.' ");
    TwAddVarRW(controlBar, "face_culling", TW_TYPE_BOOLCPP, &gFaceCulling, " group=Render label='face culling' help='Toggle face culling.' ");
	TwAddVarRW(controlBar, "buffer_tex", TW_TYPE_BOOLCPP, &gDisplayBuffTextures, " group=Render label='display tex' help='Toggle display textures' ");

	
    // Scene panel setup
    TwEnumVal geometry_type[] = { 
        { SCENE2			, "Elephant"},
        { CUBE_GEOMETRY		, "Cube"    },
		{ SCENE				, "Scene"   },
    };
	TwEnumVal effect_type[] = { 
        { E_FIXED			, "Fixed"},
		{ E_EDGE			, "Edge"},
		{ E_TOON			, "Toon"},
		{ E_SHADOW_TOON		, "ShadowsToon"},
		{ E_EDGE_TOON		, "Toon&Edge"},
        { E_HATCH			, "Hatching"},
		{ E_NOISY			, "Noise"},
		{ E_NOISY_SHADOW	, "NoiseShadow"},
		{ E_OIL				, "Oil"},
    };
    TwType geom_type = TwDefineEnum("Model", geometry_type, NUM_GEOMETRY_TYPES);
	TwType eff_type  = TwDefineEnum("Effect", effect_type, NUM_EFFECTS);
	
	TwAddVarRW(controlBar, "texlod", TW_TYPE_UINT32, &gTextureLOD, " group='Scene' label='ToonTexLOD' min=0 max=10 step=1 keyIncr=P keyDecr=O help='change level of detail of toon tex' ");
    TwAddVarRW(controlBar, "model", geom_type, &g_GeometryType, " group='Scene' keyIncr=* help='Change scene.' ");
	TwAddVarRW(controlBar, "effect", eff_type, &gCurEffect, " group='Scene' keyIncr=Space help='Change effect.' ");
    //TwAddVarRW(controlBar, "Translate", TW_TYPE_FLOAT, &cameraPosition.z, " group='Scene' label='translate' min=1 max=1000 step=0.5 keyIncr=t keyDecr=T help='Scene translation.' ");
    //TwAddVarRW(controlBar, "SceneRotation", TW_TYPE_QUAT4F, &g_SceneRot, " group='Scene' label='rotation' open help='Toggle scene orientation.' ");
#endif
}


#endif