#!/usr/bin/python2
# Timer and TimerControl written by nitzel, Dez 2013
#
# Timers consist of a callback, a delay in seconds and two flags: running and repeat
# 	callback: a method that is called after delay seconds have passed
#		running: Timer is active or not
#		repeat: After delay seconds have passed and callback has been called, reset the timer or set running to false depending on this flag
# TimerControl extends threading.Thread
# consists of a list of Timers, an updateInterval in seconds and a flag whether its running or not
#		timers: list of Timers, their callbacks will be called on time
#		updateInterval: waits this time between the checks if callbacks have to be executed
#		running: whether the thread is running or not. as parameter to __init__ it tells TimerControl to start a thread immediateley
#
# Use (e.g.):
# 	Create some Timer's, create a TimeControl. Pass the Timer's to TimeControl via __init__ als list or by using TimeControl.append(Timer)
#		Tell TimeControl to start thread and init timeouts of Timers via TimeControl.restart()
#		Push more Timers via append or appendS (appendS starts the timer immediately), control their status via Timer.restart()/.stop() and 
#		stop TimeControl at the end via TimeControl.stop(). This does not stop the Timer's (does not set their running flag to false, so other 
#		 TimeControllers using these Timers still work)
#		Have fun :P
import threading
import time
import sys
import os

class Timer:
	""" Class for storing information about time-based callbacks """
	def __init__(self, callback, delay, running=False, repeat=False, args=None):
		""" callback = funktion to call
			delay = time to wait after timer has started before callback is called (float, in seconds)
			running = start directly after creation?
			repeat = call callback on a regular basis 
			args = parameters for callback (Something or None, None-> callback(), not callback(None) !)"""
		self.callback = callback
		self.delay = delay
		self.running = False # will be set in restart if running is true
		self.repeat = repeat
		self.timeout = 0 
		self.args = args
		
		if running:
			self.restart()
	def __str__(self):
		s = "[Timer:running="+str(self.running)+"; delay="+str(self.delay)+"; repeat="+str(self.repeat)+"; timeout="+str(self.timeout)+"; callback="+str(self.callback)+"]"
		return s
	def setRunning(self,running):
		""" sets running flag and updates timout if necessary """
		if running:
			self.timeout = time.time()+self.delay
		self.running = running
	def isRunning(self):
		""" just tells if running flag is set """
		return self.running
	def isRepeating(self):
		return self.repeat
	def getTimeout(self):
		""" Returns timeout """
		return self.timeout
	def execute(self, currentTime):
		""" start callback if time is over, set running to false and restart if repeat=true 
		returns True if thread is still running - False otherwise"""
		if self.isRunning() and self.getTimeout()<currentTime:
			self.stop()
			if self.args is None:
				self.callback()
			else:
				self.callback(self.args)
			if self.isRepeating():
				self.restart()
		return self.isRunning()
	def restart(self):
		""" To start it. If it's already running, the timeout will be resetted.
		Should be quite useful, you do not have to reset it on your own or stop&start. """
		self.setRunning(True)
	def stop(self):
		""" Stop it. """
		self.setRunning(False)
		
class TimerControl(threading.Thread):
	def __init__(self, updateInterval=0.5, timers=None, running=False):
		""" Take parameters and start itself if asked for """
		threading.Thread.__init__(self)
		
		self.updateInterval = updateInterval
		self.timers = timers
		self.running = False # will be set in restart if running was true
		
		if running:
			self.restart()
	def isRunning(self):
		return self.running
	def append(self, timer):
		""" only appends timer, does not start it. if timer is already in list, its not inserted again """
		if timer not in self.timers:
			self.timers.append(timer)
	def appendS(self, timer):
		""" appends and (re)starts timer """
		timer.restart()
		self.append(timer)
	def restart(self):
		""" restart all Timer objects and start new thread if necessary """
		for t in self.timers:
			t.restart()
		if not self.running:
			self.running = True
			self.start() # calls Timer.run() in a new thread
	def stop(self):
		""" indirectly stops thread by setting running flag """
		self.running = False
	def run(self):
		""" Loop of thread calling the timers' callbacks if necessary """
		try:
			while self.running:
				#print "TimeControl running:",self.running,"(TODO remove this line)"
				currentTime = time.time() # current time
				for t in self.timers:
					t.execute(currentTime)
					# if you want to remove "finished" timers = timers, that are not running - replace the line above with the two below 
					#if not t.execute(currentTime):
					#	self.timers.remove(t)
				time.sleep(self.updateInterval) # maybe better than line below :/
				#time.sleep(currentTime-time.time()+self.updateInterval) # calc real sleeptime
		finally:
			self.running = False
	def __str__(self):
		s = "[TimeControl: Timers["+str(len(self.timers))+"]:\n"
		for t in self.timers:
			s+=str(t)+"\n"
		s+="running="+str(self.running)+"; updateInterval="+str(self.updateInterval)+"]"
		return s


if __name__ == "__main__":			
	""" Some testing, may be removed later - is only executed if this is the mainfile (and not an include) """
	def end():
		print "kill!!"
		os._exit(1)
	def jojo():
		print "jojo"
	def heya():
		print "Heya"
	# create some timers
	timer1 = Timer(jojo,1,True, True)  # repeated printing timer
	timer2 = Timer(heya,2,False,False) # non-repeated printing timer
	timer3 = Timer(end, 8,False,False) # non-repeated kill timer
	# create a TimeControl managing the timers and starting their callbacks
	timemgr = TimerControl(1,[timer1],True)
	timemgr.append (timer2) # append without starting it
	#timemgr.appendS(timer2) # append starting it (already in list -> only starting it) - you could alternatively just write: timer2.restart() [this may not work if not-running timers are kicked out of list (which is disabled atm)
	timer2.restart()
	timemgr.appendS(timer3)
	print str(timemgr)
	
	try:
		time.sleep(4)
	finally:
		timemgr.stop() # if aborted (^C) stop timecontrol
		
	print str(timemgr)

