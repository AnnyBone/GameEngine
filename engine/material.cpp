/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>
*/

#include "engine_base.h"
#include "video.h"
#include "video_shader.h"
#include "EngineScript.h"

bool material_initialized = false;

Material_t g_materials[MATERIAL_MAX];    // Global array.

#if 0
MaterialType_t material_surface_types[]=
{
    { MATERIAL_TYPE_NONE,		"default" },
    { MATERIAL_TYPE_METAL,		"metal" },
    { MATERIAL_TYPE_GLASS,		"glass" },
    { MATERIAL_TYPE_CONCRETE,	"concrete" },
    { MATERIAL_TYPE_WOOD,		"wood" },
    { MATERIAL_TYPE_DIRT,		"dirt" },
    { MATERIAL_TYPE_RUBBER,		"rubber" },
    { MATERIAL_TYPE_WATER,		"water" },
    { MATERIAL_TYPE_FLESH,		"flesh" },
    { MATERIAL_TYPE_SNOW,		"snow" },
    { MATERIAL_TYPE_MUD,		"mud" },
};
#endif

int material_count = -1;

Material_t *Material_Allocate(void);

// Global Materials
Material_t *g_mHDAccess;
Material_t *g_mMissingMaterial;
Material_t *g_mBlobShadow;
Material_t *g_mGlobalColour;
Material_t *g_mGlobalConChars;

// Console Variables
ConsoleVariable_t cv_material_simplified = {
        "material_simplified",
        (char *) "0",
        true,
        false,
        "If enabled, this will result in a very basic render for each surface using a material."
};

void Material_List(void);

void Material_Initialize(void) {
    if (material_initialized)
        return;

    Con_Printf("Initializing material system...\n");

    Cmd_AddCommand("material_list", Material_List);

    Cvar_RegisterVariable(&cv_material_simplified, NULL);

    // Must be set to initialized before anything else.
    material_initialized = true;

    // Load base materials...

    g_mMissingMaterial = Material_Load("base/missing");
    if (!g_mMissingMaterial)
        Sys_Error("Failed to load notexture material!\n");

    g_mGlobalConChars = Material_Load("engine/conchars");
    if (!g_mGlobalConChars)
        Sys_Error("Failed to load conchars material!\n");

    g_mGlobalColour = Material_Load("colours/colours");
    if (!g_mGlobalColour)
        Sys_Error("Failed to load colours material!\n");

    g_mHDAccess = Material_Load("sprites/base/access");
    g_mBlobShadow = Material_Load("engine/shadow");
}

/*	Lists all the currently active materials.
*/
void Material_List(void) {
    int i;

    Con_Printf("Listing materials...\n");

    for (i = 0; i < material_count; i++) {
        if (!g_materials[i].cName[0] && !g_materials[i].cPath[0])
            continue;

        Con_Printf(" %s (%s) (%i)\n", g_materials[i].cName, g_materials[i].cPath, g_materials[i].num_skins);
    }

    Con_Printf("\nListed %i active materials!\n", i);
}

/*
	Management
*/

Material_t *Material_Allocate(void) {
    material_count++;
    if (material_count > MATERIAL_MAX)
        Sys_Error("Failed to add new material onto global array! (%i)\n", material_count);

    memset(&g_materials[material_count], 0, sizeof(Material_t));

#ifdef _MSC_VER
#	pragma warning(suppress: 6386)
#endif
    g_materials[material_count].id = material_count;
    g_materials[material_count].fAlpha = 1.0f;
    g_materials[material_count].bind = true;

    return &g_materials[material_count];
}

/*	Clears out the specific skin.
*/
void Material_ClearSkin(Material_t *material, unsigned int skinnum) {
    MaterialSkin_t *skin = Material_GetSkin(material, skinnum);
    if (!skin)
        Sys_Error("Attempted to clear invalid skin! (%s) (%i)\n", material->cPath, skinnum);

#if 0
#ifdef _MSC_VER
#	pragma warning(suppress: 6011)
#endif
    for (unsigned int i = 0; i < skin->num_textures; i++)
        TexMgr_FreeTexture(skin->texture[i].gMap);
#endif
}

void Material_Clear(Material_t *material, bool force) {
    if (!force && (material->flags & MATERIAL_FLAG_PRESERVE))
        return;

    for (unsigned int i = 0; i < material->num_skins; i++)
        Material_ClearSkin(material, i);

    memset(material, 0, sizeof(Material_t));

    material_count--;
}

/*	Clears all the currently active materials.
*/
void Material_ClearAll(void) {
    int i;

    for (i = material_count; i > 0; i--)
        Material_Clear(&g_materials[i], false);

    // TODO: Reshuffle and move preserved to start.
}

void Material_SetSkin(Material_t *material, unsigned int skin) {
    if (!material)
        return;

    if (skin > MATERIAL_MAX_SKINS)
        Sys_Error("Invalid skin identification, should be greater than 0 and less than %i! (%i)\n", MATERIAL_MAX_SKINS,
                  skin);
    else if (skin > material->num_skins)
        Sys_Error("Attempted to get an invalid skin! (%i) (%s)\n", skin, material->cName);
    material->current_skin = skin;
}

MaterialSkin_t *Material_GetSkin(Material_t *mMaterial, unsigned int iSkin) {
    if (iSkin > MATERIAL_MAX_SKINS)
        Sys_Error("Invalid skin identification, should be greater than 0 and less than %i! (%i)\n", MATERIAL_MAX_SKINS,
                  iSkin);
    else if (iSkin > mMaterial->num_skins)
        Sys_Error("Attempted to get an invalid skin! (%i) (%s)\n", iSkin, mMaterial->cName);
    return &mMaterial->skin[iSkin];
}

