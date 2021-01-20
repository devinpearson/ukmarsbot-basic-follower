# UKMARSBOT Settings and defaults

Settings values for UKMARSBOT are found in four places. In the code a structure SettingsData has fields representing the individual settings. For storage efficiency, the fields should be of differing widths which would require more care in reading or writing values. If all fields are the same type then a smple array access technique is appropriate and simple albeit with type casting for ints, floats and so on. Boolean flags should be compressed into a single field in any case. It makes no sense to store booleans as etire bytes or worse.

1.  defaults.h A set of default values is defined as macros in the defaults.h file. These are literal values an should represent a safe set of values that enable to robot to run. If the code is intended to run on a number of different physical robots it might be appropriate to have a single defaults.h file with preprocessor directives that test for a particular robot and include a specific header file for that robot.

2. defaults constant. This is defined as a global constant in the settings.h and settings.c files. Because it is defined as const and all the fields are populated from macros or other const expressions, the vaues will be known at compile time and can be stored in flash. Depending on the code useage, there may be no actual instances of the variable and the compiler is free to substitute literals. Otherwise, the defaults variable is always available and can be used to reset the working settings to the values defined when the software was built.

3. settings variable. This variable is the working copy of the settings stored in RAM and the values can be modified by the code at run time. Although it is wasteful of RAM to have all the settings stored in this way, it does make it easier to adjust values during system characterisation and tuning. If RAM space pressure becomes too great, consider placing some settings in flash only. These might be things like motor gear ratio, encoder count, wheel diameter and track width. These are items that can be determined once for a particular robot and are unlikely to change over time without the expectation of modifying the source code anyway.

4. EEPROM. Non-volatile storage of settings allows the values to survive a hard reset or power cycle. A choice would have to be made whether it is best to always write to EEPROM any changes to the settings variable or to wait until a specific write request is made. Probably, it is best to always write changes to EEPROM in case of a crash. Alternatively, it might be desireable to have two sets of EEPROM settings. one is the 'live' working set - always backed up for insurance, the other is the last known-good working set which is written on-demand from the code. That would ncessarily double the amount of EEPROM used for settings and the Nano Every, with its ATMega4809 processor, only has 256 bytes available in the first place. Note that, for a maze solver, at least 64 bytes is needed to store the maze wall data. There would be no expectation of acces to the EEPROM during operation of the robot unless it is safe to do so. Forr example, when the robot is stationary.

EEPROM values might want some addtional protection. For example, a checksum could provide some protection against read or write errors. More importantly, the initialisation code needs to have some way of knowing that it must update the EEPROM values if the defaults in the source code change. This is a bit tricky because the EEPROM values might have been modified by the user and should not be overwritten. On the other hand, if the settings data changes in size or structure, the old values will be invalid. EEPROM is only accessed by offset from some base address - like an array. Consequently, if a field is altered in the settings strucure, it will no longer align with the EEPROM contents and garbage will be read. One solution is to give the defaults a version number. At startup, compare the program defaults version with the version stored in EEPROM and replace the EEPROM copy if they differ. At the same time, this needs to be kept as simple as possiblefor the user. Transparent if at all possible.

User-mode commands to read, write and save settings will be needed as well as some way to dump settings to the terminal so that the values can be preserved when re-flashing the target. For example, after an hours tuning, the steering controller finally has the right tuning. unless the user can see and or save the tuning values on a host machine, those values could be lost when the robot is next programmed.

---

## Methods

* settingsRestoreDefaults()
  - copy compiled defaults into working settings and then write them to EEPROM
  - should be performed whenever settings structure changes in sources or there is a version change
  - somewhat equivalent to a 'factory reset'

* settingsWrite()
  - save all settings to EEPROM

* settingsRead()
  - read all settings from EEPROM

* settingsSetParameter(n,v)
  - save a single parameter value. Should the changing of a parameter be enforced via this method or is it OK to set the parameter directly? The issues are about always writing to EEPROM and the handling of bit flags. If the settings structure is a class then the members can be private to enforce this. What would the overhead be? inline getters and setters should be reasonably direct.

* settingsInit()
  - read parameters from EEPROM
  - restore defaults if there is a structure or version change

* settingsReport()
  - list all the current settings on the serial console

There is not really a need for saving/loading single parameters It is much easier to read and write the entire settings structure as one block of memory.

If the user changes a single setting then the working copy can be saved en-bloc after the change



