#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/random.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <sstream>

class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram volumeProg, renderProg, compProg, flatProg;
    GLuint colorDepthFBO, fsQuad, quad;
    GLuint spotTex, fenceTex, ufoTex, grassTex, mudTex, cloudTex, skyTex;

    Random rand;
    
    std::unique_ptr<ObjMesh> spot, ufo;
    std::unique_ptr<ObjMesh> fenceMid, fenceMid2, fenceEnd, fenceCorner, plane;

    glm::vec4 lightPos;
    float ufoRotation, ufoHeight;
    bool increaseHeight;
    float angle, tPrev, rotSpeed, time, deltaT;

    void setMatrices(GLSLProgram&);

    void compile();

    void setupFBO();
    void drawScene(GLSLProgram&, bool);
    void pass1();
    void pass2();
    void pass3();
    void updateLight();
    void updateUFO();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
