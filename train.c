/* Train 3 Open Source Edition 

Copyright (c) 2020,
Michael Kalouš, Petr Postava (BAHA Software)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software must
   display the following acknowledgement: This product includes software
   developed by the BAHA Software
4. Neither the name of the <organization> nor the names of its contributors may
   be used to endorse or promote products derived from this software without
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY BAHA Software ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL BAHA Software BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/


/* Main program */
#include <stdio.h>
#include <peekpoke.h>
#include <stdlib.h>
#include <string.h>
#include <atari.h>

#include "train_text.h"

/*Configuration defines*/
#define CFG_MAX_LEVEL_INDEX (24)

//#define CFG_ALLOW_CHEATS
//#define CFG_SINGLE_TREASURE


/*External data*/
extern unsigned char TRAIN_DATA_CHARSET1;
extern unsigned char TRAIN_DATA_CHARSET1_PAGE;
extern unsigned char TRAIN_DATA_CHARSET2;
extern unsigned char TRAIN_DATA_CHARSET2_PAGE;
extern unsigned char TRAIN_DATA_DL_MENU;
extern unsigned char TRAIN_DATA_DL_GAME;
extern unsigned char TRAIN_DATA_DL_INTERMISSION;
extern unsigned char TRAIN_DATA_DL_SCENESELECTION;
extern unsigned char TRAIN_DATA_PMGAREA;
extern unsigned char TRAIN_DATA_GAMESCREEN;
extern unsigned char TRAIN_DATA_STATUSBARSCREEN;

extern unsigned char TRAIN_FIGURE_RIGHT;
extern unsigned char TRAIN_FIGURE_LEFT;
extern unsigned char TRAIN_FIGURE_UP;
extern unsigned char TRAIN_FIGURE_DOWN;

extern unsigned char MENU_TRAIN_TITLE;
extern unsigned char TRAIN_GAME_LEVELS;
extern unsigned char TRAIN_GAME_ELEMENTS;

extern unsigned char menuScrollTextFlag;


/*Screen related*/
#define GAMESCREEN_SIZE (960)
#define PLAYFIELD_OFFSET (82)
#define STATUSBARSCREEN_SIZE (40)

/*Joystick*/
#define JS_LEFT (11)
#define JS_RIGHT (7)
#define JS_UP (14)
#define JS_DOWN (13)
#define JS_C (15)

#define JS_BIT_LEFT (4)
#define JS_BIT_RIGHT (8)
#define JS_BIT_UP (1)
#define JS_BIT_DOWN (2)

/*Game speed*/
#define GAME_SPEED_NORMAL (0)
#define GAME_SPEED_SLOW (1)


/*PMG*/
#define PMG_P0_AREA (1024)
#define PMG_P1_AREA (1280)
#define PMG_P2_AREA (1536)
#define PMG_P3_AREA (1792)

/*Level and level elements*/
#define LEVEL_DATA_SIZE (144)
#define ELEMENT_DATA_SIZE (33)

#define EL_BLANK (0)
#define EL_TR1 (4)
#define EL_TR2 (8)
#define EL_TR3 (12)
#define EL_TR4 (16)
#define EL_WALL (20)
#define EL_GATE_CLOSED (24)
#define EL_GATE_OPEN (28)
#define EL_POISONED_WALL2 (32)

/*Direction element masks*/
#define EL_DIR_LEFT (0)
#define EL_DIR_RIGHT (1)
#define EL_DIR_UP (2)
#define EL_DIR_DOWN (3)

/*Characters for separator and track*/
#define CHAR_SEPARATOR (89)
#define CHAR_TRACK (94)

/*Index of the first car character*/
#define EL_CARS_FIRST (64)

/*Quick Vertical SYNC*/
#define VERTICAL_SYNC(ID) __asm__ (" lda 20"); \
                      __asm__ ("@vsync_%s: cmp 20",ID); \
                      __asm__ (" beq @vsync_%s",ID);

/*Game status*/
#define ADVANCE_NEXT_CONTINUE (0)
#define ADVANCE_NEXT_CONGRATS (1)
#define LOSE_LIFE_CONTINUE (0)
#define LOSE_LIFE_GAMEOVER (1)
#define NEW_HISCORE_YES (1)
#define NEW_HISCORE_NO (0)
#define LEVEL_NONE (254)
#define DEATH_TYPE_CRASH (0)
#define DEATH_TYPE_POISON (1)

#define LEVEL_FLAG_POISONED_TREASURE (0x80)

/*Quickly erase element at loco position*/
#define ERASE_AT_LOCO(PTR_ID,PROBE_PTR_ID) \
        PTR_ID = PROBE_PTR_ID; \
        *(PTR_ID) = 0; \
        ++PTR_ID; \
        *(PTR_ID) = 0; \
        PTR_ID += 39;  \
        *(PTR_ID) = 0; \
        ++PTR_ID;      \
        *(PTR_ID) = 0; \


/*Display and handle main game menu*/
void handleMenu();
void clearScreen();
void clearStatusBar();
void paintTrainTitle();
void paintMenuItems();
unsigned char screenSceneSelection();
void clearPlayField();
void hideLoco();

/*Game toggles*/
void setGameSpeed(unsigned char speed);
void toggleGameInit(unsigned char gameInitType);


/*Level and element expansion*/
void expandAndPaintLevel(unsigned char levelIndex);
void transposeElement(unsigned char* ptr);
void eraseElement(unsigned char* ptr);
void expandMask(char* elementDataPtr, unsigned char elementCharIndex);
void paintGameStatusBar();

/*Decimal displays*/
void decimalDisplaysInit();
void resetZonedScore();
extern void incrementScore();
extern void decrementScore();
unsigned char updateMenuScores();
void incrementNextLifeScore();

/*PMG*/
void pmgInit();
void setLocoPos(unsigned char x, unsigned char y, unsigned char xoffset, unsigned char yoffset);
void locoMove(unsigned char direction);
void repaintLoco();
void pmgSetForLoco();
void pmgSetForSceneSelection();
void pmgSetDefaultLocoColors();

/*Time and timing*/
void delay(unsigned char w);

/*Graphics mode displays*/
void setScreen(void* dlPtr,
        unsigned char statusBarFg,
        unsigned char statusBarBg,
        void* dliHndl,
        unsigned char chsetPage,
        unsigned char c0,
        unsigned char c1,
        unsigned char c2,
        unsigned char c3,
        unsigned char c4
        );

/*Display settings and visual effects*/
void enableDisplay();
void disableDisplay();
void fadeInAndOut();
void levelFadeOut();
void levelFadeIn();
void verticalSync(unsigned char count);


/*Music numbers*/
#define MUSIC_INGAME (0x00)
#define MUSIC_MENU (0x21)
#define MUSIC_NEW_HISCORE (0x2B)
#define MUSIC_GAME_OVER (0x33)
#define MUSIC_SCENE_SELECTION (0x37)
#define MUSIC_CONGRATULATIONS (0x3A)
#define MUSIC_EMPTY (0x49)

/*SFX Numbers*/
#define SFX_TRAIN (1)
#define SFX_TREASURE (2)
#define SFX_CRASH (4)
#define SFX_SCENE_COMPLETE (8)
#define SFX_POISONED_CARGO (16)


/* Audio-related imports*/
extern unsigned char _sfxRequested;
extern unsigned char _songLineRequested;
extern unsigned char _inGameAudioFlag;

/*Scrolling text imports*/
extern unsigned char hscroll;
extern unsigned char scrollCount;

/* Music routines - low level*/
extern void rmtSetUniversalVBI();
extern void rmtRestoreOriginalVBI();


/* Music routines - high level*/
void audioRequestSongLine(unsigned char songLine);
void audioRequestSFX(unsigned char sfxNumber);
void audioSetInGameFlag(unsigned char inGameFlag);
void audioSetForSilence();
void audioStopInGameAudio();


/*Train movement*/
void updateTrainArray(unsigned char dirCode);
void drawTrainCars(unsigned char doErase);

/*Level outcomes and continuations*/
unsigned char gameToNextLevel();
unsigned char gameLoseLife(unsigned char deathElement);
void screenGameOver();
void screenGratulation();
void screenNewHighScore();

/*DLI handlers*/
extern unsigned char dliHandler;
extern unsigned char dliMenuHandler;
extern unsigned char dliInterMission;
/*DLI colors*/
extern unsigned char statusBarForeground;
extern unsigned char statusBarBackground;

/*Swap all controls*/
void swapAllControls();
extern unsigned char filteredJS;
extern void getFilteredJS();


/* Train */
/* Position of the train and eraser in the screen memory*/
unsigned char locoX, locoY;
unsigned char* locoPtr;
/* PMG coordinates*/
unsigned char p0x, p0y;
/* PMG Player areas*/
unsigned char* p0Area, *p1Area, *p2Area, *p3Area;
/* Direction */
unsigned char locoJoystickDirection;
unsigned char locoOldJoystickDirection;
unsigned char locoDirectionCode;
/* Element in the train path*/
unsigned char* locoProbePtr;
unsigned char probedElement;

