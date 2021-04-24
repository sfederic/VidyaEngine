#include "DebugSphere.h"
#include "RenderSystem.h"

DebugSphere::DebugSphere()
{
	shaderName = L"debugDraw.hlsl";
	modelName = "ico_sphere.fbx";
	CreateActors<Actor>(&gRenderSystem, 1);
}

void DebugSphere::Tick(float deltaTime)
{
}