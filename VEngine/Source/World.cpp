#include "World.h"
#include "Actor.h"
#include "Debug.h"
#include "DebugMenu.h"
#include "CoreSystem.h"

World gCurrentWorld;

//Returns the current active world
World* GetWorld()
{
	return &gCurrentWorld;
}

void World::Load(std::string levelName)
{
	FILE* file;
	fopen_s(&file, levelName.c_str(), "rb");
	assert(file);

	fclose(file);
}

void World::TickAllActorSystems(float deltaTime)
{
	//Skip actor ticks if game is paused.
	if (gCoreSystem.bGamePaused)
	{
		return;
	}

	for (int asIndex = 0; asIndex < actorSystems.size(); asIndex++)
	{
		for (int actorIndex = 0; actorIndex < actorSystems[asIndex]->actors.size(); actorIndex++)
		{
			actorSystems[asIndex]->actors[actorIndex]->Tick(deltaTime);
		}

		actorSystems[asIndex]->Tick(deltaTime);
	}
}

void World::CleaupAllActors()
{
	actorSystems.clear();
	actorSystemMap.clear();
}

void World::AddActorSystem(ActorSystem* actorSystem)
{
	auto actorSystemIt = actorSystemMap.find(actorSystem->id);
	if (actorSystemIt == actorSystemMap.end())
	{
		actorSystems.push_back(actorSystem);
		actorSystemMap[actorSystem->id] = actorSystem;
	}
	else
	{
		DebugPrint("Actor System already exists in world. Cannot create.\n");
		gDebugMenu.AddNotification(L"Cannot create actor system.");
	}
}

void World::RemoveActorSystem(EActorSystemID id)
{
	auto actorSystemIt = actorSystemMap.find(id);
	if (actorSystemIt == actorSystemMap.end())
	{
		DebugPrint("Actor system ID: %d not found to remove.\n", id);
		gDebugMenu.AddNotification(L"Actor system not found to remove.");
		return;
	}

	for (int i = 0; i < actorSystems.size(); i++)
	{
		if (i == (int)actorSystems[i]->id)
		{
			actorSystems.erase(actorSystems.begin() + i);
			actorSystemMap.erase(id);
			break;
		}
	}
}

Actor* World::FindActorByString(std::wstring name)
{
	Actor* foundActor = nullptr;

	for (int i = 0; i < actorSystems.size(); i++)
	{
		for (int j = 0; j < actorSystems[i]->actors.size(); j++)
		{
			if (actorSystems[i]->actors[j]->name == name)
			{
				foundActor = actorSystems[i]->GetActor(j);
				return foundActor;
			}
		}
	}

	return foundActor;
}

ActorSystem* World::FindActorSystem(EActorSystemID id)
{
	auto actorSystemIt = actorSystemMap.find(id);
	if (actorSystemIt == actorSystemMap.end())
	{
		DebugPrint("Actor system not found in GetActorSystem/n");
		return nullptr;
	}
	else
	{
		return actorSystemIt->second;
	}
}

ActorSystem* World::GetActorSystem(unsigned int id)
{
	if (id < actorSystems.size())
	{
		return actorSystems[id];
	}

	return nullptr;
}

Actor* World::GetActor(unsigned int systemId, unsigned int actorId)
{
	if (systemId < actorSystems.size())
	{
		if (actorId < actorSystems[systemId]->actors.size())
		{
			return actorSystems[systemId]->actors[actorId];
		}
	}

	return nullptr;
}
