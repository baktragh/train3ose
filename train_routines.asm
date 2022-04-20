;===============================================================================
; Train 3 Open Source Edition
; Assembler routines
;===============================================================================
.include "atari.inc"

;Import symbols
.import _TRAIN_DATA_CHARSET2_PAGE
.import _TRAIN_DATA_STATUSBARSCREEN
.import _p0x
.import _menuCycleTrainFlag
.import _menuScrollTextFlag
.import _menuGameAudio
.import _TRAIN_DATA_DL_MENU_SCROLL_LMS
.import _TRAIN_DATA_SCROLLER

;Variables
.segment "DATA"
;VBI
_vbistorel:             .byte 0                 ;Original VBI routine LO
_vbistoreh:             .byte 0                 ;Original VBI routine HI 

;Audio       
_sfxRequested:          .byte 0                 ;SFX requested (high level)
_songLineRequested:     .byte 0                 ;Song line requested 
_llsfx:                 .byte 0                 ;Low level SFX (instrument nr)
_inGameAudioFlag:       .byte 0                 ;In-game audio indication

;Controls
_filteredJS:            .byte 0                 ;Filtered JS value

;Visuals
_statusBarForeground:   .byte 0                 ;Status bar foreground for DLI
_statusBarBackground:   .byte 0                 ;Status bar background for DLI
_menuDliCount:          .byte 0                 ;Counter to control DLI

;Menu scroller 
menu_scroll_limit=163+36                        ;Maximum number for counter
menu_scroll_maxhsc=3                            ;Maximum HSCROLL value

_menu_scroll_hscroll:     .byte menu_scroll_maxhsc   ;Antic scroll counter
_menu_scroll_scrollCount: .word 0                    ;Scroller text counter
                         


.segment "ZEROPAGE"
_zp_ptr1:   .word 0
_zp_ptr2:   .word 0
_zp_ptr3:   .word 0
_zp_x1:     .byte 0
_zp_z1:     .byte 0


;==============================================================================
; DLI Handlers
;==============================================================================

;------------------------------------------------------------------------------
;DLI handler 1 - Colors and character set for the status bar
;------------------------------------------------------------------------------
.segment "CODE"
_dliHandler:
	pha
	sta WSYNC	         ;Horizontal retrace
	lda _TRAIN_DATA_CHARSET2_PAGE
        sta CHBASE
	lda _statusBarBackground ;Status bar background
	sta COLPF2
	lda _statusBarForeground ;
	sta COLPF1	         ;Status bar foreground
	pla
	rti
;------------------------------------------------------------------------------
;DLI handler 2- Handle DLI for menus
; set colors for separator
; set colors for main title
; set colors for subtitle
; set colors for separator
; set colors for menu items
; set colors for the scroller
; set colors for the status bar
;------------------------------------------------------------------------------

_dliMenuHandler:
        pha
        
        lda _menuDliCount         ;Check where we are
        beq dli_menu_sep                 

        cmp #1                   ;For the main title
        beq dli_menu_title

        cmp #2                   ;For the main title, 2
        beq dli_menu_title2

        cmp #3
        beq dli_menu_subtitle       ;For the subtitle (darker gray)

        cmp #4                   ;For the second separator
        beq dli_menu_sep                

        cmp #5                   ;For the menu items
        beq dli_menu_items

        cmp #6                   ;For the scroller
        beq dli_menu_scroll

        cmp #7                   ;For the status bar
        jmp dli_menu_sbar


dli_menu_title:
        lda #$EA                        ;Set colors for main title part 1
        sta WSYNC
        sta COLPF1
        lda #$EC
        sta COLPF3
        inc _menuDliCount
        pla
        rti

dli_menu_title2:
        lda #$EC                         ;Set colors for main title part 2
        sta WSYNC
        sta COLPF1
        lda #$EE
        sta COLPF3
        inc _menuDliCount
        pla
        rti

