;===============================================================================
; Train 3 Open Source Edition
; Game data
;===============================================================================


;-------------------------------------------------------------------------------
; Export symbols, so the symbols are visible for the main program
;-------------------------------------------------------------------------------
.export  _TRAIN_DATA_CHARSET1
.export  _TRAIN_DATA_CHARSET1_PAGE
.export  _TRAIN_DATA_CHARSET2
.export  _TRAIN_DATA_CHARSET2_PAGE
.export  _TRAIN_DATA_DL_MENU
.export  _TRAIN_DATA_DL_GAME
.export  _TRAIN_DATA_DL_INTERMISSION
.export  _TRAIN_DATA_DL_SCENESELECTION
.export  _TRAIN_DATA_PMGAREA
.export  _TRAIN_DATA_GAMESCREEN
.export  _TRAIN_DATA_STATUSBARSCREEN
.export  _TRAIN_DATA_DL_MENU_SCROLL_LMS
.export  _TRAIN_DATA_SCROLLER

.export  _TRAIN_FIGURE_RIGHT
.export  _TRAIN_FIGURE_LEFT
.export  _TRAIN_FIGURE_UP
.export  _TRAIN_FIGURE_DOWN

.export  _MENU_TRAIN_TITLE
.export  _TRAIN_GAME_LEVELS
.export  _TRAIN_GAME_ELEMENTS

;-------------------------------------------------------------------------------
; Character sets 
; Character sets must be aligned at page boudnary
;-------------------------------------------------------------------------------
; Character set 1 - Level plane elements
.segment "TRAIN_SEG_CHARSET1"
_TRAIN_DATA_CHARSET1:
.incbin "data/fonts/trainfont1.fnt"
_TRAIN_DATA_CHARSET1_END:
.segment "CODE"
_TRAIN_DATA_CHARSET1_PAGE: .byte >(_TRAIN_DATA_CHARSET1)

;Character set 2 - Status bar
.segment "TRAIN_SEG_CHARSET2"
_TRAIN_DATA_CHARSET2:
.incbin "data/fonts/trainfont2.fnt"
_TRAIN_DATA_CHARSET2_END:
.segment "CODE"
_TRAIN_DATA_CHARSET2_PAGE: .byte >(_TRAIN_DATA_CHARSET2)

;===============================================================================
; Display lists
;===============================================================================

;-------------------------------------------------------------------------------
; Display list for menu
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_DL"
_TRAIN_DATA_DL_MENU:
.byte 112,112
.byte 112+128                 ; Blank + DLI for separator colors
.byte 4+64                    ; Separator line with LMS                       1      
.word _TRAIN_DATA_GAMESCREEN  ; Screen memory
.byte 32+128                  ; Three blank lines + DLI for main title          
.byte 4,4,4,4,4,0+128,4,4,4,4   ; 8 lines for main title, DLI for more colors 9 
.byte 32+128                  ; Three blank lines + DLI for subtitle colors
.byte 2                       ; 1 line for subtitle                           1
.byte 32+128                  ; Three blank lines + DLI for separator colors
.byte 4                       ; Separator line                                1
.byte 80+128                 ; More blank lines + DLI for menu option colors
.byte 2                       ; Menu line - Start game                        1 
.byte 16                      ; 2 blank lines
.byte 2                       ; Menu line - Game Speed: xxxx                  1 
.byte 16                      ; 2 blank lines
.byte 2                       ; Menu line - In-Game Audio:                    1      
.byte 112,112,16+128          ; More blank lines between menu items and bar
.byte 2+64+16                 ; Scrolling area                                1
_TRAIN_DATA_DL_MENU_SCROLL_LMS:
.word _TRAIN_DATA_SCROLLER    ; Scrolling screen memory
.byte 32+128                  ; Three blank lines + DLI for status bar
.byte 2+64                    ; One line for status bar (scores)              1
.word _TRAIN_DATA_STATUSBARSCREEN
.byte 65                      ; JVB
.word _TRAIN_DATA_DL_MENU     ; DL jump                                      17
_TRAIN_DATA_DL_MENU_END:

_TRAIN_DATA_DL_GAME:
.byte 112 ,112                ; Blank
.byte 068                     ; LMS for GR.12
.word _TRAIN_DATA_GAMESCREEN  ; Screen memory
.byte 004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004 ,004
.byte 004 ,004 ,004 ,004 ,004 ,004, 004, 004
.byte 16+128                  ; Blank + DLI

.byte 066                          ; LMS for GR.0
.word _TRAIN_DATA_STATUSBARSCREEN  ; Status bar screen memory
.byte 065                          ; JVB
.byte <_TRAIN_DATA_DL_GAME,>_TRAIN_DATA_DL_GAME  ;DL Jump
_TRAIN_DATA_DL_GAME_END:

