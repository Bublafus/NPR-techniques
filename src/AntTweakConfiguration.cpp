#ifndef __ANT_TWEAK_BAR_
#define __ANT_TWEAK_BAR_


#include "../common/common.h"

void initGUI() {
    // Initialize AntTweakBar GUI
    if (!TwInit(TW_OPENGL, NULL))
    {
        assert(0);
    }

    TwWindowSize(g_WindowWidth, g_WindowHeight);
    TwBar *controlBar = TwNewBar("Controls");
    TwDefine(" Controls position='10 10' size='200 320' refresh=0.1 ");

    TwAddVarCB(controlBar, "use_shaders", TW_TYPE_BOOLCPP, cbSetShaderStatus, cbGetShaderStatus, NULL, " label='shaders' key=s help='Turn programmable pipeline on/off.' ");

    // Shader panel setup
    TwAddVarRW(controlBar, "vs", TW_TYPE_BOOLCPP, &g_UseVertexShader, " group='Shaders' label='vertex' key=v help='Toggle vertex shader.' ");
    TwAddVarRW(controlBar, "gs", TW_TYPE_BOOLCPP, &g_UseGeometryShader, " group='Shaders' label='geometry' key=g help='Toggle geometry shader.' ");
    TwAddVarRW(controlBar, "fs", TW_TYPE_BOOLCPP, &g_UseFragmentShader, " group='Shaders' label='fragment' key=f help='Toggle fragment shader.' ");
    TwAddButton(controlBar, "build", cbCompileShaderProgram, NULL, " group='Shaders' label='build' key=b help='Build shader program.' ");
	//TwDefine( " Controls/Shaders readonly=true "); 

    // Render panel setup
    TwAddVarRW(controlBar, "wiremode", TW_TYPE_BOOLCPP, &g_WireMode, " group='Render' label='wire mode' key=w help='Toggle wire mode.' ");
    TwAddVarRW(controlBar, "face_culling", TW_TYPE_BOOLCPP, &g_FaceCulling, " group=Render label='face culling' key=c help='Toggle face culling.' ");

    // Scene panel setup
    TwEnumVal geometry_type[] = { 
        { ELEPHANT_GEOMETRY    , "Elephant"},
        { CUBE_GEOMETRY        , "Cube"    },
    };
    TwType geom_type = TwDefineEnum("Model", geometry_type, NUM_GEOMETRY_TYPES);
    TwAddVarRW(controlBar, "model", geom_type, &g_GeometryType, " group='Scene' keyIncr=Space help='Change model.' ");
    TwAddVarRW(controlBar, "auto-rotation", TW_TYPE_BOOLCPP, &g_SceneRotEnabled, " group='Scene' label='rotation' key=r help='Toggle scene rotation.' ");
    TwAddVarRW(controlBar, "Translate", TW_TYPE_FLOAT, &g_SceneTraZ, " group='Scene' label='translate' min=1 max=1000 step=0.5 keyIncr=t keyDecr=T help='Scene translation.' ");
    TwAddVarRW(controlBar, "SceneRotation", TW_TYPE_QUAT4F, &g_SceneRot, " group='Scene' label='rotation' open help='Toggle scene orientation.' ");
}


#endif 
