#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <unordered_map>
#include <functional>

namespace Snowflake
{

	constexpr uint32_t InvalidEntity = ~0;

	using Entity = uint32_t;

	template<class Component>
	class ComponentPool;

	class Pool;

	template<class TComponent>
	uint32_t Hash()
	{
		return typeid(TComponent).hash_code();
	}

	class Manager
	{
	public:
		Manager()
		{
			s_Instance = *this;
		}

		Entity CreateEntity()
		{
			Entity entity = m_Entities.size();
			m_Entities.emplace_back(entity);
			return entity;
		}

		bool DestroyEntity(Entity& entity)
		{
			if (entity == InvalidEntity) return false;
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			if(it != m_Entities.end())
			{
				std::swap(*it, m_Entities.back());
				m_Entities.pop_back();
				entity = InvalidEntity;
				return true;
			}
			return false;
		}

		bool ValidateEntity(Entity entity)
		{
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			return it != m_Entities.end();
		}

		// TODO: Create pool if it does not exist.
		template<class TComponent>
		TComponent& AddComponent(Entity entity)
		{
			if (entity == InvalidEntity) return TComponent();
			auto component = static_cast<ComponentPool<TComponent>*>(m_ComponentPools[typeid(TComponent).hash_code()]);
			component->RegisterEntity(entity);
			return component->GetComponent(entity);
		}

		// TODO: add support to check if the pool exists
		template<class TComponent>
		bool HasComponent(Entity entity)
		{
			if (entity == InvalidEntity) return false;
			auto& component = *static_cast<ComponentPool<TComponent>*>(m_ComponentPools[typeid(TComponent).hash_code()]);;
			return component.IsEntityRegistered(entity);
		}

		template<class TComponent>
		TComponent& GetComponent(Entity entity)
		{
			if(!HasComponent<TComponent>(entity))
			{
				// assert
				return TComponent();
			}
			auto& component = *static_cast<ComponentPool<TComponent>*>(m_ComponentPools[typeid(TComponent).hash_code()]);
			return component.GetComponent(entity);
		}

		template<class TComponent>
		void RemoveComponent(Entity entity)
		{
			if (entity == InvalidEntity) return;
			auto& component = *static_cast<ComponentPool<TComponent>*>(m_ComponentPools[typeid(TComponent).hash_code()]);;
			component.DeRegisterEntity(entity);
		}

		template<class TComponent>
		bool CreateComponent()
		{
			size_t hash = typeid(TComponent).hash_code();
			m_ComponentPools[hash] = new ComponentPool<TComponent>();
			return true;
		}

		//TODO: Add Execute function.


		static Manager& GetManager()
		{
			return s_Instance;
		}
	private:
		
		static Snowflake::Manager s_Instance;
		std::vector<Entity> m_Entities;

		std::unordered_map<size_t, void*> m_ComponentPools ;

	};
	Snowflake::Manager Manager::s_Instance;

	inline Manager& GetManager()
	{
		return Manager::GetManager();
	}


	template<class Component>
	class ComponentPool
	{
		friend Manager;
	public:

		void RegisterEntity(Entity entity)
		{
			if (!m_RegisteredEntities[entity])
			{
				m_RegisteredEntities[entity] = true;
			}
		}

		void DeRegisterEntity(Entity entity)
		{
			m_RegisteredEntities[entity] = false;
		}

		bool IsEntityRegistered(Entity entity)
		{
			return m_RegisteredEntities[entity];
		}

		Component& GetComponent(Entity entity)
		{
			if (m_RegisteredEntities[entity])
			{
				return Component();
			}
			return m_Components[entity];
		}
	private:
		std::array<Component, 8192> m_Components = {};
		std::bitset<8192> m_RegisteredEntities = {false};
	};

}