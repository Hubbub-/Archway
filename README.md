# Arco
## by Hubbub
### (Reuben Poharama, Braeden Foster and Martin Hill)

![Arco Image](https://github.com/Hubbub-/Arco/raw/master/IMG_2922.JPG "Arco")

Arco was originally created for NZ Sculpture on Shore 2016.
Revised for Splore Festival 2017.
Revised for Theia's "Treat You" Music Video.
Revised for Taupō Winter Festival 2017.

Find out more about Arco [here](http://hubbub.nz/arco.html)

Basically, the code does the following...  

1. Measure light cast on array of LDRs (light dependant resistors)
2. Create blocks of pulsing colour around any LDR that was "triggered"
3. Increase size of block when the LDR is "triggered" again
4. If there is no one inside the ring, it will play animations
5. If there is no one near Arco, it will pulse while cycling though colours

##Serial commands:

- "pb": Print pixels (prints info about blocks)
- "pl": Print LDRs (prints readings from LDRs)
- "pt": Print Trigs (prints trigger information any time trigger happens)
- "pm": Print Moves (prints whenever PIRs sense movement)
- "ps": Print Strips (prints visual of what the LED strips are showing)
- "pd": Print Distance (prints info from distance sensor)
- "tr": Triggering (Toggle whether to allow LDRs to trigger, default true)
- "br###" Brightness (change overall brightness (### = 0-255))
- "ft##" Force Trigger (trigger LDR (### = 0-13))
- "fm#" Force Mode (manually switch between idle, animation and interaction states)
- "fp##" Force Preset (manually choose which animation preset to use (## = 0-26))



The music video code allowed for the performer to change the behaviour of Arco by using a midi device. Midi signals selected which block and property to change as well as presets.

![Midi Map image](https://raw.githubusercontent.com/Hubbub-/Arco/master/ArcoMidiMap.png)

Makes use of the FastLED, EEPROM, and NewPing Libraries

Equipment list:
- Arduino teensy
- Shift Register x2
- Optocoupler (for using multiple power supplies with LED strips)
- 5m waterproof LED strips x4