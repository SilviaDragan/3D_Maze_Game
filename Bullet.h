#pragma once
struct bullet {
    /*float pozX;
	float pozY;
	float pozZ;
    
    float directionX;
    float directionY;
    float directionZ;*/

    glm::vec3 position;
    glm::vec3 direction;
    
};

/*
 // move projectile forwards
            proj->moveForward();
             {
                move forward:
                this->location_x += (speed * forward.x);
                this->location_y += (speed * forward.y);
                this->location_z += (speed * forward.z);
             }


            glm::mat4 projectileMatrix = glm::mat4(1);
            projectileMatrix = glm::translate(projectileMatrix,
                glm::vec3
                (
                    proj->getLocationX(),
                    proj->getLocationY(),
                    proj->getLocationZ()
                )
            );

            RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], projectileMatrix, glm::vec3(1, 0, 0));
*/