# e-puck2_rgb_panel
Example of controlling the [RGB panel](https://www.gctronic.com/doc/index.php?title=RGB_Panel) directly from the e-puck2 robot.
There are 3 demos based on the selector position:
* position 0: generate a random color for each LED every second
* position 1: "chamaleon" behavior: take a small picture and reflect the main color on the RGB panel
* position 2: generate a random color for the entire panel (all LEDs have the same color) every second

For information on how to build the project refer to the [e-puck2 wiki](https://www.gctronic.com/doc/index.php?title=e-puck2_robot_side_development#Project_template).

Beware that in order to change the selector postion, **both the e-puck2 and RGB panel must have the selector in the same position**.
