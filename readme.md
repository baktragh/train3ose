TRAIN 3 Open Source Edition
===========================
This is the open source edition (OSE) of the TRAIN 3 game for Atari 8-bit computers originally created by BAHA Software in 2020. The source code is provided as is without any guaranteed support.
The source code should help users of the CC65 Toolkit to create fully-featured games.

License
-------
The open source edition is licensed under the BSD license. For full terms of the licence, refer to the train.c source module.

More information on the game
----------------------------
Visit http://baha.webowna.cz/2019/11/22/train-3/ to get more information on the game.

Building the Game from Source
-----------------------------
* Download and install CC65 from https://cc65.github.io/
* Download and install Java for your PC from  https://www.java.com/en/download/
* Modify build.bat to point to the CC65 directories
* Run build.bat batch file

Using the Editor
----------------
The editor is located in the tools directory. To run the editor, run the traineditor.exe. The editor requires Java.
The editor works with the project file (located in the data\project) directory.

* To load and save work in progress, always use the project file (Open project and Save project items from the File menu).
* To export the work, select the Export project to data files item from the File menu. This will export levels.dat and elements.dat files to a selected directory.

Note that the editor cannot load levels.dat or elements.dat files.

Source code of the tools
------------------------
The source code of the CTexter and TrainEditor tools is in the .jar files.