/*Array of train cars and their directions*/
unsigned char trainCarArray[180];
unsigned char trainDirectionArray[180];

/*Zero page variables for repainting of the train*/
extern unsigned char *zp_ptr1, *zp_ptr2, *zp_ptr3;
extern unsigned char zp_z1, zp_x1;

#pragma zpsym ("zp_ptr1")
#pragma zpsym ("zp_ptr2")
#pragma zpsym ("zp_ptr3")
#pragma zpsym ("zp_z1")
#pragma zpsym ("zp_x1")


/* Door pointer*/
unsigned char* doorPtr;

/*Element masks*/
unsigned char inverseMasks[128];

/*Direction updates*/
unsigned int directionReverseUpdate[4] = {2, -2, 80, -80};

/*Zoned decimals*/
#define ZN_0 16
#define ZN_2 18
#define ZN_3 19
#define ZN_5 21
#define ZN_9 25
#define ZN_6 22
#define ZN_7 23
#define ZN_8 24

#define ZN_SCORE_LENGTH (5)

unsigned char dScore[5];
unsigned char dLastScore[5];
unsigned char dHighScore[5];
unsigned char dNextLifeScore[5];
unsigned char dLevelInitialScore[5];
unsigned char d300[5] = {ZN_0, ZN_0, ZN_0, ZN_3, ZN_0};
unsigned char d5000[5] = {ZN_0, ZN_0, ZN_5, ZN_0, ZN_0};

#ifdef CFG_ALLOW_CHEATS
unsigned char d68000[5] = {ZN_0, ZN_6, ZN_8, ZN_0, ZN_0};
unsigned char d70000[5] = {ZN_0, ZN_7, ZN_0, ZN_0, ZN_0};
#endif


/*Movement control*/
unsigned char normalMoveDelay;
unsigned char fastMoveDelay;
unsigned char realMoveDelay;
unsigned char lastKey;

unsigned char PALSpeeds[2] = {17, 20};
unsigned char NTSCSpeeds[2] = {20, 24};

/*Colors changed in DLI*/
extern unsigned char colorStore1;
extern unsigned char colorStore2;

/*NTSC color correction*/
unsigned char PAL2NTSC[16] = {0, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 240, 224, 16, 32};
/*Pointer to current train figure*/
unsigned char* trainData = &TRAIN_FIGURE_RIGHT;

/*Game initial parameters*/
#define GAME_INIT_NORMAL (0)
#define GAME_INIT_HEAD (1)
unsigned char gameInitLives;
unsigned char dGameInitScore[5];
unsigned char dGameInitNextLifeScore[5];
unsigned char gameInitType;

/*Game status*/
unsigned char gameLevelIndex;
unsigned char gameMaxLevelIndex;
unsigned char gameLives;
unsigned char lastLevelExpanded;

/*Level status*/
unsigned char levelTreasure;
unsigned char levelTrainLength;
unsigned char* levelInitialLocoFigure;
unsigned char levelFlags;


/*Score, scene, and lives display*/
unsigned char* gameScorePtr;
unsigned char* gameLevelPtr;
unsigned char* gameLivesPtr;

unsigned char* lastScorePtr;
unsigned char* highScorePtr;

/*Menu*/
#define MENU_ITEM_START_GAME (0)
#define MENU_ITEM_SPEED (1)
#define MENU_ITEM_AUDIO (2)
#define MENU_ITEM_MAX (3)

#define GAME_AUDIO_SFX (0)
#define GAME_AUDIO_BGM (1)

unsigned char menuCurrentItem;
unsigned char menuGameSpeed;
unsigned char menuGameAudio;
unsigned char menuCycleTrainFlag;
unsigned char menuScrollTextFlag;

/*Sounds and music*/
extern unsigned char songLineRequested;
extern unsigned char sfxRequested;
extern unsigned char inGameAudioFlag;

int main() {

    unsigned char* ptr1;
    unsigned char deathType;

    disableDisplay();
    verticalSync(2);
    
    /*Housekeeping*/
    POKE(580, 1);
    gameInitType = GAME_INIT_NORMAL;
    setGameSpeed(GAME_SPEED_NORMAL);
    menuGameSpeed=GAME_SPEED_NORMAL;
    menuGameAudio=GAME_AUDIO_SFX;
    
    toggleGameInit(gameInitType);

    /*Init the PMG*/
    pmgInit();
    pmgSetForLoco();
    decimalDisplaysInit();

    /*Game-relate variables independent on a single game*/
    gameMaxLevelIndex = 0;

    memset(dHighScore, ZN_0, sizeof (dHighScore));
    memset(dLastScore, ZN_0, sizeof (dLastScore));
    
    /*Set audio for silence*/
    audioSetInGameFlag(0);
    audioSetForSilence();
    rmtSetUniversalVBI();

/*Main menu ==================================================================*/
menu:
    /*Handle the main menu*/
    handleMenu();

    disableDisplay();
    verticalSync(1);

    /*Reset single-game variables*/
    gameLevelIndex = 0;
    lastLevelExpanded = LEVEL_NONE;

    /*Prepare normal or head start - score, lives, next life score*/
    gameLives = gameInitLives;
    memcpy(dNextLifeScore, dGameInitNextLifeScore, sizeof (dNextLifeScore));
    memcpy(dScore, dGameInitScore, sizeof (dScore));
    
    /*Set game speed to the speed selected in menu*/
    setGameSpeed(menuGameSpeed);

    /*Select scene*/
    if (gameMaxLevelIndex > 0) {
        disableDisplay();
        fadeInAndOut();
        gameLevelIndex = screenSceneSelection();
    }
    
    /*Play either empty music or in-game music*/
    if (menuGameAudio==GAME_AUDIO_BGM) {
        audioRequestSongLine(MUSIC_INGAME);
    }
    else {
        audioRequestSongLine(MUSIC_EMPTY);
    }
    /*This is now in-game audio, so one can switch between SFX and music*/
    audioSetInGameFlag(1);

    
/*Level loop =================================================================*/
level:

    /*Clear everything*/
    setScreen(&TRAIN_DATA_DL_GAME,
            12, 48,
            &dliHandler,
            TRAIN_DATA_CHARSET1_PAGE,
            0x24, 12, 0x32, 14, 0);

    pmgSetForLoco();
    hideLoco();
    clearStatusBar();
    clearPlayField();
    
    /*Keep initial level score*/
    memcpy(dLevelInitialScore,dScore,sizeof(dLevelInitialScore));

    /*Expand and paint level and update status bar*/
    expandAndPaintLevel(gameLevelIndex);
    paintGameStatusBar();

    /*Prepare loco for the level*/
    setLocoPos(locoX, locoY, 0, 0);
    trainData = levelInitialLocoFigure;
    repaintLoco();
    
    /*Re-enable display with fading in*/
    levelFadeIn();
    swapAllControls();

    /*Reset train status*/
    levelTrainLength = 0;
    memset(trainCarArray, 0, sizeof (trainCarArray));
    memset(trainCarArray, 0, sizeof (trainDirectionArray));

    /*Wait for joystick movement*/
    locoOldJoystickDirection = JS_C;
    lastKey = 255;
    while (getFilteredJS(), lastKey = PEEK(764), filteredJS == JS_C && lastKey != 28);

    /*Check for premature exit*/
    if (lastKey == 28) {
        audioStopInGameAudio();
        goto menu;
    }

    /*Clear time and keyboard*/
    POKE(20U, 0);
    POKE(764, 0);
    POKE(77, 0);
    
    /*Initial movement delay*/
    realMoveDelay=normalMoveDelay;
    fastMoveDelay=normalMoveDelay>>1;

/*Game loop ================================================================= */
gameLoop:
    
    /*If fire is pressed, then move faster*/
    if (PEEK(644U)==0) realMoveDelay=fastMoveDelay;
    
    /*Determine what is the direction*/
    getFilteredJS();
    if (filteredJS != JS_C) locoJoystickDirection = filteredJS;

    /*Check for ESC, if pressed, go to menu*/
    if (PEEK(764) == 28) {
        audioStopInGameAudio();
        goto menu;
    }

    /*Equalized movement of the loco*/
    if (PEEK(20) < realMoveDelay) goto gameLoop;

    /*Reset timer and movement delay*/
    POKE(20, 0);
    realMoveDelay=normalMoveDelay;

    /*Point to the element in train's path*/
    if (locoJoystickDirection == JS_LEFT) {
        locoProbePtr = (locoPtr - 2);
        locoDirectionCode = EL_DIR_LEFT;
        trainData = &TRAIN_FIGURE_LEFT;
    } else if (locoJoystickDirection == JS_RIGHT) {
        locoProbePtr = (locoPtr + 2);
        locoDirectionCode = EL_DIR_RIGHT;
        trainData = &TRAIN_FIGURE_RIGHT;
    } else if (locoJoystickDirection == JS_UP) {
        locoProbePtr = locoPtr - 80;
        locoDirectionCode = EL_DIR_UP;
        trainData = &TRAIN_FIGURE_UP;
    } else if (locoJoystickDirection == JS_DOWN) {
        locoProbePtr = locoPtr + 80;
        locoDirectionCode = EL_DIR_DOWN;
        trainData = &TRAIN_FIGURE_DOWN;
    }


    /*Repaint train if direction has changed*/
    if (locoOldJoystickDirection != locoJoystickDirection) {
        repaintLoco();
        locoOldJoystickDirection = locoJoystickDirection;
    }

    /*Check what the element is*/
    probedElement = ((*locoProbePtr) & 0x7C);

    /*Open door - move train, paint train and terminate level*/
    if (probedElement == EL_GATE_OPEN) {
        ERASE_AT_LOCO(ptr1, locoProbePtr)
        locoMove(locoJoystickDirection);
        if (levelTrainLength != 0) {
            updateTrainArray(locoDirectionCode);
            drawTrainCars(1);
        }
        if (gameToNextLevel() == ADVANCE_NEXT_CONTINUE) {
            levelFadeOut();
            goto level;
        } else {
            audioStopInGameAudio();
            levelFadeOut();
            screenGratulation();
            if (updateMenuScores() == NEW_HISCORE_YES) {
                screenNewHighScore();
            }
            goto menu;
        }
    }

    /*Something solid - death*/
    if (probedElement > EL_TR4) {

        /*If poisoned treasure, move the train before death*/
        if (probedElement == EL_POISONED_WALL2 && (levelFlags & LEVEL_FLAG_POISONED_TREASURE)) {
            ERASE_AT_LOCO(ptr1, locoProbePtr)
            locoMove(locoJoystickDirection);
            if (levelTrainLength != 0) {
                updateTrainArray(locoDirectionCode);
                drawTrainCars(1);
            }
            deathType = DEATH_TYPE_POISON;
        } else {
            deathType = DEATH_TYPE_CRASH;
        }
        

        if (gameLoseLife(deathType) == LOSE_LIFE_CONTINUE) {
            levelFadeOut();
            goto level;
        } else {
            levelFadeOut();
            audioStopInGameAudio();
            screenGameOver();
            if (updateMenuScores() == NEW_HISCORE_YES) {
                screenNewHighScore();
            }
            goto menu;
        }

    }

    /*Some treasure - pick it and grow the train*/
    if (probedElement >= EL_TR1 && probedElement <= EL_TR4) {

        /*Sound*/
        audioRequestSFX(SFX_TREASURE);

        /*Erase treasure*/
        ERASE_AT_LOCO(ptr1, locoProbePtr)

        /*Move the loco into the treasure*/
        locoMove(locoJoystickDirection);

        /*Prepare for new train*/
        if (levelTrainLength == 0) {
            trainDirectionArray[0] = locoDirectionCode;
        }
        /*Update train array*/
        trainCarArray[levelTrainLength] = EL_CARS_FIRST + ((probedElement - EL_TR1) << 2);
        ++levelTrainLength;
        updateTrainArray(locoDirectionCode);
        incrementScore();

        /*Check for new life*/
        if (memcmp(gameScorePtr, dNextLifeScore, sizeof (dNextLifeScore)) > 0) {

            if (gameLives < 9) {
                ++gameLives; //Increment internal counter
                ++(*gameLivesPtr); //Increment counter on the screen  
            }
            incrementNextLifeScore(); //Next life in next 5000
        }

        /*Draw the train*/
        drawTrainCars(0);

        locoPtr = locoProbePtr;

        /*Count treasure*/
        levelTreasure--;
        if (levelTreasure == 0) {
            *(doorPtr) = EL_GATE_OPEN + inverseMasks[EL_GATE_OPEN];
            *(doorPtr + 1) = EL_GATE_OPEN + 1 + inverseMasks[EL_GATE_OPEN + 1];
            *(doorPtr + 40) = EL_GATE_OPEN + 2 + inverseMasks[EL_GATE_OPEN + 2];
            *(doorPtr + 41) = EL_GATE_OPEN + 3 + inverseMasks[EL_GATE_OPEN + 3];
        }

    }/*Otherwise just move the train*/
    else {
        /*Sound*/
        audioRequestSFX(SFX_TRAIN);

        /*Move the loco*/
        locoMove(locoJoystickDirection);

        updateTrainArray(locoDirectionCode);
        drawTrainCars(1);

        locoPtr = locoProbePtr;

    }

    goto gameLoop;

    return 0;
}

