////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   imoviesystem.h
//  Version:     v1.00
//  Created:     26/4/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __imoviesystem_h__
#define __imoviesystem_h__

#if _MSC_VER > 1000
#pragma once
#endif

#include "CryCommon/CryCore/smartptr.h"
#include "CryCommon/Cry3DEngine/CGF/CryHeaders.h"

#include "CryCommon/CryMath/Range.h"
#include "AnimKey.h"
#include "CryCommon/CryEntitySystem/IEntity.h"

// forward declaraton.
struct IAnimTrack;
struct IAnimNode;
struct IAnimSequence;
struct IMovieSystem;
struct IKey;
class XmlNodeRef;

typedef IMovieSystem* (*PFNCREATEMOVIESYSTEM)(struct ISystem*);

#define MAX_ANIM_NAME_LENGTH 64
//! Very high priority for cut scene sounds.
#define MOVIE_SOUND_PRIORITY 230

typedef std::vector<IAnimSequence*> AnimSequences;

// Common parameters of animation node.
enum AnimParamType
{
	APARAM_FOV = 0,	//!< FOV parameter id.
	APARAM_POS,			//!< Position parameter id.
	APARAM_ROT,			//!< Rotation parameter id.
	APARAM_SCL,			//!< Scale parameter id.
	APARAM_EVENT,		//!< Entity parameter id.
	APARAM_VISIBLE,	//!< Visibilty parameter id.
	APARAM_CAMERA,	//!< Camera parameter id.
	APARAM_SOUND1,	//!< Sound1 parameter id.
	APARAM_SOUND2,	//!< Sound2 parameter id.
	APARAM_SOUND3,	//!< Sound3 parameter id.
	APARAM_CHARACTER1,	//!< Character animation layer 1 parameter id.
	APARAM_CHARACTER2,	//!< Character animation layer 2 parameter id.
	APARAM_CHARACTER3,	//!< Character animation layer 3 parameter id.
	APARAM_SEQUENCE,//!< Sequence parameter id
	APARAM_EXPRESSION1,//!< Expression1 parameter id
	APARAM_EXPRESSION2,//!< Expression2 parameter id
	APARAM_EXPRESSION3,//!< Expression3 parameter id
	APARAM_CONSOLE,		//!< Console Commands.
	APARAM_MUSIC,			//!< Music triggering
	APARAM_FLOAT_1,		//!< General float parameter.
	APARAM_FACE_SEQUENCE, //!< Facial animation sequence.
	APARAM_LOOKAT,    //!< Look at parameter.

	APARAM_USER = 100,		//!< User node params.
	APARAM_LAST		//!< Last paramater id, must be always last in this enum.
};

#define APARAM_CHARACTER4 (APARAM_USER + 0x10)
#define APARAM_CHARACTER5 (APARAM_USER + 0x11)
#define APARAM_CHARACTER6 (APARAM_USER + 0x12)
#define APARAM_CHARACTER7 (APARAM_USER + 0x13)
#define APARAM_CHARACTER8 (APARAM_USER + 0x14)
#define APARAM_CHARACTER9 (APARAM_USER + 0x15)
#define APARAM_CHARACTER10 (APARAM_USER + 0x16)

#define APARAM_EXPRESSION4 (APARAM_USER + 0x20)
#define APARAM_EXPRESSION5 (APARAM_USER + 0x21)
#define APARAM_EXPRESSION6 (APARAM_USER + 0x22)
#define APARAM_EXPRESSION7 (APARAM_USER + 0x23)
#define APARAM_EXPRESSION8 (APARAM_USER + 0x24)
#define APARAM_EXPRESSION9 (APARAM_USER + 0x25)
#define APARAM_EXPRESSION10 (APARAM_USER + 0x26)


