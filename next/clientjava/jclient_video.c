/*	Copyright (C) 2015 OldTimes Software
*/

#include "jclient_main.h"

#include "jclient_video.h"

GLFWwindow *gMainWindow;	// Main GL window

static unsigned int	iSavedCapabilites[VIDEO_MAX_UNITS][2];

#define VIDEO_STATE_ENABLE   0
#define VIDEO_STATE_DISABLE  1

#define VIDEO_MIN_WIDTH		640
#define VIDEO_MIN_HEIGHT	480
#define VIDEO_MAX_SAMPLES	cvMultisampleMaxSamples.iValue
#define VIDEO_MIN_SAMPLES	0

bool	bVideoIgnoreCapabilities = false,
bVideoDebug = false;

MathVector2_t	**vVideoTextureArray;
MathVector3_t	*vVideoVertexArray;
MathVector4_t	*vVideoColourArray;

unsigned int	uiVideoArraySize = 32768;

void Video_DebugCommand(void);
void Video_AllocateArrays(int iSize);

/*	Initialize the renderer
*/
bool JVideo_Initialize(void)
{
	int i, iSupportedUnits;

	if (Video.bInitialized)
		return true;

	printf("Initializing video... ");

	// Set default values.
	Video.iCurrentTexture = (unsigned int)-1;	// [29/8/2012] "To avoid unnecessary texture sets" ~hogsy
	Video.bVertexBufferObject = false;
	Video.bActive = true;						// Window is intially assumed active.
	Video.bUnlocked = true;						// Video mode is initially locked.

	// All units are initially disabled.
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		Video.bUnitState[i] = false;

	Video_AllocateArrays(uiVideoArraySize);

	if (!glfwInit())
	{
		gWindow_MessageBox(CLIENTJVM_TITLE" Error", "Failed to initialize GLFW library!\n");
		
		return false;
	}

	gMainWindow = glfwCreateWindow(640, 480, "JClient", NULL, NULL);
	if (!gMainWindow)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE" Error", "Failed to create GLFW window!\n");

		return false;
	}

	glfwMakeContextCurrent(gMainWindow);

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iSupportedUnits);
	if (iSupportedUnits < VIDEO_MAX_UNITS)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE" Error", "Your system doesn't support the required number of TMUs! (%i)", iSupportedUnits);

		return false;
	}

	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &Video.fMaxAnisotropy);

	// Get any information that will be presented later.
	Video.cGLVendor = (char*)glGetString(GL_VENDOR);
	Video.cGLRenderer = (char*)glGetString(GL_RENDERER);
	Video.cGLVersion = (char*)glGetString(GL_VERSION);
	Video.cGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	// Set the default states...

	Video_EnableCapabilities(VIDEO_TEXTURE_2D);

	Video_SetBlend(VIDEO_BLEND_TWO, VIDEO_DEPTH_IGNORE);

	glClearColor(0, 0, 0, 0);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glAlphaFunc(GL_GREATER, 0.5f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glDepthRange(0, 1);
	glDepthFunc(GL_LEQUAL);

	Video_SelectTexture(0);

	Video.bInitialized = true;

	printf("DONE!\n");

	return true;
}

/*	Clears the color and depth buffers.
*/
void Video_ClearBuffer(void)
{
	glClearStencil(1);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/*
Window Management
*/

/*	Set the gamma level.
	Based on Darkplaces implementation.
*/
void Video_SetGamma(float fGamma)
{
	if (fGamma == Video.fGamma)
		return;

	glfwSetGamma(glfwGetPrimaryMonitor(), fGamma);

	Video.fGamma = fGamma;
}

/*	Get the current displays width.
*/
unsigned int Video_GetDesktopWidth(void)
{
	int iWidth;

	glfwGetWindowSize(gMainWindow,&iWidth,0);

	return iWidth;
}

/*	Get the current displays height.
*/
unsigned int Video_GetDesktopHeight(void)
{
	int iHeight;

	glfwGetWindowSize(gMainWindow, &iHeight, 0);

	return iHeight;
}

/*
	Coordinate Generation
*/

void Video_GenerateSphereCoordinates(void)
{
	// OpenGL makes this pretty easy for us (though this should probably be more abstract)...
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
}

/**/

/*	Bind our current texture.
*/
void Video_SetTexture(VideoTexture_t *vtTexture)
{
	if (!vtTexture)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Invalid texture!");

		JClient_Shutdown();
	}
	// Same as the last binded texture?
	else if (vtTexture->iID == Video.iCurrentTexture)
		return;

	Video.iCurrentTexture = vtTexture->iID;

	glBindTexture(GL_TEXTURE_2D, vtTexture->iID);

	if (bVideoDebug)
		printf("Video: Bound texture (%s) (%i)\n", vtTexture->cName, Video.uiActiveUnit);
}