/*Move all train cars in proper direction*/
void updateTrainArray(unsigned char dirCode) {

    memmove(trainDirectionArray + 1, trainDirectionArray, levelTrainLength);
    trainDirectionArray[0] = dirCode;

}

/*Draw train cars. Making these fast by usage of the zp*/
void drawTrainCars(unsigned char doErase) {

    /*Point to the first car*/
    zp_ptr1 = locoPtr;
    /*Point to the train array and direction array*/
    zp_ptr2 = trainCarArray;
    zp_ptr3 = trainDirectionArray;


    for (zp_z1 = 0; zp_z1 < levelTrainLength; ++zp_z1) {

        /*Combine car type and direction*/
        zp_x1 = *(zp_ptr2)+((*zp_ptr3) << 2);

        /*Paint the car*/
        *zp_ptr1 = zp_x1 | inverseMasks[zp_x1];
        ++zp_x1;
        *(zp_ptr1 + 1) = zp_x1 | inverseMasks[zp_x1];
        ++zp_x1;
        *(zp_ptr1 + 40) = zp_x1 | inverseMasks[zp_x1];
        ++zp_x1;
        *(zp_ptr1 + 41) = zp_x1 | inverseMasks[zp_x1];

        zp_ptr3++;
        zp_ptr1 += directionReverseUpdate[(*zp_ptr3)];
        zp_ptr2++;
    }

    if (!doErase) return;

    zp_x1 = 0;
    /*Erase last car*/
    *zp_ptr1 = zp_x1;
    ++zp_ptr1;
    *zp_ptr1 = zp_x1;
    zp_ptr1 += 39;
    *zp_ptr1 = zp_x1;
    ++zp_ptr1;
    *zp_ptr1 = zp_x1;

}