dli_menu_sep:                            ;Set colors for separator (blue)
        lda #$72
        sta WSYNC
        sta COLPF0
        lda #$74
        sta COLPF1
        lda #$76
        sta COLPF2
        lda #$78
        sta COLPF3
        inc _menuDliCount
        pla 
        rti

dli_menu_subtitle:                       ;Set colors for subtitle (gray)
        lda #8
        sta WSYNC
        sta COLPF1
        lda #0
        sta COLPF2
        inc _menuDliCount
        pla 
        rti


dli_menu_items:                          ;Set colors for menu title
        lda #14
        sta WSYNC
        sta COLPF1
        lda #0
        sta COLPF2
        inc _menuDliCount
        pla 
        rti

dli_menu_scroll:                         ;Set colors for the scroller
        lda #0
        sta WSYNC
        sta COLPF2
        lda #8
        sta COLPF1
        lda #12
        sta WSYNC
        sta WSYNC
        sta WSYNC
        sta COLPF1
        inc _menuDliCount
        pla
        rti

dli_menu_sbar:                           ;Set colors for the status bar
        lda _statusBarBackground ;Status bar background
        sta WSYNC                ;Synchronize
	sta COLPF2
	lda _statusBarForeground ;
	sta COLPF1	         ;Status bar foreground
        inc _menuDliCount
        pla
        rti


;===============================================================================
; DLI handler for intermission screen
; We will reuse some code for the menu
; 0. Set colors for screen title in graphics 2
; 1. Set colors for separator
; 2. Set colors for main contents in graphics 2
; 3. Set colors for train track
; 4. Set colors for second separator
; 5. Set colors for status bar
;===============================================================================
_dliInterMission:
        pha
        txa
        pha
        
        lda _menuDliCount        ;Check where we are
        asl                      ;Double
        tax                      ;Transfer to x
        lda dli_jump_table,x
        sta @djmp+1
        lda dli_jump_table+1,x
        sta @djmp+2
        
@djmp:  jmp $1000

dli_inter_title:
        sta WSYNC
        lda #180
        sta COLPF0
        inc _menuDliCount
        pla
        tax
        pla
        rti

dli_inter_main:
        sta WSYNC
        lda #$CA
        sta COLPF0
        lda #$EA
        sta COLPF1
        lda #$0F
        sta COLPF2
        sta COLPF3
        inc _menuDliCount
        pla
        tax
        pla
        rti

dli_inter_subtitle:
        lda #8
        sta WSYNC
        sta COLPF1
        lda #0
        sta COLPF2
        inc _menuDliCount
        pla
        tax
        pla 
        rti

dli_inter_sep:                            ;Set colors for separator (blue)
        lda #$72
        sta WSYNC
        sta COLPF0
        lda #$74
        sta COLPF1
        lda #$76
        sta COLPF2
        lda #$78
        sta COLPF3
        inc _menuDliCount
        pla 
        tax
        pla
        rti

dli_inter_sbar:                  ;Set colors for the status bar
        lda #0
        sta WSYNC                ;Synchronize
	sta COLPF2
	lda #14
	sta COLPF1	         ;Status bar foreground
        inc _menuDliCount
        pla
        tax
        pla
        rti

;-------------------------------------------------------------------------------
; DLI jump table
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_DLI_JMPTAB"
dli_jump_table:
       .word dli_inter_title
       .word dli_inter_sep
       .word dli_inter_main
       .word dli_inter_subtitle
       .word dli_inter_sep
       .word dli_inter_sbar

;===============================================================================
; Universal VBI Routine
; Play menu music
; Play ingame music (when enabled)
; Play ingame SFX (when enabled)
; Allow switching between music and SFX
; Train cycling (when enabled)
; Resetting DLI counter
;===============================================================================

