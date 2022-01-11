#pragma once
#include "components/simple_scene.h"
namespace enemy {
	class Enemy {
		public:
			Enemy();
			~Enemy();
		
			// se misca in forma patrata in sensiul acelor de ceasornic
			// pornind din coltul stg jos al celulei
			int direction; // 1 = up, 2 = right, 3 = down, 4 = left
			glm::vec3 initialPoz;
			glm::vec3 currentPoz;
	};
}

//// move an enemy in a cross form, based on its walking direction
//enemy moveeEnemy(vector<enemy> enemies, int i, float deltaTimeSeconds) {
//    float difference = deltaTimeSeconds;
//    enemies[i].walkedDistance += difference;
//    if (enemies[i].direction == 1) {
//        enemies[i].x += difference;
//    }
//    if (enemies[i].direction == 2) {
//        enemies[i].x -= difference;
//    }
//    if (enemies[i].direction == 3) {
//        enemies[i].x += difference;
//    }
//    if (enemies[i].direction == 4) {
//        enemies[i].z += difference;
//    }
//
//    if (enemies[i].direction == 5) {
//        enemies[i].z -= difference;
//    }
//
//    if (enemies[i].direction == 6) {
//        enemies[i].z += difference;
//    }
//
//    if (enemies[i].direction == 1) {
//        if (enemies[i].walkedDistance >= 1) {
//            enemies[i].direction++;
//            enemies[i].walkedDistance = 0;
//        }
//    }
//
//    if (enemies[i].direction == 2) {
//        if (enemies[i].walkedDistance >= 2) {
//            enemies[i].direction++;
//            enemies[i].walkedDistance = 0;
//        }
//    }
//
//    if (enemies[i].direction == 3) {
//        if (enemies[i].walkedDistance >= 1) {
//            enemies[i].direction++;
//            enemies[i].walkedDistance = 0;
//        }
//    }
//
//    if (enemies[i].direction == 4) {
//        if (enemies[i].walkedDistance >= 1) {
//            enemies[i].direction++;
//            enemies[i].walkedDistance = 0;
//        }
//    }
//
//    if (enemies[i].direction == 5) {
//        if (enemies[i].walkedDistance >= 2) {
//            enemies[i].direction++;
//            enemies[i].walkedDistance = 0;
//
//        }
//    }
//
//    if (enemies[i].direction == 6) {
//        if (enemies[i].walkedDistance >= 1) {
//            enemies[i].direction = 1;
//            enemies[i].walkedDistance = 0;
//        }
//    }
//    return enemies[i];
//}