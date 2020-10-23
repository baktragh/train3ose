@ECHO OFF
REM TRAIN 3 OSE BUILD 
REM -----------------------------------
SET CC65_HOME=C:\UTILS\A8\CC65
SET PATH=%PATH%;C:\UTILS\A8\CC65\BIN
REM -----------------------------------

ECHO Bulding Train...
ECHO CC65 HOME DIR: %CC65_HOME%

REM Process text
ECHO [1/5] Processing texts...
java -jar tools\CTexter.jar data\train_text.txt train_text.h
IF %ERRORLEVEL% NEQ 0 GOTO :XIT

REM Compile and assemble main program
ECHO [2/5] Compiling and assembling main program...
cc65 -t atari -O --all-cdecl --static-locals -o build\train.s train.c
IF %ERRORLEVEL% NEQ 0 GOTO :XIT
ca65 -t atari -o build\train.o build\train.s
IF %ERRORLEVEL% NEQ 0 GOTO :XIT

REM Assemble native routines in assembler
ECHO [3/5] Assembling assembler routines...
ca65 -t atari -o build\train_routines.o train_routines.asm
IF %ERRORLEVEL% NEQ 0 GOTO :XIT

REM Assemble game data
ECHO [4/5] Assembling game data...
ca65 -t atari -o build\train_data.o train_data.asm
IF %ERRORLEVEL% NEQ 0 GOTO :XIT

REM Link binary file
ECHO [5/5] Linking binary load file...
ld65 -C linker.cfg --mapfile build\map.txt -o train3.xex build\train.o build\train_data.o build\train_routines.o atari.lib
IF %ERRORLEVEL% NEQ 0 GOTO :XIT

Echo Build OK
PAUSE
EXIT /B 0

:XIT
ECHO An error occured during build
PAUSE
EXIT /B -1