#ifndef PROGRAMPARAMS_H
#define PROGRAMPARAMS_H

#include "Tools/Camera.h"

class ProgramParams
{
public:
	const unsigned int SCR_WIDTH = 1200;
	const unsigned int SCR_HEIGHT = 800;

	float deltaTime;
	float lastFrame;
	float lastX;
	float lastY;
	bool firstMouse;

	Camera* userView;

	ProgramParams()
	{
		deltaTime = 0.0f;
		lastFrame = 0.0f;
		lastX = SCR_WIDTH / 2;
		lastY = SCR_HEIGHT / 2;
		firstMouse = true;
		userView = NULL;
	}
};


#endif