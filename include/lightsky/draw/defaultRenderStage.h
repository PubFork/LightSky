/* 
 * File:   defaultRenderStage.h
 * Author: Miles Lacey
 *
 * Created on January 20, 2015, 9:29 PM
 */

#ifndef __LS_DRAW_DEFAULT_RENDER_STAGE_H__
#define __LS_DRAW_DEFAULT_RENDER_STAGE_H__

#include "lightsky/draw/renderStage.h"
#include "lightsky/draw/shaderObject.h"
#include "lightsky/draw/shaderProgram.h"

namespace ls {
namespace draw {

/**------------------------------------
 * @brief The default render stage contains all OpenGL resources required to
 * render a scene after it has been imported from a sceneResource.
-------------------------------------*/
class defaultRenderStage final : public renderStage {
    private:
        /**
         * @brief Shader uniform to set the scene's camera in a shader.
         */
        mutable int vpMatUniformId = 0;

        /**
         * @brief Shader uniform to set a scene node's model matrix in a
         * shader.
         */
        mutable int modelMatUniformId = 0;

        /**
         * @brief Vertex shader which permits meshes to be manipulated before
         * being passed to the fragment shader.
         */
        vertexShader vertShader;

        /**
         * @brief Fragment shader which renders all transformed vertices and
         * geometry data to the currently bound framebuffer.
         */
        fragmentShader fragShader;

        /**
         * @brief The shader binary contains the linked vertex and fragment
         * shaders.
         */
        shaderProgram shaderBinary;

    public:
        /**
         * @brief Destructor
         * 
         * Calls "terminate()" if there are resources in *this which need to be
         * released.
         */
        virtual ~defaultRenderStage();

        /**
         * @brief Constructor
         * 
         * Initializes all members to their default values.
         */
        defaultRenderStage();

        /**
         * @brief Copy Constructor
         * 
         * Temporarily deleted.
         * 
         * @param rs
         * A constant reference to another defaultRenderStage object who's data
         * will be used to initialize *this.
         */
        defaultRenderStage(const defaultRenderStage& rs) = delete;

        /**
         * @brief Move Constructor
         * 
         * Move all members from the input parameter into *this without
         * performing any copies or re-initializations.
         * 
         * @param rs
         * An r-value reference to another defaultRenderStage object.
         */
        defaultRenderStage(defaultRenderStage&& rs);

        /**
         * @brief Copy Operator
         * 
         * Temporarily deleted.
         * 
         * @param rs
         * A constant reference to another defaultRenderStage object who's data
         * will be used to initialize *this.
         * 
         * @return A reference to *this.
         */
        defaultRenderStage& operator=(const defaultRenderStage& rs) = delete;

        /**
         * @brief Move Operator
         * 
         * Move all members from the input parameter into *this without
         * performing any copies or re-initializations.
         * 
         * @param rs
         * An r-value reference to another defaultRenderStage object.
         * 
         * @return A reference to *this.
         */
        defaultRenderStage& operator=(defaultRenderStage&& rs);

        /**
         * @brief Initialize all GPU-side resources in *this in order to
         * render scene data to the currently bound framebuffer.
         * 
         * @return TRUE if all data was successfully initialized, FALSE if not.
         */
        virtual bool init() override;

        /**
         * @brief Terminate *this by freeing all members and resources.
         */
        virtual void terminate() override;

        /**
         * @brief Bind *this in preparation to render a scene.
         * 
         * This method must be called in order to render a scene graph to the
         * currently bound framebuffer.
         */
        virtual void bind() override;

        /**
         * @brief Unbind *this in order to allow other OpenGL operations to be
         * used without interference.
         */
        virtual void unbind() override;

        /**
         * @brief Draw all nodes in the current scene.
         * 
         * @param scene
         * A constant reference to the scene object containing data to render.
         */
        virtual void draw(const sceneGraph& scene) override;
};

} // end draw namespace
} // end ls namespace

#endif /* __LS_DRAW_DEFAULT_RENDER_STAGE_H__ */

