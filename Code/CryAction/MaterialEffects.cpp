#include <cstdint>

#include "MaterialEffects.h"

void MaterialEffects::Update(float frameTime)
{
#ifdef BUILD_64BIT
	std::uintptr_t func = 0x30762890;
#else
	std::uintptr_t func = 0x306a0240;
#endif

	(this->*reinterpret_cast<void(MaterialEffects::*&)(float)>(func))(frameTime);
}
