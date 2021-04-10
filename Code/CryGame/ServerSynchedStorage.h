#pragma once

#include "ClientSynchedStorage.h"

class CServerSynchedStorage : public CNetMessageSinkHelper<CServerSynchedStorage, CSynchedStorage>
{
	struct SChannel
	{
		INetChannel *pNetChannel = nullptr;
		SSendableHandle lastOrderedMessage;
		bool local = false;
		bool onhold = false;

		SChannel() = default;

		SChannel(INetChannel *pNetChannel, bool isLocal)
		: pNetChannel(pNetChannel),
		  local(isLocal),
		  onhold(false)
		{
		}
	};

	struct SChannelQueueEnt
	{
		int channel = 0;
		TSynchedKey key = 0;

		SChannelQueueEnt() = default;

		SChannelQueueEnt(int channel, TSynchedKey key)
		: channel(channel),
		  key(key)
		{
		}

		bool operator<(const SChannelQueueEnt & rhs) const
		{
			return rhs.channel < channel || (rhs.channel == channel && rhs.key < key);
		}
	};

	struct SChannelEntityQueueEnt
	{
		int channel = 0;
		EntityId entity = 0;
		TSynchedKey key = 0;

		SChannelEntityQueueEnt() = default;

		SChannelEntityQueueEnt(int channel, EntityId entity, TSynchedKey key)
		: channel(channel),
		  entity(entity),
		  key(key)
		{
		}

		bool operator<(const SChannelEntityQueueEnt& rhs) const
		{
			if (channel < rhs.channel)
				return true;
			else if (channel > rhs.channel)
				return false;
			else if (key < rhs.key)
				return true;
			else if (key > rhs.key)
				return false;
			else if (entity < rhs.entity)
				return true;
			else if (entity > rhs.entity)
				return false;
			else
				return false;
		}
	};

	using TChannelQueueMap = std::map<SChannelQueueEnt, SSendableHandle>;
	using TChannelEntityQueueMap = std::map<SChannelEntityQueueEnt, SSendableHandle>;
	using TChannelMap = std::map<int, SChannel>;

	TChannelQueueMap m_globalQueue;
	TChannelQueueMap m_channelQueue;
	TChannelEntityQueueMap m_entityQueue;
	TChannelMap m_channels;
	CCryMutex m_mutex;

	SChannel *GetChannel(int channelId);
	SChannel *GetChannel(INetChannel *pNetChannel);
	int GetChannelId(INetChannel *pNetChannel) const;

public:
	CServerSynchedStorage(IGameFramework *pGameFramework)
	{
		m_pGameFramework = pGameFramework;
	}

	virtual void DefineProtocol(IProtocolBuilder *pBuilder);

	virtual void Reset();
	virtual void ResetChannel(int channelId);

	virtual bool OnSetGlobalMsgComplete(CClientSynchedStorage::CSetGlobalMsg *pMsg, int channelId, uint32 fromSeq, bool ack);
	virtual bool OnSetChannelMsgComplete(CClientSynchedStorage::CSetChannelMsg *pMsg, int channelId, uint32 fromSeq, bool ack);
	virtual bool OnSetEntityMsgComplete(CClientSynchedStorage::CSetEntityMsg *pMsg, int channelId, uint32 fromSeq, bool ack);

	// these should only be called from the main thread
	virtual void AddToGlobalQueue(TSynchedKey key);
	virtual void AddToEntityQueue(EntityId entityId, TSynchedKey key);
	virtual void AddToChannelQueue(int channelId, TSynchedKey key);

	virtual void AddToGlobalQueueFor(int channelId, TSynchedKey key);
	virtual void AddToEntityQueueFor(int channelId, EntityId entityId, TSynchedKey key);

	virtual void FullSynch(int channelId, bool reset);

	virtual void OnClientConnect(int channelId);
	virtual void OnClientDisconnect(int channelId, bool onhold);
	virtual void OnClientEnteredGame(int channelId);

	virtual void OnGlobalChanged(TSynchedKey key, const TSynchedValue & value);
	virtual void OnChannelChanged(int channelId, TSynchedKey key, const TSynchedValue & value);
	virtual void OnEntityChanged(EntityId entityId, TSynchedKey key, const TSynchedValue & value);
};
