#ifndef LARGE_ROCK_1_H
#define LARGE_ROCK_1_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "renderable.h"

#include <string>
#include <vector>
#include "model.h"
using namespace std;



class LargeRock1 : public Renderable {
public:
    // renderable Data
    vector<Model> models;
    vector<glm::vec3> positions;
    vector<glm::vec3> minScalings;
    vector<glm::vec3> maxScalings;

    LargeRock1()
    {
        models.push_back(Model("resources/objects/rock/Lrock1.obj"));
        positions.push_back(glm::vec3(0.0f));
        minScalings.push_back(glm::vec3(0.8f));
        maxScalings.push_back(glm::vec3(1.1f));
        cout << "constructed straight" << endl;
    }


    glm::vec3 generateScalings()
    {
        
        float rx, ry, rz, sx, sy, sz;
        for (int i = 0; i < models.size(); i++)
        {
            rx = (float)((rand() % 100) + 1) * 0.01f;
            ry = (float)((rand() % 100) + 1) * 0.01f;
            rz = (float)((rand() % 100) + 1) * 0.01f;
            sx = minScalings[i].x + ((maxScalings[i].x - minScalings[i].x) * rx);
            sy = minScalings[i].y + ((maxScalings[i].y - minScalings[i].y) * ry);
            sz = minScalings[i].z + ((maxScalings[i].z - minScalings[i].z) * rz);
        }
        return glm::vec3(sx, sy, sz);
    }
    float generateRotations()
    {
        return glm::radians((float)(rand() % 360));
    }
    // render renderable
    void Draw(glm::mat4 position, Shader& shader, glm::vec3 scaling, float rotation)
    {
        
        glm::mat4 temp = glm::mat4(1.0f);
        temp = glm::translate(position, positions[0]) * glm::rotate(temp, rotation, glm::vec3(0.0f, 1.0f, 0.0f));

        temp = glm::scale(temp, scaling);
        shader.setMat4("model", temp);
        models[0].Draw(shader);
    };

};
#endif