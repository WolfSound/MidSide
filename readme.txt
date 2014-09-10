  MidSide version 0.01 (Wolf Sound)

==================================================================
  MidSide is a MS delay/mixer, that can be use for widening
          effects of an input stereo signal
==================================================================

  VST source code is provided for making the plug-in working 
  under DAW software running on both Windows and Linux.

  A copy of the VST 2.4 SDK is not included because it is not 
  compatible with the GPL license of this plug-in.

  To compile the plug-in the files Makefile must be adjusted.
  In particular, folders are expressed in a Linux-like 
  standards, and cross compilation of the Windows plug-in is set
  by default.

  From a termial session, type the following code
      make all cleanobj
  to get the shared object file of the MidSide plug-in.

==================================================================

  Luca Sartore (drwolf85@gmail.com) - September 2014
