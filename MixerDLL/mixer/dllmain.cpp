#pragma once

#include <windows.h>

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#include <atlstr.h>

#include <conio.h> // getch()
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>

#include "Psapi.h"  
#pragma comment (lib,"Psapi.lib")  



#define MIXER_DLL extern "C" __declspec(dllexport)

CComPtr<IAudioEndpointVolume> pEpVol;					  // global volume
CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator; // to go through sessions (local volume) 

// filename
static char VERSION[] = "2015-06-02-0044j";
static char g_pidName[MAX_PATH] = { 0 };
static char g_pidTitle[MAX_PATH] = { 0 };

// ErrorMessages
#define ERRMSG_OK "OK"
#define ERRMSG_NOTIMPLEMENTEDYET "Not implemented yet"
#define ERRMSG_UNDEFINEDSESSIONID "Undefined sessionid"
#define ERRMSG_AUIDOSESSIONENUMERATOR_INIT_FAILED "AudioSessionEnumerator Init failed"
#define ERRMSG_SESSIONCONTROL_INIT_FAILED "SessionControl Init failed"
#define ERRMSG_SESSIONCONTROL2_INIT_FAILED "SessionControl2 Init failed"
#define ERRMSG_AUDIOVOLUME_INIT_FAILED "ISimpleAudioVolume Init failed"
#define ERRMSG_GETPROCESSINFO_FAILED "Get info about process failed"


/// functions
//	stuff
MIXER_DLL char* version();
MIXER_DLL void initAudio(); // call before using other audio functions!
MIXER_DLL void exitAudio(); // call before exit
MIXER_DLL char* getErrorMessage();

//	strings...
MIXER_DLL int getSessionCount();
MIXER_DLL DWORD getSessionPID(const int sessionId);
MIXER_DLL const char* getSessionName(const int sessionId);
MIXER_DLL const char* getSessionTitle(const int sessionId);
MIXER_DLL const char* getPIDName(DWORD);
MIXER_DLL const char* getPIDTitle(DWORD);

//	mute
MIXER_DLL bool setSessionMute(const int sessionId, const bool mute);
MIXER_DLL bool getSessionMute(const int sessionId);
MIXER_DLL bool setMasterMute(const bool mute);
MIXER_DLL bool getMasterMute();
// 	volume
MIXER_DLL bool setSessionVolume(const int sessionId, const float volume);
MIXER_DLL float getSessionVolume(const int sessionId);
MIXER_DLL bool setMasterVolume(const float volume);
MIXER_DLL float getMasterVolume();
MIXER_DLL void printAudioInfo();


/// implementations

char* version(){
	return VERSION;
}

void initAudio(){
	CComPtr<IMMDevice> pDevice; // get Mgr & EndpointVolume
	CComPtr<IMMDeviceEnumerator> pDeviceEnumerator; // select output device
	CComPtr<IAudioSessionManager2> pAudioSessionManager2; // get enumerator

	// Init
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	// Device Enumerator
	pDeviceEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));
	// get Default Audio Endpoint Device
	pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
	// get AudioSessionMgr2 and AudioEndpointVolume
	pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (VOID**)&pAudioSessionManager2);
	pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (VOID**)&pEpVol);
	// get Audio Session Enumerator
	pAudioSessionManager2->GetSessionEnumerator(&pAudioSessionEnumerator);
}

char* errorMessage = NULL;
char* getErrorMessage(){
	return errorMessage;
}
int getSessionCount(){
	int sessionCount = 0;
	exitAudio();
	initAudio();
	pAudioSessionEnumerator->GetCount(&sessionCount);
	return sessionCount;
}

/**
 * Find out if a session is muted. -1 -> Master
 */
bool getSessionMute(const int sessionId){
	int sessionCount = getSessionCount();
	if (sessionId == -1)
		return getMasterMute();
	if (sessionId >= sessionCount || sessionId < 0) {
		errorMessage = ERRMSG_UNDEFINEDSESSIONID;
		return false;
	}
	CComPtr<IAudioSessionControl> pSessionControl;
	CComPtr<ISimpleAudioVolume> pSAV;
	// Get SessionControl
	if (FAILED(pAudioSessionEnumerator->GetSession(sessionId, &pSessionControl))){
		errorMessage = ERRMSG_SESSIONCONTROL_INIT_FAILED;
		return false;
	}
	// Ask for SimpleAudioVolume
	if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV))){
		errorMessage = ERRMSG_AUDIOVOLUME_INIT_FAILED;
		return false;
	}
	BOOL mute;
	pSAV->GetMute(&mute);
	errorMessage = ERRMSG_OK;
	return mute != 0; // BOOL(is int, so int->bool via not null)
}
/**
 Mutes/Unmutes a session. If sessionId=-1, info about master
 */
