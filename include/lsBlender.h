/* 
 * File:   lsBlender.h
 * Author: miles
 *
 * Created on August 3, 2014, 6:53 PM
 */

#ifndef __LS__BLENDER_H__
#define	__LS__BLENDER_H__

#include <GL/glew.h>

#include "lsColor.h"
#include "lsSetup.h"

//-----------------------------------------------------------------------------
//      Enumerations
//-----------------------------------------------------------------------------
/**
 * Blending equations that can be used when rendering.
 */
enum : int {
    LS_BLEND = GL_BLEND
};

/**
 * Blending colors
 */
enum ls_blend_color_t : int {
    LS_BLEND_COLOR = GL_BLEND_COLOR
};

/**
 * Blending equations
 */
enum ls_blend_eq_t : int {
    LS_BLEND_ADD            = GL_FUNC_ADD,
    LS_BLEND_SUB            = GL_FUNC_SUBTRACT,
    LS_BLEND_REV_SUB        = GL_FUNC_REVERSE_SUBTRACT,
    LS_BLEND_MIN            = GL_MIN,
    LS_BLEND_MAX            = GL_MAX,
    LS_DEFAULT_BLEND_EQU    = GL_FUNC_ADD
};

/**
 * Blending Equation Color
 */
enum ls_blend_equ_color_t : int {
    LS_BLEND_EQUATION_RGB   = GL_BLEND_EQUATION_RGB,
    LS_BLEND_EQUATION_ALPHA = GL_BLEND_EQUATION_ALPHA
};

/**
 * Blend functions which help determine the type of blending that's applied to
 * a renderable object.
 */
enum ls_blend_func_t : int {
    LS_ZERO                     = GL_ZERO,
    LS_ONE                      = GL_ONE,
    LS_SRC_COLOR                = GL_SRC_COLOR,
    LS_ONE_MINUS_SRC_COLOR      = GL_ONE_MINUS_SRC_COLOR,
    LS_DST_COLOR                = GL_DST_COLOR,
    LS_ONE_MINUS_DST_COLOR      = GL_ONE_MINUS_DST_COLOR,
    LS_SRC_ALPHA                = GL_SRC_ALPHA,
    LS_ONE_MINUS_SRC_ALPHA      = GL_ONE_MINUS_SRC_ALPHA,
    LS_DST_ALPHA                = GL_DST_ALPHA,
    LS_ONE_MINUS_DST_ALPHA      = GL_ONE_MINUS_DST_ALPHA,
    LS_CONST_COLOR              = GL_CONSTANT_COLOR,
    LS_ONE_MINUS_CONST_COLOR    = GL_ONE_MINUS_CONSTANT_COLOR,
    LS_CONST_ALPHA              = GL_CONSTANT_ALPHA,
    LS_ONE_MINUS_CONST_ALPHA    = GL_ONE_MINUS_CONSTANT_ALPHA
};

/**
 * Blending Function Color
 */
enum ls_blend_func_color_t : int {
    LS_BLEND_SRC_RGB    = GL_BLEND_SRC_RGB,
    LS_BLEND_DST_RGB    = GL_BLEND_DST_RGB,
    LS_BLEND_SRC_ALPHA  = GL_BLEND_SRC_ALPHA,
    LS_BLEND_DST_ALPHA  = GL_BLEND_DST_ALPHA
};

//-----------------------------------------------------------------------------
//      Classes
//-----------------------------------------------------------------------------
/**
 * The blender manages the current blend state within OpenGL.
 */
class lsBlender {
    private:
        /**
         * The color to be used when performing blending operations in OpenGL.
         */
        lsColor blendCol = lsBlank;
        
        /**
         * Class Member that's used to set the RGB blending equation in OpenGL.
         */
        ls_blend_eq_t rgbBlendEq = LS_BLEND_ADD;
        
        /**
         * Class Member that's used to set the alpha blending equation in OpenGL
         */
        ls_blend_eq_t alphaBlendEq = LS_BLEND_ADD;
        
        /**
         * Class Member that's used to set the source RGB blending function in
         * OpenGL.
         */
        ls_blend_func_t srcRgbBlendFunc = LS_ONE;
        
        /**
         * Class Member that's used to set the destination RGB blending function
         * in OpenGL.
         */
        ls_blend_func_t dstRgbBlendFunc = LS_ZERO;
        
        /**
         * Class Member that's used to set the source alpha blending function in
         * OpenGL.
         */
        ls_blend_func_t srcAlphaBlendFunc = LS_ONE;
        
        /**
         * Class Member that's used to set the destination alpha blending
         * function in OpenGL.
         */
        ls_blend_func_t dstAlphaBlendFunc = LS_ZERO;

    public:
        /**
         * Enable blending in OpenGL and apply all stored parameters (blending
         * equations, functions, and color).
         */
        void bind() const;
        
        /**
         * Disable Blending in OpenGL.
         * 
         * This method does not reset OpenGL to its defaults.
         */
        void unbind() const;
        
        /**
         * Reset all currently held blend parameters to OpenGL's defaults.
         */
        void reset();
        
        /**
         * Set the blending equation to be used within OpenGL.
         * 
         * @param ls_blend_eq_t
         */
        void setBlendEquation(ls_blend_eq_t rgbaMode = LS_BLEND_ADD);
        
        /**
         * Set the OpenGL blending equation, but with different values for RGB
         * and Alpha.
         * 
         * @param rgbMode
         * The RGB blending equation
         * 
         * @param alphaMode
         * The blending equation for the alpha channel.
         */
        void setBlendEquation(
            ls_blend_eq_t rgbMode = LS_BLEND_ADD,
            ls_blend_eq_t alphaMode = LS_BLEND_ADD
        );
        
