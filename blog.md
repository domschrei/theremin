# Blog

***

*TL;DR:* I developed a sensor-driven, Theremin-like instrument powered by a plain old computer with some interesting and fun features (see [this little demo](http://dominikschreiber.de/vid/theremin-demo.webm)). If you want to build one yourself, visit [the main page of this repository](https://github.com/domschrei/theremin).

_Used technologies: [Tinkerforge](https://www.tinkerforge.com/de/doc/), C++ with [SDL](http://libsdl.org/) libraries and [CMake](https://cmake.org/) for building_

***

![All of the periphery I use for playing: The instrument itself, a triple foot switch, and some cupboard "gloves" which look idiotic but provide an optimal surface for the sensors](http://dominikschreiber.de/theremin/theremin-periphery-small.jpg)
_All of the periphery I use for playing: The instrument itself, a triple foot switch, and some cupboard "gloves" which look idiotic but provide an optimal surface for the sensors_

Over the course of the last two weeks, I developed a sensor-driven, Theremin-like instrument, being played completely touchless. In the context of a study project in 2015, I got in touch with <a href="https://www.tinkerforge.com/en/">Tinkerforge</a> sensors providing high-level access to interesting sensor data. At that time I thought that it would be neat to make some kind of Theremin out of the distance sensors. Now, I actually went through with it and developed a framework featuring fun and interesting ways to play the instrument.

## A software proof-of-concept

As a computer science guy, I didn't start off with the hardware but instead designed the software portion of the instrument first: an application running on a computer, periodically fetching sensor data and synthesizing audio out of it. I first looked into linux commands such as `aplay` which take raw sample data and play them as audio waves; however, it wasn't well-suited for my application as I did not find a way to tell it WHEN exactly to play the data you pipe into it, so the latency got far too high. Instead, I wrote a C++ application using the <a href="http://libsdl.org">SDL (Simple DirectMedia Layer)</a> audio interface. In a first prototype, the mouse cursor's x and y position on screen were mapped to a frequency and a volume respectively. Then, I wrote several routines which produce time-dependant data points resembling an audio wave with the current frequency and volume – like a sine wave, a sawtooth wave and more.

![An early stage of the application, without a graphical interface: Frequency and volume can be controlled by moving the mouse cursor over the entire screen](http://dominikschreiber.de/theremin/ui-early.png)  
_An early stage of the application, without a graphical interface: Frequency and volume could only be controlled by moving the mouse cursor over the entire screen_

A strange problem I stumbled upon was that the audio output periodically "cracked" (many times per second). I thought that the audio buffer was somehow broken, but after a while of debugging (and outputting the sample data into a `pyplot`) I found that the problem is part of generating the data in waveform. Each time the frequency changes, the function to evaluate changes as well, thus producing a jump in height – there was no continuity. The fix was to make the program remember at which part of the period the wave was last time and to apply this offset to the new function when the frequency changes.

After implementing the prototype and fixing some issues like the one just mentioned, I was pleased with the audio output. The latency was pretty low and I set a sample rate of roughly 16k samples per second with the frequency being updated each time X11 reported a mouse move event. Confident that the instrument I imagined was feasible, I looked into hardware which would be able to provide the neccessary sensor data with high resolution and not-too-high latency.

## Getting the hardware

![The master brick and the two distance bricklets](http://dominikschreiber.de/theremin/brick-and-bricklets.jpg)
_The two distance bricklets (right) strapped to an empty box and connected to the master brick (left), which is connected to the computer via Mini USB_

I decided to use Tinkerforge sensors as they seemed to provide nice and easy application interfaces. Having the choice between infrared and ultrasonic distance sensors, I bought two of the <a href="https://www.tinkerforge.com/de/doc/Hardware/Bricklets/Distance_US.html#distance-us-bricklet">ultrasonic bricklets</a> and the <a href="https://www.tinkerforge.com/de/doc/Hardware/Bricks/Master_Brick.html">master brick</a> needed to connect the sensors to the computer. I also bought a <a href="https://www.amazon.de/SODIAL-Dreibettzimmer-Tastatur-PC-Spiel-Kunststoff/dp/B00L313TUI/ref=sr_1_2?ie=UTF8&qid=1484478418&sr=8-2&keywords=usb+foot+pedal">triple foot switch</a> to be able to trigger special software events while playing. The total expenses of the theremin were roughly at 90€ including shipping.

The foot switch turned out to be plug-and-play: the three pedals work just like keyboard keys, triggering the events for the letters "a", "b", and "c". Those events are really easy to catch using the SDL Event interface. 

Concerning the Tinkerforge sensors, I had to install the daemon `brickd` (needed for communication between the master brick and my application) and the UI assistant `brickv` (for initially fetching the needed UIDs of the two sensors). After starting the daemon, a connection via the USB cable was established instantly.

## Connecting

![The blueprint for all the components](http://dominikschreiber.de/theremin/blueprint.png)
_A blueprint of the different components_

When first connecting the sensor data from the Tinkerforge API with my application's audio output, I was disappointed. The latency was far too high (maybe a fourth of a second of reaction time until a change of the hand's position resulted in a different tone), and there was very much noise and many unjustifiable frequency jumps.

The latency problem was fixed after a short look into the documentation: the master brick would calculate the moving average out of ten sensor values by default, which naturally made the latency grow. I turned the moving average completely off.

I also calibrated the application to only accept distance values inside a sane range interval where one's hands can conveniently be moved to (the sensors have a maximal range of four meters!). The frequency jumps came from the bumpy and irregular surface of the hands, which cannot inherently be fixed – a constraint of the instrument caused by the hardware I chose. Consequently, I cut out two large circles of cardboard and sticked some straps on one side of them (duct tape rocks!). With the flat cardboard directly beneath the hands, the ultrasonic waves get reflected very well, which finally lead to a clear and reliably "correct" tone.

![Two cupboard "gloves" for better sensor recognition and less noise](http://dominikschreiber.de/theremin/cupboard.jpg)  
_Two cupboard "gloves" for better sensor recognition and less noise_

Since this point, the theremin has been fully playable – the application made a clear tone appear which depended on the two hand's positions.  
For the visuals and to essentially "blackbox" the sensors (quite literally), I took a small box (which could have been much smaller if it wasn't for the long bricklet-brick connection cables I bought) and crammed the brick and the bricklets into it, such that all that peaks out is the two sensors and the mini USB port.

![One of the two sensors tucked into the box](http://dominikschreiber.de/theremin/theremin-inside.jpg)  
_One of the two sensors tucked into the box_

![The finished theremin box](http://dominikschreiber.de/theremin/theremin-box.jpg)  
_The finished (and painted) theremin box_

Now I had a nice little black-box of sensors. Time to abstract from the periphery and get into improving the software side.

## Enhancing

A difficult part of playing the instrument is intonation, as there is no feedback whether you have hit the right tone – apart from the audio that plays. It is challenging (and fun!) to learn how much you have to move your hand to get a third, a fourth and so on. To support the player with finding the right notes, I added a real-time display where the current note as well as some other information is shown. The two bars visualize the relative volume and frequency at the moment, and inside the black square the note name which most closely fits the current frequency is shown. The more exact the frequency fits the note, the brighter it is displayed. The display also shows when events like the ones from the foot switch are triggered.

![A simple interface showing information about the instrument's current state](http://dominikschreiber.de/theremin/ui.jpg)  
_A simple interface showing information about the instrument's current state_

For the fine-tuning of the frequency, I have implemented different types of frequency alignment which one could call "autotune": one of the modes aggressively shifts the current frequency to the halftone which has the minimal distance to this frequency. This produces jumps in frequency but ensures that you aren't off by a fraction of a tone. Another mode is the "smooth autotune" which pushes the current tone _towards_ the next halftone but still allows all frequencies in-between (you could say that it gets more probable to hit a proper halftone). You can also turn autotune completely off such that you are on your own finding the exact frequency.

![How the different modes change the time-dependant frequency](http://dominikschreiber.de/theremin/frequency-alignment.png)  
_How the different modes change the time-dependant frequency_

Due to the purely digital audio synthesis, there are many possibilities to tweak the instrument. Being triggered by one of the pedals, the currently playing note can be "locked" so that it will continue to play even if you move your hand. This way, you can play in two voices, with a sustained note and a melody voice. With another pedal, the currently playing note (not the locked one, though, if there is one) will be played an octave higher. This widens the pitch range of the instrument by another octave and enables easy octave jumps in certain songs. The third pedal changes the waveform. I implemented some pretty random waveforms (amongst others, there is a _singleslit_ waveform and a narrowly scaled _gauss_ function) with very varying results – some sound very aggressive while others are soft and smooth.

## Conclusion, or: Is it an instrument?

![The theremin being played](http://dominikschreiber.de/theremin/theremin-playing.jpg)

I wasn't sure if I could build a device out of these sensors which could actually be considered an _instrument_, and not only a toy or a funny gadget. I practised a bit and am confident to say that it can actually be used as an instrument, with different possible playstyles and the potential to play many melodies (although maybe not the fastest and most complex ones). Of course, there are major drawbacks of my instrument compared to a conventional theremin, namely the lower accuracy and less opportunities to do fine, subtle variations (like tremoli). But there are also some benefits like its potential to tinker with the audio output however you wish and, as a consequence, creative new playstyles like playing in two voices or with an auto-tune enabled. (Also, its low price is a plus.)

If you would like to build such an instrument of your own, just read the README on [this repository's main page](https://github.com/domschrei/theremin) to learn how to get started. The code is GPL-licenced, so you are free to use, modify and redistribute it as long as the modified code is accessible and usable under the same terms. If you have a problem or an idea to share, please do so on the github page – I am looking forward to hearing from you!  
