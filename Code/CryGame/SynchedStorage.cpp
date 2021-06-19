#include "CryCommon/CrySystem/ISystem.h"
#include "CryCommon/CryEntitySystem/IEntitySystem.h"

#include "SynchedStorage.h"

static void DumpValue(TSynchedKey key, const TSynchedValue & value)
{
	switch(value.GetType())
	{
		case eSVT_Bool:
		{
			CryLogAlways("  %.08d -     bool: %s", key, *value.GetPtr<bool>() ? "true" : "false");
			break;
		}
		case eSVT_Float:
		{
			CryLogAlways("  %.08d -    float: %f", key, *value.GetPtr<float>());
			break;
		}
		case eSVT_Int:
		{
			CryLogAlways("  %.08d -      int: %d", key, *value.GetPtr<int>());
			break;
		}
		case eSVT_EntityId:
		{
			CryLogAlways("  %.08d - entityId: %.08x", key, *value.GetPtr<EntityId>());
			break;
		}
		case eSVT_String:
		{
			CryLogAlways("  %.08d -  string: %s", key, value.GetPtr<string>()->c_str());
			break;
		}
		default:
		{
			CryLogAlways("  %.08d - unknown: %.08x", key, *value.GetPtr<uint32>());
			break;
		}
	}
}

void CSynchedStorage::Reset()
{
	m_globalStorage.clear();
	m_entityStorage.clear();
	m_channelStorageMap.clear();
	m_channelStorage.clear();
}

void CSynchedStorage::Dump()
{
	CryLogAlways("---------------------------");
	CryLogAlways(" SYNCHED STORAGE DUMP");
	CryLogAlways("---------------------------\n");
	CryLogAlways("Globals:");

	for (const auto & item : m_globalStorage)
	{
		DumpValue(item.first, item.second);
	}

	CryLogAlways("---------------------------\n");
	CryLogAlways("Local Channel:");

	for (const auto & item : m_channelStorage)
	{
		DumpValue(item.first, item.second);
	}

	if (gEnv->bServer)
	{
		CryLogAlways("---------------------------\n");

		for (const auto & channelItem : m_channelStorageMap)
		{
			INetChannel *pNetChannel = m_pGameFramework->GetNetChannel(channelItem.first);
			const char *name = pNetChannel ? pNetChannel->GetName() : "null";

			CryLogAlways("Channel %d (%s)", channelItem.first, name);

			for (const auto & item : channelItem.second)
			{
				DumpValue(item.first, item.second);
			}
		}
	}

	CryLogAlways("---------------------------\n");

	for (const auto & entityItem : m_entityStorage)
	{
		IEntity *pEntity = gEnv->pEntitySystem->GetEntity(entityItem.first);
		const char *name = pEntity ? pEntity->GetName() : "null";

		CryLogAlways("Entity %.08d(%s)", entityItem.first, name);

		for (const auto & item : entityItem.second)
		{
			DumpValue(item.first, item.second);
		}
	}
}

void CSynchedStorage::SerializeValue(TSerialize ser, TSynchedKey & key, TSynchedValue & value, int type)
{
	ser.Value("key", key, 'ssk');

	switch (type)
	{
		case eSVT_Bool:
		{
			bool b;

			if (ser.IsWriting())
			{
				b = *value.GetPtr<bool>();
			}

			ser.Value("value", b, 'bool');

			if (ser.IsReading())
			{
				SetGlobalValue(key, b);
			}

			break;
		}
		case eSVT_Float:
		{
			float f;

			if (ser.IsWriting())
			{
				f = *value.GetPtr<float>();
			}

			ser.Value("value", f, 'ssfl');

			if (ser.IsReading())
			{
				SetGlobalValue(key, f);
			}

			break;
		}
		case eSVT_Int:
		{
			int i;

			if (ser.IsWriting())
			{
				i = *value.GetPtr<int>();
			}

			ser.Value("value", i, 'ssi');

			if (ser.IsReading())
			{
				SetGlobalValue(key, i);
			}

			break;
		}
		case eSVT_EntityId:
		{
			EntityId e;

			if (ser.IsWriting())
			{
				e = *value.GetPtr<EntityId>();
			}

			ser.Value("value", e, 'eid');

			if (ser.IsReading())
			{
				SetGlobalValue(key, e);
			}

			break;
		}
		case eSVT_String:
		{
			string s;

			if (ser.IsWriting())
			{
				s = *value.GetPtr<string>();
			}

			ser.Value("value", s);

			if (ser.IsReading())
			{
				SetGlobalValue(key, s);
			}

			break;
		}
	}
}

void CSynchedStorage::SerializeEntityValue(TSerialize ser, EntityId id, TSynchedKey & key, TSynchedValue & value, int type)
{
	ser.Value("key", key, 'ssk');

	switch (type)
	{
		case eSVT_Bool:
		{
			bool b;

			if (ser.IsWriting())
			{
				b = *value.GetPtr<bool>();
			}

			ser.Value("value", b, 'bool');

			if (ser.IsReading())
			{
				SetEntityValue(id, key, b);
			}

			break;
		}
		case eSVT_Float:
		{
			float f;

			if (ser.IsWriting())
			{
				f = *value.GetPtr<float>();
			}

			ser.Value("value", f, 'ssfl');

			if (ser.IsReading())
			{
				SetEntityValue(id, key, f);
			}

			break;
		}
		case eSVT_Int:
		{
			int i;

			if (ser.IsWriting())
			{
				i = *value.GetPtr<int>();
			}

			ser.Value("value", i, 'ssi');

			if (ser.IsReading())
			{
				SetEntityValue(id, key, i);
			}

			break;
		}
		case eSVT_EntityId:
		{
			EntityId e;

			if (ser.IsWriting())
			{
				e = *value.GetPtr<EntityId>();
			}

			ser.Value("value", e, 'eid');

			if (ser.IsReading())
			{
				SetEntityValue(id, key, e);
			}

			break;
		}
		case eSVT_String:
		{
			string s;

			if (ser.IsWriting())
			{
				s = *value.GetPtr<string>();
			}

			ser.Value("value", s);

			if (ser.IsReading())
			{
				SetEntityValue(id, key, s);
			}

			break;
		}
	}
}

CSynchedStorage::TStorage *CSynchedStorage::GetEntityStorage(EntityId id, bool create)
{
	auto it = m_entityStorage.find(id);
	if (it == m_entityStorage.end())
	{
		if (create)
		{
			return &m_entityStorage[id];
		}

		return nullptr;
	}

	return &it->second;
}

CSynchedStorage::TStorage *CSynchedStorage::GetChannelStorage(int channelId, bool create)
{
	auto it = m_channelStorageMap.find(channelId);
	if (it == m_channelStorageMap.end())
	{
		INetChannel *pNetChannel = m_pGameFramework->GetNetChannel(channelId);
		if ((!gEnv->bServer && !pNetChannel) || (gEnv->bServer && pNetChannel->IsLocal()))
			return &m_channelStorage;

		if (gEnv->bServer && create)
		{
			return &m_channelStorageMap[channelId];
		}

		return nullptr;
	}

	return &it->second;
}