/*Get level data and expand them to arrays*/
void expandAndPaintLevel(unsigned char level) {

    unsigned char *ptr1, *ptr2, *ptr3;
    unsigned char i1;
    unsigned char x1, y1, z1;
    unsigned char* elPtr;

    /*Point to the beginning of a level and reset counters*/
    ptr1 = &TRAIN_GAME_LEVELS + (level * LEVEL_DATA_SIZE);
    ptr3 = &TRAIN_GAME_ELEMENTS;

    levelTreasure = 0;

    /*Level data has the following structure  
     * 5 bytes - Color pallette 712,708,709,710,711
     * 1 byte  - Gate closed element number
     * 1 byte  - Gate open element number
     * 1 byte  - Wall element number
     * 1 byte  - Poisoned/Wall2 element number
     * 4 bytes - Numbers of treasure elements
     * 8 bytes - Numbers of car elements, two cars per element
     * 
     * 1 byte - Train Y and X coordinates
     * 1 byte - Gate Y and X coordinates 
     * 1 byte - Flags (train orientation and poisoned treasure flag)
     * 120 bytes - Element indexes follow, one element per nibble
     */

    /*Perform what is needed only once for level*/
    if (lastLevelExpanded != level) {

        /*Process the color pallette*/
        POKE(712, *ptr1);
        ptr1++;

        memcpy((unsigned char*) 708, ptr1, 4);
        ptr1 += 4;

        /*Change pallette*/
        if (get_tv() == AT_NTSC) {
            for (i1 = 0; i1 < 4; i1++) {

                /*Color shift*/
                z1 = PEEK(708 + i1);
                z1 = PAL2NTSC[(z1 & 0xF0) >> 4];
                POKE(708 + i1, (PEEK(708 + i1)&0x0F) | z1);

                /*Not too dark*/
                z1 = PEEK(708 + i1);
                if ((z1 & 0x0F) == 0) z1 |= 2;
                POKE(708 + i1, z1);
            }
        }

        /*Clear the inversion masks for everything except train cars*/
        memset(inverseMasks, 0, 36);

        /*Set characters for the gate closed element*/
        ptr2 = &TRAIN_DATA_CHARSET1 + (EL_GATE_CLOSED << 3);
        memcpy(ptr2, ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), 32);
        expandMask(ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), EL_GATE_CLOSED);
        ++ptr1;

        /*Set characters for the gate open element*/
        ptr2 = &TRAIN_DATA_CHARSET1 + (EL_GATE_OPEN << 3);
        memcpy(ptr2, ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), 32);
        expandMask(ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), EL_GATE_OPEN);
        ++ptr1;

        /*Set characters for the wall element*/
        ptr2 = &TRAIN_DATA_CHARSET1 + (EL_WALL << 3);
        memcpy(ptr2, ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), 32);
        expandMask(ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), EL_WALL);
        ++ptr1;

        /*Set characters for the poisoned treasure/wall2 element*/
        ptr2 = &TRAIN_DATA_CHARSET1 + (EL_POISONED_WALL2 << 3);
        memcpy(ptr2, ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), 32);
        expandMask(ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), EL_POISONED_WALL2);
        ++ptr1;

        /*Set characters for the treasure elements - 4 treasure elements*/
        ptr2 = &TRAIN_DATA_CHARSET1 + (EL_TR1 << 3);

        for (i1 = 0; i1 < 4; i1++) {
            memcpy(ptr2, ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), 32);
            expandMask(ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE), EL_TR1 + (i1 << 2));
            ptr1++;
            ptr2 += 32;
        }

        /*Car elements. For each treasure, we have 4 cars. But only two are
         * defined in the elements (right and up), because the other figures are
         * just mirrors of the two. 
         * 
         * Each treasure requires 4 figures of car (16 characters, 128 character bytes)
         */

        /*For each treasure*/
        for (i1 = 0; i1 < 4; i1++) {

            elPtr = (ptr3 + ((*ptr1) * ELEMENT_DATA_SIZE));

            ptr2 = &TRAIN_DATA_CHARSET1 + (EL_CARS_FIRST << 3)+(i1 << 7);


            /*Expand right car - Primary Image*/
            memcpy(ptr2 + 32, elPtr, 32);
            expandMask(elPtr, EL_CARS_FIRST + 4 + (i1 << 4));

            /*Copy to the left car*/
            memcpy(ptr2, elPtr, 32);
            expandMask(elPtr, EL_CARS_FIRST + (i1 << 4));

            /*Transpose the left car*/
            transposeElement(ptr2);

            ++ptr1;
            elPtr += ELEMENT_DATA_SIZE;

            /*Expand top car*/
            memcpy(ptr2 + 64, elPtr, 32);
            expandMask(elPtr, EL_CARS_FIRST + 8 + (i1 << 4));
            /*Copy to the bottom car*/
            memcpy(ptr2 + 96, elPtr, 32);
            expandMask(elPtr, EL_CARS_FIRST + 12 + (i1 << 4));
            /*Transpose the bottom car*/
            transposeElement(ptr2 + 96);

            ++ptr1;
            elPtr += ELEMENT_DATA_SIZE;
        }


    } else {
        ptr1 += 21;
    }

    /*Now process the rest that must be refreshed always*/

    /*Set initial train position*/
    /*Train X,Y - for PMG display*/
    locoY = *ptr1 / 20;
    locoX = *ptr1 % 20;
    ++ptr1;

    /*Train pointer - screen memory (top left corner)*/
    locoPtr = &TRAIN_DATA_GAMESCREEN + (locoX << 1)+(locoY * 80);

    /*Door position and pointer*/
    y1 = *ptr1 / 20;
    x1 = *ptr1 % 20;
    ptr1++;

    /*Paint door*/
    doorPtr = &TRAIN_DATA_GAMESCREEN + (x1 << 1)+(y1 * 80);
    *(doorPtr) = EL_GATE_CLOSED + inverseMasks[EL_GATE_CLOSED];
    *(doorPtr + 1) = EL_GATE_CLOSED + 1 + inverseMasks[EL_GATE_CLOSED + 1];
    *(doorPtr + 40) = EL_GATE_CLOSED + 2 + inverseMasks[EL_GATE_CLOSED + 2];
    *(doorPtr + 41) = EL_GATE_CLOSED + 3 + inverseMasks[EL_GATE_CLOSED + 3];

    /*Process the level flags*/
    z1 = (*(ptr1)&0x0F);
    switch (z1) {
        case JS_LEFT: levelInitialLocoFigure = &TRAIN_FIGURE_LEFT;
            break;
        case JS_RIGHT: levelInitialLocoFigure = &TRAIN_FIGURE_RIGHT;
            break;
        case JS_UP: levelInitialLocoFigure = &TRAIN_FIGURE_UP;
            break;
        case JS_DOWN: levelInitialLocoFigure = &TRAIN_FIGURE_DOWN;
            break;
    }
    z1 = (*(ptr1)&0xF0);
    levelFlags = z1;
    ++ptr1;

    /*Now paint the level elements */
    ptr2 = &TRAIN_DATA_GAMESCREEN;

    /*12 Rows*/
    for (y1 = 0; y1 < 12; ++y1) {
        /*And 20 columns*/
        for (x1 = 0; x1 < 20; ++x1) {

            /*Get element*/
            if (x1 & 0x01) {
                z1 = (*ptr1)&0xF;
                ++ptr1;
            } else {
                z1 = ((*ptr1) >> 4)&0xF;
            }

            /*Multiply by 4 to get screen code*/
            z1 = z1 << 2;

            /*Count treasure*/
            if (z1 >= EL_TR1 && z1 <= EL_TR4) {
                levelTreasure++;
            }

            /*Top left*/
            *ptr2 = z1 + inverseMasks[z1];
            ++z1;
            /*Top right*/
            *(ptr2 + 1) = z1 + inverseMasks[z1];
            ++z1;
            /*Bottom left*/
            *(ptr2 + 40) = z1 + inverseMasks[z1];
            ++z1;
            /*Bottom right*/
            *(ptr2 + 41) = z1 + inverseMasks[z1];
            /*Move two columns in screen memory*/
            ptr2 += 2;

        }
        /*Move two rows in screen memory*/
        ptr2 += 40;

    }

#ifdef CFG_SINGLE_TREASURE
    levelTreasure = 1;
#endif

}

/*Transpose element. An element consists of 4 characters, pointer points
  to the first of the four. Each character is 8 bytes*/
void transposeElement(unsigned char* elFirstCharPtr) {

    unsigned char cLeft, cRight, cCur;
    unsigned char i1;

    /*Transpose pairs of characters*/
    for (i1 = 0; i1 < 8; i1++) {

        cLeft = *(elFirstCharPtr + i1);
        cRight = *(elFirstCharPtr + 8 + i1);

        cCur = (cRight & 0x03) << 6;
        cCur |= (cRight & 0x0C) << 2;
        cCur |= (cRight & 0x30) >> 2;
        cCur |= (cRight & 0xC0) >> 6;
        *(elFirstCharPtr + i1) = cCur;

        cCur = (cLeft & 0xC0) >> 6;
        cCur |= (cLeft & 0x30) >> 2;
        cCur |= (cLeft & 0x0C) << 2;
        cCur |= (cLeft & 0x03) << 6;
        *(elFirstCharPtr + 8 + i1) = cCur;

        cLeft = *(elFirstCharPtr + i1 + 16);
        cRight = *(elFirstCharPtr + 24 + i1);

        cCur = (cRight & 0x03) << 6;
        cCur |= (cRight & 0x0C) << 2;
        cCur |= (cRight & 0x30) >> 2;
        cCur |= (cRight & 0xC0) >> 6;
        *(elFirstCharPtr + i1 + 16) = cCur;

        cCur = (cLeft & 0xC0) >> 6;
        cCur |= (cLeft & 0x30) >> 2;
        cCur |= (cLeft & 0x0C) << 2;
        cCur |= (cLeft & 0x03) << 6;
        *(elFirstCharPtr + 24 + i1) = cCur;
    }

    /*Transpose also the inverse masks*/
    cLeft = (elFirstCharPtr - &TRAIN_DATA_CHARSET1) >> 3;
    cRight = cLeft + 1;
    cCur = inverseMasks[cLeft];
    inverseMasks[cLeft] = inverseMasks[cRight];
    inverseMasks[cRight] = cCur;
    cLeft += 2;
    cRight += 2;
    cCur = inverseMasks[cLeft];
    inverseMasks[cLeft] = inverseMasks[cRight];
    inverseMasks[cRight] = cCur;


}

void expandMask(unsigned char* elementDataPtr, unsigned char elementCharIndex) {

    unsigned char* maskArrayPtr;
    unsigned char bitMask = 128;
    unsigned char k;

    /*Skip over element characters*/
    elementDataPtr += 32;
    /*Point to the mask array*/
    maskArrayPtr = inverseMasks + elementCharIndex;

    /*Expand mask bits to bytes of 0 and 128*/
    for (k = 0; k < 4; k++) {
        if ((*elementDataPtr) & bitMask) {
            *maskArrayPtr = 128;
        } else {
            *maskArrayPtr = 0;
        }
        /*Advance to next mask bit and mask byte */
        ++maskArrayPtr;
        bitMask = bitMask >> 1;
    }
}

/*Erase element, ptr points to the top left char*/
void eraseElement(unsigned char* ptr) {

    *ptr = 0;
    ++ptr;
    *ptr = 0;
    ptr += 39;
    *ptr = 0;
    ++ptr;
    *ptr = 0;
}

void paintGameStatusBar() {
    memcpy(&TRAIN_DATA_STATUSBARSCREEN, T_SCORE_BAR, T_SCORE_BAR_L);
    memcpy(gameScorePtr, dScore, sizeof (dScore));
    *(gameLivesPtr) = ZN_0 + gameLives;
    *(gameLevelPtr) = ZN_0 + (gameLevelIndex + 1) / 10;
    *(gameLevelPtr + 1) = ZN_0 + (gameLevelIndex + 1) % 10;
}

