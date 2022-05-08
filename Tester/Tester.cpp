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
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity1 = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			Snowflake::Entity entity3 = manager.CreateEntity();

			Assert::AreEqual(0, (int)entity1);
			Assert::AreEqual(1, (int)entity2);
			Assert::AreEqual(2, (int)entity3);
		}

		TEST_METHOD(CreatePool)
		{
			struct TransformComponent
			{
				float x = 0;
				float y = 0;
			};

			struct TestComponent
			{
				float a = 0;
				float b = 0;
				float c = 0;
			};

			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(false, manager.HasComponent<TestComponent>(entity));
			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));

			manager.DestroyEntity(entity);

		}
	};
}
