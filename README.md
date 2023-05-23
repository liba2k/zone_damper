# Zone Damper
This project is a controller for a 24v motorized zone damper (see image). 
It uses an ESP32-S2 as a controller and provides a web user interface.
The control interface can be reached at [http://zone_damper.local](http://zone_damper.local) on the local network, but using a static IP is a good option.
A cheap 24V DC motor from AliExpress will actuate a manual zone damper.
An NPN/PNP transistor pair control each direction (Open & Close) since the motor uses a common ground configuration.

<img src="Motorized-Zone-Dampers.webp" width=50% />
<img src="Switch.png"/>
In this image, R3 represents the motor, and the SW1/V2 pair represents a controller GPIO pin. We use this circuit twice, using two GPIO pins to control open and close.

### Installation 
0° should be closed, so we must mount the horizontal position as closed.
90° is vertical and should be open.
