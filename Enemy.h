#pragma once
#include "tema2.h"

namespace e {
	class Enemy {
	public:
		Enemy();
		~Enemy();
		
		glm::vec3 initialPoz;
		glm::vec3 currentPoz;
		bool defeated;
	};
}