//! Types of animation track.
enum EAnimTrackType
{
	ATRACK_TCB_FLOAT,
	ATRACK_TCB_VECTOR,
	ATRACK_TCB_QUAT,
	ATRACK_BOOL,
	ATRACK_EVENT,
	ATRACK_SELECT,
	ATRACK_CHARACTER,
	ATRACK_SOUND,
	ATRACK_EXPRESSION,
	ATRACK_CONSOLE,
	ATRACK_MUSIC,
	ATRACK_FACESEQ,
	ATRACK_LOOKAT
};

//! Values that animation track can hold.
enum EAnimValue
{
	AVALUE_FLOAT,
	AVALUE_VECTOR,
	AVALUE_QUAT,
	AVALUE_BOOL,
	AVALUE_EVENT,
	AVALUE_SELECT,
	AVALUE_CHARACTER,
	AVALUE_SOUND,
	AVALUE_EXPRESSION,
	AVALUE_CONSOLE,
	AVALUE_MUSIC,
	AVALUE_FACESEQ,
	AVALUE_LOOKAT
};

//! Flags that can be set on animation track.
enum EAnimTrackFlags
{
	ATRACK_LINEAR	= 0x01,		//!< Use only linear interpolation between keys.
	ATRACK_LOOP = 0x02,			//!< Play this track in a loop.
	ATRACK_CYCLE = 0x04,		//!< Cycle track.
	
	//////////////////////////////////////////////////////////////////////////
	// Used by editor.
	//////////////////////////////////////////////////////////////////////////
	ATRACK_HIDDEN = 0x010, //!< Set when track is hidden in track view.
};

//////////////////////////////////////////////////////////////////////////
//! Node-Types
enum EAnimNodeType
{
	ANODE_ENTITY = 0x01,
	ANODE_SCENE = 0x02,
	ANODE_CAMERA = 0x03,
	ANODE_CVAR = 0x04,
	ANODE_SCRIPTVAR = 0x05,
	ANODE_MATERIAL = 0x06,
};

//! Flags that can be set on animation node.
enum EAnimNodeFlags
{
	//////////////////////////////////////////////////////////////////////////
	// Used by editor.
	//////////////////////////////////////////////////////////////////////////
	ANODE_FLAG_EXPANDED = 0x01, //!< Set when all track of animation node is expanded in track view.
	ANODE_FLAG_CAN_CHANGE_NAME = 0x02, //!< Set if this node allow changing of its name.
	ANODE_FLAG_SELECTED = 0x02, //!< Set if this node selected in editor.
};

//! Structure passed to Animate function.
struct SAnimContext
{
	SAnimContext()
	{
		bSingleFrame = false;
		bResetting = false;
		time = 0;
		dt = 0;
		fps = 0;
		sequence = 0;
	}
	float time;	//!< Current time in seconds.
	float	dt;		//!< Delta of time from previous animation frame in seconds.
	float	fps;	//!< Last calculated frames per second value.
	bool bSingleFrame;	//!< This is not a playing animation, more a single-frame update
	bool bResetting;	//!< Set when animation sequence is resetted.
	IAnimSequence *sequence; //!< Sequence in which animation performed.
};

//! Interface for Sequence-iterator.
struct ISequenceIt
{
	virtual void Release() = 0;
	virtual void add( IAnimSequence* element ) = 0;
	virtual void clear() = 0;
	virtual bool	empty() const = 0;
	virtual int	count() const = 0;
	virtual IAnimSequence*	first() = 0;
	virtual IAnimSequence*	next() = 0;
};

/** Parameters for cut-scene cameras
*/
struct SCameraParams
{
	SCameraParams()
	{
		cameraEntityId = 0;
		fFOV = 0.0f;
	}
	EntityId cameraEntityId; // Camera entity id.
	float fFOV;
};

//! Interface for movie-system implemented by user for advanced function-support
struct IMovieUser
{
	//! Called when movie system requests a camera-change.
	virtual void SetActiveCamera( const SCameraParams &Params ) = 0;
	//! Called when movie system enters into cut-scene mode.
	virtual void BeginCutScene( IAnimSequence* pSeq, unsigned long dwFlags,bool bResetFX) = 0;
	//! Called when movie system exits from cut-scene mode.
	virtual void EndCutScene( IAnimSequence* pSeq) = 0;
	//! Called when movie system wants to send a global event.
	virtual void SendGlobalEvent( const char *pszEvent ) = 0;
	//! Called to play subtitles for specified sound.
	virtual void PlaySubtitles( IAnimSequence* pSeq, ISound *pSound ) = 0;
};

