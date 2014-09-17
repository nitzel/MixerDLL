# A test for MixerDLL  
#                      
# By nitzel
#
# You might want to configure the dll-path in line 16
#

import mixer_dll
import TimerControl # timed callbacks
import os # OS-check
import copy
if os.name != 'nt':
	print "Need to be run on windows"
	exit()

mixerdll = mixer_dll.get_mixer_dll("../MixerDLL/Release/mixer.dll")

from Tkinter import *	
	
mixerdll.initAudio();

print "Mixerdll v. "+mixerdll.version()
mixerdll.printAudioInfo()
master = None
scaler = []


def sendToMC(data):
	print "sending to mc"
	for c in data:
		print c, bin(c)
	print "done"
def updateMC(id):
	pass
def sliderChanged(id, value): # callback whenever a slider has changed
	global mixerdll, scaler
	value = float(value)

	if scaler[id][1] != value:
		scaler[id][1] = value
		try:
			mixerdll.setSessionVolume(id, value)
		except: # catch some 0pointer erros, dont know where thy come from
			print "error set", mixerdll.getErrorMessage()
			pass
def volumeChanged():
	global mixerdll, scaler
	
	if mixerdll.getSessionCount()+1 != len(scaler):
		initSliders()
	else:
		for s in scaler:
			try:
				if s[3]!=mixerdll.getSessionTitle(s[2]): # name changed
					initSliders()
			except:
				print "error volume changed getsestitle",s
				
	
	for s in scaler:
		try:
			vol = mixerdll.getSessionVolume(s[2])
			if s[1] != vol:
				s[1] = vol
				s[0].set(s[1])
		except: # catch some 0pointer erros, dont know where thy come from
			print "error get", mixerdll.getErrorMessage()

def initSliders():
	global scaler, master
	newScaler = []
	# init sliders (for each audio session (master volume(-1) inclusive))
	for i in range(-1,mixerdll.getSessionCount()):
		print "session",i,"/",mixerdll.getSessionCount()
		m = int(copy.deepcopy(str(i)))# to loose reference to i
		s = [Scale(master, from_=1, to=0, resolution=0.01, command=eval("lambda v: sliderChanged("+str(i)+",v)"), label=mixerdll.getSessionTitle(i)),mixerdll.getSessionVolume(i), i, mixerdll.getSessionTitle(i)] # create (slider, volume, id) obj
		print "["+str(mixerdll.getSessionTitle(i))+"]"

		s[0].set(s[1]) # udpate slider
		newScaler.append(s) # put obj in list
		if mixerdll.getSessionTitle(i)!=None:
			s[0].pack(side=LEFT) # use slider in window
	for s in newScaler:
		print s
	for s in scaler:
		s[0].destroy()
	scaler = newScaler
	
			
try:
	# init window
	master = Tk()
	master.title("Testing MixerDLL - Juppiduppi")
	Label(master, text="Using MixerDLL v."+mixerdll.version()).pack()
	
	initSliders()
	
	# TimerControl for timed polling
	timemgr = TimerControl.TimerControl(0.01, [TimerControl.Timer(volumeChanged,0.01,True,True,None)],True)
	# Tkinter.mainloop
	mainloop() 	# blocking, runs until window closes

finally: # shutdown
	print "\n\nExit & Shutdown"
	timemgr.stop()
	mixerdll.exitAudio()

