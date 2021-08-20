# COVID-19 Contact Tracing

COVID-19 Contact Tracing is the process of identifying all people that a COVID-19 patient has come in contact with in the last two
weeks. This project deals with the development of an application that simulates contact tracing, collecting virtual Bluetooth MAC
Addresses of the contacts of the user and notifying his close contacts in case of a positive COVID-19 test. The application was tested
on a Raspberry Pi 4 Model B - 2GB RAM, running Raspberry Pi OS Lite and working as an embedded computer. 

## Build instructions

Compile and run in a *x86 architecture*: 

```bash
make
```

Compile and run output file reader:

```bash
make read
```

## Cross-compile for *ARM architecture* and run on RPI instructions:
1. Download cross-compile tools:

```bash
wget https://github.com/raspberrypi/tools/archive/master.zip
sudo unzip master.zip
``` 
2. Update PATH:
```bash 
cd tools-master/arm-bcm2708
cd <toolchain fitting your OS> (check if u have 32 or 64 bit)
cd bin
pwd -> copy the path
export PATH=$PATH:<copied path>
```
3. Cross-compile:
```bash	
make pi
```
4. Run the executable `build/main_pi` on `Raspberry Pi`:
```bash
./main_pi
```