void saveZonedScore() {
    memcpy(dScore, gameScorePtr, sizeof (dScore));
}

void resetZonedScore() {
    memset(dScore, ZN_0, sizeof (dScore));
}


void audioStopInGameAudio() {
    inGameAudioFlag=0;
    sfxRequested=0;
    songLineRequested=MUSIC_EMPTY;
    verticalSync(2);
}


void clearPlayField() {

    unsigned char* ptr1;
    unsigned char y1;

    ptr1 = &TRAIN_DATA_GAMESCREEN + PLAYFIELD_OFFSET;

    /*Clear 20 lines*/
    for (y1 = 0; y1 < 20; ++y1) {
        memset(ptr1, 0, 36);
        ptr1 += 40;
    }

}

void hideLoco() {
    POKE(53248U, 0);
    POKE(53249U, 0);
    POKE(53250U, 0);
    POKE(53251U, 0);
}

void handleMenu() {

    /*First set the screen*/
    setScreen(&TRAIN_DATA_DL_MENU,
            12, 48,
            &dliMenuHandler,
            TRAIN_DATA_CHARSET2_PAGE,
            0x24, 12, 0x32, 14, 0);

    /*Clear the screen*/
    clearScreen();
    clearStatusBar();
    pmgSetForLoco();
    pmgSetDefaultLocoColors();
    
    /*Set menu items*/
    menuCurrentItem = MENU_ITEM_START_GAME;
    
    
    /*Paint title*/
    paintTrainTitle();
    /*Paint menu items*/
    paintMenuItems();
    
    /*Cycle the train and scroll the text*/
    menuCycleTrainFlag=1;
    menuScrollTextFlag=2;
    audioRequestSongLine(MUSIC_MENU);
    verticalSync(2);
    

    /*Last and top score*/
    memcpy(&TRAIN_DATA_STATUSBARSCREEN, T_TOPSCORE_BAR, T_TOPSCORE_BAR_L);
    memcpy(lastScorePtr, dLastScore, sizeof (dLastScore));
    memcpy(highScorePtr, dHighScore, sizeof (dHighScore));
    
    /*Show the loco*/
    locoX=0;
    locoY=9;
    trainData = &TRAIN_FIGURE_RIGHT;
    setLocoPos(locoX, locoY, 4, 6);

    /*Enable display*/
    enableDisplay();

    /*Swap keyboard and joystick*/
    swapAllControls();

    /*Display the first status bar lines*/
    POKE(20, 0);
    
    /*Almost endless loop - terminated by either RESET or game start*/

    while (1) {

        /* START will start the game*/
        if (PEEK(53279U) == 6) break;
        
        /*Fire will either start game or toggle game speed*/
        if (PEEK(644)==0) {
           
            POKE(77U,0);
            
            /*Start game?*/
            if (menuCurrentItem==MENU_ITEM_START_GAME) break;
            
            /*If not, then wait until trigger released*/
            while(PEEK(644)==0);
            
            if (menuCurrentItem==MENU_ITEM_SPEED) {
                menuGameSpeed=!menuGameSpeed;
                
            }
            else if (menuCurrentItem==MENU_ITEM_AUDIO) {
                menuGameAudio=!menuGameAudio;
            }
            
            paintMenuItems();
            delay(10);
                    
        }
        
        
        
        if (PEEK(632)==JS_UP && menuCurrentItem>MENU_ITEM_START_GAME) {
            menuCurrentItem--;
            paintMenuItems();
            POKE(77U,0);
            delay(10);
        }
        if (PEEK(632)==JS_DOWN && menuCurrentItem<MENU_ITEM_AUDIO) {
            menuCurrentItem++;
            paintMenuItems();
            POKE(77U,0);
            delay(10);
        }
        
        
#ifdef CFG_ALLOW_CHEATS
        if (PEEK(53279U) == 5) {
            if (gameMaxLevelIndex == CFG_MAX_LEVEL_INDEX) {
                gameMaxLevelIndex = 0;
                POKE(712, 22);
            }
            else {
                gameMaxLevelIndex = CFG_MAX_LEVEL_INDEX;
                POKE(712, 180);
            }
            delay(20);
            POKE(712, 0);
        }
        if (PEEK(53279U) == 3) {

            if (gameInitType == GAME_INIT_NORMAL) {
                POKE(712, 0x94);
            } else {
                POKE(712, 0x04);
            }
            gameInitType = !gameInitType;
            toggleGameInit(gameInitType);
            delay(20);
            POKE(712, 0);
        }
#endif

    }
    
    /*Stop cycling the train*/
    disableDisplay();
    menuCycleTrainFlag=0;
    menuScrollTextFlag=0;
    
    /*And stop music*/
    audioRequestSongLine(MUSIC_EMPTY);
    
}

void clearScreen() {
    memset(&TRAIN_DATA_GAMESCREEN, 0, GAMESCREEN_SIZE);
}

void clearStatusBar() {
    memset(&TRAIN_DATA_STATUSBARSCREEN, 0, STATUSBARSCREEN_SIZE);
}

void paintTrainTitle() {

    unsigned char* ptr1, *ptr2;
    unsigned char x1;
    
    /*Title*/
    ptr1 = &TRAIN_DATA_GAMESCREEN + (1 * 40);
    ptr2 = &MENU_TRAIN_TITLE;

    for (x1 = 0; x1 < 9; ++x1) {
        memcpy(ptr1, ptr2, 39);
        ptr1 += 40;
        ptr2 += 39;
    }
    
    /*Subtitle*/
    memcpy(&TRAIN_DATA_GAMESCREEN+(10*40)+T_TRAIN_TITLE_C,T_TRAIN_TITLE,T_TRAIN_TITLE_L);
    
    /*Separators*/
    memset(&TRAIN_DATA_GAMESCREEN+(0*40),CHAR_SEPARATOR,40);
    memset(&TRAIN_DATA_GAMESCREEN+(11*40),CHAR_SEPARATOR,40);
    
    /*Track*/
    memset(&TRAIN_DATA_GAMESCREEN+(15*40),CHAR_TRACK,40);

}

void paintMenuItems() {
    
    unsigned char i;
    unsigned char l;
    unsigned char* ptr;
    
    verticalSync(1);
    
    /*Clear everything*/
    memset(&TRAIN_DATA_GAMESCREEN+(12*40),0,120);
    
    /*Paint "START GAME"*/
    memcpy(&TRAIN_DATA_GAMESCREEN+(12*40)+T_MENU_ITEM_1_C,T_MENU_ITEM_1,T_MENU_ITEM_1_L);
    
    /*Paint "Game Speed" */
    if (menuGameSpeed==GAME_SPEED_NORMAL) {
        memcpy(&TRAIN_DATA_GAMESCREEN+(13*40)+T_MENU_ITEM_SPD_1_C,T_MENU_ITEM_SPD_1,T_MENU_ITEM_SPD_1_L);
    }
    else {
        memcpy(&TRAIN_DATA_GAMESCREEN+(13*40)+T_MENU_ITEM_SPD_2_C,T_MENU_ITEM_SPD_2,T_MENU_ITEM_SPD_2_L);
    }
    
    /*Paint "Audio"*/
    if (menuGameAudio==GAME_AUDIO_SFX) {
        memcpy(&TRAIN_DATA_GAMESCREEN+(14*40)+T_MENU_ITEM_AUDIO_1_C,T_MENU_ITEM_AUDIO_1,T_MENU_ITEM_AUDIO_1_L);
    }
    else {
        memcpy(&TRAIN_DATA_GAMESCREEN+(14*40)+T_MENU_ITEM_AUDIO_2_C,T_MENU_ITEM_AUDIO_2,T_MENU_ITEM_AUDIO_2_L);
    }

    /*Inverse the current menu item*/
    switch (menuCurrentItem) {
        case(MENU_ITEM_START_GAME):
        {
            ptr = &TRAIN_DATA_GAMESCREEN + (12 * 40) + T_MENU_ITEM_1_C - 1;
            l = T_MENU_ITEM_1_L + 2;
            break;
        }
        case (MENU_ITEM_SPEED):
        {
            ptr = &TRAIN_DATA_GAMESCREEN + (13 * 40) + T_MENU_ITEM_SPD_1_C - 1;
            l = T_MENU_ITEM_SPD_1_L + 2;
            break;
        }
        case (MENU_ITEM_AUDIO):
        {
            ptr = &TRAIN_DATA_GAMESCREEN + (14 * 40) + T_MENU_ITEM_AUDIO_1_C - 1;
            l = T_MENU_ITEM_AUDIO_1_L + 2;
            break;
        }
    }
    
    for(i=0;i<l;i++) {
        *(ptr)=(*ptr)|128;
        ++ptr;
    }
    
    
}


