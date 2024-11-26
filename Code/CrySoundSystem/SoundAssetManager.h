////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   SoundAssetManager.h
//  Version:     v1.00
//  Created:     26/4/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: SoundAssetManager is responsible for memory allocation of
//							 assets and answer queries about their state and priority
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SOUNDASSETMANAGER_H__
#define __SOUNDASSETMANAGER_H__

#pragma once

#include "ISoundAssetManager.h"

#define PRIORITY_MULTIPLYER 2.5
#define RENEW_FMOD_STATS_LIMIT 5

class CSoundAssetManager : public ISoundAssetManager
{
	struct SBufferCommand 
	{
		SBufferCommand(CSoundBuffer* const _pBuf, eUnloadDataOptions const _eUnloadOption, bool const _bRemoveBuffer)
			:	pBuffer(_pBuf),
				eUnloadOption(_eUnloadOption),
				bRemoveBuffer(_bRemoveBuffer){}

		CSoundBufferPtr			pBuffer;
		eUnloadDataOptions	eUnloadOption;
		bool 								bRemoveBuffer;

		friend bool operator == (const SBufferCommand& a, const SBufferCommand& b)
		{
			return (a.pBuffer == b.pBuffer) && (a.eUnloadOption == b.eUnloadOption) && (a.bRemoveBuffer == b.bRemoveBuffer);
		}
	};

	typedef std::vector<CSoundBufferPtr>				tvecSoundBufferPtr;
	typedef tvecSoundBufferPtr::iterator				tvecSoundBufferPtrIter;
	typedef tvecSoundBufferPtr::const_iterator	tvecSoundBufferPtrConstIter;

	typedef std::vector<SBufferCommand>					tvecBufferCommands;
	typedef tvecBufferCommands::iterator				tvecBufferCommandsIter;
	typedef tvecBufferCommands::const_iterator	tvecBufferCommandsConstIter;

	friend class CSoundSystem;

public:
	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////
	
	CSoundAssetManager(CSoundSystem *pSoundSystem);
	virtual ~CSoundAssetManager(void);

	// Cleans up dead bufffers, defrags or kicks out low priority Buffers
	bool Update();

	//////////////////////////////////////////////////////////////////////////
	// Memory Usage
	//////////////////////////////////////////////////////////////////////////

	// gives a big memory block in Bytes to the Manager
	bool SetMemoryBlock( void* pBlockStart, unsigned int nBlockSize);

	// Let the Manager allocate a big memory block in Bytes to deal with
	bool AllocMemoryBlock(unsigned int nBlockSize);

	// returns a Ptr to a free chunk of memory to be used
	void* GetMemoryChunk(SSoundBufferProps BufferProps, unsigned int nChunkSize);
	
	// frees a chunk of memory that was used in a SoundBuffer
	bool FreeMemoryChunk(SSoundBufferProps BufferProps);

	// frees all SoundBuffers, deletes all data;
	bool ShutDown();

	// Returns current Memory Usage
	unsigned int GetMemUsage();
	virtual void GetMemoryUsage(class ICrySizer* pSizer) const;		// compute memory-consumption, returns rough estimate in MB

	//////////////////////////////////////////////////////////////////////////
	
	unsigned int GetNumberBuffersLoaded();

	// writes output to screen in debug
	void DrawInformation(IRenderer* pRenderer, float xpos, float ypos, enumAssetManagerDrawInformation DrawMode); 

	//////////////////////////////////////////////////////////////////////////
	// Asset Management

	// gets or creates a SoundBuffer for a referred sound on a specific platform and returns the pointer to it
	CSoundBuffer* GetOrCreateSoundBuffer(const SSoundBufferProps &AssetProps, CSound* pRefSound);

	// removes a SoundBuffer from the AssetManager
	bool					RemoveSoundBuffer(const SSoundBufferProps &BufferProps);

	// locks the SoundBuffers so they don't get unloaded
	void					LockResources();

	// unlocks the SoundBuffers so they could be unload if needed
	void					UnlockResources();

	// unloads ALL SoundBuffers
	void					UnloadAllSoundBuffers();

	// removes project dependency
	void					RemoveProjectDependency();

	// informs about project availability
	void					ProjectWasLoaded(const ProjectIndex nProjectIndex);

	// Checks and returns a Ptr to a loaded Buffer or NULL if its not there
	CSoundBuffer* GetSoundBufferPtr(const SSoundBufferProps &BufferProps) const;

	// Check if a Buffer is loaded 
	bool IsLoaded(const SSoundBufferProps &BufferProps);			



private:

	CSoundSystem*	m_pSoundSystem;
	bool					m_bUseMemoryBlock;	// toggles if internal Memory Block should be used or not
	void*					m_MemoryBlockStart;	
	uint32				m_nMemoryBlockSize;
	uint32				m_nUsedMemorySize;
	uint32				m_nLastSoundBuffersIndex;

	float					m_fTotalSoundMemory;
	float					m_fTotalSecondaryMemory;
	uint32				m_nUsingOldFMODStatsCounter;
	float					m_fBudgetRatio;
	float					m_fMainThreshold;
	float					m_fSecondaryThreshold;
	
	SoundBufferPropsMap						m_soundBuffers; // Map of CSoundBuffer* with the filename as the key
	tvecSoundBufferPtr						m_lockedResources; // vector of SmartPtrs
	tvecBufferCommands						m_BufferCommands;

	typedef std::map<string, IWavebank* > tmapWavebanks;
	tmapWavebanks									m_Wavebanks;

	// what is the strategy to prevent memory defragmentation?
	bool DefragMemoryBlock();

	// This one decides if an asset should be kept in memory or kicked out ( == 0)
	float AssetStillNeeded(CSoundBuffer* pBuf);

	// internally removes a SoundBuffer from the AssetManager using reverse iterator
	//bool	RemoveSoundBuffer(SoundBufferPropsMapRevItor Iter);

};

#endif 
