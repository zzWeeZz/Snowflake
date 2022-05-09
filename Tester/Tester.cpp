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
			manager.DestroyEntity(entity1);
			manager.DestroyEntity(entity2);
			manager.DestroyEntity(entity3);
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
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			manager.DestroyEntity(entity);

		}

		TEST_METHOD(AddMultipleComponents)
		{
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
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

		TEST_METHOD(RemoveComponent)
		{
			auto& manager = Snowflake::Manager::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			manager.AddComponent<TestComponent>(entity);
			manager.RemoveComponent<TransformComponent>(entity);
			Assert::AreEqual(false, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			manager.DestroyEntity(entity);
			
			Snowflake::Entity entity2 = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity2);
			manager.AddComponent<TestComponent>(entity2);

			manager.RemoveComponent<TestComponent>(entity2);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity2));
			Assert::AreEqual(false, manager.HasComponent<TestComponent>(entity2));
			manager.DestroyEntity(entity2);
			
		}

		TEST_METHOD(GetComponent)
		{
			auto& manager = Snowflake::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);

			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			Assert::AreEqual(0.f, manager.GetComponent<TestComponent>(entity).a);
			manager.DestroyEntity(entity);
		}

		TEST_METHOD(ChangeComponent)
		{
			auto& manager = Snowflake::GetManager();
			Snowflake::Entity entity = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			auto& tfComponent = manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			tfComponent.x = 1.f;
			auto& b = manager.GetComponent<TransformComponent>(entity);
			Assert::AreEqual(1.f, b.x);
			manager.DestroyEntity(entity);
			manager.DestroyEntity(entity2);
		}
		TEST_METHOD(ExecuteFunction)
		{
			auto& manager = Snowflake::GetManager();

			{
				Snowflake::Entity entity = manager.CreateEntity();
				Snowflake::Entity entity2 = manager.CreateEntity();
				Snowflake::Entity entity3 = manager.CreateEntity();
				manager.AddComponent<TransformComponent>(entity);
				manager.AddComponent<TransformComponent>(entity2);
				manager.AddComponent<TransformComponent>(entity3);
				manager.Execute<TransformComponent>([](Snowflake::Entity entity, TransformComponent& transform) {
					transform.x = 1.f;
					});
				Assert::AreEqual(1.f, manager.GetComponent<TransformComponent>(entity).x);
				Assert::AreEqual(1.f, manager.GetComponent<TransformComponent>(entity2).x);
				Assert::AreEqual(1.f, manager.GetComponent<TransformComponent>(entity3).x);

				manager.DestroyEntity(entity);
				manager.DestroyEntity(entity2);
				manager.DestroyEntity(entity3);
			}

			Snowflake::Entity entity = manager.CreateEntity();
			Snowflake::Entity entity2 = manager.CreateEntity();
			Snowflake::Entity entity3 = manager.CreateEntity();


			manager.AddComponent<TransformComponent>(entity).x = 0;
			manager.AddComponent<TransformComponent>(entity2).x = 0;
			manager.AddComponent<TestComponent>(entity2);
			manager.AddComponent<TransformComponent>(entity3).x = 0;

			manager.Execute<TransformComponent, TestComponent>([](Snowflake::Entity entity, TransformComponent& transform, TestComponent& test) {
				transform.x = 100.f;
				test.a = 100.f;
				});
			Assert::AreEqual(100.f, manager.GetComponent<TransformComponent>(entity2).x);
			Assert::AreEqual(100.f, manager.GetComponent<TestComponent>(entity2).a);

			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity3).x);
			Assert::IsNotNull(manager.TryGetComponent<TestComponent>(entity2));
			manager.RemoveComponent<TestComponent>(entity2);
			manager.Execute<TransformComponent, TestComponent>([](Snowflake::Entity entity, TransformComponent& transform, TestComponent& test) {
				transform.x = 200.f;
				test.a = 200.f;
				});
			Assert::AreEqual(100.f, manager.GetComponent<TransformComponent>(entity2).x);
			Assert::IsNull(manager.TryGetComponent<TestComponent>(entity2));

			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity).x);
			Assert::AreEqual(0.f, manager.GetComponent<TransformComponent>(entity3).x);
		}

	};
}