; RMT addresses
vRMT_BASE = $8300
vRMT_SFX  = vRMT_BASE+15
vRMT_UPDATE = vRMT_BASE+3 
vRMT_STOP = vRMT_BASE+9
vRMT_SETPOKEY = vRMT_BASE+12

; RMT Music address
aRMT_MUSIC = $8A00

; Some songlines
cMSX_EMPTY_LINE = $49
cMSX_INGAME_LINE = $00

; SFX equates
;High level requests
cSFX_TRAIN=1 
cSFX_TREASURE=2
cSFX_CRASH=4
cSFX_SCENE_COMPLETE=8
cSFX_POISONED_TREASURE=16

;Instrument numbers (doubled)
rSFX_TRAIN=$15*2
rSFX_TREASURE=$16*2
rSFX_CRASH=$17*2
rSFX_SCENE_COMPLETE=$18*2
rSFX_POISONED_TREASURE=$19*2


.segment "CODE"
_vbiUniversalRoutine:
;-------------------------------------------------------------------------------
; Clear display list counter
;-------------------------------------------------------------------------------
        lda #0                  ;Clear DLI counter for all screens
        sta _menuDliCount
;-------------------------------------------------------------------------------
;Handle SFX if enabled
;-------------------------------------------------------------------------------
        lda _menuGameAudio      ;Check game audio (0 SFX, 1 BGM)
        bne @nosfx              ;SFX not enabled, skip it.   
        
        ;Check if an SFX is requested
        lda _sfxRequested
        beq @nosfx

;Determine what SFX
        ;Check if train sound
        cmp #cSFX_TRAIN
        beq @train
        
        cmp #cSFX_TREASURE
        beq @treasure

        cmp #cSFX_CRASH
        beq @crash

        cmp #cSFX_POISONED_TREASURE
        beq @poisoned
        
        cmp #cSFX_SCENE_COMPLETE
        beq @scene

;Handle crash
@crash: ldy #rSFX_CRASH
        bne @t2

@poisoned: ldy #rSFX_POISONED_TREASURE
        bne @t2

;Handle scene complete
@scene: ldy #rSFX_SCENE_COMPLETE
        ldx #1
        lda #30
        sty _llsfx
        jsr vRMT_SFX
        ldy _llsfx
        lda #30
        ldx #5
        jsr vRMT_SFX
        jmp @ready

;Handle treasure (combine with train sound)
@treasure:
        ldx #1          ;Channel 0+1
        lda #53         ;Note 30
        ldy #rSFX_TREASURE   ;SFX number
        sty _llsfx      
        jsr vRMT_SFX    ;Tell RMT
        ldy _llsfx
        lda #53
        ldx #5          ;Channel 4+1
        jsr vRMT_SFX

;Handle train sound
@train: ldy #rSFX_TRAIN  ;SFX number
@t2:    sty _llsfx
        ldx #3          ;Channel 0+3
	lda #30         ;Note 30
	jsr vRMT_SFX    ;Tell RMT
        lda #30
        ldy _llsfx
        ldx #7          ;Channel 4+3
        jsr vRMT_SFX    ;Tell RMT

;Reset requested SFX
@ready: lda #0
	sta _sfxRequested
	
;-------------------------------------------------------------------------------
; Handle any updates in music
;-------------------------------------------------------------------------------
@nosfx: lda _songLineRequested      ;Check request for song line
        cmp #255                    ;$FF Indicates no request
        beq @update                 ;Skip if no request

        ldx #<aRMT_MUSIC            ;Request playing a songline
	ldy #>aRMT_MUSIC
	;Initialize the tracker
	jsr vRMT_BASE
        lda #255
        sta _songLineRequested      ;Clear the request
;-------------------------------------------------------------------------------
; Handle RMT player update
;-------------------------------------------------------------------------------
@update: 
	;Music update - Call RMT	
	jsr vRMT_UPDATE	