/*	Get an animated skin.
*/
MaterialSkin_t *Material_GetAnimatedSkin(Material_t *mMaterial) {
    if (mMaterial->animation_time < cl.time) {
        // Increment current frame.
        mMaterial->animation_frame++;

        // If we're beyond the frame count, step back to 0.
        if (mMaterial->animation_frame >= mMaterial->num_skins)
            mMaterial->animation_frame = 0;

        mMaterial->animation_time = cl.time + ((double) mMaterial->animation_speed);
    }

    return Material_GetSkin(mMaterial, mMaterial->animation_frame);
}

/*	Returns a material from the given ID.
*/
Material_t *Material_Get(int iMaterialID) {
    int i;

    // The identification would never be less than 0, and never more than our maximum.
    if (iMaterialID < 0 || iMaterialID > MATERIAL_MAX) {
        Con_Warning("Invalid material ID! (%i)\n", iMaterialID);
        return NULL;
    }

    for (i = 0; i < material_count; i++)
        if (g_materials[i].id == iMaterialID) {
            g_materials[i].bind = true;
            return &g_materials[i];
        }

    return NULL;
}

/*	Returns true on success.
	Unfinished
*/
Material_t *Material_GetByName(const char *ccMaterialName) {
    if (ccMaterialName[0] == ' ') {
        Con_Warning("Attempted to find material, but recieved invalid material name!\n");
        return NULL;
    }

    for (int i = 0; i < material_count; i++)
        // If the material has no name, then it's not valid.
        if (g_materials[i].cName[0])
            if (!strncmp(g_materials[i].cName, ccMaterialName, sizeof(g_materials[i].cName)))
                return &g_materials[i];

    return NULL;
}

Material_t *Material_GetByPath(const char *ccPath) {
    if (ccPath[0] == ' ') {
        Con_Warning("Attempted to find material, but recieved invalid path!\n");
        return NULL;
    }

    for (int i = 0; i < material_count; i++)
        if (g_materials[i].cPath[0])
            if (!strncmp(g_materials[i].cPath, ccPath, sizeof(g_materials[i].cPath)))
                return &g_materials[i];

    return NULL;
}

XTexture *Material_LoadTexture(Material_t *material, MaterialSkin_t *mCurrentSkin, char *arg) {
    // Check if it's trying to use a built-in texture.
    if (arg[0] == '@') {
        arg++;

        if (!strcasecmp(arg, "notexture"))
            return textures::nulltexture;
        else if (!strcasecmp(arg, "lightmap")) {
            material->override_lightmap = true;
            mCurrentSkin->texture[mCurrentSkin->num_textures].mttType = MATERIAL_TEXTURE_LIGHTMAP;
            return textures::nulltexture;
        } else {
            Con_Warning("Attempted to set invalid internal texture! (%s)\n", material->cPath);
            return textures::nulltexture;
        }
    }

    PLuint texflags = XTEXTURE_FLAG_ALPHA | XTEXTURE_FLAG_MIPMAP;
    if (material->flags & MATERIAL_FLAG_PRESERVE)
        texflags |= XTEXTURE_FLAG_PRESERVE;
    if (material->flags & MATERIAL_FLAG_NEAREST)
        texflags |= XTEXTURE_FLAG_NEAREST;

    XTexture *tex = g_texturemanager->CreateTexture(arg, texflags);
    if (!tex) {
        Con_Warning("Failed to load texture! (%s) (%s)\n", arg, material->cPath);
        return textures::nulltexture;
    }

    mCurrentSkin->texture[mCurrentSkin->num_textures].uiWidth = tex->GetWidth();
    mCurrentSkin->texture[mCurrentSkin->num_textures].uiHeight = tex->GetHeight();

    return tex;
}

/*	Scripting	*/

typedef enum {
    MATERIAL_CONTEXT_UNIVERSAL,        // Everything
    MATERIAL_CONTEXT_GLOBAL,        // Material
    MATERIAL_CONTEXT_SKIN,            // Skin
    MATERIAL_CONTEXT_TEXTURE,        // Texture
    //MATERIAL_CONTEXT_SHADER,		// Shader
} MaterialContext_t;
MaterialContext_t material_currentcontext;    // Indicates that any settings applied are global.

void Material_CheckFunctions(Material_t *material);

// Material Functions...

typedef struct {
    const char *name;

    MaterialTextureType_t type;
} MaterialTextureTypeX_t;

MaterialTextureTypeX_t material_types[] =
        {
                {"diffuse",    MATERIAL_TEXTURE_DIFFUSE},    // Default
                {"detail",     MATERIAL_TEXTURE_DETAIL},    // Detail map
                {"sphere",     MATERIAL_TEXTURE_SPHERE},    // Sphere map
                {"normal",     MATERIAL_TEXTURE_NORMAL},    // Normal map
                {"fullbright", MATERIAL_TEXTURE_FULLBRIGHT}    // Fullbright map
        };

void _Material_SetTextureType(Material_t *mCurrentMaterial, MaterialContext_t mftContext, char *cArg);

