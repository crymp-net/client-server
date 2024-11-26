/*
    fmod_event.hpp - Data-driven event classes
    Copyright (c), Firelight Technologies Pty, Ltd. 2004-2009.

    This header is the base header for all other FMOD EventSystem headers. If you are programming in C use FMOD_EVENT.H
*/

#ifndef __FMOD_EVENT_HPP__
#define __FMOD_EVENT_HPP__

#ifndef _FMOD_HPP
#include "fmod.hpp"
#endif
#ifndef __FMOD_EVENT_H__
#include "fmod_event.h"
#endif

namespace FMOD
{
    class EventSystem;
    class EventCategory;
    class EventProject;
    class EventGroup;
    class Event;
    class EventParameter;
    class EventReverb;
    class Event3DReverb;
    class MusicSystem;
    class MusicPrompt;

    /*
        FMOD EventSystem factory functions.
    */
    inline FMOD_RESULT EventSystem_Create(EventSystem **eventsystem) { return FMOD_EventSystem_Create((FMOD_EVENTSYSTEM **)eventsystem); }

    /*
       'EventSystem' API
    */
    class EventSystem
    {
        public :

        // Initialization / system functions.
        virtual FMOD_RESULT F_API init                      (int maxchannels, FMOD_INITFLAGS flags, void *extradriverdata, FMOD_EVENT_INITFLAGS eventflags = FMOD_EVENT_INIT_NORMAL) = 0;
        virtual FMOD_RESULT F_API release                   () = 0;
        virtual FMOD_RESULT F_API update                    () = 0;
        virtual FMOD_RESULT F_API setMediaPath              (const char *path) = 0;
        virtual FMOD_RESULT F_API setPluginPath             (const char *path) = 0;
        virtual FMOD_RESULT F_API getVersion                (unsigned int *version) = 0;
        virtual FMOD_RESULT F_API getInfo                   (FMOD_EVENT_SYSTEMINFO *info) = 0;
        virtual FMOD_RESULT F_API getSystemObject           (System **system) = 0;
        virtual FMOD_RESULT F_API getMusicSystem            (MusicSystem **musicsystem) = 0;
                                                            
        // FEV load/unload.                                 
        virtual FMOD_RESULT F_API load                      (const char *name_or_data, FMOD_EVENT_LOADINFO *loadinfo, EventProject **project) = 0;
        virtual FMOD_RESULT F_API unload                    () = 0;
                                                            
