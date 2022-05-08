#include "pch.h"
#include "CppUnitTest.h"
#include "Snowflake/Snowflake.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tester
{
	TEST_CLASS(EntityManagment)
	{
	public:

		TEST_METHOD(EntityCreation)
		{
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity1 = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			Snowflake::Entity entity3 = manager.CreateEntity();

			Assert::AreEqual(true, manager.ValidateEntity(entity1));
			Assert::AreEqual(true, manager.ValidateEntity(entity2));
			Assert::AreEqual(true, manager.ValidateEntity(entity3));
		}

		TEST_METHOD(EntityDestruction)
		{
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity1 = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			Snowflake::Entity entity3 = manager.CreateEntity();

			manager.DestroyEntity(entity1);
			manager.DestroyEntity(entity2);
			manager.DestroyEntity(entity3);

			Assert::AreEqual(Snowflake::InvalidEntity, entity1);
			Assert::AreEqual(Snowflake::InvalidEntity, entity2);
			Assert::AreEqual(Snowflake::InvalidEntity, entity3);
		}
	};
	TEST_CLASS(ComponentHandlíng)
	{
	public:
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
		TEST_METHOD(AddSingleComponent)
		{
			struct TransformComponent
			{
				float x = 0;
				float y = 0;
			};

			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			manager.DestroyEntity(entity);

		}

		TEST_METHOD(AddMultipleComponents)
		{
			

			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.CreateComponent<TestComponent>();
			manager.AddComponent<TransformComponent>(entity);
			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			manager.DestroyEntity(entity);
		}

		TEST_METHOD(ComponentsOnDestroyedEntity)
		{

			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			manager.DestroyEntity(entity);
		}

		TEST_METHOD(GetComponent)
		{
			auto& manager = Snowflake::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.CreateComponent<TestComponent>();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);

			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			Assert::AreEqual(0.f, manager.GetComponent<TestComponent>(entity).a);
		}

		TEST_METHOD(ChangeComponent)
		{
			auto& manager = Snowflake::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.CreateComponent<TestComponent>();
			auto& tfComponent = manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			tfComponent.x = 1.f;
			auto& b = manager.GetComponent<TransformComponent>(entity);
			Assert::AreEqual(1.f, b.x);
		}
		/*TEST_METHOD(ExecuteFunction)
		{
			auto& manager = Snowflake::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.CreateComponent<TransformComponent>();
			manager.AddComponent<TransformComponent>(entity);
			manager.Execute<TransformComponent>([](Snowflake::Entity entity, TransformComponent& transform) {
				transform.x = 1.f;
			});
			Assert::AreEqual(1.f, manager.GetComponent<TransformComponent>(entity).x);
		}*/
	};
}