/*  Changes the active blending mode.
	This should be used in conjunction with the VIDEO_BLEND mode.
*/
void Video_SetBlend(VideoBlend_t voBlendMode, VideoDepth_t vdDepthMode)
{
	if (vdDepthMode != VIDEO_DEPTH_IGNORE)
		glDepthMask(vdDepthMode);

	if (voBlendMode != VIDEO_BLEND_IGNORE)
	{
		switch (voBlendMode)
		{
		case VIDEO_BLEND_ONE:
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		case VIDEO_BLEND_TWO:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case VIDEO_BLEND_THREE:
			glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
			break;
		case VIDEO_BLEND_FOUR:
			glBlendFunc(GL_ZERO, GL_ZERO);
			break;
		default:
			printf("Unknown blend mode! (%i)\n", voBlendMode);
		}
	}

	if (bVideoDebug)
		printf("Video: Setting blend mode (%i) (%i)\n", voBlendMode, vdDepthMode);
}

/*
	Multitexturing Management
*/

/*	Conversion between our TMU selection and OpenGL.
*/
unsigned int Video_GetGLUnit(unsigned int uiTarget)
{
	unsigned int uiReturn;

	uiReturn = GL_TEXTURE0 + uiTarget;

	if (bVideoDebug)
		printf("Video: Returning TMU %i\n", uiReturn);

	return uiReturn;
}

void Video_SelectTexture(unsigned int uiTarget)
{
	if (uiTarget == Video.uiActiveUnit)
		return;

	if (uiTarget > VIDEO_TEXTURE_MAX)
		printf("Invalid texture unit! (%i)\n", uiTarget);

	glActiveTexture(Video_GetGLUnit(uiTarget));

	Video.uiActiveUnit = uiTarget;

	if (bVideoDebug)
		printf("Video: Texture Unit %i\n", Video.uiActiveUnit);
}

/*
	Object Management
*/

MathVector4_t	mvVideoGlobalColour;

void Video_ObjectTexture(VideoObject_t *voObject, unsigned int uiTextureUnit, float S, float T)
{
	voObject->vTextureCoord[uiTextureUnit][0] = S;
	voObject->vTextureCoord[uiTextureUnit][1] = T;
}

void Video_ObjectVertex(VideoObject_t *voObject, float X, float Y, float Z)
{
	voObject->vVertex[0] = X;
	voObject->vVertex[1] = Y;
	voObject->vVertex[2] = Z;
}

void Video_ObjectNormal(VideoObject_t *voObject, float X, float Y, float Z)
{
	voObject->vNormal[0] = X;
	voObject->vNormal[1] = Y;
	voObject->vNormal[2] = Z;
}

void Video_ObjectColour(VideoObject_t *voObject, float R, float G, float B, float A)
{
	voObject->vColour[pRED] = R;
	voObject->vColour[pGREEN] = G;
	voObject->vColour[pBLUE] = B;
	voObject->vColour[pALPHA] = A;
}

/*	Used to override any colour given to a video object.
	Cleared by Video_ResetCapabilities.
*/
void Video_SetColour(float R, float G, float B, float A)
{
	mvVideoGlobalColour[pRED] = R;
	mvVideoGlobalColour[pGREEN] = G;
	mvVideoGlobalColour[pBLUE] = B;
	mvVideoGlobalColour[pALPHA] = A;

	Video.bColourOverride = true;
}