//! Callback-class
struct IMovieCallback
{
	//! Callback-reasons
	enum ECallbackReason
	{
		CBR_ADDNODE,
		CBR_REMOVENODE,
		CBR_CHANGENODE,
	};

	// Called by movie system.
	virtual void OnMovieCallback( ECallbackReason reason,IAnimNode *pNode ) = 0;
	virtual void OnSetCamera( const SCameraParams &Params ) = 0;
};

/**	Interface of Animation Track.
*/
struct IAnimTrack : public _i_reference_target_t
{
	virtual EAnimTrackType GetType() = 0;
	virtual EAnimValue GetValueType() = 0;

	//! Return number of keys in track.
	virtual int GetNumKeys() = 0;

	//! Set number of keys in track.
	//! If needed adds empty keys at end or remove keys from end.
	virtual void SetNumKeys( int numKeys ) = 0;

	//! Remove specified key.
	virtual void RemoveKey( int num ) = 0;

	//! Get key at specified location.
	//! @param key Must be valid pointer to compatable key structure, to be filled with specified key location.
	virtual void GetKey( int index,IKey *key ) = 0;

	//! Get time of specified key.
	//! @return key time.
	virtual float GetKeyTime( int index ) = 0;

	//! Find key at givven time.
	//! @return Index of found key, or -1 if key with this time not found.
	virtual int FindKey( float time ) = 0;

	//! Get flags of specified key.
	//! @return key time.
	virtual int GetKeyFlags( int index ) = 0;

	//! Set key at specified location.
	//! @param key Must be valid pointer to compatable key structure.
	virtual void SetKey( int index,IKey *key ) = 0;

	//! Set time of specified key.
	virtual void SetKeyTime( int index,float time ) = 0;

	//! Set flags of specified key.
	virtual void SetKeyFlags( int index,int flags ) = 0;

	//! Sort keys in track (after time of keys was modified).
	virtual void SortKeys() = 0;

	//! Get track flags.
	virtual int GetFlags() = 0;
	
	//! Set track flags.
	virtual void SetFlags( int flags ) = 0;

	//! Create key at givven time, and return its index.
	//! @return Index of new key.
	virtual int CreateKey( float time ) = 0;

	//! Clone key at specified index.
	//! @retun Index of new key.
	virtual int CloneKey( int key ) = 0;

	//! Clone key at specified index from another track of SAME TYPE.
	//! @retun Index of new key.
	virtual int CopyKey( IAnimTrack *pFromTrack, int nFromKey ) = 0;

