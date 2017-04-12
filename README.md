# Arco
## by Hubbub
### (Reuben Poharama, Braeden Foster and Martin Hill)

![Arco Image](https://github.com/Hubbub-/Arco/raw/master/IMG_2922.JPG "Arco")

Arco was originally created for NZ Sculpture on Shore 2016.
Revised for Splore Festival 2017.
Revised for Music Video.

Find out more about Arco [here](http://hubbub.nz/arco.html)

Basically, the code does the following...  

1. Measure light cast on array of LDRs (light dependant resistors)
2. Create blocks of pulsing colour around any LDR that was "triggered"
3. Increase size of block when the LDR is "triggered" again

##Serial commands:

- "p": Print pixels (prints info about pixels to the serial every frame)
- "l": Print LDRs (prints readings from LDRs every frame)
- "t": Print Trigs (prints trigger information any time trigger happens)
- "m": Print Moves (prints whenever PIRs sense movement)
- "s": Print Strips (prints visual of what the LED strips are showing)
- "d": Force Idle state
- "bxxx": Brightness (change brightness to xxx {where xxx = 0-255})
- "fxxx": Force trigger (cause LDR number xxx to trigger)



The music video code allowed for the performer to change the behaviour of Arco by using a midi device. Midi signals selected which block and property to change as well as presets.

  Button mapping:
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  |Block1|   |Strobe Speed |  |             |  |             |  |             |
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  |Block2|   |             |  |             |  |             |  |             |
-  +------+   |  Preset 5   |  |  Preset 6   |  |  Preset 7   |  |  Preset 8   |
-             |             |  |             |  |             |  |             |
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  |      |
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-             |             |  |             |  |             |  |             |
-  +------+   |  Preset 1   |  |  Preset 2   |  |  Preset 3   |  |  Preset 4   |
-  |      |   |             |  |             |  |             |  |             |
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+
-  |Reset |   |     Hue     |  | Saturation  |  |  Velocity   |  |    Width    |
-  +------+   +-------------+  +-------------+  +-------------+  +-------------+


Makes use of the FastLED library and the EEPROM Library

Equipment list:
- Arduino teensy
- Shift Register x2
- Optocoupler (for using multiple power supplies with LED strips)
- 5m waterproof LED strips x4