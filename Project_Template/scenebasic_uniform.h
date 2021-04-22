#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include <sstream>

class SceneBasic_Uniform : public Scene
{
private:
    //GLuint vaoHandle;
    GLSLProgram prog;
    GLSLProgram skyProg;
    //float angle;

    GLuint obj1Tex;
    GLuint obj2Tex;
    GLuint overlapTex;
    GLuint cubeTex;

    Plane plane;
    std::unique_ptr<ObjMesh> mesh;
    SkyBox sky;

    void compile();
    void setMatrices();
    void setSkyMatrices();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
