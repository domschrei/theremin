# Manual

The most important information about building and using the instrument

* [Construction and installation](#construction-and-installation)
    * [Hardware](#hardware)
    * [Software](#software)
        * [Dependencies](#dependencies)
        * [Tinkerforge software](#tinkerforge-software)
        * [Sensor setup](#sensor-setup)
        * [Compile and run](#compile-and-run)
    * [Raspberry Pi Configuration](#raspberry-pi-configuration)
* [Using and tweaking](#using-and-tweaking)
    * [Foot switch](#foot-switch)
    * [Autotune modes](#autotune-modes)
    * [Configuration](#configuration)
        * [Performance tuning](#performance-tuning)

***

## Construction and installation

In the following, I will explain the process of building a "theremin" just like mine.

### Hardware

What you will need:

* **A computer capable of running the application**  
(Any better-than-potato computer with USB input and audio output should do the job of running the application. I can also confirm that a Raspberry Pi 3 (Model B) with default Raspbian (_Pixel_ Environment) runs the application well, although the realtime info display needs to be turned off in order to maintain the default sample rate. Use a GNU/Linux system for better support from my side.)
* **Two [ultrasonic distance bricklets](https://www.tinkerforge.com/de/doc/Hardware/Bricklets/Distance_US.html#distance-us-bricklet) and a [master brick](https://www.tinkerforge.com/de/doc/Hardware/Bricks/Master_Brick.html)**  
(also buy the corresponding bricklet ↔ brick connection cables)
* **A triple foot switch**
(like [this one](https://www.amazon.de/dp/B00WS2GZU2/ref=cm_sw_em_r_mt_dp_TK9GybS27T1YN); there are many providers for this specific piece, and others should work fine, too, as long as they are mapped (or mappable) to any printable character) 
* **A mini (not micro) USB cable** to connect the master brick with the computer
* **Cupboard and duct tape** or something similar to improvise a pair of "gloves"  
(to provide a flat surface for the sensors for better results, see below)
* Boxing material at will

### Software

I am developing and executing the program on Linux only, so I will describe the process for Linux systems. 
However, it should not really be a problem to get it running on other platforms, as all used tools and frameworks are cross-platform (with more or less of a hassle).

#### Dependencies

Install `gcc`, `make`, `cmake` and the libraries [SDL2](https://www.libsdl.org/) (the core library with version >= 2.0.4 and the _ttf_ addition) and [libconfig](http://www.hyperrealm.com/libconfig/) (should be in every distribution's repositories – don't forget installing the `*-dev` packages as well, if available).  
Beware: SDL2.0.4 is not available yet in some repositories. For example, you have to build it from sources on the Raspberry Pi 3. This takes about half an hour but shouldn't be complicated otherwise:  
Download the sources from the homepage ([main library](https://www.libsdl.org/release/SDL2-2.0.5.zip), [ttf addition](https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.zip)) and do the following steps, first for the main library and then for the addition:
* unzip the archive and cd into the directory 
* execute the commands `./configure`, `make`, and `sudo make install` (if something fails, look at the error description, you might be missing a dependency)

#### Tinkerforge software
    
The Tinkerforge framework is needed, especially the [C(++) bindings for the Tinkerforge sensors](https://www.tinkerforge.com/de/doc/Software/API_Bindings_C.html).
In order to fetch and build those:

* You can just execute `bash get_tinkerforge.sh` (hereby, the script assumes that the commands `wget` and `unzip` are available), which downloads and builds the bindings.
* Alternatively, you can manually download them [here](https://www.tinkerforge.com/en/doc/Downloads.html#downloads-bindings-examples) and extract them into a `tinkerforge` directory which needs to be at the same level as the `src` directory of the Theremin application. Inside the `tinkerforge/source` directory, execute the command `make` to compile the Tinkerforge bindings.

If you want to install the bindings globally, additionally execute `sudo make install` inside the `tinkerforge/source` directory. If you skip this, you might have to explicitly provide the library path when launching the application (see below).

Also, install the daemon [brickd](https://www.tinkerforge.com/en/doc/Software/Brickd.html#brickd) which will enable communication between the computer and the brick, and the UI assistant [brickv](https://www.tinkerforge.com/en/doc/Software/Brickv.html#brickv) to initially fetch the UIDs of your sensors.

#### Sensor setup

Execute

    sudo brickd --daemon

and open `brickv`. You should now be able to connect to your master brick and see the connected two distance sensors. Get their three-figure UIDs (for example, `uvw` and `xyz`) and replace the definitions of `UID_FREQUENCY` and `UID_VOLUME` inside the configuration file `theremin.cfg` with your UIDs.

    /* Sensor settings */

    // Insert 1st bricklet UID here [alphanumeric string of size 3]
    uid_frequency = "uvw"; 
    // Insert 2nd bricklet UID here [alphanumeric string of size 3]
    uid_volume = "xyz";

(It does not matter which of the sensors is assigned to which constant, as you can just swap them.)

#### Compile and run

You can now compile the Theremin application by executing

    bash build.sh

inside the application's root folder, which should let an executable called `theremin` appear.

Now, with `brickd` still running, you can execute the application (`./theremin`) and play Theremin! (If the program doesn't launch but instead complains about a missing library, start the program with the command `LD_LIBRARY_PATH=tinkerforge/source/:/usr/local/lib/ ./theremin`.) 

The USB foot switch should be plug-and-play; for correct input mapping, see the [foot switch](#foot-switch) subsection.

### Raspberry Pi Configuration

As previously mentioned, the Raspberry Pi (3) can be used to run the application, which allows a tidy and self-contained system. For this purpose, I recommend to run the Raspberry without a display and to make the application automatically start on loading the desktop environment. (The Raspi needs to boot into the Desktop Environment nonetheless, because an X window is created to capture the inputs from the foot switches and other input devices.)

To autostart the application, create a bash script inside `/etc/xdg/lxsession/LXDE-pi/autostart` (the name can be chosen freely, e.g. `theremin.sh`) with the following contents:

    #!/bin/bash
    cd /path/to/theresa
    LD_LIBRARY_PATH=tinkerforge/source/:/usr/local/lib/ ./theremin

Replace `/path/to/theresa` accordingly. Don't forget to make the script executable:

    chmod +x theremin.sh

The application should launch after a reboot. If it doesn't, debug the script by appending

    > ~/theremin_log 2>&1

to its last line and reading the log `~/theremin_log` after a reboot.
    
## Using and tweaking

To properly play the instrument, the sensors should be fastened and aligned. A 90 degree approach as shown in the blueprint works fine, with the frequency sensor pointing upwards and the volume sensor pointing to the side. Of course, the sensors can be built into a little box just like I have done (I could have taken a much smaller box if it wasn't for the long connection cables I bought).

Playing with pure hands is possible, but I experienced rather heavy noise and inaccuracies. To achieve really clear results, you can cut out two large circles of cupboard and glue some straps for your hands on it. The cupboard will reflect the ultrasonic waves very well.

### Foot switch

The foot switch has the following options:

* Left pedal: The current frequency will be stored and will continue to play until the pedal gets pressed another time. You can move your hand and play two tones at once.
* Central pedal: Until the pedal gets pressed another time, all tones will be played exactly one octave higher. This can be used to extend the pitch range of the instrument.
* Right pedal: Each press switches the waveform which is being used for audio synthesis. I implemented some pretty random waveforms, with varying results.

By default, the foot switch used by me just puts out the letters "a", "b" and "c" respectively, just like a keyboard. Hence, you can use the keys of your keyboard as well (which, however, is much less convenient than a foot switch). You can change the triggering characters for specific actions inside the configuration file `theremin.cfg` under the paragraph `Input settings`.

### Autotune modes

The application supports some types of frequency aligning: `none`, `smooth` and `full`. By default, you can change the type by pressing 1, 2 or 3 respectively, and they have the following effects:

* `none`: The distance value of the sensor gets directly mapped to a frequency (in an exponential manner, such that moving the hand by the same amount on different height levels should yield roughly the same frequency interval).
* `smooth`: Works like `none`, but adds a specific sine wave to the frequency such that the played tone gets "pushed" towards proper halftones, but in-between frequencies are possible nonetheless.
* `full`: Every input frequency (the output of the `none` mode) will be aligned towards the halftone which has the minimal distance to the current frequency. This will make the output tone "jump" in between halftones but always produce properly aligned tones.

### Configuration

If your computer can't handle the program without audibly cracking or if you aren't happy with some of the default settings, many things can be tweaked by editing the previously mentioned file `theremin.cfg`. Due to the detailed comments inside, changing the configuration settings should be pretty self-explanatory.

Just restart the application for the changes to take effect.

#### Performance tuning

To tune performance (and trade against better audio quality and/or lower latency), especially the real-time display can be turned off (`realtime_display`) or its refresh rate can be decreased (`task_frequency_display_refresh`). Additionally, the setting `sample_rate` as well as the other different task frequencies (`task_frequency_*`) can be adjusted, resulting in a better performance as well.

 
