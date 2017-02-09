<video width="320" height="240" controls>
<source src="http://dominikschreiber.de/vid/theremin-demo.webm" type="video/webm">
Your browser does not support the video tag.
</video> 

Out of fun and interest, I built a [Theremin](https://en.wikipedia.org/wiki/Theremin)-like instrument, being played entirely touchless with both hands. The idea was to use simple distance sensors with a digital output to do the audio synthesis on a computer. This allows for interesting and fun ways to tinker with the instrument, such as "auto-tune", different audio waveforms or multiple tones at the same time.

![My Theremin-like instrument with its periphery](http://dominikschreiber.de/theremin/theremin-periphery-small.jpg)

The hardware part is really easy – I used two Tinkerforge [ultrasonic distance sensors](https://www.tinkerforge.com/de/doc/Hardware/Bricklets/Distance_US.html#distance-us-bricklet) connected to a [master brick](https://www.tinkerforge.com/de/doc/Hardware/Bricks/Master_Brick.html), which directly communicates with the computer via USB. A simple USB triple foot switch turned out to be a great addition in order to trigger specific effects.

I also developed a small framework for my Theremin to work. The application's tasks are

* periodically polling sensor data;
* synthesizing smooth audio waves out of the raw data;
* sending the samples to the SDL audio mixer;
* processing additional inputs (from keyboard or foot switch) for specific effects; and
* displaying the instrument's current state in a simple graphical interface (if desired).

![The application's UI in action](http://dominikschreiber.de/theremin/ui.jpg)

I used the [SDL2 library](http://libsdl.org), [libconfig](http://www.hyperrealm.com/libconfig/) for configuration management, and the default C(++) bindings for the Tinkerforge sensors. There aren't any other dependencies (besides for the build process and for the brick ↔ computer connectivity).

_If you are interested in building such an instrument for yourself, visit the [manual page](manual.html)._  
_If you are interested in the process of creating the whole thing, visit the [blog page](blog.html) where I explain it in detail._
