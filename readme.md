# μreMorse
---
## Microbit Morse Code Pager.
Send messages through your Microbit by typing in morse code!

## Installation
1. Install `srecord` and `yotta` using whatever method you fancy 
(package manager etc...)
2. Retrieve this Git repository.
3. Plug in your **Micro:Bit**.
4. Run `make` to build the project.
5. Run `make install` to install project into the **Micro:Bit**

---

## Usage
The program is operated in 2 modes: _Insert_ and _Telecom_ Mode

When the program is first started, the program will be in _Insert Mode_ by 
default,

In **Any** mode, hold both buttons A+B down to switch modes. 

### 1. Insert Mode
* Use Button A to choose a character using morse code, short click button A to 
specify a short signal _(dot)_, while hold button A to specify a long signal 
_(dash)_.
> Shake the **Micro:Bit** to to cancel a selection

* Use Button B to insert the selected character by short clicking.
> If no character is current selected, short clicking B would insert a space.
* A long hold of Button B will delete the previous character
* Clicking Buttons A+B together would insert a newline.

### 2. Telecom Mode
* Short click button A to print all characters inserted.
* To delete all characters currently inserted, short click button B.
* To broadcast your current all that you have currently inserted, hold button A.
* To print the previous broadcast you have recieved hold button B.

### 3. Incomming Mode
> When you recieve a broadcast, an incomming sign would flash on your screen. 
> During the duration of the flash of the incomming sign, _Incomming Mode_ is 
> activated.
* Short Click button A to print the broadcast that you have just recieved.
