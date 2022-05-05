#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>

namespace Snowflake
{
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

		template<class TComponent>
		TComponent& AddComponent(Entity entity)
		{

		}

	private:
		static std::unique_ptr<Snowflake::Manager> s_Instance;
		std::vector<Entity> m_Entities;
		std::vector<Pool> m_ComponentPools;

		template<class TComponent>
		ComponentPool<TComponent>& GetOrCreatePool()
		{
			for (size_t i = 0; i < m_ComponentPools.size(); i++)
			{
				if()
			}
		}

	};
	std::unique_ptr<Snowflake::Manager> Manager::s_Instance;

	class Pool
	{
		friend Manager;
	public:
		uint16_t GetId() const { return m_Id; }
	private:
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
			else
			{
				//TODO: entity already there;
			}
		}

		Component& GetType()
		{
			return Component;
		}

		Component& GetComponent(Entity entity)
		{
			if (m_RegisteredEntities[entity])
			{
				return Component;
			}
			return m_Components[entity];
		}
	private:
		std::array<Component, 8192> m_Components;
		std::bitset<8192> m_RegisteredEntities;
	};

}