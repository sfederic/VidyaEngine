#include "DebugBox.h"

DebugBox::DebugBox()
{
	modelName = "cube.fbx";
	shaderName = "debugDraw.hlsl";
	Init<Actor>(1);
}

void DebugBox::Tick(float deltaTime)
{
}
