#pragma once

#include <vector>
#include "UIContext.h"

int debugPrint(const char* format, ...);
int print(const char* format, va_list args);

enum class EMenuID
{
	ACTORS,
	ACTORSYSTEMS,
	RENDERING
};


struct MenuItem
{
	MenuItem(const wchar_t* initName, EMenuID id)
	{
		wcsncpy_s(name, initName, 32);
		menuID = id;
	}

	std::vector<const wchar_t*> subMenuItems;
	wchar_t name[32];
	EMenuID menuID;
};

struct DebugNotification
{
	DebugNotification(const wchar_t* initText)
	{
		wcsncpy_s(text, initText, 256);
		timeOnScreen = 0.f;
	}

	wchar_t text[256];
	float timeOnScreen;
};

class DebugMenu
{
public:
	DebugMenu();

	void Tick(class UIContext* ui, class DXUtil* dx, class ActorSystem* actorSystem, float deltaTime);

	std::vector<MenuItem> menuItems;
	std::vector<DebugNotification> notifications;
	bool bDebugMenuActive = false;
	bool bSubMenuOpen = false;
};