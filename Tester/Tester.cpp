#include "pch.h"
#include "CppUnitTest.h"
#include "Snowflake/Serializer.hpp"
#include "Snowflake/Snowflake.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tester
{

	TEST_CLASS(EntityManagment)
	{
	public:

		TEST_METHOD(EntityCreation)
		{
			Snowflake::Registry manager;
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
			Snowflake::Registry manager;
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
	COMPONENT(TransformComponent)
	{
		REGISTER_COMPONENT("{4A34E93D-A5EC-400F-A973-D38F981E2F0E}"_guid);
		float x = 0;
		float y = 0;
	};

	COMPONENT(TestComponent)
	{
		REGISTER_COMPONENT("{AA590468-495C-4EB0-BEDD-1950F3631251}"_guid);
		float a = 0;
		float b = 0;
		float c = 0;
	};

	TEST_CLASS(ComponentHandlíng)
	{
	public:

		TEST_METHOD(AddSingleComponent)
		{
			Snowflake::Registry manager;
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			manager.DestroyEntity(entity);

		}

		TEST_METHOD(AddMultipleComponents)
		{
			Snowflake::Registry manager;
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			manager.DestroyEntity(entity);
		}

		TEST_METHOD(ComponentsOnDestroyedEntity)
		{
			Snowflake::Registry manager;
			Snowflake::Entity entity = manager.CreateEntity();
			manager.AddComponent<TransformComponent>(entity);
			manager.AddComponent<TestComponent>(entity);
			Assert::AreEqual(true, manager.HasComponent<TransformComponent>(entity));
			Assert::AreEqual(true, manager.HasComponent<TestComponent>(entity));
			manager.DestroyEntity(entity);
		}

		TEST_METHOD(RemoveComponent)
		{
			Snowflake::Registry manager;
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
			Snowflake::Registry manager;
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
			Snowflake::Registry manager;
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

		TEST_METHOD(ChangeComponentOnDestroyedEntity)
		{
			Snowflake::Registry registry;
			Snowflake::Entity entt = registry.CreateEntity();
			registry.AddComponent<TransformComponent>(entt).x = 4;
			Assert::AreEqual(4.f, registry.GetComponent<TransformComponent>(entt).x);
			registry.DestroyEntity(entt);
			Snowflake::Entity newEntt = registry.CreateEntity();
			registry.AddComponent<TransformComponent>(newEntt);
			Assert::AreEqual(0.f, registry.GetComponent<TransformComponent>(newEntt).x);
		}

		TEST_METHOD(ExecuteFunction)
		{
			Snowflake::Registry manager;

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
	TEST_CLASS(Serialization)
	{
	public:
		TEST_METHOD(WriteToFile)
		{
			Snowflake::Registry registry;
			auto ent = registry.CreateEntity();
			registry.AddComponent<TransformComponent>(ent);
			Snowflake::RegistrySerializer serializer(registry);
			Assert::IsTrue(serializer.Serialize("SingleComponent.ett"));
		}
		TEST_METHOD(WriteMultibleComponentsToFile)
		{
			Snowflake::Registry registry;
			auto ent = registry.CreateEntity();
			registry.AddComponent<TransformComponent>(ent);
			registry.AddComponent<TestComponent>(ent);
			Snowflake::RegistrySerializer serializer(registry);
			Assert::IsTrue(serializer.Serialize("MultiComponent.ett"));
		}

		TEST_METHOD(ReadAndWriteToFile)
		{
			Snowflake::Registry registry;
			auto ent = registry.CreateEntity();
			registry.AddComponent<TransformComponent>(ent);
			Snowflake::RegistrySerializer serializer(registry);
			Assert::IsTrue(serializer.Serialize("SingleComponentRead.ett"));
			Assert::IsTrue(serializer.Deserialize("SingleComponentRead.ett"));
		}

		TEST_METHOD(ReadAndWriteMultibleComponentsToFile)
		{
			{
				Snowflake::Registry registry;
				{
					auto ent = registry.CreateEntity();
					
					registry.AddComponent<TestComponent>(ent).a = 10.f;
				}
				{
					auto ent = registry.CreateEntity();
					registry.AddComponent<TransformComponent>(ent);
					registry.AddComponent<TestComponent>(ent).a = 5.f;
				}
				Snowflake::RegistrySerializer serializer(registry);
				Assert::IsTrue(serializer.Serialize("MultiComponentRead.ett"));
			}
			{
				Snowflake::Registry DeserializeRegistry;
				Snowflake::RegistrySerializer serializer(DeserializeRegistry);
				Assert::IsTrue(serializer.Deserialize("MultiComponentRead.ett"));
				std::vector<Snowflake::Entity> Entities;
				DeserializeRegistry.ForEach([&](auto entity)
					{
						Entities.push_back(entity);
						Assert::IsTrue(DeserializeRegistry.HasComponent<TestComponent>(entity));
					});
				Assert::AreEqual(10.f, DeserializeRegistry.GetComponent<TestComponent>(Entities[0]).a);
				Assert::AreEqual(5.f, DeserializeRegistry.GetComponent<TestComponent>(Entities[1]).a);
				Assert::IsFalse(DeserializeRegistry.HasComponent<TransformComponent>(Entities[0]));
				Assert::IsTrue(DeserializeRegistry.HasComponent<TransformComponent>(Entities[1]));
				
			}
		}
	};
}