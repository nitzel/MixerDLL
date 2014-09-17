#pragma once

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#include <atlbase.h>
#include <atlstr.h>

#include <conio.h> // getch()
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>

// getting window caption
#using <System.dll>
using namespace System;
using namespace System::Diagnostics;
using namespace System::ComponentModel;


#define VERSION "2014-09-17-0044j"

#define DLL extern "C" __declspec(dllexport)

CComPtr<IAudioEndpointVolume> pEpVol; // global volume
CComPtr<IAudioSessionEnumerator> pAudioSessionEnumerator; // to go through sessions (local volume) 
//CComPtr<IAudioSessionManager2> pAudioSessionManager2; // get enumerator
HANDLE hSerial;

// ErrorMessages
#define ERRMSG_OK "OK"
#define ERRMSG_NOTIMPLEMENTEDYET "Not implemented yet"
#define ERRMSG_UNDEFINEDSESSIONID "Undefined sessionid"
#define ERRMSG_AUIDOSESSIONENUMERATOR_INIT_FAILED "AudioSessionEnumerator Init failed"
#define ERRMSG_SESSIONCONTROL_INIT_FAILED "SessionControl Init failed"
#define ERRMSG_SESSIONCONTROL2_INIT_FAILED "SessionControl2 Init failed"
#define ERRMSG_AUDIOVOLUME_INIT_FAILED "ISimpleAudioVolume Init failed"
#define ERRMSG_GETPROCESSINFO_FAILED "Get info about process failed"


// Deklarationen
DLL char * version();
DLL void initAudio(); // call before using other audio functions!
DLL void exitAudio(); // call before exit
DLL char * getErrorMessage();

// strings...
DLL int getSessionCount();
DLL DWORD getSessionPID(const int sessionId);
DLL char * getSessionName(const int sessionId);
DLL char * getSessionTitle(const int sessionId);
DLL char * getPIDName (DWORD);
DLL char * getPIDTitle (DWORD);

// mute
DLL bool setSessionMute(const int sessionId, const bool mute);
DLL bool getSessionMute(const int sessionId);
DLL bool setMasterMute(const bool mute);
DLL bool getMasterMute();
// volume
DLL bool setSessionVolume(const int sessionId, const float volume);
DLL float getSessionVolume(const int sessionId);
DLL bool setMasterVolume(const float volume);
DLL float getMasterVolume();
DLL void printAudioInfo();
char * StringToCharP(String^ S);


// Definitionen

char * version()
{
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
	
	pDevice.Release();
	pDeviceEnumerator.Release();
	pAudioSessionManager2.Release();
}

char * errorMessage = NULL;
char * getErrorMessage(){
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
char * getSessionName(const int sessionId){
	if( sessionId == -1)
		return "master";
	DWORD pid = getSessionPID(sessionId);

	return getPIDName(pid);
}
char * getSessionTitle(const int sessionId){
	if( sessionId == -1)
		return "master";
	DWORD pid = getSessionPID(sessionId);

	return getPIDTitle(pid);
}
char * getPIDName_cstr = NULL;
char * getPIDName (DWORD procid){
	String^ procname;
	
	try{
		Process^ proc = Process::GetProcessById(procid);
		procname = proc->ProcessName;
	} catch(Exception^ e){ e;
		errorMessage = ERRMSG_GETPROCESSINFO_FAILED;
		return NULL;
	}
	if(getPIDName_cstr) delete [] getPIDName_cstr;
	getPIDName_cstr = StringToCharP(procname);
	return getPIDName_cstr;
}
char * getPIDTitle_cstr = NULL;
char * getPIDTitle (DWORD procid){
	String^ title;
	
	try{
		Process^ proc = Process::GetProcessById(procid);
		title = proc->MainWindowTitle;
	} catch(Exception^ e){ e;
		errorMessage = ERRMSG_GETPROCESSINFO_FAILED;
		return NULL;
	}
	if(getPIDTitle_cstr) delete [] getPIDTitle_cstr;
	getPIDTitle_cstr = StringToCharP(title);
	return getPIDTitle_cstr;
}

void printAudioInfo(){
	int sessionCount = getSessionCount();
	printf("\n\n\n\n\n\n\n\n");
	printf("################################################################################");
	printf("     Audio Sessions \n");
	printf("################################################################################");
	printf("[id] PID  - %-10s - %-50s\n","process","Main-Window Title");
	printf("--------------------------------------------------------------------------------");
	for (int i = 0; i < sessionCount; i++){
		CComPtr<IAudioSessionControl> pSessionControl;
		CComPtr<ISimpleAudioVolume> pSAV;
		CComPtr<IAudioSessionControl2> pSC2;
		// Get SessionControl
		if (FAILED(pAudioSessionEnumerator->GetSession(i, &pSessionControl)))
			continue;
		// Ask for SimpleAudioVolume
		if (FAILED(pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (VOID**)&pSAV)))
			continue;
		// audio session 2 for extended name info
		if (FAILED(pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (VOID**)&pSC2)))
			continue;
		LPWSTR name1,name2,icon;
		DWORD procid;
		String^ title;
		String^ procname;
		float vol = 0;
		
		pSessionControl->GetDisplayName(&name1);
		pSessionControl->GetIconPath(&icon);
		pSC2->GetSessionIdentifier(&name2);
		pSC2->GetProcessId(&procid);
		
		try{
			Process^ proc = Process::GetProcessById(procid);
			title = proc->MainWindowTitle;
			procname = proc->ProcessName;
		} catch(Exception^ e){
			e;
			printf("[%.2d]% 5d - %-10s - %-45s\n",i,procid,"invalid","invalid session");
			CoTaskMemFree(name1); // TODO: to be done better :/
			CoTaskMemFree(name2);
			CoTaskMemFree(icon);
			continue;
		}
		pSAV->GetMasterVolume(&vol);
//		printf("Session[%d] Vol[%.2f]\nDisplayName: %ls\nSessionIdentifier: %ls\nProcessId: %d\nMainWindowTitle: %s\nProcessName: %s\nIconPath: %s\n\n", i, vol, name1, name2, procid, title, procname, icon);
//		printf("Session[%d] Vol[%.2f]\nDisplayName: %ls\nProcessId: %d\nMainWindowTitle: %s\nProcessName: %s\nIconPath: %s\n\n", i, vol, name1, procid, title, procname, icon);

		//18+name+laststring
		printf("[% 2d]% 5d - %-10.10s - %-50.50s\n",i,procid,procname,title);

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

/**
Important: Creates a buffer and returns pointer to it. you need to delete that buffer yourself!
*/
char * StringToCharP(String^ S){ 
	char *cstr = new char[S->Length + 1];
	for (int i=0;i<S->Length; i++)
	  cstr[i]=(char)S[i];
	cstr[S->Length]=0; // zero-terminated
	return cstr;
}