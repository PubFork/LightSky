/* 
 * File:   lsFontResource.cpp
 * Author: miles
 * 
 * Created on February 16, 2014, 2:36 PM
 */

#include <algorithm>
#include <utility> // std::move
#include <string>

#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

/*
 * Freetype Error Handling
 */
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST     {
#define FT_ERROR_END_LIST       { 0, 0 } };
const struct {
    int          code;
    const char*  message;
} FT_Errors[] =
#include FT_ERRORS_H

void foo() {
}

#include "lsFontResource.h"

//-----------------------------------------------------------------------------
//      Utilities
//-----------------------------------------------------------------------------
/**
 * Helper function to load a glyph
 */
void copyGlyph(lsGlyph& pGlyph, const FT_GlyphSlot ftGlyph) {
    const FT_Glyph_Metrics& metrics = ftGlyph->metrics;
    const FT_Bitmap& ftBitmap = ftGlyph->bitmap;
    
    // These need to be divided by 64 as their measurements are
    // in "points," or, 1/64th of a pixel.
    pGlyph.size = {ftBitmap.width, ftBitmap.rows};
    
    pGlyph.bearing = {metrics.horiBearingX, metrics.horiBearingY};
    pGlyph.bearing /= 64;
    
    pGlyph.advance = {metrics.horiAdvance, metrics.vertAdvance};
    pGlyph.advance /= 64;

    // Copy the data from FreeType into the glyph
    std::copy(ftBitmap.buffer, ftBitmap.buffer+(ftBitmap.width*ftBitmap.rows), pGlyph.pData);
}

//-----------------------------------------------------------------------------
//      Font Resource Implementation
//-----------------------------------------------------------------------------
/*
 * Constructor
 */
lsFontResource::lsFontResource() :
    lsResource{}
{}

/*
 * Move Constructor
 */
lsFontResource::lsFontResource(lsFontResource&& f) :
    lsResource{}
{
    this->operator =(std::move(f));
}

/*
 * Destructor
 */
lsFontResource::~lsFontResource() {
    unload();
}

/*
 * Move operator
 */
lsFontResource& lsFontResource::operator =(lsFontResource&& f) {
    unload();
    
    pData = f.pData;
    f.pData = nullptr;
    
    dataSize = f.dataSize;
    f.dataSize = 0;
    
    numGlyphs = f.numGlyphs;
    f.numGlyphs = 0;
    
    glyphSize = f.glyphSize;
    f.glyphSize = 0;
    
    maxGlyphSize = f.maxGlyphSize;
    f.maxGlyphSize = {0, 0};

    return *this;
}

/*
 * Unload all resources
 */
void lsFontResource::unload() {
    delete [] pData;
    pData = nullptr;
    
    dataSize = 0;
    
    numGlyphs = 0;
    
    glyphSize = 0;
    
    maxGlyphSize = {0, 0};
}

/*
 * Load a font file
 */
bool lsFontResource::loadFile(const std::string& filename, unsigned pixelSize) {
    FT_Library ftLib;
    FT_Face ftFace;
    FT_Error ftErr;
    
    unload();
    
    LS_LOG_MSG("Attempting to load the font file ", filename, '.');
    
    // Initialize Freetype
    ftErr = FT_Init_FreeType(&ftLib);
    if (ftErr) {
        LS_LOG_ERR(
            "\tAn error occurred while attempting to initialize FreeType.",
            "\n\tFunction:   FT_Init_FreeType",
            "\n\tError Code: ", FT_Errors[ftErr].code,
            "\n\tFT Error:   ", FT_Errors[ftErr].message, '\n'
        );
        return false;
    }
    
    // Load the font face
    ftErr = FT_New_Face(ftLib, filename.c_str(), 0, &ftFace);
    if (ftErr) {
        LS_LOG_ERR(
            "\tUnable to load the font ", filename, '.',
            "\n\tFunction:   FT_New_Face",
            "\n\tError Code: ", FT_Errors[ftErr].code,
            "\n\tFT Error:   ", FT_Errors[ftErr].message, '\n'
        );
        FT_Done_FreeType(ftLib);
        return false;
    }
    
    // Load Unicode characters
    ftErr = FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
    if (ftErr) {
        LS_LOG_ERR(
            "\tAn error occurred while selecting Unicode characters within ", filename, '.',
            "\n\tFunction:   FT_Select_Charmap",
            "\n\tError Code: ", FT_Errors[ftErr].code,
            "\n\tFT Error:   ", FT_Errors[ftErr].message, '\n'
        );
        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLib);
        return false;
    }
    
    // Set the pixel size for each character in the font
    ftErr = FT_Set_Pixel_Sizes(ftFace, 0, pixelSize);
    if (ftErr) {
        LS_LOG_ERR("\tUnable to set the pixel size of the font ", filename, '.',
            "\n\tFunction:   FT_Set_Pixel_Sizes",
            "\n\tError Code: ", FT_Errors[ftErr].code,
            "\n\tFT Error:   ", FT_Errors[ftErr].message, '\n'
        );
        FT_Done_Face(ftFace);
        FT_Done_FreeType(ftLib);
        return false;
    }
    
    // Proceed to load the glyphs
    const bool ret = loadGlyphs(ftFace);
    
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);
    
    if (ret == false) {
        LS_LOG_ERR(
            "\tAn error occurred while attempting to load the font file ", filename, ".\n"
        );
    }
    else {
        glyphSize = pixelSize;
        LS_LOG_MSG("\tData Address:  ", this->pData,
            "\n\tByte Size:       ", this->dataSize,
            "\n\tGlyph Size:      ", this->glyphSize,
            "\n\tNum Glyphs:      ", this->numGlyphs,
            "\n\tSuccessfully loaded the font file ", filename, ".\n"
        );
    }
    
    return ret;
}