/*Player missile graphics*/
void pmgInit() {

    /*Set PMG memory start*/
    POKE(54279U, ((unsigned) (&TRAIN_DATA_PMGAREA)) >> 8);

    /*Clear all PMG memory*/
    memset((void*) &TRAIN_DATA_PMGAREA, 0U, 2048);

    /*Enable PMG*/
    POKE(53277U, 2);

    /*Convenience pointers*/
    p0Area = &TRAIN_DATA_PMGAREA + PMG_P0_AREA;
    p1Area = &TRAIN_DATA_PMGAREA + PMG_P1_AREA;
    p2Area = &TRAIN_DATA_PMGAREA + PMG_P2_AREA;
    p3Area = &TRAIN_DATA_PMGAREA + PMG_P3_AREA;
}

void pmgSetForLoco() {


    /*Clear*/
    memset((void*) &TRAIN_DATA_PMGAREA, 0U, 2048);
    p0x = 0;
    p0y = 0;

    pmgSetDefaultLocoColors();

    /*Players width - default*/
    memset((void*) 0xD008U, 0, 4);

    /*Priority*/
    POKE(623U, 0x01);
    

}

void pmgSetDefaultLocoColors() {
    /*Player colors*/
    POKE(704, 0x26); //Red
    POKE(705, 0xEC); //Yellow
    POKE(706, 0x74); //Light Blue
    POKE(707, 0x72); //Dark Blue
}

void pmgSetForSceneSelection() {

    /*Clear*/
    memset((void*) &TRAIN_DATA_PMGAREA, 0U, 2048);

    /*Player P0 width*/
    POKE(0xD008U, 3);

    /*Priority*/
    POKE(623, 4);

    /*Colors - just player 0*/
    POKE(704, 0x24);

}

/*Repaint the loco*/
void repaintLoco() {

    memcpy(((unsigned char*) (p0y + p0Area)), trainData, 16);
    memcpy(((unsigned char*) (p0y + p1Area)), trainData + 16, 16);
    memcpy(((unsigned char*) (p0y + p2Area)), trainData + 32, 16);
    memcpy(((unsigned char*) (p0y + p3Area)), trainData + 48, 16);
}

/*Set position of the loco*/
void setLocoPos(unsigned char x, unsigned char y, unsigned char xoffset, unsigned char yoffset) {

    p0x = 48 + xoffset + (x << 3);
    memset(p0y + p0Area, 0, 16);
    memset(p0y + p1Area, 0, 16);
    memset(p0y + p2Area, 0, 16);
    memset(p0y + p3Area, 0, 16);
    p0y = 24 + yoffset + (y << 4);

    POKE(53248U, p0x);
    POKE(53249U, p0x);
    POKE(53250U, p0x);
    POKE(53251U, p0x);
    repaintLoco();

}

void locoMove(unsigned char direction) {

    switch (direction) {
        case JS_LEFT:
        {
            p0x -= 8;
            POKE(53248U, p0x);
            POKE(53249U, p0x);
            POKE(53250U, p0x);
            POKE(53251U, p0x);
            --locoX;
            break;
        }
        case JS_RIGHT:
        {
            p0x += 8;
            POKE(53248U, p0x);
            POKE(53249U, p0x);
            POKE(53250U, p0x);
            POKE(53251U, p0x);
            ++locoX;
            break;
        }
        case JS_UP:
        {
            memset(p0y + p0Area, 0, 16);
            memset(p0y + p1Area, 0, 16);
            memset(p0y + p2Area, 0, 16);
            memset(p0y + p3Area, 0, 16);

            p0y -= 16;
            --locoY;
            repaintLoco();
            break;
        }
        case JS_DOWN:
        {
            memset(p0y + p0Area, 0, 16);
            memset(p0y + p1Area, 0, 16);
            memset(p0y + p2Area, 0, 16);
            memset(p0y + p3Area, 0, 16);
            p0y += 16;
            ++locoY;
            repaintLoco();
            break;
        }
    }

}

/*Wait for some time*/
void delay(unsigned char w) {
    unsigned char i = 0;
    for (i = 0; i < w; i++) {
        unsigned char a = PEEK(20);
        while (PEEK(20) == a) {
        }
    }
}

/* Audio routines - high level*/
void audioRequestSongLine(unsigned char songLine) {
    songLineRequested=songLine;
}
void audioRequestSFX(unsigned char sfxNumber) {
    sfxRequested=sfxNumber;
}
void audioSetInGameFlag(unsigned char inGameFlag) {
    inGameAudioFlag=inGameFlag;
    verticalSync(2);
}
void audioSetForSilence() {
    songLineRequested=MUSIC_EMPTY;
    sfxRequested=0;
    verticalSync(2);
}


void setScreen(void* dlPtr,
        unsigned char statusBarFg,
        unsigned char statusBarBg,
        void* dliHndl,
        unsigned char chsetPage,
        unsigned char c0,
        unsigned char c1,
        unsigned char c2,
        unsigned char c3,
        unsigned char c4) {

    /*Disable screen*/
    POKE(559, 0);
    verticalSync(1);
    
    /*Set colors*/
    POKE(708, c0);
    POKE(709, c1);
    POKE(710, c2);
    POKE(711, c3);
    POKE(712, c4);
    /*Set character set*/
    POKE(756, chsetPage);

    /*Wait for vsync before setting-up DL*/
    __asm__( " SEI ");
    POKE(560, (unsigned) (dlPtr) & 0x00FF);
    POKE(561, (unsigned) (dlPtr) >> 8);
    __asm__( " CLI ");

    /*Wait until screen stabilizes*/
    verticalSync(1);

    statusBarForeground = statusBarFg;
    statusBarBackground = statusBarBg;

    /*Set DLI and enable it*/
    POKE(512, ((unsigned int) dliHndl) & 0x00FF);
    POKE(513, ((unsigned int) dliHndl) >> 8);

    /*Wait for vsync before enabling DLI*/
    verticalSync(1);
    POKE(54286U, 192);
}

void setIntermissionScreen() {
    
    setScreen(&TRAIN_DATA_DL_INTERMISSION,
            12, 0,
            &dliInterMission,
            TRAIN_DATA_CHARSET2_PAGE,
            0xC8, 12, 0, 0, 0);
}

void setSceneSelectionScreen() {
    
    setScreen(&TRAIN_DATA_DL_SCENESELECTION,
            12, 0,
            &dliInterMission,
            TRAIN_DATA_CHARSET2_PAGE,
            0xC8, 12, 0, 14, 0);
}

/*Scene selection repaint selection cursor*/
void sceneSelectionMoveHighlight(unsigned char** ptr, signed char delta) {
    unsigned char* curNumberPtr = *ptr;

    *(curNumberPtr) &= 0x7F;
    *(curNumberPtr + 1) &= 0x7F;
    curNumberPtr += delta;
    *(curNumberPtr) |= 0x80;
    *(curNumberPtr + 1) |= 0x80;
    *ptr += delta;

}

