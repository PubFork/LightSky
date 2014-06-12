/* 
 * File:   testState.cpp
 * Author: hammy
 * 
 * Created on January 19, 2014, 4:19 PM
 */

#include <string>
#include <utility>
#include <GL/glew.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <cmath>

#include "display.h"
#include "fontResource.h"
#include "imageResource.h"
#include "mesh.h"
#include "meshResource.h"
#include "testState.h"
#include "texture.h"
#include "atlas.h"

bool GAME_KEYS[512] = {false};

using math::vec2i;
using math::vec2;
using math::vec3;
using math::mat4;
using math::quat;

static const char testTextFile[] = "FiraSans-Italic.otf";

static const char testTextString[] = R"***(x
abcdefghijklmnopqrstuvwxyz
ABCDEFGHIJKLMNOPQRSTUVWXYZ
01234    56789
~!@#$%^&*()_+
`{}|:\<>?
[];',./

The [quick] (brown) fox
jumped over the slow, lazy dog! :)

)***";

/*
 * This shader uses a Logarithmic Z-Buffer, thanks to
 * http://www.gamasutra.com/blogs/BranoKemen/20090812/2725/Logarithmic_Depth_Buffer.php
 */
static const char vertShaderData[] = R"***(
#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUv;
layout (location = 2) in vec3 inNorm;
layout (location = 3) in mat4 inModelMat;

uniform mat4 vpMatrix;

out vec2 uvCoords;

void main() {
    mat4 mvpMatrix = vpMatrix * inModelMat;
    vec4 p = mvpMatrix * vec4(inPos, 1.0);
    const float C = 1.0;
    const float FAR = 100.0;
    float pz = -log(C * p.z + 1.0) / log(C * FAR + 1.0);
    
    gl_Position = vec4(p.xy, pz, p.w);
    
    uvCoords = inUv;
}
)***";

static const char fragShaderData[] = R"***(
#version 330 core

uniform sampler2D tex;

in vec2 uvCoords;
out vec4 outFragCol;

void main() {
    outFragCol = texture(tex, uvCoords);
}
)***";

// Testing Alpha Masking for font rendering.
static const char fontFS[] = R"***(
#version 330

precision lowp float;

in vec2 uvCoords;
out vec4 outFragCol;

uniform sampler2DRect texSampler;
uniform vec4 fontColor = vec4(0.0, 1.0, 1.0, 1.0);

void main() {
    float mask = texture(texSampler, uvCoords).r;
    outFragCol = fontColor*step(0.5, mask);
}
)***";

/******************************************************************************
 * Constructor & Destructor
******************************************************************************/
testState::testState() {
    SDL_StopTextInput();
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

/******************************************************************************
 * Hardware Events
******************************************************************************/
/******************************************************************************
 * Key Up Event
******************************************************************************/
void testState::onKeyboardUpEvent(const SDL_KeyboardEvent* e) {
    const SDL_Keycode key = e->keysym.sym;
    
    if (key < 0 || (unsigned)key >= LS_ARRAY_SIZE(GAME_KEYS)) {
        return;
    }
    
    if (key == SDLK_ESCAPE) {
        this->setState(state_type::GAME_STOPPED);
    }
    else {
        GAME_KEYS[key] = false;
    }
}

/******************************************************************************
 * Key Down Event
******************************************************************************/
void testState::onKeyboardDownEvent(const SDL_KeyboardEvent* e) {
    const SDL_Keycode key = e->keysym.sym;
    
    if (key < 0 || (unsigned)key >= LS_ARRAY_SIZE(GAME_KEYS)) {
        return;
    }
    
    GAME_KEYS[key] = true;
}

/******************************************************************************
 * Keyboard States
******************************************************************************/
void testState::updateKeyStates() {
    const float moveSpeed = 0.1f;
    vec3 pos = {0.f};
    
    if (GAME_KEYS[SDLK_w]) {
        pos[2] += moveSpeed;
    }
    if (GAME_KEYS[SDLK_s]) {
        pos[2] -= moveSpeed;
    }
    if (GAME_KEYS[SDLK_a]) {
        pos[0] += moveSpeed;
    }
    if (GAME_KEYS[SDLK_d]) {
        pos[0] -= moveSpeed;
    }
    
    const vec3 translation{
        math::dot(math::getAxisX(orientation), pos),
        math::dot(math::getAxisY(orientation), pos),
        math::dot(math::getAxisZ(orientation), pos)
    };
    
    const mat4& viewMatrix = matStack->getMatrix(VIEW_MATRIX);
    const mat4&& movement = math::translate(viewMatrix, translation);
    matStack->loadMatrix(VIEW_MATRIX, movement);
}

/******************************************************************************
 * Text Events
******************************************************************************/
void testState::onKeyboardTextEvent(const SDL_TextInputEvent*) {
}

/******************************************************************************
 * Window Event
******************************************************************************/
void testState::onWindowEvent(const SDL_WindowEvent* pEvent) {
    switch (pEvent->event) {
        case SDL_WINDOWEVENT_CLOSE:
            this->setState(state_type::GAME_STOPPED);
            break;
        default:
            break;
    }
}

/******************************************************************************
 * Mouse Move Event
******************************************************************************/
void testState::onMouseMoveEvent(const SDL_MouseMotionEvent* e) {
    // Prevent the orientation from drifting by keeping track of the relative mouse offset
    if (mouseX == e->xrel && mouseY == e->yrel) {
        // I would rather quit the function than have unnecessary LERPs and
        // quaternion multiplications.
        return;
    }
    
    mouseX = e->xrel;
    mouseY = e->yrel;
    
    // Get the current mouse position and LERP from the previous mouse position.
    // The mouse position is divided by the window's resolution in order to normalize
    // the mouse delta between 0 and 1. This allows for the camera's orientation to
    // be LERPed without the need for multiplying it by the last time delta.
    // As a result, the camera's movement becomes as smooth and natural as possible.
    
    const vec2i& res        = global::pDisplay->getResolution();
    const vec2&& fRes       = {(float)res[0], (float)res[1]};
    const vec2&& mouseDelta = vec2{(float)mouseX, (float)mouseY} / fRes;
    
    // Always lerp to the 
    const quat&& lerpX = math::lerp(
        quat{0.f, 0.f, 0.f, 1.f}, quat{mouseDelta[1], 0.f, 0.f, 1.f}, 1.f
    );
    
    const quat&& lerpY = math::lerp(
        quat{0.f, 0.f, 0.f, 1.f}, quat{0.f, mouseDelta[0], 0.f, 1.f}, 1.f
    );
        
    orientation *= lerpY * lerpX;
    orientation = math::normalize(orientation);
}

/******************************************************************************
 * Mouse Button Up Event
******************************************************************************/
void testState::onMouseButtonUpEvent(const SDL_MouseButtonEvent*) {
}

/******************************************************************************
 * Mouse Button Down Event
******************************************************************************/
void testState::onMouseButtonDownEvent(const SDL_MouseButtonEvent*) {
}

/******************************************************************************
 * Mouse Wheel Event
******************************************************************************/
void testState::onMouseWheelEvent(const SDL_MouseWheelEvent*) {
}

/******************************************************************************
 * termination assistant
******************************************************************************/
void testState::terminate() {
        delete matStack;
        matStack = nullptr;
        delete pScene;
        pScene = nullptr;
}

/******************************************************************************
 * Create the draw models that will be used for rendering
******************************************************************************/
bool testState::generateDrawModels() {
    atlas* pAtlas = nullptr;
    mesh* pMesh = nullptr;
    texture* pTexture = nullptr;
    
    // test model 1
    drawModel* const pMeshModel = new drawModel{};
    if (pMeshModel == nullptr) {
        LOG_ERR("Unable to generate test draw model 1");
        return false;
    }
    else {
        pScene->manageModel(pMeshModel);
        pMesh = pScene->getMeshList()[0];
        pTexture = pScene->getTextureList()[0];
        pMeshModel->setMesh(pMesh);
        pMeshModel->setTexture(pTexture);
    }
    
    // test model 2
    drawModel* const pTextModel = new drawModel{};
    if (pTextModel == nullptr) {
        LOG_ERR("Unable to generate test draw model 2");
        return false;
    }
    else {
        pScene->manageModel(pTextModel);
        pMesh = pScene->getMeshList()[1];
        pAtlas = pScene->getAtlasList()[0];
        pTextModel->setMesh(pMesh);
        pTextModel->setTexture(&(pAtlas->getTexture()));
    }
    
    return true;
}

/******************************************************************************
 * Starting state
******************************************************************************/
bool testState::onStart() {
    matStack = new matrixStack{};
    pScene = new sceneManager{};
    
    if (!matStack || !pScene) {
        LOG_ERR("An error occurred while initializing the scene.");
        terminate();
        return false;
    }
    
    meshResource* pMeshLoader   = new meshResource{};
    imageResource* pImgLoader   = new imageResource{};
    fontResource* pFontLoader   = new fontResource{};
    mesh* pMesh                 = new mesh{};
    mesh* pText                 = new mesh{};
    texture* pTex               = new texture{TEX_2D};
    atlas* pAtlas               = new atlas{};
    bool ret                    = true;
    
    if (!pMeshLoader
    ||  !pImgLoader
    ||  !pFontLoader
    ||  !pMeshLoader->loadQuad()
    ||  !pMesh->init(*pMeshLoader)
    ||  !pImgLoader->loadFile("test_img.jpg")
    ||  !pTex->init(0, GL_RGB, pImgLoader->getPixelSize(), GL_BGR, GL_UNSIGNED_BYTE, pImgLoader->getData())
    ||  !pFontLoader->loadFile(testTextFile, LS_DEFAULT_FONT_SIZE)
    ||  !pAtlas->load(*pFontLoader)
    ||  !pText->init(*pAtlas, testTextString)
    ) {
        ret = false;
    }
    
    if (ret) {
        pScene->manageMesh(pMesh); // test data at the mesh index 0
        pScene->manageMesh(pText);  // test data at the mesh index 1
        pScene->manageTexture(pTex);  // test texture at the mesh index 0
        pScene->manageAtlas(pAtlas);  // test atlas at the mesh index 0
    }
    
    delete pMeshLoader;
    delete pImgLoader;
    delete pFontLoader;
    
    if (!ret || !generateDrawModels()) {
        LOG_ERR("An error occurred while initializing the test state's resources");
        terminate();
        return false;
    }
    
    pTex->bind();
    pTex->setParameter(TEX_MAG_FILTER, LINEAR_FILTER);
    pTex->setParameter(TEX_MIN_FILTER, NEAREST_FILTER);
    
    LOG_GL_ERR();
    
    // Initialize the shaders
    {
        vertexShader vertShader;
        fragmentShader fragShader;
        
        vertShader.compile(vertShaderData);
        fragShader.compile(fragShaderData);
        meshProgram.attachShaders(vertShader, fragShader);
        meshProgram.link();
        
        fragShader.terminate();
        fragShader.compile(fontFS);
        fontProgram.attachShaders(vertShader, fragShader);
        fontProgram.link();

        // Initialize the matrix stacks
        matStack->loadMatrix(PROJECTION_MATRIX, math::perspective(60.f, 4.f/3.f, 0.01f, 100.f));
        matStack->loadMatrix(VIEW_MATRIX, math::lookAt(vec3(3.f), vec3(2.f, -2.f, -2.f), vec3(0.f, 1.f, 0.f)));
        matStack->constructVp();
        
        meshProgram.bind();
        GLuint mvpId = meshProgram.getUniformLocation("vpMatrix");
        meshProgram.setUniformValue(mvpId, matStack->getVpMatrix());
        LOG_GL_ERR();
        
        fontProgram.bind();
        mvpId = fontProgram.getUniformLocation("vpMatrix");
        meshProgram.setUniformValue(mvpId, matStack->getVpMatrix());
        LOG_GL_ERR();
    }
    LOG_GL_ERR();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    return true;
}

/******************************************************************************
 * Stopping state
******************************************************************************/
void testState::onStop() {
    terminate();
}

/******************************************************************************
 * Running state
******************************************************************************/
void testState::onRun(float dt) {
    (void)dt;
    
    updateKeyStates();
    drawScene();
}

/******************************************************************************
 * Pausing state
******************************************************************************/
void testState::onPause(float dt) {
    (void)dt;
    
    drawScene();
}

/******************************************************************************
 * Drawing a scene
******************************************************************************/
static mat4 modelMatrices[2] = {mat4{1.f}, mat4{1.f}};

void testState::drawScene() {
    LOG_GL_ERR();
    
    // Meshes all contain their own model matrices. no need to use the ones in
    // the matrix stack.
    const mat4& alignedView = matStack->getMatrix(VIEW_MATRIX);
    matStack->pushMatrix(VIEW_MATRIX, math::quatToMat4(orientation));
    matStack->constructVp();
    
    drawModel* pModel = nullptr;
    GLuint mvpId = 0;
    
    // first draw model
    {
        // shader setup
        meshProgram.bind();
        mvpId = meshProgram.getUniformLocation("vpMatrix");
        meshProgram.setUniformValue(mvpId, matStack->getVpMatrix());
        
        // billboard setup
        const mat4& orientedView = matStack->getMatrix(VIEW_MATRIX);
        const vec3& camPos{alignedView[3][0], 0.f, -alignedView[3][2]};
        modelMatrices[0] = math::translate(math::quatToMat4(math::lookAt(camPos, vec3{0.f, 0.f, 1.f})), vec3{0.f, 0.f, 0.f});
        modelMatrices[1] = math::translate(math::billboard(vec3{0.f, 0.f, 0.f}, orientedView), vec3{10.f, 0.f, 0.f});
        
        pModel = pScene->getModelList()[0];
        pModel->setNumInstances(2, modelMatrices);
        pModel->draw();
    }
    
    // second draw model
    {
        // shader setup
        fontProgram.bind();
        mvpId = fontProgram.getUniformLocation("vpMatrix");
        meshProgram.setUniformValue(mvpId, matStack->getVpMatrix());
        
        // blending setup
        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        pModel = pScene->getModelList()[1];
        pModel->draw();
        
        glDisable(GL_BLEND);
    }
    
    matStack->popMatrix(VIEW_MATRIX);
}