bool setSessionMute(const int sessionId, const bool mute){
	int sessionCount = getSessionCount();
	if (sessionId == -1)
		return setMasterMute(mute);
	if (sessionId >= sessionCount || sessionId < 0) {
		errorMessage = ERRMSG_UNDEFINEDSESSIONID;
		return false;
	}
	CComPtr<IAudioSessionControl> pSessionControl;
	CComPtr<ISimpleAudioVolume> pSAV;
	// Get SessionControl
	if (FAILED(pAudioSessionEnumerator->GetSession(sessionId, &pSessionControl))){
		errorMessage = ERRMSG_SESSIONCONTROL_INIT_FAILED;
		return false;
	}
	// Ask for SimpleAudioVolume
	if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV))){
		errorMessage = ERRMSG_AUDIOVOLUME_INIT_FAILED;
		return false;
	}

	errorMessage = ERRMSG_OK;
	return S_OK == pSAV->SetMute(mute, NULL);
}

/**
 Sets volume of sessionId. If sessionId=-1, sets Master Volume
 */
bool setSessionVolume(const int sessionId, const float volume){
	int sessionCount = getSessionCount();
	if (sessionId == -1)
		return setMasterVolume(volume);
	if (sessionId >= sessionCount || sessionId < 0) {
		//printf("session: %d / %d\n", sessionId, sessionCount);  
		errorMessage = ERRMSG_UNDEFINEDSESSIONID;
		return false;
	}
	CComPtr<IAudioSessionControl> pSessionControl;
	CComPtr<ISimpleAudioVolume> pSAV;
	// Get SessionControl
	if (FAILED(pAudioSessionEnumerator->GetSession(sessionId, &pSessionControl))){
		errorMessage = ERRMSG_SESSIONCONTROL_INIT_FAILED;
		return false;
	}
	// Ask for SimpleAudioVolume
	if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV))){
		errorMessage = ERRMSG_AUDIOVOLUME_INIT_FAILED;
		return false;
	}
	//LPWSTR name;
	//pSessionControl->GetDisplayName(&name);
	//_tprintf(name);
	// adjust session volume
	errorMessage = ERRMSG_OK;
	return S_OK == pSAV->SetMasterVolume(volume, NULL);
}
/**
 Returns volume of sessionId. If sessionId=-1, returns Master Volume
 */
float getSessionVolume(const int sessionId){
	CComPtr<IAudioSessionControl> pSessionControl;
	CComPtr<ISimpleAudioVolume> pSAV;
	int sessionCount = getSessionCount();
	float vol; 
	if (sessionId == -1)
		return getMasterVolume();
	if (sessionId >= sessionCount || sessionId < 0) {
		//printf("session: %d / %d\n", sessionId, sessionCount);  
		errorMessage = ERRMSG_UNDEFINEDSESSIONID;
		return -1;
	}

	// Get SessionControl
	if (FAILED(pAudioSessionEnumerator->GetSession(sessionId, &pSessionControl))){
		errorMessage = ERRMSG_AUIDOSESSIONENUMERATOR_INIT_FAILED;
		return -1;
	}
	// Ask for SimpleAudioVolume
	if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV))){
		errorMessage = ERRMSG_SESSIONCONTROL_INIT_FAILED;
		return -1;
	}
	//LPWSTR name;
	//pSessionControl->GetDisplayName(&name);
	//_tprintf(name);
	// adjust session volume
	pSAV->GetMasterVolume(&vol);
	
	errorMessage = ERRMSG_OK;
	return vol;
}
DWORD getSessionPID(const int sessionId){
	CComPtr<IAudioSessionControl> pSessionControl;
	CComPtr<IAudioSessionControl2> pSC2;
	int sessionCount = getSessionCount();
	if (sessionId >= sessionCount || sessionId < 0) { 
		errorMessage = ERRMSG_UNDEFINEDSESSIONID;
		return NULL;
	}
	// Get SessionControl
	if (FAILED(pAudioSessionEnumerator->GetSession(sessionId, &pSessionControl))){
		errorMessage = ERRMSG_AUIDOSESSIONENUMERATOR_INIT_FAILED;
		return NULL;
	}
	// audio session 2 for extended name info
	if (FAILED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (VOID**)&pSC2))){
		errorMessage = ERRMSG_SESSIONCONTROL2_INIT_FAILED;
		return NULL;
	}
	
	DWORD procid;
	pSC2->GetProcessId(&procid);
	return procid;
}
const char* getSessionName(const int sessionId){
	if( sessionId == -1)
		return "master";
	DWORD pid = getSessionPID(sessionId);

	return getPIDName(pid);
}
const char* getSessionTitle(const int sessionId){
	if( sessionId == -1)
		return "master";
	DWORD pid = getSessionPID(sessionId);

	return getPIDTitle(pid);
}
const char* getPIDName(DWORD procid){
	char path[_MAX_PATH + 1] = "";
	memset(g_pidName, 0, MAX_PATH);

	HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, procid);
	if (!h_Process) return g_pidName;

	GetModuleFileNameExA(h_Process, NULL, path, MAX_PATH + 1);

	char* pidName = PathFindFileNameA(path);

	strcpy(g_pidName, pidName);
	return g_pidName;
}


BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD pid = *((DWORD*)(lParam));

	DWORD enumPid = 0;
	GetWindowThreadProcessId(hwnd, &enumPid);

	if (pid == enumPid)
	{
		GetWindowTextA(hwnd, g_pidTitle, MAX_PATH);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

const char* getPIDTitle (DWORD procid){
	EnumWindows(EnumWindowsProc, (LPARAM)&procid);
	return g_pidTitle;
}

void printAudioInfo(){
	int sessionCount = getSessionCount();
	printf("\n\n\n\n\n\n\n\n");
	printf("################################################################################");
	printf("     Audio Sessions \n");
	printf("################################################################################");
	printf("[id] PID  - %-10s - %-50s\n","process","Main-Window Title");
	printf("--------------------------------------------------------------------------------");
	for (int sessionID(0); sessionID < sessionCount; sessionID++){
		CComPtr<IAudioSessionControl> pSessionControl;
		CComPtr<ISimpleAudioVolume> pSAV;
		CComPtr<IAudioSessionControl2> pSC2;
		// Get SessionControl
		if (FAILED(pAudioSessionEnumerator->GetSession(sessionID, &pSessionControl)))
			continue;
		// Ask for SimpleAudioVolume
		if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV)))
			continue;
		// audio session 2 for extended name info
		if (FAILED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (VOID**)&pSC2)))
			continue;
		LPWSTR name1,name2,icon;
		DWORD procid(0);
		float vol = 0;
		
		pSessionControl->GetDisplayName(&name1);
		pSessionControl->GetIconPath(&icon);
		pSC2->GetSessionIdentifier(&name2);
		pSC2->GetProcessId(&procid);
		
		procid = getSessionPID(sessionID);
		getPIDName(procid);
		getPIDTitle(procid);

		pSAV->GetMasterVolume(&vol);
//		printf("Session[%d] Vol[%.2f]\nDisplayName: %ls\nSessionIdentifier: %ls\nProcessId: %d\nMainWindowTitle: %s\nProcessName: %s\nIconPath: %s\n\n", i, vol, name1, name2, procid, title, procname, icon);
//		printf("Session[%d] Vol[%.2f]\nDisplayName: %ls\nProcessId: %d\nMainWindowTitle: %s\nProcessName: %s\nIconPath: %s\n\n", i, vol, name1, procid, title, procname, icon);

		//18+name+laststring
		printf("[% 2d]% 5d - %-10.10s - %-50.50s\n", sessionID, procid, g_pidName, g_pidTitle);

		CoTaskMemFree(name1); // TODO: to be done better :/
		CoTaskMemFree(name2);
		CoTaskMemFree(icon);
	}
	printf("--------------------------------------------------------------------------------");
}


bool getMasterMute(){
	BOOL mute;
	pEpVol->GetMute(&mute);
	return mute != 0; // BOOL(is int, so int->bool via not null)
}
bool setMasterMute(const bool mute){
	// adjust master volume
	return S_OK == pEpVol->SetMute(mute, NULL);
}
bool setMasterVolume(const float volume){
	// adjust master volume
	return S_OK == pEpVol->SetMasterVolumeLevelScalar(volume, NULL);
}
float getMasterVolume(){
	float vol;
	pEpVol->GetMasterVolumeLevelScalar(&vol);
	errorMessage = ERRMSG_OK;
	return vol;
}

void exitAudio(){
	pEpVol.Release();
	pAudioSessionEnumerator.Release();
	CoUninitialize();
}