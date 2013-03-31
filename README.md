# Microprocessor Interfacing Laboratory - ICOM5217


***


##Descriptions


####Lab 01 Homework
Lab 01's Homework consisted in taking an LCD and two push-buttons and allowing the user to scroll through a list of frases. Depending on the button that was pressed would be the direction in which the frases would scroll, always displaying two at a time. Upon reaching the end of the list, it would loop around. 

Video: <http://www.youtube.com/watch?v=1cjBIWhjsu0>


####Lab 02 Homework
Lab 02's Homework consisted in building an encoder wheel and then modifying Lab 01's Homework to allow the user to control the scrolling direction of the list of frases. Depending on the direction in which the wheel was spun would be the direction in which the frases would scroll.

Video: <http://www.youtube.com/watch?v=Umj0l7NvHds>


####Lab 03 Exercise 1
Lab 03's Exercise 1 consisted in taking a buzzer and two push-buttons and allowing the user to control the sound which the buzzer was making. Depending on the button that was pressed the frequency was either increased or decreased.

Video: <http://www.youtube.com/watch?v=LHhVy4bvqok>


####Lab 03 Exercise 2
Lab 03's Exercise 2 consisted in taking an LCD and two push-buttons and writing a game for the player to guess any number between 0 and 9. The player is given three attempts at guessing the number before being shown the game over screen. Depending on the button pressed the player's number would increase or decrease. Pressing both buttons would allow the player to select the displayed number. 

Video: <http://www.youtube.com/watch?v=dlT4J7IxGwY>


####Lab 03 Homework
Lab 03's Homework consisted in taking the previously built encoder wheel and calculating its RPM and spinning direction. 

Video: <http://www.youtube.com/watch?v=PkS0cY9fZZM>


***


##How to run this code
In order to run and compile these examples you're required to have a Stellaris LM3S1968 development board along with IAR Embedded Workbench IDE installed in a Windows machine. This was all tested and programmed using a Windows 7 VM running on a 2009 MacBook. Version 6.5 of IAR Embedded Workbench IDE was used. 

Copy whichever example you wish to compile from its respective Lab folder into the following directory (the directory may change if you modified the default install path): 

	C:\Program Files\IAR Systems\Embedded Workbench 6.5\arm\examples\TexasInstruments\Stellaris\boards\ek-lm3s1968\

Open the *.eww project file inside the folder with IAR and right click on driverlib-cm3 and select Make. Then open the program's source code inside IAR and select Download and Debug. Finally, select run.