/*
	Drawing
*/

/*	Reallocates video arrays.
*/
void Video_AllocateArrays(int iSize)
{
	int i;

	if (bVideoDebug)
		printf("Video: Allocating arrays...\n");

	// Check that each of these have been initialized before freeing them.
	if (vVideoVertexArray)
		free(vVideoVertexArray);
	if (vVideoColourArray)
		free(vVideoColourArray);
	if (vVideoTextureArray)
		free(vVideoTextureArray);

	vVideoTextureArray = (MathVector2_t**)malloc(VIDEO_MAX_UNITS*sizeof(MathVector2_t));
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		vVideoTextureArray[i] = (MathVector2_t*)malloc(iSize*sizeof(MathVector2_t));

	vVideoVertexArray = (MathVector3_t*)malloc(iSize*sizeof(MathVector3_t));
	vVideoColourArray = (MathVector4_t*)malloc(iSize*sizeof(MathVector4_t));

	if (!vVideoColourArray || !vVideoTextureArray || !vVideoVertexArray)
	{
		gWindow_MessageBox(CLIENTJVM_TITLE, "Failed to allocate video arrays!\n");

		JClient_Shutdown();
	}

	// Keep this up to date.
	uiVideoArraySize = iSize;
}

/*	Draw terrain.
*/
void Video_DrawTerrain(VideoObject_t *voTerrain)
{
	if (!voTerrain)
		printf("Invalid video object!\n");
}

/*  Draw a simple rectangle.
*/
void Video_DrawFill(VideoObject_t *voFill)
{
	Video_DrawObject(voFill, VIDEO_PRIMITIVE_TRIANGLE_FAN, 4);
}

/*	Draw 3D object.
TODO: Add support for VBOs ?
*/
void Video_DrawObject(VideoObject_t *voObject, VideoPrimitive_t vpPrimitiveType, unsigned int	uiVerts)
{
	unsigned int	i, j;
	GLenum			gPrimitive = 0;

	if (!voObject)
	{
		printf("Invalid video object!\n");
		JClient_Shutdown();
	}
	else if (!uiVerts)
	{
		printf("Invalid number of vertices for video object! (%i)\n", uiVerts);
		JClient_Shutdown();
	}

	if (bVideoDebug)
		printf("Video: Drawing object (%i) (%i)\n", uiVerts, vpPrimitiveType);

	bVideoIgnoreCapabilities = true;

	// Vertices count is too high for this object, bump up array sizes to manage it.
	if (uiVerts > uiVideoArraySize)
		// Double the array size to cope.
		Video_AllocateArrays(uiVerts * 2);

	// Copy everything over...
	for (i = 0; i < uiVerts; i++)
	{
		if (!Video.vmrActiveMode != VIDEO_MODE_WIREFRAME)
		{
			// Allow us to override the colour if we want/need to.
			if (Video.bColourOverride)
				Math_Vector4Copy(mvVideoGlobalColour, vVideoColourArray[i]);
			else
				Math_Vector4Copy(voObject[i].vColour, vVideoColourArray[i]);

			// Copy over coords for each active TMU.
			for (j = 0; j < VIDEO_MAX_UNITS; j++)
				if (Video.bUnitState[j])
					Math_Vector2Copy(voObject[i].vTextureCoord[j], vVideoTextureArray[j][i]);
		}
		else
			Math_Vector4Set(1.0f, vVideoColourArray[i]);

		Math_VectorCopy(voObject[i].vVertex, vVideoVertexArray[i]);
	}

	// Handle different primitive types...
	switch (Video.vmrActiveMode)
	{
	case VIDEO_MODE_DEFAULT:
		switch (vpPrimitiveType)
		{
		case VIDEO_PRIMITIVE_TRIANGLES:
			gPrimitive = GL_TRIANGLES;
		case VIDEO_PRIMITIVE_TRIANGLE_FAN_WIRE:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		case VIDEO_PRIMITIVE_TRIANGLE_FAN:
			gPrimitive = GL_TRIANGLE_FAN;
			break;
		}

		// Handle textures.

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		for (i = 0; i < VIDEO_MAX_UNITS; i++)
			if (Video.bUnitState[i])
			{
				glClientActiveTexture(Video_GetGLUnit(i));
				glTexCoordPointer(2, GL_FLOAT, 0, vVideoTextureArray[i]);
			}
		break;
	case VIDEO_MODE_WIREFRAME:
		switch (vpPrimitiveType)
		{
		case VIDEO_PRIMITIVE_TRIANGLES:
			gPrimitive = GL_LINES;
		case VIDEO_PRIMITIVE_TRIANGLE_FAN_WIRE:
		case VIDEO_PRIMITIVE_TRIANGLE_FAN:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			gPrimitive = GL_TRIANGLE_FAN;
			break;
		}
		break;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vVideoVertexArray);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, vVideoColourArray);

	glDrawArrays(gPrimitive, 0, uiVerts);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	switch (Video.vmrActiveMode)
	{
	case VIDEO_MODE_DEFAULT:
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		break;
	case VIDEO_MODE_WIREFRAME:
		if (vpPrimitiveType == VIDEO_PRIMITIVE_TRIANGLE_FAN_WIRE)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}

	bVideoIgnoreCapabilities = false;
}

