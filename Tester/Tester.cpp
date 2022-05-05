#include "pch.h"
#include "CppUnitTest.h"
#include "Snowflake/Snowflake.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tester
{
	TEST_CLASS(Tester)
	{
	public:
		
		TEST_METHOD(EntityCreation)
		{
			auto manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity1 = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			Snowflake::Entity entity3 = manager.CreateEntity();
			
			Assert::AreEqual((int)entity1, 0);
			Assert::AreEqual((int)entity2, 1);
			Assert::AreEqual((int)entity3, 2);
		}
	};
}