/*Scene selection*/
unsigned char screenSceneSelection() {

    unsigned char *ptr1, *ptr2;
    unsigned char maxCurY;
    unsigned char maxCurX;
    unsigned char z1, cursorX, cursorY;
    unsigned char *curNumberPtr;

    /*Set screen*/
    setSceneSelectionScreen();

    /*Set PMG for scene selection*/
    pmgSetForSceneSelection();

    /*Clear everything*/
    clearScreen();
    clearStatusBar();
    
    audioRequestSongLine(MUSIC_SCENE_SELECTION);

    /*Draw title line*/
    ptr1 = &TRAIN_DATA_GAMESCREEN;
    memcpy(ptr1 + T_SCENE_SEL_C, T_SCENE_SEL, T_SCENE_SEL_L);
    ptr1+20;
    
    /*Draw separators*/
    memset(&TRAIN_DATA_GAMESCREEN+20,CHAR_SEPARATOR,40);
    memset(&TRAIN_DATA_GAMESCREEN+160,CHAR_SEPARATOR,40);
    
    ptr1=&TRAIN_DATA_GAMESCREEN+60;
    
    /*Draw main contents - lines with numbers*/
    if (gameMaxLevelIndex > 0) {
        memcpy(ptr1, T_SCENE_ROW1, T_SCENE_ROW1_L);
        ptr1 += 20;
        maxCurY = 0;
    }
    if (gameMaxLevelIndex > 4) {
        memcpy(ptr1, T_SCENE_ROW2, T_SCENE_ROW2_L);
        ptr1 += 20;
        maxCurY = 1;
    }
    if (gameMaxLevelIndex > 9) {
        memcpy(ptr1, T_SCENE_ROW3, T_SCENE_ROW3_L);
        ptr1 += 20;
        maxCurY = 2;
    }
    if (gameMaxLevelIndex > 14) {
        memcpy(ptr1, T_SCENE_ROW4, T_SCENE_ROW4_L);
        ptr1 += 20;
        maxCurY = 3;
    }
    if (gameMaxLevelIndex > 19) {
        memcpy(ptr1, T_SCENE_ROW5, T_SCENE_ROW5_L);
        ptr1 += 20;
        maxCurY = 4;
    }
    
    /*Every other line is highlighted*/
    for (ptr2 = &TRAIN_DATA_GAMESCREEN+60;ptr2<&TRAIN_DATA_GAMESCREEN+160;ptr2+=40) {
        for (curNumberPtr=ptr2;curNumberPtr<ptr2+20;curNumberPtr++) {
            *(curNumberPtr)|=64;
        }
    }
    
    /*Show what to do in the status bar*/
    memcpy(&TRAIN_DATA_STATUSBARSCREEN + T_SCENE_BEGIN_C, T_SCENE_BEGIN, T_SCENE_BEGIN_L);

    /*Determine maximum X cursor coordinate*/
    maxCurX = gameMaxLevelIndex % 5;

    /*Determine cursor coordinates*/
    cursorY = gameMaxLevelIndex / 5;
    cursorX = gameMaxLevelIndex % 5;

    /*Now delete extraneous scene numbers*/
    ptr2 = ptr1;
    ptr1 -= 20; //Point to the last line
    ptr1 += 3; //Position to the first triplet
    ptr1 += 3 * ((cursorX) + 1); //Position for deletion
    curNumberPtr = ptr1 - 3; //Position in screen memory for current
    memset(ptr1, 0, ptr2 - ptr1); //Delete rest of the line

    /*Position the player so that the maximum reached level is selected*/
    p0x = 66 + (cursorX * 24);
    p0y = 59 + (cursorY * 18);
    POKE(53248U, p0x);
    memset(p0Area + p0y, 254, 20);

    /*Highlight the selected level*/
    *(curNumberPtr) |= 0x80;
    *(curNumberPtr + 1) |= 0x80;

    enableDisplay();

    /*Swap all controls*/
    swapAllControls();

    while (PEEK(644) == 1) {

        z1 = PEEK(632);

        switch (z1) {
            case JS_LEFT:
            {
                if (cursorX == 0) break;
                cursorX--;
                p0x -= 24;
                POKE(53248U, p0x);
                sceneSelectionMoveHighlight(&curNumberPtr, -3);
                delay(10);
                break;
            }
            case JS_RIGHT:
            {
                if ((cursorX == 4) || (cursorY == maxCurY && cursorX == maxCurX)) break;
                cursorX++;
                p0x += 24;
                POKE(53248U, p0x);
                sceneSelectionMoveHighlight(&curNumberPtr, +3);
                delay(10);
                break;
            }
            case JS_UP:
            {
                if (cursorY == 0) break;
                memset(p0Area + p0y, 0, 20);
                cursorY--;
                p0y -= 18;
                memset(p0Area + p0y, 254, 20);
                sceneSelectionMoveHighlight(&curNumberPtr, -20);
                delay(9);
                break;
            }
            case JS_DOWN:
            {
                if ((cursorY == maxCurY) || (cursorY == maxCurY - 1 && cursorX > maxCurX)) break;
                memset(p0Area + p0y, 0, 20);
                cursorY++;
                p0y += 18;
                memset(p0Area + p0y, 254, 20);
                sceneSelectionMoveHighlight(&curNumberPtr, +20);
                delay(9);
                break;
            }
        }

    }

    audioRequestSongLine(MUSIC_EMPTY);

    return (cursorY * 5)+cursorX;
}

void screenGameOver() {

    

    /*Set screen*/
    setIntermissionScreen();

    /*Set PMG for scene selection*/
    hideLoco();

    /*Clear everything*/
    clearScreen();
    clearStatusBar();

    /*Two separator lines*/
    memset(&TRAIN_DATA_GAMESCREEN+20,CHAR_SEPARATOR,40);
    memset(&TRAIN_DATA_GAMESCREEN+200,CHAR_SEPARATOR,40);

    /*Game Over inscription*/
    memcpy(&TRAIN_DATA_GAMESCREEN+100+ T_GAME_OVER_C, T_GAME_OVER, T_GAME_OVER_L);
    
    audioRequestSongLine(MUSIC_GAME_OVER);

    enableDisplay();

    delay(100);
    swapAllControls();
    memcpy(&TRAIN_DATA_STATUSBARSCREEN + T_PRESS_FIRE_C, T_PRESS_FIRE, T_PRESS_FIRE_L);

    /*Wait for FIRE*/
    while (PEEK(644) == 1);

    /*And disable music*/
    audioRequestSongLine(MUSIC_EMPTY);

}



void screenRainbow() {

    unsigned char p20;
    unsigned char vc;
    unsigned char c = 0;

    while (PEEK(644) == 1) {
        
        p20 = PEEK(20);
        while (p20 == PEEK(20)) {
            vc = PEEK(0xD40B);
            POKE(0xD40A, 0);
            POKE(0xD01A, vc+c & 0xF2);
        }
        ++c;
    }

}

void screenGratulation() {

    unsigned char *ptr1;

    /*Set screen*/
    setIntermissionScreen();
    pmgSetForLoco();

    fadeInAndOut();

    /*Clear everything*/
    clearScreen();
    clearStatusBar();
    
    /*Two separator lines*/
    memset(&TRAIN_DATA_GAMESCREEN+20,CHAR_SEPARATOR,40);
    memset(&TRAIN_DATA_GAMESCREEN+200,CHAR_SEPARATOR,40);
    /*Track*/
    memset(&TRAIN_DATA_GAMESCREEN+160,CHAR_TRACK,40);
    
    /*Title*/
    ptr1 = &TRAIN_DATA_GAMESCREEN;
    memcpy(ptr1 + T_CONGRAT_1_C, T_CONGRAT_1, T_CONGRAT_1_L);
    
    /*Main text for congratulations*/
    ptr1 = &TRAIN_DATA_GAMESCREEN+60;
    memcpy(ptr1 + T_CONGRAT_2_C, T_CONGRAT_2, T_CONGRAT_2_L);
    ptr1 += 20;
    memcpy(ptr1 + T_CONGRAT_3_C, T_CONGRAT_3, T_CONGRAT_3_L);
    ptr1 += 40;
    memcpy(ptr1 + T_CONGRAT_4_C, T_CONGRAT_4, T_CONGRAT_4_L);

    /*Set PMG for scene selection*/
    trainData = &TRAIN_FIGURE_RIGHT;
    setLocoPos(0, 6, 0, 14);
    menuCycleTrainFlag=1;
    audioRequestSongLine(MUSIC_CONGRATULATIONS);
    enableDisplay();

    delay(100);
    swapAllControls();
    memcpy(&TRAIN_DATA_STATUSBARSCREEN + T_PRESS_FIRE_C, T_PRESS_FIRE, T_PRESS_FIRE_L);

    /*Cycle train and wait for FIRE*/
    screenRainbow();
    menuCycleTrainFlag=0;
    audioRequestSongLine(MUSIC_EMPTY);

}

/*The new high score screen*/
void screenNewHighScore() {

    disableDisplay();
    clearStatusBar();
    clearScreen();

    fadeInAndOut();
    pmgSetForLoco();
    
    /*Two separator lines*/
    memset(&TRAIN_DATA_GAMESCREEN+20,CHAR_SEPARATOR,40);
    memset(&TRAIN_DATA_GAMESCREEN+200,CHAR_SEPARATOR,40);
     /*Track*/
    memset(&TRAIN_DATA_GAMESCREEN+160,CHAR_TRACK,40);
    
    /*Display 'Well Done' as screen title*/
    memcpy(&TRAIN_DATA_GAMESCREEN+T_WELL_DONE_C, T_WELL_DONE, T_WELL_DONE_L);

    /*Place new high score*/
    memcpy(&TRAIN_DATA_GAMESCREEN+80+ T_NEW_HISCORE_C, T_NEW_HISCORE, T_NEW_HISCORE_L);
    memcpy(&TRAIN_DATA_GAMESCREEN+100+7, dHighScore, sizeof (dHighScore));
    *(&TRAIN_DATA_GAMESCREEN+100+7+ sizeof (dHighScore)) = 16;
    
    /*Prepare train for cycling*/
    trainData = &TRAIN_FIGURE_RIGHT;
    setLocoPos(0, 6, 0, 14);
    menuCycleTrainFlag=1;
    
    audioRequestSongLine(MUSIC_NEW_HISCORE);
    enableDisplay();

    delay(100);
    swapAllControls();
    memcpy(&TRAIN_DATA_STATUSBARSCREEN + T_PRESS_FIRE_C, T_PRESS_FIRE, T_PRESS_FIRE_L);

    /*Wait for FIRE*/
    while (PEEK(644) == 1);

    menuCycleTrainFlag=0;
    audioRequestSongLine(MUSIC_EMPTY);

}

