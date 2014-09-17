MixerDLL
========
Change program volumes separately.


WASAPI Audio Mixer Simplification DLL for Volume Management of Sessions

What is MixerDLL?
--------
It is kind of a wrapper-dll, enabling the use of WASAPIs Session Volume Management without digging into it too deep.
It was created when we were building a hardware-mixer like the windows audio mixer that could change the volumes of sessions (different programs) separately.
For fast testing we wanted to do it via Python, but could not find a way to access the sessions' volume.
So I went through Microsofts Documentation and this is what came out of the effort:
Some messy code, hopefully working without any memory leaks. But I think someone else could use it - I want to share it with you.

Implemented functionality
-----
I hope this is quite self-explaining. If not, you might want to take a look at the python examples.

* char * version();
* void initAudio();
* void exitAudio(); 
* char * getErrorMessage();

* int getSessionCount();
* DWORD getSessionPID(const int sessionId);
* char * getSessionName(const int sessionId);
* char * getSessionTitle(const int sessionId);
* char * getPIDName (DWORD);
* char * getPIDTitle (DWORD);

* bool setSessionMute(const int sessionId, const bool mute);
* bool getSessionMute(const int sessionId);
* bool setMasterMute(const bool mute);
* bool getMasterMute();

* bool setSessionVolume(const int sessionId, const float volume);
* float getSessionVolume(const int sessionId);
* bool setMasterVolume(const float volume);
* float getMasterVolume();
* void printAudioInfo();

Using MixerDLL
----
If you want to use MixerDLL - just do it :) It's unlicensed.
Leave me a note if you find it useful.