/**
 * Load glyphs from a FT_Face and return an array of metrics
 * 
 * Most of this information was found at MBSoftworks' OpenGL tutorials.
 * http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=12
 */
bool lsFontResource::loadGlyphs(FT_FaceRec_* ftFace) {
    const unsigned ftNumGlyphs = (unsigned) ftFace->num_glyphs;
    lsGlyph* const pGlyphs = new(std::nothrow) lsGlyph[ftNumGlyphs];
    
    // Dynamic array error
    if (pGlyphs == nullptr) {
        return false;
    }
    
    for (unsigned i = 0; i < ftNumGlyphs; ++i) {
        FT_Error ftErr;
        unsigned charIndex = FT_Get_Char_Index(ftFace, i);
        
        ftErr = FT_Load_Glyph(ftFace, charIndex, FT_LOAD_RENDER);
        
        if (ftErr) {
            LS_LOG_ERR(
                "\tUnable to load a glyph at index ", charIndex,
                "\n\tFunction:   FT_Load_Glyph",
                "\n\tError Code: ", FT_Errors[ftErr].code,
                "\n\tFT Error:   ", FT_Errors[ftErr].message
            );
            delete [] pGlyphs;
            return false;
        }
        
        ftErr = FT_Render_Glyph(ftFace->glyph, FT_RENDER_MODE_NORMAL);
        if (ftErr) {
            LS_LOG_ERR(
                "\tUnable to render the glyph at index ", charIndex,
                "\n\tFunction:   FT_Render_Glyph",
                "\n\tError Code: ", FT_Errors[ftErr].code,
                "\n\tFT Error:   ", FT_Errors[ftErr].message
            );
            delete [] pGlyphs;
            return false;
        }
        
        const FT_GlyphSlot ftGlyph  = ftFace->glyph;
        const FT_Bitmap* ftBitmap   = &ftGlyph->bitmap;
        const math::vec2i bmpSize   = {ftBitmap->width, ftBitmap->rows};
        const int byteSize          = bmpSize[0]*bmpSize[1];
        char* const data            = new (std::nothrow) char[byteSize];
        
        if (data == nullptr) {
            LS_LOG_ERR("\tUnable to allocate ", byteSize, " bytes for glyph data.");
            dataSize = 0;
            maxGlyphSize = math::vec2i{0,0};
            delete [] pGlyphs;
            return false;
        }
        
        pGlyphs[i].pData = data;
        
        copyGlyph(pGlyphs[i], ftGlyph);
        
        dataSize += byteSize;
        
        maxGlyphSize[0] = math::max(maxGlyphSize[0], bmpSize[0]);
        maxGlyphSize[1] = math::max(maxGlyphSize[1], bmpSize[1]);
    }
    
    pData = reinterpret_cast<char*>(pGlyphs);
    numGlyphs = ftNumGlyphs;
    
    return true;
}

/*
 * Save a file
 */
bool lsFontResource::saveFile(const std::string&) const {
    return false;
}
