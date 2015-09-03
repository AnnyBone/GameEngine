#include "../EngineBase.h"

class CMaterialTexture
{
public:
	CMaterialTexture(const char *TexturePath);

private:
	gltexture_t *Instance;

	bool Manipulated;

	float Rotate;

	MathVector2f_t Scroll;

	unsigned int
		Width, Height,
		Flags;

	MaterialTextureType_t Type;
};

CMaterialTexture::CMaterialTexture(const char * TexturePath)
{
}

class CMaterialSkin
{
public:
	CMaterialTexture Texture[VIDEO_MAX_UNITS];

	unsigned int
		Flags,
		Textures;

	MaterialProperty_t SurfaceType;
};

class CMaterial
{
public:
private:
	int 
		Identification,
		Flags,
		Skins;

	char
		Name[64],
		Path[PLATFORM_MAX_PATH];

	bool
		OverrideAlpha,
		OverrideWireframe;

	float Alpha;

	// Animation
	float AnimationSpeed;
	double AnimationTime;
	int AnimationFrame;
};

class CMaterialManager
{
public:
	CMaterialManager();
	~CMaterialManager();

	CMaterial *Load(const char *MaterialPath);

	void Shutdown();
private:
};

CMaterialManager::CMaterialManager()
{
	Con_Printf("Initializing material manager...\n");
}

CMaterialManager::~CMaterialManager()
{}

CMaterial *CMaterialManager::Load(const char *MaterialPath)
{
	return nullptr;
}

void CMaterialManager::Shutdown()
{
}