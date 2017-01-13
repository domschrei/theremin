# A sensor-driven, digital Theremin

Out of fun and interest, I built a [Theremin](https://en.wikipedia.org/wiki/Theremin)-like instrument. The idea was to use simple distance sensors with a digital output to do the audio synthesis on a computer. This allows for interesting and fun ways to tinker with the instrument, such as "auto-tune", different audio waveforms or multiple tones at the same time.

![My Theremin-like instrument](theremin.jpg)

The hardware part is really easy – I used two "Tinkerforge" [ultrasonic distance sensors](https://www.tinkerforge.com/de/doc/Hardware/Bricklets/Distance_US.html#distance-us-bricklet) connected to a [master brick](https://www.tinkerforge.com/de/doc/Hardware/Bricks/Master_Brick.html), which directly communicates with the computer via USB. A simple USB triple foot switch turned out to be a great addition in order to trigger specific effects.

I also developed a small framework for my Theremin to work. The application's tasks are

* periodically polling sensor data;
* synthesizing smooth audio waves out of the raw data;
* sending the samples to the SDL audio mixer;
* processing additional inputs (from keyboard or foot switch) for specific effects; and
* displaying the instrument's current state in a simple graphical interface.

I am using C++ with the [SDL2](https://www.libsdl.org/) library (_core_ and _ttf_) and the [C(++) bindings for the Tinkerforge sensors](https://www.tinkerforge.com/de/doc/Software/API_Bindings_C.html). No further dependencies are needed.
