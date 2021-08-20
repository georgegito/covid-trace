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

Compile for *ARM architecture* (to run on *Raspberry Pi*):

```bash
make pi
```

Compile and run reader:

```bash
make read
```
