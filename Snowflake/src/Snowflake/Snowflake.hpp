#pragma once
#include <algorithm>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
#include <cassert>
#include <unordered_map>
#include <functional>
#include "SnowID.h"
#define COMPONENT(comp) struct comp \
						
#define REGISTER_COMPONENT(GUID) const SnowID hashID = GUID;

namespace Snowflake
{
	inline std::unordered_map<SnowID, size_t> componentSizes;
	inline std::unordered_map<SnowID,type_info> componentTemplate;
	constexpr uint32_t InvalidEntity = ~0;

	using Entity = uint32_t;

	template<class Component>
	class ComponentPool;

	class IPool
	{
	public:
		virtual void RegisterEntity(Entity entity) = 0;
		virtual void DeRegisterEntity(Entity entity) = 0;
		virtual bool IsEntityRegistered(Entity entity) = 0;
		virtual std::vector<uint8_t> GetComponentData(Entity entity) = 0;
	};

	class Registry
	{
#ifdef USE_SERIALIZER
		friend class RegistrySerializer;
#endif
	public:
		Entity CreateEntity()
		{
			Entity entity = m_Entities.size();
			m_Entities.emplace_back(entity);
			return entity;
		}

		// fix complexety to O(1)
		bool DestroyEntity(Entity& entity)
		{
			if (entity == InvalidEntity) return false;
			auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
			if (it != m_Entities.end())
			{
				if (m_Entities.size() > 1)
					std::swap(*it, m_Entities.back());
				m_Entities.pop_back();
				while (!m_Registry[entity].empty())
				{
					static_cast<IPool*>(m_ComponentPools[m_Registry[entity].back()])->DeRegisterEntity(entity);
					m_Registry[entity].pop_back();
				}
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

		
		template<class TComponent>
		TComponent& AddComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("AddComponent called with invalid entity.");
			}
			auto& pool = MakeOrGetPool<TComponent>();

			pool.RegisterEntity(entity);
			m_Registry[entity].push_back(TComponent().hashID);
			return pool.GetComponent(entity);
		}

		template<class TComponent>
		TComponent* TryGetComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("TryGetComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			if (HasComponent<TComponent>(entity))
			{
				return &component.GetComponent(entity);
			}
			return nullptr;
		}

		template<class TComponent>
		TComponent& GetComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("GetComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			return component.GetComponent(entity);
		}

		template<class TComponent>
		void RemoveComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("RemoveComponent called with invalid entity.");
			}
			auto& pool = MakeOrGetPool<TComponent>();
			pool.DeRegisterEntity(entity);
			auto& registry = m_Registry[entity];
			auto it = std::find(registry.begin(), registry.end(), TComponent().hashID);
			if (it != registry.end())
			{
				registry.erase(it);
			}
		}

		template<typename TComponent>
		bool HasComponent(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("HasComponent called with invalid entity.");
			}
			auto& component = MakeOrGetPool<TComponent>();
			return component.IsEntityRegistered(entity);
		}

		template<typename ...TComponents>
		bool HasComponents(Entity entity)
		{
			if (entity == InvalidEntity)
			{
				throw std::invalid_argument("HasComponents called with invalid entity.");
			}
			return (HasComponent<TComponents>(entity) && ...);
		}

		template<class TFunction>
		void ForEach(TFunction&& func)
		{
			for (auto entity : m_Entities)
			{
				func(entity);
			}
		}

		template<class ...TComponents, class TFunction>
		void Execute(TFunction&& func)
		{
			for (auto entity : m_Entities)
			{
				if (HasComponents<TComponents...>(entity))
				{
					func(entity, GetComponent<TComponents>(entity)...);
				}
			}
		}
	private:

		void AddComponentFromData(std::vector<uint8_t>& data, SnowID& id, Entity entt)
		{
			/*AddComponent<>()*/
		}

		template<class TComponent>
		ComponentPool<TComponent>& MakeOrGetPool()
		{
			SnowID hash = TComponent().hashID;
			auto it = m_ComponentPools.find(hash);
			if (it == m_ComponentPools.end())
			{
				m_ComponentPools[hash] = new ComponentPool<TComponent>();
				componentSizes[hash] = sizeof(TComponent);
				//componentTemplate[hash] = TComponent;
			}
			return *static_cast<ComponentPool<TComponent>*>(m_ComponentPools[hash]);
		}
		template<class TComponent>
		ComponentPool<TComponent>& GetPoolWithHash(size_t hash)
		{
			return *static_cast<ComponentPool*>(m_ComponentPools[hash]);
		}

		std::vector<Entity> m_Entities;
		std::unordered_map<Entity, std::vector<SnowID>> m_Registry;
		std::unordered_map<SnowID, IPool*> m_ComponentPools;

	};
	

	template<class Component>
	class ComponentPool : public IPool
	{
		friend Registry;
	public:

		void RegisterEntity(Entity entity) override
		{
			if (!m_RegisteredEntities[entity])
			{
				m_RegisteredEntities[entity] = true;
				memcpy(&m_Components[entity], &m_Components[m_RegisteredEntities.size()], sizeof(Component));
			}
		}

		void DeRegisterEntity(Entity entity) override
		{
			m_RegisteredEntities[entity] = false;
			memcpy(&m_Components[entity], &m_Components[m_RegisteredEntities.size()], sizeof(Component));
		}

		bool IsEntityRegistered(Entity entity)override
		{
			return m_RegisteredEntities[entity];
		}

		Component& GetComponent(Entity entity)
		{
			return m_Components[entity];
		}

		std::vector<uint8_t> GetComponentData(Entity entity) override
		{
			std::vector<uint8_t> data;
			data.resize(sizeof(m_Components[m_RegisteredEntities.size()]));
			memcpy(data.data(), &m_Components[m_RegisteredEntities.size()], sizeof(m_Components[m_RegisteredEntities.size()]));
			return data;
		}
	private:
#ifdef COMPONENT_POOL_SIZE
		std::array<Component, COMPONENT_POOL_SIZE + 1> m_Components = {};
		std::bitset<COMPONENT_POOL_SIZE> m_RegisteredEntities = { false };
#else
		std::array<Component, 8192 + 1> m_Components = {};
		std::bitset<8192> m_RegisteredEntities = { false };
#endif
	};
}