void _Material_SetType(Material_t *mCurrentMaterial, MaterialContext_t context, char *cArg) {
    switch (context) {
        case MATERIAL_CONTEXT_SKIN: {
            unsigned int iMaterialType = (unsigned int) Q_atoi(cArg);

            // Ensure that the given type is valid.
            if (iMaterialType >= MATERIAL_TYPE_MAX)
                Con_Warning("Invalid material type! (%i)\n", iMaterialType);

            mCurrentMaterial->skin[mCurrentMaterial->num_skins].uiType = iMaterialType;
        }
            break;
        case MATERIAL_CONTEXT_TEXTURE:
            _Material_SetTextureType(mCurrentMaterial, context, cArg);
            break;
        default:
            Sys_Error("Invalid context!\n");
    }
}

void _Material_SetWireframe(Material_t *mCurrentMaterial, MaterialContext_t context, char *cArg) {
    if (context != MATERIAL_CONTEXT_GLOBAL) return;
    mCurrentMaterial->override_wireframe = (bool) atoi(cArg);
}

void _Material_SetLightmap(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_GLOBAL) return;
    material->override_lightmap = (bool) atoi(arg);
}

void _Material_SetAlpha(Material_t *mCurrentMaterial, MaterialContext_t context, char *cArg) {
    if (context != MATERIAL_CONTEXT_GLOBAL) return;
    mCurrentMaterial->fAlpha = strtof(cArg, NULL);
}

void _Material_SetAnimationSpeed(Material_t *mCurrentMaterial, MaterialContext_t context, char *cArg) {
    if (context != MATERIAL_CONTEXT_GLOBAL) return;
    mCurrentMaterial->animation_speed = strtof(cArg, NULL);
}

// Skin Functions...

void _Material_ParseSkin(Material_t *material, MaterialContext_t context, char *args) {
    if (context != MATERIAL_CONTEXT_GLOBAL) return;

    MaterialSkin_t *skin = Material_GetSkin(material, material->num_skins);

    CoreShaderProgram *program = nullptr;
    if (args && ((args[0] != '\0') && (args[0] != ' '))) {
        program = g_shadermanager->GetProgram(args);
        if (!program)
            Sys_Error("Shader program isn't registered! (%s)\n", args);

        // Update the skin to use the given shader.
        skin->program = program;
    }

    // Proceed to the next line.
    Script_GetToken(true);

    if (cToken[0] == '{') {
        while (true) {
            if (!Script_GetToken(true)) {
                Con_Warning("End of field without closing brace! (%s) (%i)\n", material->cPath, iScriptLine);
                break;
            }

            material_currentcontext = MATERIAL_CONTEXT_SKIN;

            if (cToken[0] == '}') {
                material->num_skins++;
                break;
            }
                // '$' declares that the following is a function.
            else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
                Material_CheckFunctions(material);
                // '%' declares that the following is a variable.
            else if (cToken[0] == SCRIPT_SYMBOL_VARIABLE) {
                if (!program)
                    continue;

                program->Enable();

                PLUniform *var = program->GetUniform(cToken + 1);
                if (!var) {
                    Con_Warning("Invalid shader uniform! (%s) (%i)\n", cToken, iScriptLine);
                    continue;
                }

                Script_GetToken(false);

                switch (var->type) {
                    case PL_UNIFORM_DOUBLE:
                        program->SetUniformVariable(var, std::strtod(cToken, NULL));
                        break;
                    case VL_UNIFORM_FLOAT:
                        program->SetUniformVariable(var, std::strtof(cToken, NULL));
                        break;
                    case VL_UNIFORM_BOOL:
                        if (!strncmp(cToken, "true", sizeof(cToken)))
                            strncpy(cToken, "1", sizeof(cToken));
                        else if (strncmp(cToken, "false", sizeof(cToken)))
                            strncpy(cToken, "0", sizeof(cToken));
                    case VL_UNIFORM_INT:
                        program->SetUniformVariable(var, std::atoi(cToken));
                        break;
                    case VL_UNIFORM_TEXTURE2D: {
                        program->SetUniformVariable(var, skin->num_textures);

                        MaterialTexture_t *texture = &skin->texture[skin->num_textures];
                        memset(texture, 0, sizeof(MaterialTexture_t));
                        if (skin->num_textures > 0) // If we have more textures, use decal mode.
                            texture->env_mode = PL_TEXTUREMODE_DECAL;
                        else // By default textures are modulated... Inherited Quake behaviour, yay.
                            texture->env_mode = PL_TEXTUREMODE_MODULATE;
                        texture->scale = 1;

                        texture->instance = Material_LoadTexture(material, skin, cToken);
                        skin->num_textures++;
                    }
                    case VL_UNIFORM_UINT: {
                        unsigned int _val = (unsigned int) std::strtoul(cToken, NULL, 0);
                        program->SetUniformVariable(var, _val);
                    }
                        break;
                    case VL_UNIFORM_VEC2: {
                        plVector2f_t vec = {0};
                        if (sscanf(cToken, "%f %f", &vec[0], &vec[1]) < 2)
                            Con_Warning("Field did not return expected number of arguments! (%s) (%i)\n", cToken,
                                        iScriptLine);
                        program->SetUniformVariable(var, vec);
                    }
                        break;
                    case VL_UNIFORM_VEC3: {
                        plVector3f_t vec = {0};
                        if (sscanf(cToken, "%f %f %f", &vec[0], &vec[1], &vec[2]) < 3)
                            Con_Warning("Field did not return expected number of arguments! (%s) (%i)\n", cToken,
                                        iScriptLine);
                        program->SetUniformVariable(var, vec);
                    }
                        break;
                    case VL_UNIFORM_VEC4: {
                        plVector4f_t vec = {0};
                        if (sscanf(cToken, "%f %f %f %f", &vec[0], &vec[1], &vec[2], &vec[3]) < 4)
                            Con_Warning("Field did not return expected number of arguments! (%s) (%i)\n", cToken,
                                        iScriptLine);
                        program->SetUniformVariable(var, vec);
                    }
                        break;
                    default:
                        Con_Warning("Unsupported or invalid data type! (%s) (%i)\n", cToken, iScriptLine);
                }

                program->Disable();
            } else {
                Con_Warning("Invalid field! (%s) (%i) (%s)\n", cToken, iScriptLine, material->cPath);
                break;
            }
        }
    } else
        Con_Warning("Invalid skin, no opening brace! (%s) (%i)\n", material->cPath, iScriptLine);
}

