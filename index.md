<video style="width: 100%;" controls>
<source src="http://dominikschreiber.de/vid/theremin-demo.webm" type="video/webm">
Your browser does not support the video tag.
</video><br/>  

Out of fun and interest, I built a [Theremin](https://en.wikipedia.org/wiki/Theremin)-like instrument, being played entirely touchless with both hands. The idea was to use simple distance sensors with a digital output to do the audio synthesis on a computer. This allows for interesting and fun ways to tinker with the instrument, such as "auto-tune", different audio waveforms or multiple tones at the same time.

![My Theremin-like instrument with its periphery](http://dominikschreiber.de/theremin/theremin-periphery-small.jpg)  
_My Theremin-like instrument with its periphery_

The hardware part is really easy – two Tinkerforge [ultrasonic distance sensors](https://www.tinkerforge.com/de/doc/Hardware/Bricklets/Distance_US.html#distance-us-bricklet) are connected to a [master brick](https://www.tinkerforge.com/de/doc/Hardware/Bricks/Master_Brick.html), which directly communicates with the computer via USB. USB foot switches can be used to trigger specific effects.

On the computer (e.g. a Raspberry Pi), an application manages the instrument by: 

* periodically polling sensor data;
* synthesizing smooth audio waves out of the raw data;
* sending the samples to the SDL audio mixer;
* processing additional inputs (from keyboard or foot switch) for specific effects; and
* displaying the instrument's current state in a simple graphical interface (if desired).

![The application's UI in action](http://dominikschreiber.de/theremin/ui.jpg)  
_The application's UI in action_

If you are interested in building such an instrument for yourself, visit the [manual page](manual.html).  
If you are interested in the process of how I developed the whole thing, visit the [blog page](blog.html) where I explain it in detail.