;-------------------------------------------------------------------------------
; Cycle the train when requested
;-------------------------------------------------------------------------------
        ;Cycle the train if requested
        lda _menuCycleTrainFlag
        bne @cycledo
        jmp @checkToggle
@cycledo:
        ;Check current position of the train
        lda _p0x
        cmp #208
        bne @cycleInc   ;If not 200, then move to the right
        lda #32         ;Otherwise reset
        sta _p0x        ;Store new coordinate
        bne @cycleDone  ;And set GTIA registers (always jumps)
@cycleInc:
        inc _p0x        ;Just increment the position
        lda _p0x        ;And keep it

@cycleDone:             ;Set GTIA registers
        sta HPOSP0
        sta HPOSP1
        sta HPOSP2
        sta HPOSP3

;-------------------------------------------------------------------------------
; Scroll the text
;-------------------------------------------------------------------------------
@scroll:
        lda _menuScrollTextFlag ;Are we scrolling ?
        beq @checkToggle        ;No, skip all this

        cmp #2                  ;Are we resetting?
        bne @scroll_cont        ;No we are continuing

        ldy #menu_scroll_maxhsc      ;Set default scroll value
        sty _menu_scroll_hscroll
        tay                     ;Set flag to normal one
        dey
        sty _menuScrollTextFlag 
        jmp @scroll_reset       ;And continue to scroll reset

@scroll_cont:

        ldy _menu_scroll_hscroll       ;Is scrolling counter 0
        beq @lmsinc        ;Yes, change LMS
        dey                ;Decrement counter
        sty _menu_scroll_hscroll       ;Store new value
        sty HSCROL         ;To the scrolling register     
        jmp @no_lms   ;Skip LMS change

@lmsinc:
        ldy #menu_scroll_maxhsc        ;Reset value
        sty _menu_scroll_hscroll
        sty HSCROL 
         
        lda _menu_scroll_scrollCount+1 ;Check if scrolled enough
        cmp #>menu_scroll_limit
        bne @inclms
        lda _menu_scroll_scrollCount
        cmp #<menu_scroll_limit
        bne @inclms

@scroll_reset:
        lda #<_TRAIN_DATA_SCROLLER
        sta _TRAIN_DATA_DL_MENU_SCROLL_LMS
        lda #>_TRAIN_DATA_SCROLLER
        sta _TRAIN_DATA_DL_MENU_SCROLL_LMS+1
        lda #0
        sta _menu_scroll_scrollCount
        sta _menu_scroll_scrollCount+1
        jmp @sc_nohi
    
@inclms:      
        inc _TRAIN_DATA_DL_MENU_SCROLL_LMS
        bne @lmsinc_nohi
        inc _TRAIN_DATA_DL_MENU_SCROLL_LMS+1
@lmsinc_nohi:
        inc _menu_scroll_scrollCount
        bne @sc_nohi
        inc _menu_scroll_scrollCount+1
@sc_nohi:
@no_lms: 


;-------------------------------------------------------------------------------
; Check toggling of BGM/SFX
;-------------------------------------------------------------------------------
@checkToggle:
        lda CONSOL              ;A console key pressed (StSeOp) ?
        cmp #7                  ;No, skip
        beq @callOriginal

        lda _inGameAudioFlag    ;Are we in-game?
        beq @callOriginal       ;No, skip

@checkOption:                   ;Option = switch to BGM
        lda CONSOL              ;Re-check
        cmp #3                  ;Option pressed
        bne @checkSelect        ;No, skip
        
        lda #cMSX_INGAME_LINE    
        sta _songLineRequested
        lda #1
        sta _menuGameAudio
        lda #0
        sta _sfxRequested
        beq @callOriginal

@checkSelect:
        cmp #5
        bne @callOriginal
        lda #cMSX_EMPTY_LINE
        sta _songLineRequested
        lda #0
        sta _menuGameAudio
        sta _sfxRequested