unsigned char gameToNextLevel() {

    unsigned char i2;
    unsigned char z1;

    /*Display level complete*/
    memset(&TRAIN_DATA_STATUSBARSCREEN + 14, 0, 26);
    memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_LEVEL_COMPLETE_C, T_LEVEL_COMPLETE, T_LEVEL_COMPLETE_L);

    audioRequestSFX(SFX_SCENE_COMPLETE);

    /*Increment score*/
    z1 = gameLives * 10;
    for (i2 = 0; i2 < z1; i2++) {
        incrementScore();
        incrementScore();
        VERTICAL_SYNC(GTNL_1);
    }
    /*And save the score*/
    saveZonedScore();

    /*Check for new life*/
    if (memcmp(dScore, dNextLifeScore, sizeof (dScore)) > 0) {
        if (gameLives < 9) {
            gameLives++;
        }
        incrementNextLifeScore();
    }

    /*If all levels complete, show the congratulations screen*/
    if (gameLevelIndex == CFG_MAX_LEVEL_INDEX) {
        delay(150 - z1);
        return ADVANCE_NEXT_CONGRATS;
    }/*Otherwise proceed to the next level*/
    else {
        gameLevelIndex++;
        /*Update maximum level reached*/
        if (gameLevelIndex > gameMaxLevelIndex) {
            gameMaxLevelIndex = gameLevelIndex;
        }
        delay(150 - z1);
        return ADVANCE_NEXT_CONTINUE;
    }

}

unsigned char gameLoseLife(unsigned char deathType) {

    unsigned char z1;
    unsigned char y1;
    unsigned char i2;
    
    unsigned char glowColor1;
    unsigned char glowColor2;

    /*Clear portion of the staus bar*/
    memset(&TRAIN_DATA_STATUSBARSCREEN + 14, 0, 26);

    if (deathType == DEATH_TYPE_POISON) {
        memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_TRAIN_POISONED_C, T_TRAIN_POISONED, T_TRAIN_POISONED_L);
    } else {
        /*Random message*/
        z1 = PEEK(53770U) & 0x03;
        switch (z1) {
            case 0:
            {
                memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_TRAIN_CRASHED0_C, T_TRAIN_CRASHED0, T_TRAIN_CRASHED0_L);
                break;
            }
            case 1:
            {
                memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_TRAIN_CRASHED1_C, T_TRAIN_CRASHED1, T_TRAIN_CRASHED1_L);
                break;
            }
            case 2:
            {
                memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_TRAIN_CRASHED2_C, T_TRAIN_CRASHED2, T_TRAIN_CRASHED2_L);
                break;
            }
            case 3:
            {
                memcpy(&TRAIN_DATA_STATUSBARSCREEN + 14 + T_TRAIN_CRASHED3_C, T_TRAIN_CRASHED3, T_TRAIN_CRASHED3_L);
                break;
            }
        }
    }
    
    /*Determine the glow style*/
    if (deathType==DEATH_TYPE_CRASH) {
        glowColor1=0xF4;
        glowColor2=0xFF;
        audioRequestSFX(SFX_CRASH);
    }
    /*Otherwise poisoned cargo*/
    else {
        glowColor1=0xB4;
        glowColor2=0xBf;
        audioRequestSFX(SFX_POISONED_CARGO);
    }

    /*Glowing loco phase 1*/
    for (y1 = glowColor1; y1 <glowColor2; y1++) {
        memset((void*) 704U, y1, 4);
        delay(3);
    }

    /*Glowing loco - phase 2*/
    for (y1 = glowColor2; y1 > glowColor1; y1--) {
        memset((void*) 704U, y1, 4);
        delay(2);
    }

    
    /*Hide loco and restore colors*/
    hideLoco();
    pmgSetForLoco();

    
    /*If crashed, then decrease score by 300 (if possible)*/
    if (deathType == DEATH_TYPE_CRASH) {
        if (memcmp(gameScorePtr, d300, ZN_SCORE_LENGTH) > 0) {
            for (i2 = 0; i2 < 30; i2++) {
                verticalSync(1);
                decrementScore();
            }
        } else {
            memset(gameScorePtr, ZN_0, ZN_SCORE_LENGTH);
        }
    }
    /*If poisoned, revert the score back*/
    else {
        memcpy(gameScorePtr,dLevelInitialScore,ZN_SCORE_LENGTH);
    }
    delay(50);

    /*Store the score*/
    saveZonedScore();

    /*Lose life when crashed*/
    if (deathType == DEATH_TYPE_CRASH) {
        if (gameLives > 0) {
            gameLives--;
            return LOSE_LIFE_CONTINUE;
        } else {
            return LOSE_LIFE_GAMEOVER;
        }
    /*If poisoned, then just continue*/    
    } else {
        return LOSE_LIFE_CONTINUE;
    }

}

void decimalDisplaysInit() {
    gameScorePtr = &TRAIN_DATA_STATUSBARSCREEN + 6;
    gameLivesPtr = &TRAIN_DATA_STATUSBARSCREEN + 39;
    gameLevelPtr = &TRAIN_DATA_STATUSBARSCREEN + 24;
    lastScorePtr = &TRAIN_DATA_STATUSBARSCREEN + 11;
    highScorePtr = &TRAIN_DATA_STATUSBARSCREEN + 34;
    resetZonedScore();
}

unsigned char updateMenuScores() {
    memcpy(dLastScore, dScore, sizeof (dLastScore));
    if (memcmp(dLastScore, dHighScore, sizeof (dLastScore)) > 0) {
        memcpy(dHighScore, dLastScore, sizeof (dLastScore));
        return NEW_HISCORE_YES;
    }
    return NEW_HISCORE_NO;
}

void incrementNextLifeScore() {
    if (dNextLifeScore[2] == ZN_5) {
        dNextLifeScore[2] = ZN_0;
        if (dNextLifeScore[1] == ZN_9) {
            dNextLifeScore[1] = ZN_0;
            ++dNextLifeScore[0];
        } else {
            ++dNextLifeScore[1];
        }
    } else {
        dNextLifeScore[2] = ZN_5;
    }
}

/* Enable text mode display*/
void enableDisplay() {
    POKE(559, 62);
}

void disableDisplay() {
    POKE(559, 0);
}

void swapAllControls() {
    POKE(764U, 255);
    delay(1);
    while (!PEEK(644) || PEEK(53279U) != 7 || PEEK(632) != JS_C);
}

void setGameSpeed(unsigned char speed) {
    if (get_tv() == AT_PAL) {
        normalMoveDelay = PALSpeeds[speed];

    } else {
        normalMoveDelay = NTSCSpeeds[speed];
    }
    
}

void toggleGameInit(unsigned char gameInitType) {

    if (gameInitType == GAME_INIT_NORMAL) {

        gameInitLives = 3;
        memset(dGameInitScore, ZN_0, sizeof (dGameInitScore));
        memcpy(dGameInitNextLifeScore, d5000, sizeof (dGameInitScore));

    }
#ifdef CFG_ALLOW_CHEATS            
    else {
        gameInitLives = 8;
        memcpy(dGameInitScore, d68000, sizeof (dGameInitScore));
        memcpy(dGameInitNextLifeScore, d70000, sizeof (dGameInitScore));
    }
#endif    
}

void fadeInAndOut() {

    unsigned char i1;

    for (i1 = 0; i1 < 16; i1 += 2) {
        POKE(712, i1);
        verticalSync(3);
    }
    for (i1 = 0; i1 < 16; i1 += 2) {
        POKE(712, 14 - i1);
        verticalSync(3);
    }
}


void verticalSync(unsigned char count) {
    int i=0;
    for(i=0;i<count;i++) {
        VERTICAL_SYNC(VSYNC_0);
    }
    
}



/*When a level is over or a life is lost, fade out everything*/
void levelFadeOut() {
    
    unsigned char i,k;
    unsigned char* ptr;
    unsigned char count;
    
    verticalSync(1);
    
    for(i=0;i<16;i++) {
        
        count=0;
        ptr=(unsigned char*)704U;
        
        for(k=0;k<9;k++) {
            if (*ptr>((*ptr)&0xF0) ) {
                *ptr-=2;
            }
            else {
                count++;
            }
            ptr++;
        }
        if (count==9) break;
        verticalSync(3);
    }
    
}

/*When a level is about to start, fade it in 
  We have desired colors in the shadow registers, we will use them*/
void levelFadeIn() {
    
    unsigned char i,k,c;
    
    unsigned char desiredColors[9];
    unsigned char initialColors[5];
    
    /*Create array of desired colors, PMG + Standard graphics*/
    memcpy(desiredColors,(unsigned char*)704U,9);
    
    /*Set initial color array for standard graphis*/
    for(i=0;i<5;i++) {
        initialColors[i]=desiredColors[i+4]&0xF0;
    }
    
    /*Set registers for the initial colors*/
    memcpy((unsigned char*)708U,initialColors,5);
    memset((unsigned char*)704U,0,4);
    
    /*Synchronize*/
    verticalSync(1);
    
    /*Show display*/
    enableDisplay();
    
    /*Start fading in the standard graphics*/ 
    for (i=0;i<16;i++) {
        
        c=0;
        for(k=0;k<5;k++) {
            if (initialColors[k]<desiredColors[k+4]) {
                ++initialColors[k];
                POKE(708U+k,initialColors[k]);
            }
            else {
                c++;
            }
        }
        if (c==5) break;
        verticalSync(3);
    }
    
   /*Then suddenly display the loco, so it is obvious the game can begin*/
   memcpy((unsigned char*)704U,desiredColors,4);
    
}