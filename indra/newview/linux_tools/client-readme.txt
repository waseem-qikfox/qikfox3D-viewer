qikfox3D - Linux Beta README
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

This document contains information about the qikfox3D Linux
client

1. Introduction
2. System Requirements
3. Installing & Running
4. Known Issues
5. Troubleshooting
   5.1. 'Error creating window.'
   5.2. System hangs
   5.3. Blank window after minimizing it
   5.4. Audio
   5.5. 'Alt' key for camera controls doesn't work
   5.6. In-world streaming movie, music and Flash playback
6. Advanced Troubleshooting
   6.1. Audio
   6.2. OpenGL
7. Obtaining and working with the client source code
8. Getting more help, and reporting problems


1. INTRODUCTION
-=-=-=-=-=-=-=-

Hi!  This is a BETA release of the qikfox3D client for Linux.
The 'beta' status means that although we're still smoothing-out a few rough
edges, this version of the client is functionally complete and should
work quite well 'out of the box'.

We encourage you to try it out and let us know of its compatibility
with your system.  Be aware that although this is a 'beta' client, it connects
to the main Second Life world and changes you make there are permanent.

Please enjoy!


2. SYSTEM REQUIREMENTS
-=-=-=-=-=-=-=-=-=-=-=

Minimum requirements:
    * Internet Connection: Cable or DSL
    * Computer Processor: 800MHz Pentium III or Athlon or better
      (recommended: 1.5GHz or more)
    * Computer Memory: 2048MB (ie, 2GB)
    * Linux Operating System: A reasonably modern 32-bit Linux environment
          is required.  If you are running a 64-bit Linux distribution then
          you will need its 32-bit compatibility environment installed, but
          this configuration is not currently supported.
    * PulseAudio or ALSA Linux system sound software.  A recent PulseAudio
      is the recommended configuration; see README-linux-voice.txt for more
      information.
    * Video/Graphics Card:
          o nVidia GeForce 2, GeForce 4mx, or better (recommend one of the
            following: 6700, 6800, 7600, 7800, 7900, 8400, 8500, 8600,
            8800, Go 7400, Go 7600, Go 7800, Go 7900, +)
          o OR ATI Radeon 8500, 9250, or better
          (nVidia cards are recommended for the Linux client)

      **NOTE**: qikfox3D absolutely requires you to have recent, correctly-
      configured OpenGL 3D drivers for your hardware - the graphics drivers
      that came with your operating system may not be good enough!  See the
      TROUBLESHOOTING section if you encounter problems starting qikfox3D.

For a more comfortable experience, the RECOMMENDED hardware for the Second
Life Linux client is very similar to that for Windows, as detailed at:
<https://secondlife.com/corporate/sysreqs.php>


3. INSTALLING & RUNNING
-=-=-=-=-=-=-=-=-=-=-=-

The qikfox3D Linux client can entirely run from the directory you have
unpacked it into - no installation step is required.  If you wish to
perform a separate installation step anyway, you may run './install.sh'

Run './firestorm' from the installation directory to start qikfox3D.

For in-world MOVIE and MUSIC PLAYBACK, you will need (32-bit) GStreamer 0.10
installed on your system.  This is optional - it is not required for general
client functionality.  If you have GStreamer 0.10 installed, the selection of
in-world movies you can successfully play will depend on the GStreamer
plugins you have; if you cannot play a certain in-world movie then you are
probably missing the appropriate GStreamer plugin on your system - you may
be able to install it (see TROUBLESHOOTING).

User data is stored in the hidden directory ~/.firestorm by default; you may
override this location with the FIRESTORM_USER_DIR, or FIRESTORM_X64_USER_DIR
if using the 64 bit version, environment variable if you wish.


4. KNOWN ISSUES
-=-=-=-=-=-=-=-

* No significant known issues at this time.


5. TROUBLESHOOTING
-=-=-=-=-=-=-=-=-=

The client prints a lot of diagnostic information to the console it was
run from.  Most of this is also replicated in ~/.firestorm/logs/qikfox3D.log
- this is helpful to read when troubleshooting, especially 'WARNING' and
'ERROR' lines.

VOICE PROBLEMS?  See the separate README-linux-voice.txt file for Voice
  troubleshooting information.

SPACENAVIGATOR OR JOYSTICK PROBLEMS?  See the separate
  README-linux-joystick.txt file for configuration information.

PROBLEM 1:- qikfox3D fails to start up, with a warning on the console like:
   'Error creating window.' or
   'Unable to create window, be sure screen is set at 32-bit color' or
   'SDL: Couldn't find matching GLX visual.'
SOLUTION:- Usually this indicates that your graphics card does not meet
   the minimum requirements, or that your system's OpenGL 3D graphics driver is
   not updated and configured correctly.  If you believe that your graphics
   card DOES meet the minimum requirements then you likely need to install the
   official so-called 'non-free' nVidia or ATI (fglrx) graphics drivers; we
   suggest one of the following options:
 * Consult your Linux distribution's documentation for installing these
   official drivers.  For example, Ubuntu provides documentation here:
   <https://help.ubuntu.com/community/BinaryDriverHowto>
 * If your distribution does not make it easy, then you can download the
   required Linux drivers straight from your graphics card manufacturer:
   - <https://wiki.firestormviewer.org/downloads#video_drivers>