// Texture Functions...

void _Material_AddTexture(Material_t *material, MaterialContext_t context, char *cArg) {
    if (context != MATERIAL_CONTEXT_SKIN) return;

    MaterialSkin_t *curskin = Material_GetSkin(material, material->num_skins);
    if (!curskin) {
        Sys_Error("Failed to get skin!\n");
        return;
    }

#ifdef _MSC_VER
#	pragma warning(suppress: 6011)
#endif
    MaterialTexture_t *curtexture = &curskin->texture[curskin->num_textures];
    memset(curtexture, 0, sizeof(MaterialTexture_t));
    if (curskin->num_textures > 0) // If we have more textures, use decal mode.
        curtexture->env_mode = PL_TEXTUREMODE_DECAL;
    else // By default textures are modulated... Inherited Quake behaviour, yay.
        curtexture->env_mode = PL_TEXTUREMODE_MODULATE;
    curtexture->scale = 1;

    char texturepath[MAX_QPATH];
    strcpy(texturepath, cArg);

    // Get following line.
    Script_GetToken(true);

    if (cToken[0] == '{') {
        // Update state.
        material_currentcontext = MATERIAL_CONTEXT_TEXTURE;

        for (;;) {
            if (!Script_GetToken(true)) {
                Con_Warning("End of field without closing brace! (%s) (%i)\n", material->cPath, iScriptLine);
                break;
            }

            if (cToken[0] == '}') {
                curtexture->instance = Material_LoadTexture(material, curskin, texturepath);
                curskin->num_textures++;
                break;
            }
                // '$' declares that the following is a function.
            else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
                Material_CheckFunctions(material);
                // '%' declares that the following is a variable.
            else if (cToken[0] == SCRIPT_SYMBOL_VARIABLE) {
                /*	TODO:
                * Collect variable
                * Check it against internal solutions
                * Otherwise declare it, figure out where/how it's used
                */
            } else {
                Con_Warning("Invalid field! (%s) (%i)\n", material->cPath, iScriptLine);
                break;
            }
        }
    } else
#if 1
        Con_Warning("Invalid texture, no opening brace! (%s) (%i)\n", material->cPath, iScriptLine);
#else
    {
        msSkin->texture[msSkin->num_textures].gMap = Material_LoadTexture(mCurrentMaterial, msSkin, cTexturePath);
        msSkin->num_textures++;

        Script_GetToken(true);
    }
#endif
}

void _Material_SetTextureType(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_TEXTURE) return;

    MaterialSkin_t *curskin = Material_GetSkin(material, material->num_skins);
    MaterialTexture_t *curtex = &curskin->texture[curskin->num_textures];

    // Search through and copy each flag into the materials list of flags.
    for (int i = 0; i < plArrayElements(material_types); i++)
        if (strstr(arg, material_types[i].name)) {
            curtex->mttType = material_types[i].type;
            switch (curtex->mttType) {
                case MATERIAL_TEXTURE_DETAIL:
                    curtex->env_mode = PL_TEXTUREMODE_COMBINE;
                    curtex->scale = cv_video_detailscale.value;
                    break;
                case MATERIAL_TEXTURE_FULLBRIGHT:
                    curtex->env_mode = PL_TEXTUREMODE_ADD;
                    break;
                case MATERIAL_TEXTURE_SPHERE:
                    curtex->env_mode = PL_TEXTUREMODE_COMBINE;
                    break;
                default:
                    break;
            }
            break;
        }
}

void _Material_SetTextureScroll(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_TEXTURE) return;

    plVector2f_t scroll = {0};
    // Ensure there's the correct number of arguments being given.
    if (sscanf(arg, "%f %f", &scroll[0], &scroll[1]) != 2)
        // Otherwise throw us an error.
        Sys_Error("Invalid arguments! (%s) (%i)", material->cPath, iScriptLine);

    // Get the current skin.
    MaterialSkin_t *curskin = Material_GetSkin(material, material->num_skins);
    // Apply the scroll variables.
    curskin->texture[curskin->num_textures].scroll[0] = scroll[0];
    curskin->texture[curskin->num_textures].scroll[1] = scroll[1];

    // Optimisation; let the rendering system let us know to manipulate the matrix for this texture.
    curskin->texture[curskin->num_textures].matrixmod = true;
}

void _Material_SetTextureScale(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_TEXTURE) return;
    MaterialSkin_t *skin = Material_GetSkin(material, material->num_skins);
    skin->texture[skin->num_textures].scale = strtof(arg, NULL);
}

typedef struct {
    const char *name;

    PLTextureEnvironmentMode mode;
} MaterialTextureEnvironmentModeType_t;

MaterialTextureEnvironmentModeType_t material_textureenvmode[] =
        {
                {"add",      PL_TEXTUREMODE_ADD},
                {"modulate", PL_TEXTUREMODE_MODULATE},
                {"decal",    PL_TEXTUREMODE_DECAL},
                {"blend",    PL_TEXTUREMODE_BLEND},
                {"replace",  PL_TEXTUREMODE_REPLACE},
                {"combine",  PL_TEXTUREMODE_COMBINE}
        };