        // Event,EventGroup,EventCategory Retrieval.        
        virtual FMOD_RESULT F_API getProject                (const char *name, EventProject **project) = 0;
        virtual FMOD_RESULT F_API getProjectByIndex         (int index,        EventProject **project) = 0;
        virtual FMOD_RESULT F_API getNumProjects            (int *numprojects) = 0;
        virtual FMOD_RESULT F_API getCategory               (const char *name, EventCategory **category) = 0;
        virtual FMOD_RESULT F_API getCategoryByIndex        (int index,        EventCategory **category) = 0;
        virtual FMOD_RESULT F_API getMusicCategory          (EventCategory **category) = 0;
        virtual FMOD_RESULT F_API getNumCategories          (int *numcategories) = 0;
        virtual FMOD_RESULT F_API getGroup                  (const char *name, bool cacheevents, EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getEvent                  (const char *name, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getEventBySystemID        (unsigned int systemid, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getEventByGUID            (const FMOD_GUID *guid, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getEventByGUIDString      (const char *guid, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getNumEvents              (int *numevents) = 0;

        // Reverb interfaces.
        virtual FMOD_RESULT F_API setReverbProperties       (const FMOD_REVERB_PROPERTIES *prop) = 0;
        virtual FMOD_RESULT F_API getReverbProperties       (FMOD_REVERB_PROPERTIES *prop) = 0;

        virtual FMOD_RESULT F_API getReverbPreset           (const char *name, FMOD_REVERB_PROPERTIES *prop, int *index = 0) = 0;
        virtual FMOD_RESULT F_API getReverbPresetByIndex    (const int index,  FMOD_REVERB_PROPERTIES *prop, char **name = 0) = 0;
        virtual FMOD_RESULT F_API getNumReverbPresets       (int *numpresets) = 0;

        virtual FMOD_RESULT F_API createReverb              (EventReverb **reverb) = 0;
        virtual FMOD_RESULT F_API setReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop) = 0;
        virtual FMOD_RESULT F_API getReverbAmbientProperties(FMOD_REVERB_PROPERTIES *prop) = 0;

        // 3D Listener interface.
        virtual FMOD_RESULT F_API set3DNumListeners         (int numlisteners) = 0;
        virtual FMOD_RESULT F_API get3DNumListeners         (int *numlisteners) = 0;
        virtual FMOD_RESULT F_API set3DListenerAttributes   (int listener, const FMOD_VECTOR *pos, const FMOD_VECTOR *vel, const FMOD_VECTOR *forward, const FMOD_VECTOR *up) = 0;
        virtual FMOD_RESULT F_API get3DListenerAttributes   (int listener, FMOD_VECTOR *pos, FMOD_VECTOR *vel, FMOD_VECTOR *forward, FMOD_VECTOR *up) = 0;

        // Get/set user data
        virtual FMOD_RESULT F_API setUserData               (void *userdata) = 0;
        virtual FMOD_RESULT F_API getUserData               (void **userdata) = 0;

        // Pre-loading FSB files (from disk or from memory, use FMOD_OPENMEMORY_POINT to point to pre-loaded memory).
        virtual FMOD_RESULT F_API preloadFSB                (const char *filename, int streaminstance, Sound *sound) = 0;
        virtual FMOD_RESULT F_API unloadFSB                 (const char *filename, int streaminstance) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo             (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;

        virtual ~EventSystem(){};
    };

    /*
       'EventProject' API
    */
    class EventProject
    {
        public :

        virtual FMOD_RESULT F_API release            () = 0;
        virtual FMOD_RESULT F_API getInfo            (int *index, char **name) = 0;
        virtual FMOD_RESULT F_API getGroup           (const char *name, bool cacheevents, EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getGroupByIndex    (int index,        bool cacheevents, EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getNumGroups       (int *numgroups) = 0;
        virtual FMOD_RESULT F_API getEvent           (const char *name, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getEventByProjectID(unsigned int projectid, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getNumEvents       (int *numevents) = 0;
        virtual FMOD_RESULT F_API loadSampleData     (int *eventid_array, int sizeof_eventid_array, char **groupname_array, int sizeof_groupname_array, FMOD_EVENT_MODE eventmode) = 0;
        virtual FMOD_RESULT F_API setUserData        (void *userdata) = 0;
        virtual FMOD_RESULT F_API getUserData        (void **userdata) = 0;

        virtual FMOD_RESULT F_API stopAllEvents      (bool immediate = false) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~EventProject(){};
    };

    /*
       'EventGroup' API
    */
    class EventGroup
    {
        public :

        virtual FMOD_RESULT F_API getInfo            (int *index, char **name) = 0;
        virtual FMOD_RESULT F_API loadEventData      (FMOD_EVENT_RESOURCE resource = FMOD_EVENT_RESOURCE_STREAMS_AND_SAMPLES, FMOD_EVENT_MODE mode = FMOD_EVENT_DEFAULT) = 0;
        virtual FMOD_RESULT F_API freeEventData      (Event *event = 0, bool waituntilready = true) = 0;
        virtual FMOD_RESULT F_API getGroup           (const char *name, bool cacheevents, EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getGroupByIndex    (int index,        bool cacheevents, EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getParentGroup     (EventGroup **group) = 0;
        virtual FMOD_RESULT F_API getParentProject   (EventProject **project) = 0;
        virtual FMOD_RESULT F_API getNumGroups       (int *numgroups) = 0;
        virtual FMOD_RESULT F_API getEvent           (const char *name, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getEventByIndex    (int index,        FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getNumEvents       (int *numevents) = 0;
        virtual FMOD_RESULT F_API getProperty        (const char *propertyname, void *value) = 0;
        virtual FMOD_RESULT F_API getPropertyByIndex (int propertyindex, void *value) = 0;
        virtual FMOD_RESULT F_API getNumProperties   (int *numproperties) = 0;
        virtual FMOD_RESULT F_API getState           (FMOD_EVENT_STATE *state) = 0;
        virtual FMOD_RESULT F_API setUserData        (void *userdata) = 0;
        virtual FMOD_RESULT F_API getUserData        (void **userdata) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~EventGroup(){};
    };

    /*
       'EventCategory' API
    */
    class EventCategory
    {
        public :

        virtual FMOD_RESULT F_API getInfo            (int *index, char **name) = 0;
        virtual FMOD_RESULT F_API getCategory        (const char *name, EventCategory **category) = 0;
        virtual FMOD_RESULT F_API getCategoryByIndex (int index, EventCategory **category) = 0;
        virtual FMOD_RESULT F_API getNumCategories   (int *numcategories) = 0;
        virtual FMOD_RESULT F_API getEventByIndex    (int index, FMOD_EVENT_MODE mode, Event **event) = 0;
        virtual FMOD_RESULT F_API getNumEvents       (int *numevents) = 0;
        virtual FMOD_RESULT F_API getParentCategory  (EventCategory **category) = 0;

        virtual FMOD_RESULT F_API stopAllEvents      () = 0;
        virtual FMOD_RESULT F_API setVolume          (float volume) = 0;
        virtual FMOD_RESULT F_API getVolume          (float *volume) = 0;
        virtual FMOD_RESULT F_API setPitch           (float pitch, FMOD_EVENT_PITCHUNITS units = FMOD_EVENT_PITCHUNITS_RAW) = 0;
        virtual FMOD_RESULT F_API getPitch           (float *pitch, FMOD_EVENT_PITCHUNITS units = FMOD_EVENT_PITCHUNITS_RAW) = 0;
        virtual FMOD_RESULT F_API setPaused          (bool paused) = 0;
        virtual FMOD_RESULT F_API getPaused          (bool *paused) = 0;
        virtual FMOD_RESULT F_API setMute            (bool mute) = 0;
        virtual FMOD_RESULT F_API getMute            (bool *mute) = 0;
        virtual FMOD_RESULT F_API getChannelGroup    (ChannelGroup **channelgroup) = 0;
        virtual FMOD_RESULT F_API setUserData        (void *userdata) = 0;
        virtual FMOD_RESULT F_API getUserData        (void **userdata) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~EventCategory(){};
    };

    /*
       'Event' API
    */
    class Event
    {
        public :

        FMOD_RESULT F_API release                    (bool freeeventdata = false, bool waituntilready = true);

        FMOD_RESULT F_API start                      ();
        FMOD_RESULT F_API stop                       (bool immediate = false);

        FMOD_RESULT F_API getInfo                    (int *index, char **name, FMOD_EVENT_INFO *info);
        FMOD_RESULT F_API getState                   (FMOD_EVENT_STATE *state);
        FMOD_RESULT F_API getParentGroup             (EventGroup **group);
        FMOD_RESULT F_API getChannelGroup            (ChannelGroup **channelgroup);
        FMOD_RESULT F_API setCallback                (FMOD_EVENT_CALLBACK callback, void *userdata);

        FMOD_RESULT F_API getParameter               (const char *name, EventParameter **parameter);
        FMOD_RESULT F_API getParameterByIndex        (int index, EventParameter **parameter);
        FMOD_RESULT F_API getNumParameters           (int *numparameters);
        FMOD_RESULT F_API getProperty                (const char *propertyname, void *value, bool this_instance = false);
        FMOD_RESULT F_API getPropertyByIndex         (int propertyindex, void *value, bool this_instance = false);
        FMOD_RESULT F_API setProperty                (const char *propertyname, void *value, bool this_instance = false);
        FMOD_RESULT F_API setPropertyByIndex         (int propertyindex, void *value, bool this_instance = false);
        FMOD_RESULT F_API getNumProperties           (int *numproperties);
        FMOD_RESULT F_API getPropertyInfo            (int *propertyindex, char **propertyname, FMOD_EVENTPROPERTY_TYPE *type = 0);
        FMOD_RESULT F_API getCategory                (EventCategory **category);

        FMOD_RESULT F_API setVolume                  (float volume);
        FMOD_RESULT F_API getVolume                  (float *volume);
        FMOD_RESULT F_API setPitch                   (float pitch, FMOD_EVENT_PITCHUNITS units = FMOD_EVENT_PITCHUNITS_RAW);
        FMOD_RESULT F_API getPitch                   (float *pitch, FMOD_EVENT_PITCHUNITS units = FMOD_EVENT_PITCHUNITS_RAW);
        FMOD_RESULT F_API setPaused                  (bool paused);
        FMOD_RESULT F_API getPaused                  (bool *paused);
        FMOD_RESULT F_API setMute                    (bool mute);
        FMOD_RESULT F_API getMute                    (bool *mute);
        FMOD_RESULT F_API set3DAttributes            (const FMOD_VECTOR *position, const FMOD_VECTOR *velocity, const FMOD_VECTOR *orientation = 0);
        FMOD_RESULT F_API get3DAttributes            (FMOD_VECTOR *position, FMOD_VECTOR *velocity, FMOD_VECTOR *orientation = 0);
        FMOD_RESULT F_API set3DOcclusion             (float directocclusion, float reverbocclusion);
        FMOD_RESULT F_API get3DOcclusion             (float *directocclusion, float *reverbocclusion);
        FMOD_RESULT F_API setReverbProperties        (const FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT F_API getReverbProperties        (FMOD_REVERB_CHANNELPROPERTIES *prop);
        FMOD_RESULT F_API setUserData                (void *userdata);
        FMOD_RESULT F_API getUserData                (void **userdata);

        FMOD_RESULT F_API getMemoryInfo              (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array);
    };

    /*
       'EventParameter' API
    */
    class EventParameter
    {
        public :

        FMOD_RESULT F_API getInfo                    (int *index, char **name);
        FMOD_RESULT F_API getRange                   (float *rangemin, float *rangemax);
        FMOD_RESULT F_API setValue                   (float value);
        FMOD_RESULT F_API getValue                   (float *value);
        FMOD_RESULT F_API setVelocity                (float value);
        FMOD_RESULT F_API getVelocity                (float *value);
        FMOD_RESULT F_API setSeekSpeed               (float value);
        FMOD_RESULT F_API getSeekSpeed               (float *value);
        FMOD_RESULT F_API setUserData                (void *userdata);
        FMOD_RESULT F_API getUserData                (void **userdata);
        FMOD_RESULT F_API keyOff                     ();

        FMOD_RESULT F_API getMemoryInfo              (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array);
    };

    /*
       'EventReverb ' API
    */
    class EventReverb
    {
        public :

        virtual FMOD_RESULT F_API release            () = 0;
        virtual FMOD_RESULT F_API set3DAttributes    (const FMOD_VECTOR *position, float mindistance, float maxdistance) = 0;
        virtual FMOD_RESULT F_API get3DAttributes    (FMOD_VECTOR *position, float *mindistance,float *maxdistance) = 0;
        virtual FMOD_RESULT F_API setProperties      (const FMOD_REVERB_PROPERTIES *props) = 0;
        virtual FMOD_RESULT F_API getProperties      (FMOD_REVERB_PROPERTIES *props) = 0;
        virtual FMOD_RESULT F_API setActive          (bool active) = 0;
        virtual FMOD_RESULT F_API getActive          (bool *active) = 0;
        virtual FMOD_RESULT F_API setUserData        (void *userdata) = 0;
        virtual FMOD_RESULT F_API getUserData        (void **userdata) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~EventReverb(){};
    };

    /*
        'MusicSystem' API
    */
    class MusicSystem
    {
        public :

        virtual FMOD_RESULT F_API reset              () = 0;
        virtual FMOD_RESULT F_API setVolume          (float volume) = 0;
        virtual FMOD_RESULT F_API getVolume          (float *volume) = 0;
        virtual FMOD_RESULT F_API setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES *prop) = 0;
        virtual FMOD_RESULT F_API getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES *prop) = 0;
        virtual FMOD_RESULT F_API setPaused          (bool paused) = 0;
        virtual FMOD_RESULT F_API getPaused          (bool *paused) = 0;
        virtual FMOD_RESULT F_API setMute            (bool mute) = 0;
        virtual FMOD_RESULT F_API getMute            (bool *mute) = 0;
        virtual FMOD_RESULT F_API getInfo            (FMOD_MUSIC_INFO *info) = 0;
        virtual FMOD_RESULT F_API promptCue          (FMOD_MUSIC_CUE_ID id) = 0;
        virtual FMOD_RESULT F_API prepareCue         (FMOD_MUSIC_CUE_ID id, MusicPrompt **prompt) = 0;
        virtual FMOD_RESULT F_API getParameterValue  (FMOD_MUSIC_PARAM_ID id, float *parameter) = 0;
        virtual FMOD_RESULT F_API setParameterValue  (FMOD_MUSIC_PARAM_ID id, float parameter) = 0;

        virtual FMOD_RESULT F_API getCues            (FMOD_MUSIC_ITERATOR *it, const char *filter = 0) = 0;
        virtual FMOD_RESULT F_API getNextCue         (FMOD_MUSIC_ITERATOR *it) = 0;
        virtual FMOD_RESULT F_API getParameters      (FMOD_MUSIC_ITERATOR *it, const char *filter = 0) = 0;
        virtual FMOD_RESULT F_API getNextParameter   (FMOD_MUSIC_ITERATOR *it) = 0;

        virtual FMOD_RESULT F_API loadSoundData      (FMOD_EVENT_RESOURCE resource = FMOD_EVENT_RESOURCE_SAMPLES, FMOD_EVENT_MODE mode = FMOD_EVENT_DEFAULT) = 0;
        virtual FMOD_RESULT F_API freeSoundData      (bool waituntilready = true) = 0;

        virtual FMOD_RESULT F_API setCallback        (FMOD_MUSIC_CALLBACK callback, void *userdata) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~MusicSystem(){};
    };

    /*
        'MusicPrompt' API
    */
    class MusicPrompt
    {
        public :

        virtual FMOD_RESULT F_API release            () = 0;
        virtual FMOD_RESULT F_API begin              () = 0;
        virtual FMOD_RESULT F_API end                () = 0;
        virtual FMOD_RESULT F_API isActive           (bool *active) = 0;

        virtual FMOD_RESULT F_API getMemoryInfo      (unsigned int memorybits, unsigned int event_memorybits, unsigned int *memoryused, unsigned int *memoryused_array) = 0;
        virtual ~MusicPrompt(){};
    };
}


#endif