PROBLEM 2:- My whole system seems to hang when running qikfox3D.
SOLUTION:- This is typically a hardware/driver issue.  The first thing to
   do is to check that you have the most recent official drivers for your
   graphics card (see PROBLEM 1).
SOLUTION:- Some residents with ATI cards have reported that running
   'sudo aticonfig --locked-userpages=off' before running qikfox3D solves
   their stability issues.
SOLUTION:- As a last resort, you can disable most of qikfox3D's advanced
   graphics features by editing the 'firestorm' script and removing the '#'
   from the line which reads '#export LL_GL_NOEXT=x'

PROBLEM 3:- After I minimize the qikfox3D window, it's just blank when
   it comes back.
SOLUTION:- Some Linux desktop 'Visual Effects' features are incompatible
   with qikfox3D.  One reported solution is to use your desktop
   configuration program to disable such effects.  For example, on Ubuntu 7.10,
   use the desktop toolbar menu to select System -> Preferences -> Appearance,
   then change 'Visual Effects' to 'None'.

PROBLEM 4:- Music and sound effects are silent or very stuttery.
SOLUTION:- The most common solution is to ensure that you have the 'esd'
   program (part of the 'esound' package) installed and running before you
   start qikfox3D.  Users of Ubuntu (and some other) Linux systems can
   simply run the following to install and configure 'esound':
     sudo apt-get install esound
  For others, simply running 'esd&' from a command-line should get it running.

PROBLEM 5:- Using the 'Alt' key to control the camera doesn't work or just
   moves the qikfox3D window.
SOLUTION:- Some window managers eat the Alt key for their own purposes; you
   can configure your window manager to use a different key instead (for
   example, the 'Windows' key!) which will allow the Alt key to function
   properly with mouse actions in qikfox3D and other applications.

PROBLEM 6:- In-world movie, music, or Flash playback doesn't work for me.
SOLUTION:- You need to have a working installation of GStreamer 0.10; this
   is usually an optional package for most versions of Linux.  If you have
   installed GStreamer 0.10 and you can play some music/movies but not others
   then you need to install a wider selection of GStreamer plugins, either
   from your vendor (i.e. the 'Ugly' plugins) or an appropriate third party.
   For Flash playback, you need to have Flash 10 installed for your normal
   web browser (for example, Firefox).  PulseAudio is required for Flash
   volume control / muting to fully function inside qikfox3D.


6. ADVANCED TROUBLESHOOTING
-=-=-=-=-=-=-=-=-=-=-=-=-=-

The 'firestorm' script which launches qikfox3D contains some
configuration options for advanced troubleshooters.

* AUDIO - Edit the 'secondlife' script and you will see these audio
  options: LL_BAD_OPENAL_DRIVER, LL_BAD_FMODSTUDIO_DRIVER.
  qikfox3D tries to use OpenAL, FMODSTUDIO (PULSEAUDIO, ALSA)
  audio drivers in this order; you may uncomment the corresponding LL_BAD_*
  option to skip an audio driver which you believe may be causing you trouble.

* OPENGL - For advanced troubleshooters, the LL_GL_BLACKLIST option lets
  you disable specific GL extensions, each of which is represented by a
  letter ("a"-"o").  If you can narrow down a stability problem on your system
  to just one or two GL extensions then please post details of your hardware
  (and drivers) to the Linux Client Testers forum (see link below) along
  with the minimal LL_GL_BLACKLIST which solves your problems.  This will help
  us to improve stability for your hardware while minimally impacting
  performance.
  LL_GL_BASICEXT and LL_GL_NOEXT should be commented-out for this to be useful.


7. OBTAINING AND WORKING WITH THE CLIENT SOURCE CODE
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

We're pleased to have released qikfox3D client's source code under
an Open Source license compatible with the 'LGPL'.
<https://wiki.firestormviewer.org/downloads#source_code>


8. GETTING MORE HELP AND REPORTING PROBLEMS
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

For general help and support with qikfox3D:
<http://wiki.firestormviewer.org>

In-world discussion: 

Main support: Join one of our in-world support groups.
<http://wiki.firestormviewer.org/firestorm_in-world_groups>

There is also a 'Linux Client Users' group inside Second Life which 
is free to join.  You can find it by pressing the 'Search' button at 
the bottom of the window and then selecting the 'Groups' tab and 
searching for 'Linux'.  This group is useful for discussing Linux 
issues with fellow Linux client users who are online.

The qikfox3D Issue Tracker:
<https://jira.firestormviewer.org/>
This is the right place for finding known issues and reporting new
bugs in all qikfox3D releases if you find that the Troubleshooting
section in this file hasn't helped.

