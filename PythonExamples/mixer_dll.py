# Configured instance of MixerDLL
#
# By nitzel
#
# TODO: OOP / System of Classes for Volume Management


import ctypes # DLL support

def get_mixer_dll(path_to_dll):
	""" A configured instance of MixerDLL """
	mixerdll = ctypes.CDLL(path_to_dll) 

	#version
	mixerdll.version.restype = ctypes.c_char_p
	mixerdll.version.argtypes = None
	#initAudio
	mixerdll.initAudio.restype = None
	mixerdll.initAudio.argtypes = None
	#exitAudio
	mixerdll.exitAudio.restype = None
	mixerdll.exitAudio.argtypes = None
	#getErrorMessage
	mixerdll.getErrorMessage.restype = ctypes.c_char_p
	mixerdll.getErrorMessage.argtypes = None
	#getSessionCount
	mixerdll.getSessionCount.restype = ctypes.c_int
	mixerdll.getSessionCount.argtypes = None
	#getSessionPID
	mixerdll.getSessionPID.restype = ctypes.c_int
	mixerdll.getSessionPID.argtypes = [ctypes.c_int]
	#getSessionName
	mixerdll.getSessionName.restype = ctypes.c_char_p
	mixerdll.getSessionName.argtypes = [ctypes.c_int]
	#getSessionTitle
	mixerdll.getSessionTitle.restype = ctypes.c_char_p
	mixerdll.getSessionTitle.argtypes = [ctypes.c_int]
	#getPIDName
	mixerdll.getPIDName.restype = ctypes.c_char_p
	mixerdll.getPIDName.argtypes = [ctypes.c_int]
	#getSessionTitle
	mixerdll.getPIDTitle.restype = ctypes.c_char_p
	mixerdll.getPIDTitle.argtypes = [ctypes.c_int]
	#getSessionVolume, id=-1 -> getMaster
	mixerdll.getSessionVolume.restype = ctypes.c_float
	mixerdll.getSessionVolume.argtypes = [ctypes.c_int]
	#setSessionVolume, id=-1 -> setMaster
	mixerdll.setSessionVolume.restype = ctypes.c_bool
	mixerdll.setSessionVolume.argtypes = [ctypes.c_int, ctypes.c_float] 
	#printAudioInfo
	mixerdll.printAudioInfo.restype = None
	mixerdll.printAudioInfo.argtypes = None
	#getmastervolume
	mixerdll.getMasterVolume.restype = ctypes.c_float
	mixerdll.getMasterVolume.argtypes = None #=void
	#setmastervolume
	mixerdll.setMasterVolume.restype = ctypes.c_bool
	mixerdll.setMasterVolume.argtypes = [ctypes.c_float]
	#muting / unmuting
	mixerdll.setSessionMute.restype = ctypes.c_bool
	mixerdll.setSessionMute.argtypes = [ctypes.c_int, ctypes.c_bool]
	mixerdll.getSessionMute.restype = ctypes.c_bool
	mixerdll.getSessionMute.argtypes = [ctypes.c_int]
	mixerdll.setMasterMute.restype = ctypes.c_bool
	mixerdll.setMasterMute.argtypes = [ctypes.c_bool]
	mixerdll.getMasterMute.restype = ctypes.c_bool
	mixerdll.getMasterMute.argtypes = None
	
	return mixerdll