void _Material_SetTextureEnvironmentMode(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_TEXTURE) return;

    MaterialSkin_t *sCurrentSkin = Material_GetSkin(material, material->num_skins);
    for (int i = 0; i < plArrayElements(material_textureenvmode); i++)
        if (!strncmp(material_textureenvmode[i].name, arg, strlen(material_textureenvmode[i].name))) {
            sCurrentSkin->texture[sCurrentSkin->num_textures].env_mode = material_textureenvmode[i].mode;
            return;
        }

    Con_Warning("Invalid texture environment mode! (%s) (%s)\n", arg, material->cName);
}

void _Material_SetRotate(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_TEXTURE) return;

    // Get the current skin.
    MaterialSkin_t *msSkin = Material_GetSkin(material, material->num_skins);
    // Apply the rotate variable.
    msSkin->texture[msSkin->num_textures].fRotate = strtof(arg, NULL);

    // Optimisation; let the rendering system let us know to manipulate the matrix for this texture.
    msSkin->texture[msSkin->num_textures].matrixmod = true;
}

void _Material_SetAdditive(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_SKIN) return;

    if (atoi(arg) == PL_TRUE)
        material->skin[material->num_skins].uiFlags |= MATERIAL_FLAG_ADDITIVE | MATERIAL_FLAG_BLEND;
    else
        material->skin[material->num_skins].uiFlags &= ~MATERIAL_FLAG_ADDITIVE | MATERIAL_FLAG_BLEND;
}

void _Material_SetBlend(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_SKIN) return;

    if (atoi(arg) == PL_TRUE)
        material->skin[material->num_skins].uiFlags |= MATERIAL_FLAG_BLEND;
    else
        material->skin[material->num_skins].uiFlags &= ~MATERIAL_FLAG_BLEND;
}

void _Material_SetAlphaTest(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_SKIN) return;

    if (atoi(arg) == PL_TRUE)
        material->skin[material->num_skins].uiFlags |= MATERIAL_FLAG_ALPHA;
    else
        material->skin[material->num_skins].uiFlags &= ~MATERIAL_FLAG_ALPHA;
}

void _Material_SetAlphaTrick(Material_t *material, MaterialContext_t context, char *arg) {
    if (context != MATERIAL_CONTEXT_SKIN) return;

    if (atoi(arg) == PL_TRUE)
        material->skin[material->num_skins].uiFlags |= MATERIAL_FLAG_ALPHATRICK;
    else
        material->skin[material->num_skins].uiFlags &= ~MATERIAL_FLAG_ALPHATRICK;
}

// Universal Functions...

typedef struct {
    int flags;

    const char *ccName;

    MaterialContext_t mftContext;
} MaterialFlag_t;

MaterialFlag_t material_flags[] =
        {
                // Global
                {MATERIAL_FLAG_PRESERVE, "PRESERVE", MATERIAL_CONTEXT_GLOBAL},
                {MATERIAL_FLAG_ANIMATED, "ANIMATED", MATERIAL_CONTEXT_GLOBAL},
                {MATERIAL_FLAG_MIRROR,   "MIRROR",   MATERIAL_CONTEXT_GLOBAL},
                {MATERIAL_FLAG_WATER,    "WATER",    MATERIAL_CONTEXT_GLOBAL},

                // Skin
                {MATERIAL_FLAG_NEAREST,  "NEAREST",  MATERIAL_CONTEXT_SKIN},
                {MATERIAL_FLAG_BLEND,    "BLEND",    MATERIAL_CONTEXT_SKIN},
                {MATERIAL_FLAG_BLEND | MATERIAL_FLAG_ADDITIVE, "ADDITIVE", MATERIAL_CONTEXT_SKIN},
                {MATERIAL_FLAG_ALPHA,    "ALPHA",    MATERIAL_CONTEXT_SKIN},
        };

/*	Set flags for the material.
*/
void _Material_SetFlags(Material_t *material, MaterialContext_t context, char *arg) {
    // Search through and copy each flag into the materials list of flags.
    for (int i = 0; i < plArrayElements(material_flags); i++) {
        if (strstr(arg, material_flags[i].ccName)) {
            if (material_flags[i].mftContext != context)
                continue;

            switch (context) {
                case MATERIAL_CONTEXT_GLOBAL:
                    if (material_flags[i].flags == MATERIAL_FLAG_ANIMATED)
                        material->animation_time = 0;

                    material->flags |= material_flags[i].flags;
                    break;
                case MATERIAL_CONTEXT_SKIN:
                    material->skin[material->num_skins].uiFlags |= material_flags[i].flags;
                    break;
                case MATERIAL_CONTEXT_TEXTURE:
                    material->skin[material->num_skins].texture
                    [material->skin[material->num_skins].num_textures].uiFlags |= material_flags[i].flags;
                    break;
                default:
                    Con_Warning("Invalid context! (%s) (%s) (%i) (%i)\n", material->cName,
                                material_flags[i].ccName, context, iScriptLine);
            }
            return;
        }
    }

    // Warn us if we didn't find anything.
    Con_Warning("Invalid flag! (%s) (%i) (%i)", material->cName, context, iScriptLine);
}

typedef struct {
    const char *key;

    void (*Function)(Material_t *material, MaterialContext_t context, char *arg);

    MaterialContext_t type;
} MaterialKey_t;

