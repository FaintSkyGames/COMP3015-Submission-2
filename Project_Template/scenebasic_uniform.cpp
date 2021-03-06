#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
#include <iostream>
#include <sstream>

using std::cerr;
using std::endl;


#include "helper/glutils.h"
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"

#include "helper/particleutils.h"
#include "helper/noisetex.h"
#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;


SceneBasic_Uniform::SceneBasic_Uniform() : rotSpeed(0.1f), tPrev(0)
{
    spot = ObjMesh::loadWithAdjacency("media/spot/spot_triangulated.obj");
    fenceMid = ObjMesh::loadWithAdjacency("media/Fence/fence2_middle2.obj");
    fenceMid2 = ObjMesh::loadWithAdjacency("media/Fence/fence2_middle.obj");
    fenceEnd = ObjMesh::loadWithAdjacency("media/Fence/fence2_end.obj");
    fenceCorner = ObjMesh::loadWithAdjacency("media/Fence/fence2_corner2.obj");
    ufo = ObjMesh::loadWithAdjacency("media/UFO/ufo.obj");
    plane = ObjMesh::loadWithAdjacency("media/Plane.obj");
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClearStencil(0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    angle = 0.0f;
    ufoRotation = 0.0f;
    ufoHeight = 0.0f;
    increaseHeight = true;

    // Setup framebuffer object
    setupFBO();

    renderProg.use();
    renderProg.setUniform("LightIntensity", vec3(1.0f));

    // Set up a VAO for the full-screen quad
    GLfloat verts[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f };
    GLuint bufHandle;
    glGenBuffers(1, &bufHandle);
    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

    // Set up the vertex array object
    glGenVertexArrays(1, &fsQuad);
    glBindVertexArray(fsQuad);

    glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0); // Vertex position

    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE2);
    spotTex = Texture::loadTexture("media/spot/spot_texture.png");
    fenceTex = Texture::loadTexture("media/Fence/fence2.png");
    ufoTex = Texture::loadTexture("media/UFO/ufo.png");
    grassTex = Texture::loadTexture("media/textures/GrassGreenTexture0004.jpg");
    mudTex = Texture::loadTexture("media/textures/DirtCrack.jpg");
    skyTex = Texture::loadTexture("media/textures/SkySquare.jpg");
    cloudTex = Texture::loadTexture("media/textures/whiteSquare.png");

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, grassTex);

    // Generate noise
    GLuint noiseTex = NoiseTex::generate2DTex(6.0f);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, noiseTex);

    updateLight();

    renderProg.use();
    renderProg.setUniform("Tex", 2);
    renderProg.setUniform("EdgeWidth", 0.012f);
    renderProg.setUniform("PctExtend", 0.25f);
    renderProg.setUniform("Kd", 0.7f, 0.5f, 0.2f);
    renderProg.setUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    renderProg.setUniform("Ka", 0.2f, 0.2f, 0.2f);
    renderProg.setUniform("LightIntensity", vec3(1.0f));

    flatProg.use();
    flatProg.setUniform("Tex", 2);
    flatProg.setUniform("Tex2", 3);
    flatProg.setUniform("NoiseTex", 4);
    flatProg.setUniform("EdgeWidth", 0.012f);
    flatProg.setUniform("PctExtend", 0.25f);
    flatProg.setUniform("Kd", 0.7f, 0.5f, 0.2f);
    flatProg.setUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
    flatProg.setUniform("Ka", 0.2f, 0.2f, 0.2f);
    flatProg.setUniform("LightIntensity", vec3(1.0f));

    compProg.use();
    compProg.setUniform("DiffSpecTex", 0);

    this->animate(true);
}

void SceneBasic_Uniform::updateLight()
{
    lightPos = vec4(5.0f * vec3(cosf(angle) * 7.5f, 1.5f, sinf(angle) * 7.5f), 1.0f); // World coords
}

