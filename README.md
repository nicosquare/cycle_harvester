# Wat Harvester

Internal behavior of WAT harvester, currently as a project for Embedded Systems - 2017 (2)  cours at Universidad Nacional de Colombia. Also the hardware base of former project in 
IBM's Unchain the Frame hackathon, getting to 4th place among 200 other projects. And finally, it's also the subject of thesis of one of the collaborators in this repo. Open to any 
entushiast or interested. 

## Collaborators:

- Pablo Emilio Camacho Prieto (pecamachop) - pecamachop@unal.edu.co
- Nicolas Mauricio Cuadrado Ávila (nicosquare) - nmcuadradoa@unal.edu.co

## How it works:

The first protype will harvest energy consumption information, basically is a sensor en energy. The hardware consist in an Onion development board mounted in a custom board (ARM 
architecture) that has as peripherals: a Bluetooth communication module and a custom energy sensor. The os of the whole embedded system is ChibiOS, a real time operative system
whose tool-chain allow us to cross-compile towards the Onion board.

## Notes:

Originally the project considered communication via LoRa protocol, very handy with the low frequences we're going to handle, but hardware restriction made us postopone this 
implementation.

# Energy sensor:

An integrated circuit ADE7753 is the core of a single circuit of energy sensoring, based on the recommended common use circuit in the datasheet and on the inverse engineering project
of [NiKiZe](https://github.com/NiKiZe/EMT707CTL). 

## Instructions to clone the repo 

``` git clone https://github.com/nicosquare/wat_harvester.git ```

# TODO

- Define project file structure
- Create dummy project
- Integrate code of Bluetooth module
- Integrate code of energy sensor

Last update: 24 november 2017.
