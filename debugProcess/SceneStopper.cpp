#include "SceneStopper.h"

SceneStopper *SceneStopper::GetInstance()
{
	static SceneStopper stopper;
	return &stopper;
}