MaterialKey_t material_fixed_functions[] =
        {
                // Universal
                {"flags",              _Material_SetFlags,                  MATERIAL_CONTEXT_UNIVERSAL},
                {"type",               _Material_SetType,                   MATERIAL_CONTEXT_UNIVERSAL},

                // Material
                {"override_wireframe", _Material_SetWireframe,              MATERIAL_CONTEXT_GLOBAL},
                {"override_lightmap",  _Material_SetLightmap,               MATERIAL_CONTEXT_GLOBAL},
                {"animation_speed",    _Material_SetAnimationSpeed,         MATERIAL_CONTEXT_GLOBAL},
                {"skin",               _Material_ParseSkin,                 MATERIAL_CONTEXT_GLOBAL},
                {"alpha",              _Material_SetAlpha,                  MATERIAL_CONTEXT_GLOBAL},

                // Skin
                {"map",                _Material_AddTexture,                MATERIAL_CONTEXT_SKIN},
                {"texture",            _Material_AddTexture,                MATERIAL_CONTEXT_SKIN},
                {"additive",           _Material_SetAdditive,               MATERIAL_CONTEXT_SKIN},
                {"blend",              _Material_SetBlend,                  MATERIAL_CONTEXT_SKIN},
                {"alpha_test",         _Material_SetAlphaTest,              MATERIAL_CONTEXT_SKIN},
                {"alpha_trick",        _Material_SetAlphaTrick,             MATERIAL_CONTEXT_SKIN},

                // Shader

                // Texture
                {"scroll",             _Material_SetTextureScroll,          MATERIAL_CONTEXT_TEXTURE},
                {"rotate",             _Material_SetRotate,                 MATERIAL_CONTEXT_TEXTURE},
                {"scale",              _Material_SetTextureScale,           MATERIAL_CONTEXT_TEXTURE},
                {"env_mode",           _Material_SetTextureEnvironmentMode, MATERIAL_CONTEXT_TEXTURE},

                {0}
        };

void Material_CheckFunctions(Material_t *material) {
    // Find the related function.
    for (MaterialKey_t *key = material_fixed_functions; key->key; key++)
        // Remain case sensitive.
        if (!strcasecmp(key->key, cToken + 1)) {
            /*	todo
                account for texture slots etc
            */
            if ((key->type != MATERIAL_CONTEXT_UNIVERSAL) && (material_currentcontext != key->type))
                Sys_Error("Attempted to call a function within the wrong context! (%s) (%s) (%i)\n",
                          cToken, material->cPath, iScriptLine);

            Script_GetToken(false);

            key->Function(material, material_currentcontext, cToken);
            return;
        }

    Con_Warning("Unknown function! (%s) (%s) (%i)\n", cToken, material->cPath, iScriptLine);
}

#if 0

typedef struct
{
    const char				*key;
    int						offset;
    DataType_t				type;
    MaterialContext_t		context;
    void					(*Function)(Material_t *material, MaterialContext_t context, char *arg);
} MaterialFunction_t;

MaterialFunction_t material_fixed_variables[] =
{
    { "blend", 0, DATA_BOOLEAN, MATERIAL_CONTEXT_SKIN },
};

/*	TODO: replacement for the above
*/
void Material_ParseFunction(Material_t *material)
{
    // Find the related function.
    for (MaterialFunction_t *entry = material_fixed_variables; entry->key; entry++)
        if (!strcasecmp(entry->key, cToken + 1))
        {
            Script_GetToken(false);

            switch (entry->type)
            {
            case DATA_BOOLEAN:
                if (!strncmp(cToken, "true", sizeof(cToken)))
                    snprintf(cToken, sizeof(cToken), "1");
                else if (!strncmp(cToken, "false", sizeof(cToken)))
                    snprintf(cToken, sizeof(cToken), "0");
                // Booleans are handled in the same way as integers, so don't break here!
            case DATA_INTEGER:
                *(int*)((uint8_t*)material + entry->offset) = atoi(cToken);
                break;
            case DATA_STRING:
                // todo
                break;
            case DATA_DOUBLE:
                *(double*)((uint8_t*)material + entry->offset) = strtod(cToken, NULL);
                break;
            case DATA_FLOAT:
                *(float*)((uint8_t*)material + entry->offset) = strtof(cToken, NULL);
                break;
            default:
                // Should never EVER happen, so we'll allow this to be obscure.
                Sys_Error("Unknown data type for material!\n");
            }
            return;
        }

    Con_Warning("Unknown function! (%s) (%s) (%i)\n", cToken, material->cPath, iScriptLine);
}

#endif