;-------------------------------------------------------------------------------        
;Call the original VBI routine
;-------------------------------------------------------------------------------
@callOriginal:
	jmp (_vbistorel)

;===============================================================================
; Set-up the universal VBI routone
; Do it via proper OS call
;===============================================================================
.segment "CODE"

.proc _rmtSetUniversalVBI: near
.segment "CODE"
	;store original vbi address
	lda 548
	sta _vbistorel
	lda 549
	sta _vbistoreh
	;Set new 
	ldx #>_vbiUniversalRoutine
	ldy #<_vbiUniversalRoutine
	lda #7
	jsr $e45c
	
	rts
.endproc

;Restore VBI original routine
.segment "CODE"

;===============================================================================
; Restore original VBI routine
;===============================================================================
.proc _rmtRestoreOriginalVBI: near
.segment "CODE"
	;store original vbi address
	ldy _vbistorel
	ldx _vbistoreh
	lda #7
	jsr $e45c
	rts
.endproc

;===============================================================================
; Filtered joystick input. Either center or non-diagonal direction
;===============================================================================
        JS_LEFT =11
        JS_RIGHT =7
        JS_UP =14
        JS_DOWN =13
        JS_C =15

.segment "CODE"
.proc _getFilteredJS
        lda #JS_C
        ldx 632
        cpx #JS_LEFT
        beq @gotjs
        cpx #JS_RIGHT
        beq @gotjs
        cpx #JS_UP
        beq @gotjs
        cpx #JS_DOWN
        beq @gotjs
        sta _filteredJS
        rts
@gotjs: stx _filteredJS
        rts
.endproc


;===============================================================================
; Score increment.
; This one is written in assembler, because the routine
; must be fast. Simple zoned decimal increment.
;===============================================================================

.segment "CODE"
.proc _incrementScore: near
	scoreOffset = 6
	scoreMaxIndex = 4
	pha
	ldx #scoreMaxIndex
@loop:  lda _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X	;Get zoned digit
        cmp #(16+9)                                     ;Is that nine?
        bne @not_9                                      ;No, just increment
        lda #16                                         ;Set to 0
        sta _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X
        cpx #0                                          ;All digits?
	beq @quit                                       ;Yes, what to do?
        dex                                             ;Next digit
        jmp @loop                                       ;Do it again

@not_9:  inc _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X
@quit:	pla
	rts

.endproc

;===============================================================================
; Score decrement.
; This one is written in assembler, because the routine
; must be fast. Simple zoned decimal increment.
;===============================================================================

.segment "CODE"
.proc _decrementScore: near
	scoreOffset = 6
	scoreMaxIndex = 4
	pha
	ldx #scoreMaxIndex
@loop:  lda _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X	;Get zoned digit
        cmp #(16)                                       ;Is that zero?
        bne @not_0                                      ;No, just decrement
        lda #(16+9)                                     ;Set to 9
        sta _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X
        cpx #0                                          ;All digits?
	beq @quit                                       ;Yes, what to do?
        dex                                             ;Next digit
        jmp @loop                                       ;Do it again

@not_0: dec _TRAIN_DATA_STATUSBARSCREEN+scoreOffset,X
@quit:	pla
	rts

.endproc

;===============================================================================
; Preinitialization - code executed when loading the binary load file
; Preinitialization has two parts. First one disables BASIC and display. Second
; part displays the loading text. This prevents showing garbage on the screen.
;===============================================================================

;-------------------------------------------------------------------------------
; Initialization - part 1 - Disable basic and set screen attributes
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_PREINIT_HEADER"
        .byte <train_preinit,>train_preinit
        .byte <(train_preinit_end-1),>(train_preinit_end-1)

.segment "TRAIN_SEG_PREINIT"
train_preinit:

        lda #0               ;No screen
        sta SDMCTL

        ldx 20               ;Vertical sync
