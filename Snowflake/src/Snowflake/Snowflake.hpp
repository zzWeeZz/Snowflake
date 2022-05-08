#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <unordered_map>


namespace Snowflake
{

	template<class TComponent>
	uint32_t Hash()
	{
		return typeid(TComponent).hash_code();
	}

	using Entity = uint32_t;

	template<class Component>
	class ComponentPool;

	class Pool;

	class Manager
	{
	public:
		static Manager& GetManager() noexcept
		{
			if (!s_Instance)
				s_Instance = std::make_unique<Snowflake::Manager>();
			return *Manager::s_Instance;
		}

		Entity CreateEntity()
		{
			Entity entity = m_Entities.size();
			m_Entities.emplace_back(entity);
			return entity;
		}

		bool DestroyEntity(Entity entity)
		{
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			if(it != m_Entities.end())
			{
				std::swap(it, m_Entities.end());
				m_Entities.pop_back();
				return true;
			}
			return false;
		}

		template<class TComponent>
		TComponent& AddComponent(Entity entity)
		{
			auto& component = GetPool<TComponent>();
			component.RegisterEntity(entity);
			return component.GetComponent(entity);
		}

		template<class TComponent>
		bool HasComponent(Entity entity)
		{
			auto& component = GetPool<TComponent>();
			return component.IsEntityRegistered(entity);
		}

		template<class TComponent>
		void RemoveComponent(Entity entity)
		{
			auto& component = GetPool<TComponent>();
			component.DeRegisterEntity(entity);
		}

		template<class TComponent>
		bool CreateComponent()
		{
			auto pool = ComponentPool<TComponent>();
			size_t hash = typeid(TComponent).hash_code();
			m_ComponentPools[hash] = *reinterpret_cast<Pool*>(&pool);

			return true;
		}

	private:

		template<class TComponent>
		ComponentPool<TComponent>& GetPool()
		{
			return *reinterpret_cast<ComponentPool<TComponent>*>(&m_ComponentPools[Hash<TComponent>()]);
		}

		static std::unique_ptr<Snowflake::Manager> s_Instance;
		std::vector<Entity> m_Entities;

		std::unordered_map<size_t, Pool> m_ComponentPools;

	};
	std::unique_ptr<Snowflake::Manager> Manager::s_Instance;

	class Pool
	{
		friend Manager;
	public:
		uint16_t GetId() const { return m_Id; }
	protected:
		void SetId(uint16_t id) { m_Id = id; }

		uint16_t m_Id = -1;
	};

	template<class Component>
	class ComponentPool : Pool
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
		std::array<Component, 8192> m_Components;
		std::bitset<8192> m_RegisteredEntities = {false};
	};

}