/*	Loads and parses material.
	Returns false on complete failure.
*/
Material_t *Material_Load(const char *path) {
    char path1[PLATFORM_MAX_PATH], material_name[64] = { 0 };

    // Ensure that the given material names are correct!
    if (path[0] == ' ')
        Sys_Error("Invalid material path! (%s)\n", path);

    if (!material_initialized) {
        Con_Warning("Attempted to load material, before initialization! (%s)\n", path);
        return NULL;
    }

    // Update the given path with the base path plus extension.
    sprintf(path1, "%s%s.material", g_state.path_materials, path);

    // Check if it's been cached already...
    Material_t *material = Material_GetByPath(path1);
    if (material)
        return material;

    void *data = COM_LoadHeapFile(path1);
    if (!data) {
        Con_Warning("Failed to load material! (%s) (%s)\n", path1, path);
        return NULL;
    }

    Script_StartTokenParsing((char *) data);

    if (!Script_GetToken(true)) {
        Con_Warning("Failed to get initial token! (%s) (%i)\n", path, iScriptLine);
        return NULL;
    } else if (cToken[0] != '{') {
        // Copy over the given name.
        strncpy(material_name, cToken, sizeof(material_name));
        if (material_name[0] == ' ')
            Sys_Error("Invalid material name!\n");

        // Check if it's been cached already...
        material = Material_GetByName(material_name);
        if (material) {
            Con_Warning("Attempted to load duplicate material! (%s) (%s) vs (%s) (%s)\n",
                        path, material_name,
                        material->cPath, material->cName);

            free(data);

            return material;
        }

        Script_GetToken(true);

        if (cToken[0] != '{') {
            Con_Warning("Missing '{'! (%s) (%i)\n", path, iScriptLine);
            goto MATERIAL_LOAD_ERROR;
        }
    }

    // Assume that the material hasn't been cached yet, so allocate a new copy of one.
    material = Material_Allocate();
    if (!material) {
        Con_Warning("Failed to allocate material! (%s)\n", path);
        goto MATERIAL_LOAD_ERROR;
    }

    if (material_name[0])
        strncpy(material->cName, material_name, sizeof(material->cName));
    else {
        char cIn[PLATFORM_MAX_PATH];
        strncpy(cIn, path, sizeof(cIn));

        // Otherwise just use the filename.
        ExtractFileBase(cIn, material->cName);
    }

    strncpy(material->cPath, path, sizeof(material->cPath));

    for (;;) {
        if (!Script_GetToken(true)) {
            Con_Warning("End of field without closing brace! (%s) (%i)\n", path, iScriptLine);
            goto MATERIAL_LOAD_ERROR;
        }

        material_currentcontext = MATERIAL_CONTEXT_GLOBAL;

        // End
        if (cToken[0] == '}') {
            // TODO: Load material data at the END!

            return material;
        }
            // Start
        else if (cToken[0] == SCRIPT_SYMBOL_FUNCTION)
            Material_CheckFunctions(material);
    }

    MATERIAL_LOAD_ERROR:
    free(data);

    return NULL;
}

bool Material_Precache(const char *path) {
    Material_t *material = Material_Load(path);
    if (!material) {
        Con_Warning("Failed to load material! (%s)\n", path);
        return false;
    }

    return true;
}

/*
	Rendering
*/

plEXTERN_C_START

extern ConsoleVariable_t gl_fullbrights;

plEXTERN_C_END

