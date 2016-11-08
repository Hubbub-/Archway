# Arco
## by Hubbub
### (Reuben Poharama, Braeden Foster and Martin Hill)

Arco was originally created for NZ Sculpture on Shore 2016.

Basically, the code does the following...  

1. Measure light cast on array of LDRs (light dependant resistors)
2. Create blocks of pulsing colour around any LDR that was "triggered"
3. Increase size of block when the LDR is "triggered" again

##Serial commands:

- "p": Print pixels (prints info about pixels to the serial every frame)
- "l": Print LDRs (prints readings from LDRs every frame)
- "t": Print Trigs (prints trigger information any time trigger happens)
- "m": Print Moves (prints whenever PIRs sense movement)
- "bxxx": Brightness (change brightness to xxx {where xxx = 0-255})
- "fxxx": Force trigger (cause LDR number xxx to trigger)


Makes use of the Adafruit NeoPixel library and the HSBColor Library
