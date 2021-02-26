# ukmarsbot-line-follower-basic


In spite of the name, this repo contains a bunch of test bed code for the UKMARSBOT robot.

there is line follower and maze-based code here.

If you are using the Arduino environment, you can open the sketch by navigating to the ```ukmarsbot-basic-follower``` folder to open the file ```ukmarsbot-basic-follower.ino```. after it has loaded, you will be faced with a whole lot of editor tabs but the project should build and load regardless.

## User interface
Once loaded, the robot will be listening on the serial port for typed commands. Have a look in the main ```ukmarsbot-basic-follower.ino``` file for a function called execute() where you will find all the avaiable commands. Each command can have multiple arguments. You can look at the corresponding command inthe ```commands.cpp``` file to see what is expected. The Arduino code space is a bit tight for listing them in a help function. Maybe one day, I will turn this into something more generic and document the commands properly. Or maybe not.


