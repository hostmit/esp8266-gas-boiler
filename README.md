# ESP8266 Gas Boiler Controls over thermostat wiring via smartphone (blynk app), web (api), physical button.

This is C++ code to run on ESP8266 (platformio IDE used) as simple control unit to switch on/off generic gas boiler via thermostat wiring.

## Getting Started

In order to use this code u must have some basic skills in electronic's field. If you dont - stop. There is risk of getting shocked or damaging gas boiler.

Most of Gas Boilers on the market can be controlled via thermostat wiring (2 wires). The idea is dead simple:

* Wires shorted - gas boiler works.
* Wires arent connected - gas boiler stops.

In case u have double-pressure boiler as I do, second heating circuit for waterflow is not affected by thermostat wiring.

Thermostat wiring itself does not control the maximum temperature u getting off your gas boiler. It will heat the water, until threshold is reached. Water temperature threshold is set directly on a gas boiler.

Thermostat devices collect air temperature and turn on/off gas bolier pereodically to maintain desired temperature.
This unit itself does not implement such logic, since I will be collecting air temperature metric from another ESP8266 device.


### Prerequisites

You will need following parts to get it working
1. ESP8266 board
2. Resistors 100 Ohm, 1 KOhm.
3. Optocoupler 817c or similiar.
4. Led diod
5. Non-fixed push button (You cant use fixed buttons and have device controlled)

### Deployment

* Copy __credentials_sample.h__ to __credentials.h__, define constants, accroding to your enviroment
* __config.h__ has some optional setting, feel free to edit as you wish
* Upload FW to esp8266, make sure it's reachable over WEB
* Do the wiring according to scheme below
* Setup your blynk app (defaults State LED - Virtual Pin 0, Button - Virtual Pin 1)


### Schema

![Shcema](https://raw.githubusercontent.com/hostmit/esp8266-gas-boiler/master/img/Schematic_gas-boiler-esp8266_ESP8266-Quickly-Design_20191006162502.png)

### API
| Request | Description |
| --- | --- |
| `/api/boiler/on` | Turns ON Gas Boiler |
| `/api/boiler/off` | Turns OFF Gas Boiler |
| `/api/boiler/invert` | Inverts previous state, ON-OFF or OFF-ON |
| `/api/boiler/status` | Gets current state |



Typical JSON response is 
```
{
    "state": true
}
```
## Contributing

Feel free to discuss and contribute.

## Authors

* **Konstantin Odnoralov** - *Initial work* - 

## License

This project is licensed under the MIT License.

