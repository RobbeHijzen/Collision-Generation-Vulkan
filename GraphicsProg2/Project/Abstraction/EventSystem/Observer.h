#pragma once

#include "Events.h"
#include <functional>

struct Observer
{
	Observer(GameEvents ev, std::function<void()> func)
		: event{ev}
		, function{func}
	{}

	GameEvents event;
	std::function<void()> function;
};