@w1:    cpx 20
        beq @w1	

        sta SOUNDR           ;No noisy I/O
        sta COLOR4           ;Black background
        sta COLBK

        lda #$EA             ;Bright text
        sta COLOR0
        lda #$76             ;Darker lines
        sta COLOR1              

        ldx #0               ;Close E: device
        lda #CLOSE
        sta ICCOM,X
        jsr CIOV

        lda PORTB            ;Disable BASIC ROM
        ora #2
        sta PORTB
        lda #1               ;Set basic flag
        sta BASICF
        lda #192             ;Set new RAMTOP and RAMSIZ
        sta RAMTOP
        sta RAMSIZ 
        sta MEMTOP+1
        lda #0
        sta MEMTOP
        rts

train_preinit_end:

        .word INITAD
        .word INITAD+1
train_preinit_ivector:
        .byte <train_preinit,>train_preinit    

;-------------------------------------------------------------------------------
; Display list and screen memory for "LOADING" screen
; This must be loaded AFTER the initialization of the first part
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_LD_SCREEN_HEADER"
        .byte <train_ld_screen,>train_ld_screen
        .byte <train_ld_screen_end-1,>(train_ld_screen_end-1)

.segment "TRAIN_SEG_LD_SCREEN"
train_ld_screen:
        .byte 112,112,112   ;Three blanks
        .byte 112,112,112,112
        .byte 64+6          ;LMS GR.1
        .byte <train_ld_sep_line,>train_ld_sep_line  ;Line separator
        .byte 32            ;Three blank scanlines
        .byte 64+7          ;LMS GR.2
        .byte <train_ld_line,>train_ld_line          ;Train
        .byte 0             ;Single blank lines                   
        .byte 6             ;GR.1 for company name
        .byte 6             ;GR.1 for separator
        .byte 65
        .byte <train_ld_screen,>train_ld_screen

train_ld_line:
        .byte 52,50,33,41,46,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0
        .byte 34,33,40,33,0,51,47,38,52,55,33,50,37,0,0,0,0,0,0,0
train_ld_sep_line:
        .repeat 20
        .byte 63+64  
        .endrepeat
 
train_ld_screen_end:  

;-------------------------------------------------------------------------------
; Initialization - part 2 - Show Loading Screen
;-------------------------------------------------------------------------------
.segment "TRAIN_SEG_PREINIT2_HEADER"
        .byte <train_preinit2,>train_preinit2
        .byte <(train_preinit2_end-1),>(train_preinit2_end-1)

.segment "TRAIN_SEG_PREINIT2"
train_preinit2:

        jsr train_preinit2_vsync
        lda #0               ;No screen
        sta SDMCTL 
        jsr train_preinit2_vsync

        sei
        lda #<train_ld_screen ;Set new display list
        sta SDLSTL
        lda #>train_ld_screen
        sta SDLSTH
        cli

        jsr train_preinit2_vsync
        lda #34               ;Enable screen
        sta SDMCTL
        jsr train_preinit2_vsync
      
        rts 

train_preinit2_vsync:

        lda 20               ;Vertical sync
@tp2v:  cmp 20
        beq @tp2v
        rts	



train_preinit2_end:

        .word INITAD
        .word INITAD+1
train_preinit2_ivector:
        .byte <train_preinit2,>train_preinit2        
    

;===============================================================================
; Exports
;===============================================================================
.export _rmtSetUniversalVBI
.export _rmtRestoreOriginalVBI
.export _dliHandler
.export _dliMenuHandler
.export _dliInterMission
.export _songLineRequested
.export _statusBarForeground
.export _statusBarBackground
.export _incrementScore
.export _decrementScore
.export _filteredJS
.export _getFilteredJS
.export _sfxRequested
.export _inGameAudioFlag
.export _zp_ptr1
.export _zp_ptr2
.export _zp_ptr3
.export _zp_x1
.export _zp_z1
.export _menu_scroll_scrollCount
.export _menu_scroll_hscroll 