        /**
         * Get the blending equation for the RGB channels.
         * 
         * @return ls_blend_eq_t
         */
        ls_blend_eq_t getBlendEquationRgb() const;
        
        /**
         * Get the blending equation for the alpha channel.
         * 
         * @return ls_blend_eq_t
         */
        ls_blend_eq_t getBlendEquationAlpha() const;
        
        /**
         * Set the source and destination blending function.
         * 
         * @param srcFactor
         * The blending function for the source RGBA channels.
         * 
         * @param dstFactor
         * The blending equation for the destination RGBA channels.
         */
        void setBlendFunction(
            ls_blend_func_t srcFactor = LS_ONE,
            ls_blend_func_t dstFactor = LS_ZERO
        );
        
        /**
         * Set the source and destination blending function, with separate RGB
         * and alpha functions.
         * 
         * @param srcFactorRgb
         * The blending function for the source RGB channels.
         * 
         * @param dstFactorRgb
         * The blending equation for the destination RGB channels.
         * 
         * @param srcFactorAlpha
         * The blending function for the source alpha channel.
         * 
         * @param dstFactorAlpha
         * The blending function for the destination alpha channel.
         */
        void setBlendFunction(
            ls_blend_func_t srcFactorRgb = LS_ONE,
            ls_blend_func_t dstFactorRgb = LS_ZERO,
            ls_blend_func_t srcFactorAlpha = LS_ONE,
            ls_blend_func_t dstFactorAlpha = LS_ZERO
        );
        
        /**
         * Get the current blend function from the source RGB channel
         * 
         * @return ls_blend_func_t
         */
        ls_blend_func_t getBlendFunctionSrcRgb() const;
        
        /**
         * Get the current blend function from the source alpha channel
         * 
         * @return ls_blend_func_t
         */
        ls_blend_func_t getBlendFunctionSrcAlpha() const;
        
        /**
         * Get the current blend function from the destination RGB channel
         * 
         * @return ls_blend_func_t
         */
        ls_blend_func_t getBlendFunctionDstRgb() const;
        
        /**
         * Get the current blend function from the destination alpha channel
         * 
         * @return ls_blend_func_t
         */
        ls_blend_func_t getBlendFunctionDstAlpha() const;
        
        /**
         * Set the color that OpenGL should use for blending
         * 
         * @param rgba
         * A constant reference to an lsColor that should be used during
         * blending operations within OpenGL.
         */
        void setBlendColor(const lsColor& rgba = lsBlank);
        
        /**
         * Get the current color that OpenGL is using for blending operations.
         * 
         * @return a group of four floating-point numbers, representing the
         * RGBA colors that OpenGL is using for blending.
         */
        lsColor getBlendColor() const;
};

//-----------------------------------------------------------------------------
//      Inlined Methods
//-----------------------------------------------------------------------------

/*
 * Disable OpenGL blending
 */
inline void lsBlender::unbind() const {
    glDisable(LS_BLEND);
}

/*
 * Set the blend Equation
 */
inline void lsBlender::setBlendEquation(ls_blend_eq_t rgbaMode) {
    setBlendEquation(rgbaMode, rgbaMode);
}

/*
 * Set the blend Equation but with different values for RGB and Alpha
 */
inline void lsBlender::setBlendEquation(ls_blend_eq_t rgbMode, ls_blend_eq_t alphaMode) {
    rgbBlendEq = rgbMode;
    alphaBlendEq = alphaMode;
}

/*
 * Get the blending equation for the RGB channels.
 */
inline ls_blend_eq_t lsBlender::getBlendEquationRgb() const {
    return rgbBlendEq;
}

/*
 * Get the blending equation for the alpha channel.
 */
inline ls_blend_eq_t lsBlender::getBlendEquationAlpha() const {
    return alphaBlendEq;
}

/*
 * Set the source and destination blending function/
 */
inline void lsBlender::setBlendFunction(
    ls_blend_func_t srcFactor, ls_blend_func_t dstFactor
) {
    setBlendFunction(srcFactor, dstFactor, srcFactor, dstFactor);
}

/*
 * Set the source and destination blending function, with separate RGB
 * and alpha functions.
 */
inline void lsBlender::setBlendFunction(
    ls_blend_func_t srcRgb,     ls_blend_func_t dstRgb,
    ls_blend_func_t srcAlpha,   ls_blend_func_t dstAlpha
) {
    srcRgbBlendFunc   = srcRgb;
    dstRgbBlendFunc   = dstRgb;
    srcAlphaBlendFunc = srcAlpha;
    dstAlphaBlendFunc = dstAlpha;
}

/*
 * Get the current blend function from the source RGB channel
 */
inline ls_blend_func_t lsBlender::getBlendFunctionSrcRgb() const {
    return srcRgbBlendFunc;
}

/*
 * Get the current blend function from the source alpha channel
 */
inline ls_blend_func_t lsBlender::getBlendFunctionSrcAlpha() const {
    return srcAlphaBlendFunc;
}

/*
 * Get the current blend function from the destination RGB channel
 */
inline ls_blend_func_t lsBlender::getBlendFunctionDstRgb() const {
    return dstRgbBlendFunc;
}

/*
 * Get the current blend function from the destination alpha channel
 */
inline ls_blend_func_t lsBlender::getBlendFunctionDstAlpha() const {
    return dstAlphaBlendFunc;
}

/*
 * Set the color that should be used for blending operations in OpenGL.
 */
inline void lsBlender::setBlendColor(const lsColor& rgba) {
    blendCol = rgba;
}

/*
 * Get the current color that OpenGL is using for blending operations.
 */
inline lsColor lsBlender::getBlendColor() const {
    return blendCol;
}

#endif	/* __LS__BLENDER_H__ */
