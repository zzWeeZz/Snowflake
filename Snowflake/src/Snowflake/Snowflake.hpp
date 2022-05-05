#pragma once
#include <cstdint>

namespace Snowflake
{
	typedef uint16_t Entity;
	class Registry
	{
	public:
		
		static Entity CreateEntity()
		{
			return s_nextEntity++;
		}
		

		
	private:
		static Entity s_registry[];
		static Entity s_nextEntity;
	};
}