void SceneBasic_Uniform::updateUFO()
{
    //lightPos = vec4(5.0f * vec3(cosf(angle) * 7.5f, 1.5f, sinf(angle) * 7.5f), 1.0f); // World coords
    ufoRotation += 3.0f;

    if (ufoRotation >= 360.0f)
    {
        ufoRotation = ufoRotation - 360.0f;
    };

    if (increaseHeight == true)
    {
        ufoHeight += 0.002f;

        if (ufoHeight >= 0.1f)
        {
            increaseHeight = false;
        }
    }
    else
    {
        ufoHeight -= 0.002f;    

        if (ufoHeight <= -0.1f)
        {
            increaseHeight = true;
        }
    }
}

void SceneBasic_Uniform::setupFBO() 
{
    // The depth buffer
    GLuint depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    // The ambient buffer
    GLuint ambBuf;
    glGenRenderbuffers(1, &ambBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, ambBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, width, height);

    // The diffuse + specular component
    glActiveTexture(GL_TEXTURE0);
    GLuint diffSpecTex;
    glGenTextures(1, &diffSpecTex);
    glBindTexture(GL_TEXTURE_2D, diffSpecTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create and set up the FBO
    glGenFramebuffers(1, &colorDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ambBuf);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffSpecTex, 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);

    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer is complete. \n");
    }
    else
    {
        printf("Framebuffer is not complete. \n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneBasic_Uniform::compile()
{
    try {

        // Shader for the volumes
        volumeProg.compileShader("shader/solid.vert");
        volumeProg.compileShader("shader/solid.frag");
        volumeProg.compileShader("shader/solid.geom");
        volumeProg.link();

        // Shader for rendering and compositing
        renderProg.compileShader("shader/shadowVolume-render.vs");
        renderProg.compileShader("shader/shadowVolume-render.fs");
        renderProg.compileShader("shader/shadowVolume-render.geom");
        renderProg.link();

        // Final composite shader
        compProg.compileShader("shader/shadowVolume-comp.vs");
        compProg.compileShader("shader/shadowVolume-comp.fs");
        compProg.link(); 

        // Ground shader
        flatProg.compileShader("shader/flat.vert");
        flatProg.compileShader("shader/flat.frag");
        flatProg.compileShader("shader/flat.geom");
        flatProg.link();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}


void SceneBasic_Uniform::update( float t )
{
	// Update your angle here

    float deltaT = t - tPrev;

    if (tPrev == 0.0f)
    {
        deltaT = 0.0f;
    }
    
    tPrev = t;


    if (animating())
    {
        angle += deltaT * rotSpeed;

        if (angle > glm::two_pi<float>())
        {
            angle -= glm::two_pi<float>();
        }

        updateLight();
        updateUFO();
    }    
}

void SceneBasic_Uniform::render()
{   
    pass1();
    glFlush();
    pass2();
    glFlush();
    pass3();   
}

// Renders geometry normally with shading.
// Ambient component is render to one buffer.
// Diffuse and specular are written to a texture.
void SceneBasic_Uniform::pass1() 
{
    glDepthMask(GL_TRUE);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    projection = glm::infinitePerspective(glm::radians(50.0f), (float)width / height, 0.5f);
    view = glm::lookAt(vec3(5.0f, 5.0f, 5.0f), vec3(0, 2, 0), vec3(0, 1, 0));

    renderProg.use();
    renderProg.setUniform("LightPosition", view * lightPos);
    flatProg.setUniform("LightPosition", view * lightPos);

    glBindFramebuffer(GL_FRAMEBUFFER, colorDepthFBO);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    drawScene(renderProg, false);
}

// Generate shadow volumes using geometry shader.
void SceneBasic_Uniform::pass2()
{
    volumeProg.use();
    volumeProg.setUniform("LightPosition", view * lightPos);

    // Copy depth and color buffers from FBO to default FBO.
    // Color buffer contains ambient.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorDepthFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width - 1, height - 1, 0, 0, width - 1, height - 1,
        GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Disable writing to color & depth buffers
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);

    // Re-bind to deault framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set up stencil test.
    //  Increments for front faces, decrements for back faces.
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xffff);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

    // Draw shadow casters
    drawScene(volumeProg, true);

    // Enable writing to color buffer
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

// Read the diffuse and specular from a texture.
// Compine them with ambient from successful stencil test.
void SceneBasic_Uniform::pass3()
{
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glStencilFunc(GL_EQUAL, 0, 0xffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    compProg.use();

    model = mat4(1.0f);
    projection = model;
    view = model;
    setMatrices(compProg);

    glBindVertexArray(fsQuad);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}


void SceneBasic_Uniform::drawScene(GLSLProgram& prog, bool onlyShadowCasters) 
{
    vec3 color;

    if (!onlyShadowCasters)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, spotTex);
        color = vec3(1.0f);
        prog.setUniform("Ka", color * 0.01f);
        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.9f));
        prog.setUniform("Shininess", 150.0f);
        prog.setUniform("PctExtend", 0.25f);
        renderProg.setUniform("EdgeWidth", 0.012f);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.3f, 1.0f, 0.2f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.0f));
    setMatrices(prog);
    spot->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(2.5f, 1.0f, -1.2f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.0f));
    setMatrices(prog);
    spot->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.5f, 1.0f, 2.7f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(1.0f));
    setMatrices(prog);
    spot->render();

    if (!onlyShadowCasters)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ufoTex);
        color = vec3(1.0f);
        prog.setUniform("Ka", color * 0.01f);
        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.9f));
        prog.setUniform("Shininess", 150.0f);
        prog.setUniform("PctExtend", 0.25f);
        renderProg.setUniform("EdgeWidth", 0.012f);
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 4.0f + ufoHeight, -5.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(ufoRotation), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, vec3(1.5f));
    setMatrices(prog);
    ufo->render();

    if (!onlyShadowCasters) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fenceTex);
        color = vec3(0.5f);
        prog.setUniform("Kd", color);
        prog.setUniform("Ks", vec3(0.0f));
        prog.setUniform("Ka", vec3(0.1f));     
        prog.setUniform("Shininess", 1.0f);
        prog.setUniform("PctExtend", 0.0f);
        renderProg.setUniform("EdgeWidth", 0.008f);    
    }

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, 0.0f, -10.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceCorner->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, 0.0f, -7.5f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-7.5f, 0.0f, -10.0f));
    model = glm::rotate(model, glm::radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, 0.0f, -5.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid2->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 0.0f, -10.0f));
    model = glm::rotate(model, glm::radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid2->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, 0.0f, -2.5f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-2.5f, 0.0f, -10.0f));
    model = glm::rotate(model, glm::radians(0.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceMid->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-0.0f, 0.0f, -10.0f));
    model = glm::rotate(model, glm::radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceEnd->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-10.0f, 0.0f, -0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, vec3(0.05f));
    setMatrices(prog);
    fenceEnd->render();

    // Output the planes
    if (!onlyShadowCasters) {
        color = vec3(0.5f);

        flatProg.use();

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, mudTex);  

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, grassTex);

        flatProg.setUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
        flatProg.setUniform("LightIntensity", vec3(1.0f));
        flatProg.setUniform("Kd", color);
        flatProg.setUniform("Ks", vec3(0.0f));
        flatProg.setUniform("Ka", vec3(0.1f));
        flatProg.setUniform("Shininess", 1.0f);
        flatProg.setUniform("PctExtend", 0.0f);
        flatProg.setUniform("EdgeWidth", 0.008f);

        model = mat4(1.0f);
        model = glm::translate(model, vec3(-5.0f, 0.25f, -5.0f));        
        setMatrices(flatProg);
        plane->render();

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, skyTex);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, cloudTex);

        model = mat4(1.0f);
        model = glm::translate(model, vec3(-15.0f, 5.0f, -5.0f));
        model = glm::rotate(model, glm::radians(-90.0f), vec3(0.0f, 0.0f, 1.0f));
        setMatrices(flatProg);
        plane->render();

        model = mat4(1.0f);
        model = glm::translate(model, vec3(-5.0f, 5.0f, -15.0f));
        model = glm::rotate(model, glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices(flatProg);
        plane->render();
        model = mat4(1.0f);
    }

}

void SceneBasic_Uniform::setMatrices(GLSLProgram& prog)
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("ProjMatrix", projection);
    prog.setUniform("NormalMatrix", 
        glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
}
