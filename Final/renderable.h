#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <vector>
#include "model.h"
using namespace std;



class Renderable {
public:
    // renderable Data
    vector<Model> models;
    vector<glm::vec3> positions;
    vector<glm::vec3> minScalings;
    vector<glm::vec3> maxScalings;

    // render renderable
    virtual void Draw(glm::mat4 position, Shader& shader, glm::vec3 scaling, float rotation) = 0;
    virtual glm::vec3 generateScalings() = 0;
    virtual float generateRotations() = 0;
        //cout << "drawing" << endl;
        //
        //float rx, ry, rz, sx, sy, sz;
        //glm::mat4 temp = glm::mat4(1.0f);
        //for (int i = 0; i < models.size(); i++)
        //{
        //    rx = (float)((rand() % 100) + 1) * 0.01f;
        //    ry = (float)((rand() % 100) + 1) * 0.01f;
        //    rz = (float)((rand() % 100) + 1) * 0.01f;
        //    sx = minScalings[i].x + ((maxScalings[i].x - minScalings[i].x) * rx);
        //    sy = minScalings[i].y + ((maxScalings[i].y - minScalings[i].y) * ry);
        //    sz = minScalings[i].z + ((maxScalings[i].z - minScalings[i].z) * rz);

        //    glm::mat4 temp = glm::mat4(1.0f);
        //    temp = glm::translate(position, positions[i]);
        //    temp = glm::scale(temp, glm::vec3(sx, sy, sz));
        //    shader.setMat4("model", temp);
        //    models[i].Draw(shader);
        //}

};
#endif