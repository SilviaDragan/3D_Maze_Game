#pragma once
#include "components/simple_scene.h"
namespace enemy {
	class Enemy {
		public:
			Enemy();
			~Enemy();
		
			glm::vec3 initialPoz;
			glm::vec3 currentPoz;
	};
}