<h1 align="center">SR-16 Charging Dash</h1>
<p align ="center">Code for Elcon based SR-16 Charging Dashboard with OLED display</p>
<p align="center">
 <img width="500px" alt="navscreencropped" src="https://github.com/user-attachments/assets/a9ea62d8-c545-4bea-b75a-1eae5c8182c9" />
</p>

## Programming

### Using `VS Code`
Install the [STM32 Extension](https://marketplace.visualstudio.com/items?itemName=STMicroelectronics.stm32-vscode-extension)
```
> Delete the build folder (if there is one)  
> Open Command Palette > CMake: Configure  
> Build project (in Status Bar at the bottom of the VS Code window)
```

### Using `STM32CubeIde`
Simpy build the project

## Flashing

### Using `STM32CubeProgrammer`
```
> Navigate to Erase & Programming Screen
> Update file path to the location of the .elf file (in build folder for VS Code or Debug/Release for CubeIde)
> While holding MCU reset button, click Connect, then release the MCU reset button about half a second later
> Once connected, click Start Programming
```



## Changelog
Kanoa OS 0.1.0 -> First rev  
Kanoa OS 0.2.0 -> Added charging via profiles  
Kanoa OS 0.3.0 -> Added charging via profiles  
Kanoa OS 0.4.0 -> Refactored code to use state machine