;Intermission screen -  Game over, game end, and well done
;Primary color COLPF0 - Capital letters and numbers
_TRAIN_DATA_DL_INTERMISSION:
.byte 112,112
.byte 112+128                 ;Blank + DLI for GR.2 title line
.byte 64+7                    ;LMS for graphics 2                           (0)
.word _TRAIN_DATA_GAMESCREEN  ;Screen memory
.byte 32+128                  ;Three blank lines + DLI for separator colors
.byte 4                       ;Separator line                               (20)
.byte 32+128                  ;Three blank lines + DLI for main area colors
.byte 7,7,7,7,7               ;Graphics 2, 5 lines                          (60,80,100,120,140)
.byte 0+128                   ;Three blank lines + DLI for track
.byte 0
.byte 0
.byte 4                       ;Track                                        (160)
.byte 16+128                  ;Two blank lines + DLI for separator colors   
.byte 4                       ;Separator                                    (200)
.byte 0+128                   ;1 blank line + DLI for status bar
.byte 0                       ;One more blank line 
.byte 0                       ;One more blank line
.byte 2+64                    ;Status bar line + LMS
.word _TRAIN_DATA_STATUSBARSCREEN
.byte 065                     ;JVB
.word _TRAIN_DATA_DL_INTERMISSION


;Scene selection screen
;Primary color COLPF0 - Capital letters and numbers
_TRAIN_DATA_DL_SCENESELECTION:
.byte 112,112                 ;Blank lines    
.byte 112+128                 ;Blank line + DLI for title line
.byte 64+7                    ;LMS for graphics 2                        (0)
.word _TRAIN_DATA_GAMESCREEN  ;Screen memory
.byte 16+128                  ;Two blanks + DLI for separator 1
.byte 4                       ;Separator                                 (20)
.byte 16+128                  ;Two blanks + DLI for main contents
.byte 7,16,7,16,7,16,7,16,7,16;5 lines graphics 2 interleaved          (60)
.byte 0+128                   ;1 blank line for fake DLI for track
.byte 0+128                   ;1 blank line + DLI for separator 2
.byte 4                       ;Separator                                 (260)
.byte 32+128                  ;3 blank lines + DLI for status bar
.byte 64+2                    ;LMS for graphics 0
.word _TRAIN_DATA_STATUSBARSCREEN
.byte 065
.word _TRAIN_DATA_DL_SCENESELECTION

;-------------------------------------------------------------------------------
; Raster music tracker song - The song is a binary file
; It will be included with section headers
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_RMTSONG"
_TRAIN_DATA_RMTSONG:
.incbin "data/music/train3_stripped.rmt"

;-------------------------------------------------------------------------------
; Raster music tracker player - The player is a binary file
; It will be included with section headers
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_RMTPLAYER"
.incbin "data/music/rmtplayr.obx"

;-------------------------------------------------------------------------------
; PMG Area (dummy segment, not written to the binary file)
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_PMGAREA"
_TRAIN_DATA_PMGAREA:

;-------------------------------------------------------------------------------
; Cave display memory and status bar (dummy segment, not written to the binary)
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_GAMESCREEN"
_TRAIN_DATA_GAMESCREEN:

.segment "TRAIN_SEG_STATUSBARSCREEN"
_TRAIN_DATA_STATUSBARSCREEN:

