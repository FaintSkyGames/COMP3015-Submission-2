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

#include "helper/texture.h"

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 20.0f, 100, 100), sky()
{
    //mesh = ObjMesh::load("media/pig_triangulated.obj", true);
    //mesh = ObjMesh::load("media/source/Low Poly Ramen Shop.obj", true);
    mesh = ObjMesh::load("media/My edits/RamenShop.obj", true);
    
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    //             eye (position of cameras view point), center (where you are looking at, position), up
    view = glm::lookAt(vec3(3.5f, 0.5f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));  //xyz
    projection = mat4(1.0f);
    //float x, z;
    /*for (int i = 0; i < 3; i++)
    {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }*/


    //vec4 lightPos = vec4(2.0f * cosf((glm::two_pi<float>() / 3)), 1.2f, (2.0f * sinf((glm::two_pi<float>() / 3))) + 1.0f, 1.0f);
    vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    //prog.setUniform("directionalLight.Position", vec3(100.0f,100.0f,100.0f));

    // Set point light
    //prog.setUniform("pointLight.L", vec3(1.0f));
    prog.setUniform("pointLight.L", vec3(1.0f,1.0f,1.0f));
    prog.setUniform("pointLight.La", vec3(0.5f));
    prog.setUniform("pointLight.Position", vec4(0.0f, 2.0f, -7.5f, 1.0f));

    // Set spotlights
    prog.setUniform("spotLight[0].L", vec3(0.6f));
    prog.setUniform("spotLight[1].L", vec3(0.6f));
    prog.setUniform("spotLight[0].La", vec3(0.3f));
    prog.setUniform("spotLight[1].La", vec3(0.3f));
    //prog.setUniform("spotLight[0].Position", vec4(0.0f, 2.0f, 0.0f, 1.0f));
    //prog.setUniform("spotLight[1].Position", vec4(1.0f, 2.0f, 0.0f, 1.0f));
    //prog.setUniform("spotLight[0].Direction", vec3());
    //prog.setUniform("spotLight[1].Direction", vec3());
    prog.setUniform("spotLight[0].Exponent", 50.0f);
    prog.setUniform("spotLight[1].Exponent", 50.0f);
    prog.setUniform("spotLight[0].CutOff", glm::radians(15.0f));
    prog.setUniform("spotLight[1].CutOff", glm::radians(15.0f));

    //obj1Tex = Texture::loadTexture("media/textures/Ramen_Restaurant_Utensils_Diffuse.png");

    obj1Tex = Texture::loadTexture("media/My edits/RamenShopTexture.png");
    overlapTex = Texture::loadTexture("media/My edits/BloodOverlay.png");

    obj2Tex = Texture::loadTexture("media/textures/Ramen_Shop_Walls_and_Table_Diffuse.png");
    //overlap2Tex = Texture::loadTexture("media/My edits/BloodOverlay.png");

    //cubeTex = Texture::loadTexture("media/textures/Ramen_Shop_Walls_and_Table_Diffuse.png");
    cubeTex = Texture::loadHdrCubeMap("media/pisa-hdr/pisa");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj1Tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, overlapTex);

}

void SceneBasic_Uniform::compile()
{
    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();

        skyProg.compileShader("shader/skybox.vert");
        skyProg.compileShader("shader/skybox.frag");
        skyProg.link();
    }
    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    //update your angle here
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    prog.use();

    vec3 lightPos0 = vec3(-0.4f, 1.0f, -6.0f);
    vec3 lightPos1 = vec3(1.0f, 1.0f, -6.0f);
    prog.setUniform("spotLight[0].Position", lightPos0);
    prog.setUniform("spotLight[1].Position", lightPos1);
    mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    //prog.setUniform("spotLight[0].Direction", normalMatrix * vec3(-vec3(0.2f, 2.0f, 0.0f)));
    prog.setUniform("spotLight[0].Direction", normalMatrix * vec3(-vec3(0.2f, 2.0f, 0.0f)));
    prog.setUniform("spotLight[1].Direction", normalMatrix * vec3(-vec3(0.2f, 2.0f, 0.0f)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj1Tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, overlapTex);

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 180.0f);
    model = mat4(1.0f);
    //model = glm::translate(model, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices();
    mesh->render();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj2Tex);

    prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog.setUniform("Material.Shininess", 180.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
    setMatrices();
    plane.render();  

    skyProg.use();

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    setSkyMatrices();
    sky.render();
}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv;
    mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::setSkyMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("MVP", projection * mv);
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