/*
	Capabilities management
*/

typedef struct
{
	unsigned	int	    uiFirst,
		uiSecond;

	const       char    *ccIdentifier;
} VideoCapabilities_t;

VideoCapabilities_t	vcCapabilityList[] =
{
	{ VIDEO_ALPHA_TEST, GL_ALPHA_TEST, "ALPHA_TEST" },
	{ VIDEO_BLEND, GL_BLEND, "BLEND" },
	{ VIDEO_DEPTH_TEST, GL_DEPTH_TEST, "DEPTH_TEST" },
	{ VIDEO_TEXTURE_2D, GL_TEXTURE_2D, "TEXTURE_2D" },
	{ VIDEO_TEXTURE_GEN_S, GL_TEXTURE_GEN_S, "TEXTURE_GEN_S" },
	{ VIDEO_TEXTURE_GEN_T, GL_TEXTURE_GEN_T, "TEXTURE_GEN_T" },
	{ VIDEO_CULL_FACE, GL_CULL_FACE, "CULL_FACE" },
	{ VIDEO_STENCIL_TEST, GL_STENCIL_TEST, "STENCIL_TEST" },
	{ VIDEO_NORMALIZE, GL_NORMALIZE, "NORMALIZE" },

	{ 0 }
};

/*	Set rendering capabilities for current draw.
Cleared using Video_DisableCapabilities.
*/
void Video_EnableCapabilities(unsigned int iCapabilities)
{
	int	i;

	if (!iCapabilities)
		return;

	for (i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if (!vcCapabilityList[i].uiFirst)
			break;

		if (iCapabilities & VIDEO_TEXTURE_2D)
			Video.bUnitState[Video.uiActiveUnit] = true;

		if (iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if (bVideoDebug)
				printf("Video: Enabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

			if (!bVideoIgnoreCapabilities)
				// [24/2/2014] Collect up a list of the new capabilities we set ~hogsy
				iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_ENABLE] |= vcCapabilityList[i].uiFirst;

			glEnable(vcCapabilityList[i].uiSecond);
		}
	}
}

/*	Disables specified capabilities for the current draw.
*/
void Video_DisableCapabilities(unsigned int iCapabilities)
{
	int	i;

	if (!iCapabilities)
		return;

	for (i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if (!vcCapabilityList[i].uiFirst)
			break;

		if (iCapabilities & VIDEO_TEXTURE_2D)
			Video.bUnitState[Video.uiActiveUnit] = false;

		if (iCapabilities & vcCapabilityList[i].uiFirst)
		{
			if (bVideoDebug)
				printf("Video: Disabling %s (%i)\n", vcCapabilityList[i].ccIdentifier, Video.uiActiveUnit);

			if (!bVideoIgnoreCapabilities)
				// [24/2/2014] Collect up a list of the new capabilities we disabled ~hogsy
				iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_DISABLE] |= vcCapabilityList[i].uiFirst;

			glDisable(vcCapabilityList[i].uiSecond);
		}
	}
}