	//! Get info about specified key.
	//! @param Short human readable text description of this key.
	//! @param duration of this key in seconds.
	virtual void GetKeyInfo( int key,const char* &description,float &duration ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Get track value at specified time.
	// Interpolates keys if needed.
	//////////////////////////////////////////////////////////////////////////
	virtual void GetValue( float time,float &value ) = 0;
	virtual void GetValue( float time,Vec3 &value ) = 0;
	virtual void GetValue( float time,Quat &value ) = 0;
	virtual void GetValue( float time,bool &value ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Set track value at specified time.
	// Adds new keys if required.
	//////////////////////////////////////////////////////////////////////////
	virtual void SetValue( float time,const float &value,bool bDefault=false ) = 0;
	virtual void SetValue( float time,const Vec3 &value,bool bDefault=false ) = 0;
	virtual void SetValue( float time,const Quat &value,bool bDefault=false ) = 0;
	virtual void SetValue( float time,const bool &value,bool bDefault=false ) = 0;

	/** Assign active time range for this track.
	*/
	virtual void SetTimeRange( const Range &timeRange ) = 0;

	/** Serialize this animation track to XML.
	*/
	virtual bool Serialize( XmlNodeRef &xmlNode,bool bLoading, bool bLoadEmptyTracks=true ) = 0;


	virtual bool SerializeSelection( XmlNodeRef &xmlNode,bool bLoading, bool bCopySelected=false,float fTimeOffset=0) = 0;

protected:
	virtual ~IAnimTrack() {};
};

/** Animation block stores info about animations for one node in one sequence.
*/
struct IAnimBlock : public _i_reference_target_t
{
	/** Assign Id to this animation block.
	*/
	virtual void SetId( int id ) = 0;
	
	/** Get id of this block.
	*/
	virtual int	GetId() const = 0;

	/** Return number of parameters in block.
	*/
	virtual int GetTrackCount() const = 0;

	/** Return parameter type at index.
	*/
	virtual bool GetTrackInfo( int index,int &paramId,IAnimTrack **pTrack ) const = 0;

	/** Creates a new track.
	*/
	virtual IAnimTrack* CreateTrack( int paramId,EAnimValue valueType ) = 0;

	/** Retrieve first animation track assigned to parameter type.
	*/
	virtual IAnimTrack* GetTrack( int paramId ) const = 0;

	/** Assign animation track to parameter.
			if track parameter is NULL track with parameter id param will be removed.
	*/
	virtual void SetTrack( int paramId,IAnimTrack *track ) = 0;

	/** Set time range for all tracks in this sequence.
	*/
	virtual void SetTimeRange( Range timeRange ) = 0;

	/** Remove track from anim block.
	*/
	virtual bool RemoveTrack( IAnimTrack *pTrack ) = 0;

	/** Serialize this animation block to XML.
	*/
	virtual void Serialize( IAnimNode *pNode,XmlNodeRef &xmlNode,bool bLoading, bool bLoadEmptyTracks=true ) = 0;
};

/** Callback called by animation node when its animated.
*/
struct IAnimNodeOwner
{
	virtual void OnNodeAnimated( IAnimNode *pNode ) = 0;
};

/**	Base class for all Animation nodes,
		can host multiple animation tracks, and execute them other time.
		Animation node is reference counted.
*/
struct IAnimNode : public _i_reference_target_t
{
public:
	//! Set node name.
	virtual void SetName( const char *name ) = 0;
	
	//! Get node name.
	virtual const char* GetName() = 0;

	/** Get Type of this node.
	*/
	virtual EAnimNodeType GetType() const = 0;

	/** Set AnimNode flags.
		@param flags One or more flags from EAnimNodeFlags.
		@see EAnimNodeFlags
	*/
	virtual void SetFlags( int flags ) = 0;

	/** Get AnimNode flags.
		@return flags set on node.
		@see EAnimNodeFlags
	*/
	virtual int GetFlags() const = 0;

	/** Create a new track in this node for current animation block.
	*/
	virtual IAnimTrack* CreateTrack( int nParamId ) = 0;

	/** Remove a track from this node.
	*/
	virtual bool RemoveTrack( IAnimTrack *pTrack ) = 0;

	/** Find a track in this node and return its ParamID.
			@return ParamId of this track or -1 if track not found.
	*/
	virtual int FindTrack(IAnimTrack *pTrack) = 0;

	// Description:
	//		Creates default set of tracks supported by this node.
	virtual void CreateDefaultTracks() = 0;

	/** Get Id of this node.
	*/
	virtual int GetId() const = 0;

	// Assign an entity guid to the node, (Only for Entity targeting nodes)
	virtual void SetEntityGuid( const EntityGUID &guid ) = 0;
	// Retrieve a pointer to the entity Guid, return NULL if it is not an entity targeting node.
	virtual EntityGUID* GetEntityGuid() = 0;

	// Query entity of this node, if assigned.
	virtual IEntity* GetEntity() = 0;

	/** Return movie system that created this node.
	*/
	virtual IMovieSystem*	GetMovieSystem() = 0;

	/** Called when the node needs to be resetted (eg. when anim stops)
	*/
	virtual void Reset() = 0;
	
	/** Called when owning sequence pauses.
	*/
	virtual void Pause() = 0;
	/** Called when owning sequence resumes.
	*/
	virtual void Resume() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Space position/orientation scale.
	//////////////////////////////////////////////////////////////////////////
	//! Translate entity node.
	virtual void SetPos( float time,const Vec3 &pos ) = 0;
	//! Rotate entity node.
	virtual void SetRotate( float time,const Quat &quat ) = 0;
	//! Scale entity node.
	virtual void SetScale( float time,const Vec3 &scale ) = 0;

	//! Get current entity position.
	virtual Vec3 GetPos() = 0;
	//! Get current entity rotation.
	virtual Quat GetRotate() = 0;
	//! Get current entity scale.
	virtual Vec3 GetScale() = 0;

	// General Set param.
	// Set floating point parameter at givven time.
	// @return true if parameter set, false if this paremeter not exist in node.
	virtual bool SetParamValue( float time,AnimParamType param,float value ) = 0;
	// Get floating param at current time.
	// @return true if parameter exist, false if this paremeter not exist in node.
	virtual bool GetParamValue( float time,AnimParamType param,float &value ) = 0;
	
	//! Evaluate animation to the givven time.
	virtual void Animate( SAnimContext &ec ) = 0;

	/** Retrieve animation block interface assigned to this node.
	*/
	virtual IAnimBlock* GetAnimBlock() const = 0;

	/** Assign animation block to this node.
			Node can not be animated untill assigned with an anim block.
	*/
	virtual void SetAnimBlock( IAnimBlock *block ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Supported params.
	//////////////////////////////////////////////////////////////////////////
	enum	ESupportedParamFlags
	{
		PARAM_MULTIPLE_TRACKS = 0x01, // Set if parameter can be assigned multiple tracks.
	};
	struct SParamInfo
	{
		SParamInfo() : name(""),paramId(0),valueType(AVALUE_FLOAT),flags(0) {};
		SParamInfo( const char *_name,int _paramId,EAnimValue _valueType,int _flags ) : name(_name),paramId(_paramId),valueType(_valueType),flags(_flags) {};
		//////////////////////////////////////////////////////////////////////////
		const char *name;         // parameter name.
		int paramId;              // parameter id.
		EAnimValue valueType;     // value type, defines type of track to use for animating this parameter.
		int flags;                // combination of flags from ESupportedParamFlags.
	};
	
	// Description:
	//		Returns number of supported parameters by this animation node (position,rotation,scale,etc..).
	// Returns:
	//		Number of supported parameters.
	virtual int GetParamCount() const = 0;
	
	// Description:
	//		Returns decription of supported parameter of this animation node (position,rotation,scale,etc..).
	// Arguments:
	//		nIndex - parameter index in range 0 <= nIndex < GetSupportedParamCount()
	virtual bool GetParamInfo( int nIndex, SParamInfo &info ) const = 0;
	
	// Description:
	//		Finds param info with specified param id.
	// Returns:
	//		true if parameter found, false overwise.
	// Arguments:
	//		paramId - parameter id
	//    info - Filled with parameter info.
	virtual bool GetParamInfoFromId( int paramId, SParamInfo &info ) const = 0;

	// Description:
	//		Check if parameter is supported by this node.
	// Returns:
	//		true if parameter supported, false overwise.
	// Arguments:
	//		paramId - parameter id
	virtual bool IsParamValid( int paramId ) const = 0;

	/** Creates animation block compatable with this node.
	*/
//	virtual IAnimBlock* CreateAnimBlock() = 0;

	/** Retrieve animation track assigned to parameter.
	*/
	virtual IAnimTrack* GetTrack(  int nParamId ) const = 0;

	/** Assign animation track to parameter.
	*/
	virtual void SetTrack( int nParamId,IAnimTrack *track ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Callback for animation node used by editor.
	// Register notification callback with animation node.
	virtual void SetNodeOwner( IAnimNodeOwner *pOwner ) = 0;
	virtual IAnimNodeOwner* GetNodeOwner() = 0;

	/** Serialize this animation node to XML.
	*/
	virtual void Serialize( XmlNodeRef &xmlNode,bool bLoading ) = 0;

protected:
	virtual ~IAnimNode() {};
};

/**	Animation sequence, operates on animation nodes contained in it.
 */
struct IAnimSequence : public _i_reference_target_t
{
	//! Flags used for SetFlage(),GetFlags() methods.
	enum Flags
	{
		PLAY_ONRESET	= 0x001,	//!< Start playing this sequence immediately after reset of movie system(Level load).
		ORT_CONSTANT	= 0x002,	//!< Constant Out-Of-Range,time continues normally past sequence time range.
		ORT_LOOP			= 0x004,	//!< Loop Out-Of-Range,time wraps back to the start of range when reaching end of range.
		CUT_SCENE			= 0x008,	//!< Cut scene sequence.
		NO_HUD				= 0x010,	//!< Don`t display HUD
		NO_PLAYER			= 0x020,	//!< Disable input and drawing of player
		NO_PHYSICS		= 0x040,	//!< Don`t update physics
		NO_AI					= 0x080,	//!< Don`t update AI
		IS_16TO9			= 0x100,	//!< 16:9 bars in sequence
		NO_GAMESOUNDS	= 0x200,	//!< Suppress all game sounds.
		NO_SEEK       = 0x400,  //!< Cannot seek in sequence. 
		NO_ABORT      = 0x800,  //!< Cutscene can not be aborted
	};

	//! Set animation name.
	virtual void SetName( const char *name ) = 0;
	//! Get animation namr.
	virtual const char* GetName() = 0;

	virtual void SetFlags( int flags ) = 0;
	virtual int GetFlags() const = 0;

	//! Return number of animation nodes in sequence.
	virtual int GetNodeCount() const = 0;
	//! Get animation node at specified index.
	virtual IAnimNode* GetNode( int index ) const = 0;
	//! Get Animation assigned to node at specified index.
	virtual IAnimBlock* GetAnimBlock( int index ) const = 0;
	//! Add animation node to sequence.
	//! @return True if node added, same node will not be added 2 times.
	virtual bool AddNode( IAnimNode *node ) = 0;
	//! Remove animation node from sequence.
	virtual void RemoveNode( IAnimNode *node ) = 0;
	//! Add scene node to sequence.
	virtual IAnimNode* AddSceneNode() = 0;
	//! Remove all nodes from sequence.
	virtual void RemoveAll() = 0;

	/** Activate sequence by binding sequence animations to nodes.
			must be called prior animating sequence.
	*/
	virtual void Activate() = 0;
	
	/** Deactivates sequence by unbinding sequence animations from nodes.
	*/
	virtual void Deactivate() = 0;

	/** Evaluate animations of all nodes in sequence.
			Sequence must be activated before animating.
	*/
	virtual void Animate( SAnimContext &ec ) = 0;

	//! Set time range of this sequence.
	virtual void SetTimeRange( Range timeRange ) = 0;

	//! Get time range of this sequence.
	virtual Range GetTimeRange() = 0;

	//! Resets the sequence
	virtual void Reset( bool bSeekToStart ) = 0;

	/** Called to pause sequence.
	*/
	virtual void Pause() = 0;
	
	/** Called to resume sequence.
	*/
	virtual void Resume() = 0;

	/** Scale all key in tracks from previous time range to new time range.
	*/
	virtual void ScaleTimeRange( const Range &timeRange ) = 0;

	/** Serialize this sequence to XML.
	*/
	virtual void Serialize( XmlNodeRef &xmlNode,bool bLoading, bool bLoadEmptyTracks=true ) = 0;
};

/**	Movie Listener interface.
    Register at movie system to get notified about movie events
*/
struct IMovieListener
{
	enum EMovieEvent
	{
		MOVIE_EVENT_START = 0,   // fired when sequence is started
		MOVIE_EVENT_STOP,        // fired when sequence ended normally
		MOVIE_EVENT_ABORTED,     // fired when sequence was aborted before normal end (STOP and ABORTED event are mutually exclusive!)
	};

	//! callback on movie events
	virtual void OnMovieEvent(EMovieEvent movieEvent, IAnimSequence* pAnimSequence) = 0;
};

/**	Movie System interface.
		Main entrance point to engine movie capability.
		Enumerate available movies, update all movies, create animation nodes and tracks.
 */
struct IMovieSystem
{
	enum ESequenceStopBehavior
	{
		ONSTOP_LEAVE_TIME = 0,			// When sequence is stopped it remains in last played time.
		ONSTOP_GOTO_END_TIME = 1,		// Default behavior in game, sequence is animated at end time before stop.
		ONSTOP_GOTO_START_TIME = 2,	// Default behavior in editor, sequence is animated at start time before stop.
	};
	//! Release movie system.
	virtual void Release() = 0;
	//! Set the user.
	virtual void SetUser(IMovieUser *pUser) = 0;
	//! Get the user.
	virtual IMovieUser* GetUser() = 0;
	//! Loads all nodes and sequences from a specific file (should be called when the level is loaded).
	virtual bool Load(const char *pszFile, const char *pszMission) = 0;

	// Description:
	//		Creates a new animation node with specified type.
	// Arguments:
	//		nodeType - Type of node, one of EAnimNodeType enums.
	//		nodeId - Node ID if zero movie system will generate unique id.
	virtual IAnimNode* CreateNode( int nodeType,int nodeId=0 ) = 0;

	// Description:
	//		Creates a new animation track with specified type.
	// Arguments:
	//		type - Type of track, one of EAnimTrackType enums.
	virtual IAnimTrack* CreateTrack( EAnimTrackType type ) = 0;

	virtual void ChangeAnimNodeId( int nodeId,int newNodeId ) = 0;

	virtual IAnimSequence* CreateSequence( const char *sequence ) = 0;
	virtual IAnimSequence* LoadSequence( XmlNodeRef &xmlNode, bool bLoadEmpty=true ) = 0;
	virtual void RemoveSequence( IAnimSequence *seq ) = 0;
	virtual IAnimSequence* FindSequence( const char *sequence ) = 0;
	virtual ISequenceIt* GetSequences(bool bPlayingOnly=false, bool bCutscenesOnly=false) = 0;
	
	/** Adds a listener to a sequence
			@param pSequence Pointer to sequence
			@param pListener Pointer to an IMovieListener
			@return true on successful add, false otherwise
	*/
	virtual bool AddMovieListener(IAnimSequence* pSequence, IMovieListener* pListener) = 0;

	/** Removes a listener from a sequence
			@param pSequence Pointer to sequence
			@param pListener Pointer to an IMovieListener
			@return true on successful removal, false otherwise
	*/
	virtual bool RemoveMovieListener(IAnimSequence* pSequence, IMovieListener* pListener) = 0;

	virtual ISystem* GetSystem() = 0;

	/** Remove all sequences from movie system.
	*/
	virtual void RemoveAllSequences() = 0;

	/** Remove all nodes from movie system.
	*/
	virtual void RemoveAllNodes() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Sequence playback.
	//////////////////////////////////////////////////////////////////////////
	/** Start playing sequence.
			Call ignored if sequence is already playing.
			@param sequence Name of sequence to play.
	*/
	virtual void PlaySequence( const char *sequence,bool bResetFX ) = 0;

	/** Start playing sequence.
			Call ignored if sequence is already playing.
			@param sequence Pointer to Valid sequence to play.
	*/
	virtual void PlaySequence( IAnimSequence *seq,bool bResetFX  ) = 0;

	/** Stops currently playing sequence.
			Ignored if sequence is not playing.
			Returns true if sequence has been stopped. false otherwise
			@param sequence Name of playing sequence to stop.
	*/
	virtual bool StopSequence( const char *sequence ) = 0;

	/** Stop's currently playing sequence.
			Ignored if sequence is not playing.
			Returns true if sequence has been stopped. false otherwise
			@param sequence Pointer to Valid sequence to stop.
	*/
	virtual bool StopSequence( IAnimSequence *seq ) = 0;

	/** Aborts a currently playing sequence.
			Ignored if sequence is not playing.
			Calls IMovieListener with MOVIE_EVENT_ABORTED event (MOVIE_EVENT_DONE is NOT called)
			Returns true if sequence has been aborted. false otherwise
			@param sequence Pointer to Valid sequence to stop.
			@param bLeaveTime If false, uses default stop behaviour, otherwise leaves the sequence at time
	*/
	virtual bool AbortSequence( IAnimSequence *seq, bool bLeaveTime=false ) = 0;

	/** Stops all currently playing sequences.
	*/
	virtual void StopAllSequences() = 0;

	/** Stops all playing cut-scene sequences.
			This will not stop all sequences, but only those with CUT_SCENE flag set.
	*/
	virtual void StopAllCutScenes() = 0;

	/** Checks if specified sequence is playing.
	*/
	virtual bool IsPlaying( IAnimSequence *seq ) const = 0;

	/** Resets playback state of movie system,
			usually called after loading of level,
			sequences with PLAY_ONRESET flag will start playing after this call if bPlayOnReset is true.
	*/
	virtual void Reset( bool bPlayOnReset,bool bSeekAllToStart ) = 0;

	/** Sequences with PLAY_ONRESET flag will start playing after this call.
	*/
	virtual void PlayOnLoadSequences() = 0;

	/** Update movie system every frame to animate all playing sequences.
	*/
	virtual void Update( float dt ) = 0;


	/** Remove node from movie system.
			This node also deleted from all sequences that referencing it.
			@param node Must be a valid pointer to the animation node to remove.
	*/
	virtual void RemoveNode( IAnimNode* node ) = 0;

	/** Finds node by id.
			@param nodeId Id of requested animation node.
			@return Pointer to the animation node, or NULL if the node with specified id is not found.
	*/
	virtual IAnimNode* GetNode( int nodeId ) const = 0;

	/** Finds node by Name. Much slower then getting node by id.
			Name is case insesentive.
			@param nodeName Name of requested animation node.
			@return Pointer to the animation node, or NULL if the node with specified name is not found.
	*/
	virtual IAnimNode* FindNode( const char *nodeName ) const = 0;

	/** Set movie system into recording mode,
			While in recording mode any changes made to node will be added as keys to tracks.
	*/
	virtual void SetRecording( bool recording ) = 0;
	virtual bool IsRecording() const = 0;

	/** Pause any playing sequences.
	*/
	virtual void Pause() = 0;

	/** Resume playing sequences.
	*/
	virtual void Resume() = 0;


	/** Callback when animation-data changes
	*/
	virtual void SetCallback( IMovieCallback *pCallback ) = 0;

	/** Serialize to XML.
	*/
	virtual void Serialize( XmlNodeRef &xmlNode,bool bLoading, bool bRemoveOldNodes=false, bool bLoadEmpty=true ) = 0;

	virtual const SCameraParams& GetCameraParams() const = 0;
	virtual void SetCameraParams( const SCameraParams &Params ) = 0;
	virtual void SendGlobalEvent( const char *pszEvent ) = 0;

	/** Gets the float time value for a sequence that is already playing
	*/
	virtual float GetPlayingTime(IAnimSequence * pSeq) = 0;
	/** Sets the time progression of an already playing cutscene.
	    If IAnimSequence:NO_SEEK flag is set on pSeq, this call is ignored.
	*/
	virtual bool SetPlayingTime(IAnimSequence * pSeq, float fTime)=0;
	// Set behavior pattern for stopping sequences.
	virtual void SetSequenceStopBehavior( ESequenceStopBehavior behavior )=0;
};

#endif // __imoviesystem_h__
