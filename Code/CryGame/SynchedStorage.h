#pragma once

#include <map>

#include "CryCommon/CryNetwork/INetwork.h"
#include "CryCommon/CryAction/IGameFramework.h"
#include "CryCommon/CryCore/ConfigurableVariant.h"

using TSynchedValueTypes = NTypelist::CConstruct<bool, float, int, EntityId, string>::TType;

enum ESynchedValueTypes
{
	eSVT_None     = -1,
	eSVT_Bool     = NTypelist::IndexOf<bool,     TSynchedValueTypes>::value,
	eSVT_Float    = NTypelist::IndexOf<float,    TSynchedValueTypes>::value,
	eSVT_Int      = NTypelist::IndexOf<int,      TSynchedValueTypes>::value,
	eSVT_EntityId = NTypelist::IndexOf<EntityId, TSynchedValueTypes>::value,
	eSVT_String   = NTypelist::IndexOf<string,   TSynchedValueTypes>::value,
};

using TSynchedKey = uint16;
using TSynchedValue = CConfigurableVariant<TSynchedValueTypes, sizeof (void*)>;

class CSynchedStorage : public INetMessageSink
{
public:
	using TStorage = std::map<TSynchedKey, TSynchedValue>;

	using TEntityStorageMap = std::map<EntityId, TStorage>;

protected:
	TStorage m_globalStorage;
	TEntityStorageMap m_entityStorage;

	IGameFramework *m_pGameFramework = nullptr;

	CSynchedStorage() = default;

public:
	virtual ~CSynchedStorage() = default;

	template<typename ValueType>
	void SetGlobalValue(TSynchedKey key, const ValueType & value)
	{
		auto it = m_globalStorage.find(key);
		if (it == m_globalStorage.end())
		{
			TSynchedValue & newValue = m_globalStorage[key];
			newValue.Set(value);

			OnGlobalChanged(key, newValue);
		}
		else
		{
			const ValueType *pStoredValue = it->second.GetPtr<ValueType>();
			if (!pStoredValue || *pStoredValue != value)
			{
				it->second.Set(value);

				OnGlobalChanged(key, it->second);
			}
		}
	}

	template<typename ValueType>
	void SetEntityValue(EntityId id, TSynchedKey key, const ValueType & value)
	{
		TStorage & storage = *GetEntityStorage(id, true);

		auto it = storage.find(key);
		if (it == storage.end())
		{
			TSynchedValue & newValue = storage[key];
			newValue.Set(value);

			OnEntityChanged(id, key, newValue);
		}
		else
		{
			const ValueType *pStoredValue = it->second.GetPtr<ValueType>();
			if (!pStoredValue || *pStoredValue != value)
			{
				it->second.Set(value);

				OnEntityChanged(id, key, it->second);
			}
		}
	}

	template<typename ValueType>
	bool GetGlobalValue(TSynchedKey key, ValueType & value) const
	{
		auto it = m_globalStorage.find(key);
		if (it == m_globalStorage.end())
		{
			return false;
		}

		const ValueType *pStoredValue = it->second.GetPtr<ValueType>();
		if (!pStoredValue)
		{
			return false;
		}

		value = *pStoredValue;

		return true;
	}

	bool GetGlobalValue(TSynchedKey key, TSynchedValue & value) const
	{
		auto it = m_globalStorage.find(key);
		if (it == m_globalStorage.end())
		{
			return false;
		}

		value = it->second;

		return true;
	}

	template<typename ValueType>
	bool GetEntityValue(EntityId entityId, TSynchedKey key, ValueType & value) const
	{
		auto eit = m_entityStorage.find(entityId);
		if (eit == m_entityStorage.end())
		{
			return false;
		}

		auto it = eit->second.find(key);
		if (it == eit->second.end())
		{
			return false;
		}

		const ValueType *pStoredValue = it->second.GetPtr<ValueType>();
		if (!pStoredValue)
		{
			return false;
		}

		value = *pStoredValue;

		return true;
	}

	bool GetEntityValue(EntityId entityId, TSynchedKey key, TSynchedValue & value) const
	{
		auto eit = m_entityStorage.find(entityId);
		if (eit == m_entityStorage.end())
		{
			return false;
		}

		auto it = eit->second.find(key);
		if (it == eit->second.end())
		{
			return false;
		}

		value = it->second;

		return true;
	}

	int GetGlobalValueType(TSynchedKey key) const
	{
		auto it = m_globalStorage.find(key);
		if (it == m_globalStorage.end())
		{
			return eSVT_None;
		}

		return it->second.GetType();
	}

	int GetEntityValueType(EntityId id, TSynchedKey key) const
	{
		auto eit = m_entityStorage.find(id);
		if (eit == m_entityStorage.end())
		{
			return eSVT_None;
		}

		auto it = eit->second.find(key);
		if (it == eit->second.end())
		{
			return eSVT_None;
		}

		return it->second.GetType();
	}

	virtual void Reset();

	virtual void Dump();

	virtual void SerializeValue(TSerialize ser, TSynchedKey & key, TSynchedValue & value, int type);
	virtual void SerializeEntityValue(TSerialize ser, EntityId id, TSynchedKey & key, TSynchedValue & value, int type);

	virtual TStorage *GetEntityStorage(EntityId id, bool create = false);

	virtual void OnGlobalChanged(TSynchedKey key, const TSynchedValue & value)
	{
	}

	virtual void OnEntityChanged(EntityId id, TSynchedKey key, const TSynchedValue & value)
	{
	}
};