/*	TODO: Replace Material_Draw with this!
*/
void Material_DrawObject(Material_t *material, PLDraw *object, bool ispost) {
    bool showwireframe = r_showtris.bValue;
    if ((material && material->override_wireframe) && (r_showtris.iValue == 1))
        showwireframe = false;

    if (showwireframe) {
#ifdef VL_MODE_OPENGL
        if (!ispost) {
            switch (object->primitive) {
                case VL_PRIMITIVE_LINES:
                    break;
                case VL_PRIMITIVE_TRIANGLES:
                    object->primitive_restore = VL_PRIMITIVE_TRIANGLES;
                    object->primitive = VL_PRIMITIVE_LINES;
                    break;
                default:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
        } else {
            if ((object->primitive != VL_PRIMITIVE_LINES) &&
                (object->primitive != VL_PRIMITIVE_TRIANGLES))
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            else if (object->primitive == VL_PRIMITIVE_TRIANGLES)
                object->primitive = object->primitive_restore;
        }
#endif
    }

    Material_Draw
            (
                    material,
                    object->vertices,
                    object->primitive,
                    object->numverts,
                    ispost
            );
}

// Typically called before an object is drawn.
void Material_Draw(Material_t *material, PLVertex *ObjectVertex, PLPrimitive primitive, unsigned int size,
                   bool ispost) {
    if (!material) return; // todo, handle this better... throw error?
    else if ((material->override_wireframe && (r_showtris.iValue != 1) || !material->override_wireframe) &&
             (r_lightmap_cheatsafe || r_showtris.bValue)) {
        if (!ispost) {
            // Select the first TMU.
            plSetTextureUnit(0);

            // Set it as white.
            g_mGlobalColour->skin[MATERIAL_COLOUR_WHITE].texture->instance->Bind();
        }
        return;
    } else if (cv_material_simplified.value == 1) {
        MaterialSkin_t *skin = Material_GetSkin(material, material->current_skin);
        if (!skin) {
            Sys_Error("Failed to get valid skin! (%s)\n", material->cName);
            return;
        } // return to keep compiler happy...

        MaterialTexture_t *texture = &skin->texture[0];
        if (!texture) {
            Sys_Error("Failed to get valid texture! (%s)\n", material->cName);
            return;
        } // return to keep compiler happy...
        else if (!texture->instance) {
            Sys_Error("Invalid texture instance! (%s)\n", material->cName);
            return;
        } // return to keep compiler happy...

        plSetTextureUnit(0);

        if (!ispost) texture->instance->Bind();
        else texture->instance->Unbind();
        return;
    }

    MaterialSkin_t *current_skin;
    if (material->flags & MATERIAL_FLAG_ANIMATED) {
        current_skin = Material_GetAnimatedSkin(material);
    } else {
        current_skin = Material_GetSkin(material, material->current_skin);
    }
    if (!current_skin) {
        Sys_Error("Failed to get valid skin! (%s)\n", material->cName);
        return;
    }

    // Handle any skin effects.
    if (!ispost) {
        if (current_skin->program) {
            current_skin->program->Enable();
        }

        // Handle any generic blending.
        if ((current_skin->uiFlags & MATERIAL_FLAG_BLEND) || (material->fAlpha < 1)) {
            vlDepthMask(false);

            glEnable(GL_BLEND);

            if (current_skin->uiFlags & MATERIAL_FLAG_ADDITIVE) {
                // Additive blending isn't done by default.
                plSetBlendMode(PL_BLEND_ADDITIVE);
            }
        }
            // Alpha-testing
        else if (current_skin->uiFlags & MATERIAL_FLAG_ALPHA) {
            glEnable(GL_ALPHA_TEST);
        }
    }

    MaterialTexture_t *texture = &current_skin->texture[0];
    for (unsigned int unit = 0, i = 0; i < current_skin->num_textures; i++, texture++, unit++) {
#ifdef VIDEO_LIGHTMAP_HACKS
        // Skip the lightmap, since it's manually handled.
        if (unit == VIDEO_TEXTURE_LIGHT) unit++;
#endif

        // Attempt to select the unit (if it's already selected, then it'll just return).
        plSetTextureUnit(unit);

        if (!ispost) {
            // Enable it.
            plEnableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);

            // Bind it.
            texture->instance->Bind();

            // Allow us to manipulate the texture.
            if (texture->matrixmod) {
                glMatrixMode(GL_TEXTURE);
                glLoadIdentity();
                if ((texture->scroll[0] > 0) || (texture->scroll[0] < 0) ||
                    (texture->scroll[1] > 0) || (texture->scroll[1] < 0))
                    glTranslatef(
                            texture->scroll[0] * (float) cl.time,
                            texture->scroll[1] * (float) cl.time,
                            0);
                if ((texture->fRotate > 0) || (texture->fRotate < 0))
                    glRotatef(texture->fRotate * (float) cl.time, 0, 0, 1);
                glMatrixMode(GL_MODELVIEW);
            }

            // Anything greater than the first unit, copy the coords.
            if ((unit > 0) || (texture->scale != 1)) {
                // Check if we've been given a video object to use...
                if (ObjectVertex) {
                    // Go through the whole object.
                    for (unsigned int j = 0; j < size; j++) {
                        // Copy over original texture coords.
                        Video_ObjectTexture(&ObjectVertex[j], unit,
                                // Use base texture coordinates as a reference.
                                            ObjectVertex[j].ST[0][0] * texture->scale,
                                            ObjectVertex[j].ST[0][1] * texture->scale);

                        // TODO: Modify them to the appropriate scale.

                    }
                }
            }

            if(texture->env_mode != PL_TEXTUREMODE_MODULATE) {
                plSetTextureEnvironmentMode(texture->env_mode);
            }
        }

        switch (texture->mttType) {

            case MATERIAL_TEXTURE_LIGHTMAP:
                if (!ispost) {
                    plSetTextureEnvironmentMode(PL_TEXTUREMODE_COMBINE);
#ifdef VL_MODE_OPENGL
                    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
                    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
                    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 4);
#endif
                }
#ifdef VL_MODE_OPENGL
                else
                    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
#endif
                break;

            case MATERIAL_TEXTURE_DETAIL:
                if (!ispost) {
                    if (!cv_video_drawdetail.bValue) {
                        plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
                        break;
                    }
#ifdef VL_MODE_OPENGL
                    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
                    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 2);
#endif
                }
#ifdef VL_MODE_OPENGL
                else
                    glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
#endif
                break;

            case MATERIAL_TEXTURE_FULLBRIGHT:
                if (!ispost) {
                    if (!gl_fullbrights.bValue) {
                        plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
                        break;
                    }
                }
                break;

            case MATERIAL_TEXTURE_SPHERE:
                if (!ispost) {
                    Video_GenerateSphereCoordinates();

                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                } else {
                    glDisable(GL_TEXTURE_GEN_S);
                    glDisable(GL_TEXTURE_GEN_T);
                }
                break;

            default:break;
        }

        if (ispost) {
            // Reset any manipulation within the matrix.
            if (texture->matrixmod) {
#ifdef VL_MODE_OPENGL
                glMatrixMode(GL_TEXTURE);
                glLoadIdentity();
                glTranslatef(0, 0, 0);
                glRotatef(0, 0, 0, 0);
                glMatrixMode(GL_MODELVIEW);
#endif
            }

            plSetTextureEnvironmentMode(PL_TEXTUREMODE_MODULATE);

            // Disable the texture.
            plDisableGraphicsStates(VL_CAPABILITY_TEXTURE_2D);
        }
    }

    if (ispost) {
        // Handle any generic blending.
        if ((current_skin->uiFlags & MATERIAL_FLAG_BLEND) || (material->fAlpha < 1)) {
            vlDepthMask(true);

            glDisable(GL_BLEND);

            if (current_skin->uiFlags & MATERIAL_FLAG_ADDITIVE) {
                // Return blend mode to its default.
                plSetBlendMode(PL_BLEND_DEFAULT);
            }
        }
            // Alpha-testing
        else if (current_skin->uiFlags & MATERIAL_FLAG_ALPHA) {
            glDisable(GL_ALPHA_TEST);

            if ((current_skin->uiFlags & MATERIAL_FLAG_ALPHATRICK) &&
                (cv_video_alphatrick.bValue && (size > 0))) {
                vlDepthMask(false);
                glEnable(GL_BLEND);

                // Draw the object again (don't bother passing material).
                Video_DrawObject(ObjectVertex, primitive, size, NULL, 0);

                glDisable(GL_BLEND);
                vlDepthMask(true);
            }
        }

        if (current_skin->program) {
            current_skin->program->Disable();
        }
    }
}