/*	Checks if the given capability is enabled or not.
*/
bool Video_GetCapability(unsigned int iCapability)
{
	int	i;

	if (!iCapability)
		return false;

	for (i = 0; i < sizeof(vcCapabilityList); i++)
	{
		if (!vcCapabilityList[i].uiFirst)
			break;

		if (iCapability & iSavedCapabilites[Video.uiActiveUnit][VIDEO_STATE_ENABLE])
			return true;
	}

	return false;
}

/*	Resets our capabilities.
Give an argument of true to only clear the list, not the capabilities.
Also resets active blending mode.
*/
void Video_ResetCapabilities(bool bClearActive)
{
	int i;

	if (bVideoDebug)
		printf("Video: Resetting capabilities...\n");

	Video_SelectTexture(VIDEO_TEXTURE_DIFFUSE);

	if (bClearActive)
	{
		if (bVideoDebug)
			printf("Video: Clearing active capabilities...\n");

		bVideoIgnoreCapabilities = true;

		// Set this back too...
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		Video_DisableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_ENABLE]);
		Video_EnableCapabilities(iSavedCapabilites[VIDEO_TEXTURE_DIFFUSE][VIDEO_STATE_DISABLE]);

		Video_SetBlend(VIDEO_BLEND_TWO, VIDEO_DEPTH_TRUE);

		if (Video.bColourOverride)
			Video.bColourOverride = false;

		bVideoIgnoreCapabilities = false;

		if (bVideoDebug)
			printf("Video: Finished clearing capabilities.\n");
	}

	// [7/5/2014] Clear out capability list ~hogsy
	for (i = 0; i < VIDEO_MAX_UNITS; i++)
		iSavedCapabilites[i][0] =
		iSavedCapabilites[i][1] = 0;
}

/*
	Shader Management
	Unfinished!
*/

unsigned int Video_ShaderConvertType(VideoShaderType_t vstType)
{
	switch (vstType)
	{
	case VIDEO_SHADER_FRAGMENT:
		return GL_FRAGMENT_SHADER;
	case VIDEO_SHADER_VERTEX:
		return GL_VERTEX_SHADER;
	}

	return 0;
}

/*  Simple whipped up function to demo shader processing
	then simple error management. Replace this...
*/
void Video_ShaderLoad(const char *ccPath, VideoShaderType_t vstType)
{
#ifdef VIDEO_ENABLE_SHADERS
	int             iState;
	unsigned int    uiShader;
	char			*cShaderSource;

	if (!LoadFile(ccPath, &cShaderSource))
	{
	}

	uiShader = glCreateShader(Video_ShaderConvertType(vstType));

	glShaderSource(uiShader, 1, &cShaderSource, NULL);
	glCompileShader(uiShader);

	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iState);
	if (!iState)
	{
		char cLog[512];

		glGetShaderInfoLog(uiShader, 512, NULL, cLog);

		// [12/3/2014] Spit a log out to the console ~hogsy
		Con_Warning("Failed to compile shader!\n%s", cLog);
		return;
	}

	//    glShaderSource()
#endif
}

/**/

/*	Main rendering loop.
*/
void JVideo_Frame(void)
{
	if (bVideoDebug)
		printf("Video: Start of frame\n");

	Video_ClearBuffer();

#ifdef VIDEO_ENABLE_SHADERS
	glBindFramebuffer(GL_FRAMEBUFFER, Video.uiFrameBuffer);
#endif

	if (bVideoDebug)
	{
		printf("Video: End of frame\n");

		// TODO: End of frame stats (number of calls to video_draw etc)

		bVideoDebug = false;
	}
}

void JVideo_Shutdown(void)
{
	if (!Video.bInitialized)
		return;

	// Let us know that we're shutting down the video sub-system...
	printf("Shutting down video...\n");

	glfwTerminate();

	Video.bInitialized = false;
}