;-------------------------------------------------------------------------------
; Scrolling text. 
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_SCROLLER"
_TRAIN_DATA_SCROLLER:
.repeat 40
.byte 0
.endrepeat
.byte 0,52,50,33,41,46,0,19,0,47,112,101,110,0,51,111,117,114,99,101,0,37,100,105,116,105,111,110,0,10,0,41,100,101,97,12,0,45,117,115,105,99,12,0,44,101,118,101,108,115,12,0,51,38,56,0,97,110,100,0,39,38,56,0,98,121,0,48,101,116,114,0,48,111,115,116,97,118,97,0,8,40,97,114,100,35,111,114,101,9,0,10,0,35,111,100,101,100,0,105,110,0,35,35,22,21,0,98,121,0,45,105,99,104,97,101,108,0,43,97,108,111,117,65,0,8,34,97,107,116,114,97,9,0,10,0,54,105,115,105,116,0,104,116,116,112,26,15,15,98,97,104,97,14,119,101,98,111,119,110,97,14,99,122,15,0,10
.repeat 60
.byte 0
.endrepeat
;-------------------------------------------------------------------------------
; Train figures
;-------------------------------------------------------------------------------
.segment "CODE"
_TRAIN_FIGURE_RIGHT:
.byte  240,000,000,000,000,000,000,000,000,000,000,000,090,153,231,090  ;Red
.byte  000,000,000,000,000,000,000,000,000,000,000,000,000,102,024,000  ;Yellow
.byte  006,240,144,150,150,254,255,255,255,254,000,000,000,000,000,000  ;LightB
.byte  000,006,006,000,001,001,000,000,000,001,255,254,000,000,000,000  ;DarkB
_TRAIN_FIGURE_LEFT:
.byte  015,000,000,000,000,000,000,000,000,000,000,000,090,153,231,090  ;Red
.byte  000,000,000,000,000,000,000,000,000,000,000,000,000,102,024,000  ;Yellow
.byte  096,015,009,105,105,127,255,255,255,127,000,000,000,000,000,000  ;LightB
.byte  000,096,096,000,128,128,000,000,000,128,255,127,000,000,000,000  ;DarkB
_TRAIN_FIGURE_UP:
.byte  002,003,001,000,000,000,000,000,001,001,128,128,128,129,129,130  ;Red
.byte  000,000,002,002,002,002,002,002,002,002,002,002,002,002,002,000  ;Yellow
.byte  016,056,184,184,184,184,024,024,024,120,024,024,024,024,024,120  ;LightB
.byte  040,004,068,068,068,068,004,004,004,004,004,004,004,004,004,004  ;DarkB
_TRAIN_FIGURE_DOWN:
.byte  065,129,129,001,001,001,001,128,128,000,000,000,000,128,192,064  ;Red
.byte  000,064,064,064,064,064,064,064,064,064,064,064,064,064,000,000  ;Yellow
.byte  030,024,024,024,024,024,030,024,024,024,029,029,029,029,028,008  ;LightB
.byte  032,032,032,032,032,032,032,032,032,032,034,034,034,034,032,020  ;DarkB

;-------------------------------------------------------------------------------
; TRAIN TITLE SCREEN
;-------------------------------------------------------------------------------
_MENU_TRAIN_TITLE:
.byte 000,216,216,216,216,216,216,000,218,216,216,216,216,219,000,218,216,216,216,216,219,000,216,216,000,218,216,216,216,216,219,000,000,218,216,216,216,216,219
.byte 000,216,216,216,216,216,216,000,216,216,216,216,216,216,000,216,216,216,216,216,216,000,216,216,000,216,216,216,216,216,216,000,000,216,216,216,216,216,216
.byte 000,000,000,216,216,000,000,000,216,216,000,000,216,216,000,216,216,000,000,216,216,000,216,216,000,216,216,000,000,216,216,000,000,000,000,000,000,216,216
.byte 000,000,000,216,216,000,000,000,216,216,000,000,216,216,000,216,216,000,000,216,216,000,216,216,000,216,216,000,000,216,216,000,000,000,000,000,000,216,216
.byte 000,000,000,216,216,000,000,000,216,216,216,216,216,221,000,216,216,216,216,216,216,000,216,216,000,216,216,000,000,216,216,000,000,216,216,216,216,216,216 
.byte 000,000,000,216,216,000,000,000,216,216,216,216,219,000,000,216,216,216,216,216,216,000,216,216,000,216,216,000,000,216,216,000,000,216,216,216,216,216,216
.byte 000,000,000,216,216,000,000,000,216,216,000,216,216,000,000,216,216,000,000,216,216,000,216,216,000,216,216,000,000,216,216,000,000,000,000,000,000,216,216
.byte 000,000,000,216,216,000,000,000,216,216,000,216,216,000,000,216,216,000,000,216,216,000,216,216,000,216,216,000,000,216,216,000,000,216,216,216,216,216,216
.byte 000,000,000,216,216,000,000,000,216,216,000,216,216,219,000,216,216,000,000,216,216,000,216,216,000,216,216,000,000,216,216,000,000,220,216,216,216,216,221


;-------------------------------------------------------------------------------
; Game levels
;-------------------------------------------------------------------------------
.byte "@LE"
_TRAIN_GAME_LEVELS:
.incbin "data/levels/levels.dat"

;-------------------------------------------------------------------------------
; Game elements (64*33)
;-------------------------------------------------------------------------------
.byte "@EL"
_TRAIN_GAME_ELEMENTS:
.incbin "data/levels/elements.dat"
_TRAIN_GAME_ELEMENTS_END:

;Pad to 64 * 33
.repeat (64*33-(_TRAIN_GAME_ELEMENTS_END-_TRAIN_GAME_ELEMENTS))
.byte 000
.endrepeat
