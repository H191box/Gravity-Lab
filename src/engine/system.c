#include "system.h"

/* Global VBlank flag — set in ISR, cleared in sys_wait_vblank */
volatile u16 vblank_flag = 0;

/* Frame counter — incremented each VBlank */
static u32 frame_count = 0;

/* -------------------------------------------------------
 *  VBlank Interrupt Service Routine
 *  Fires at the start of each vertical blanking period.
 *  We only set a flag here; the main loop checks it.
 * ------------------------------------------------------- */
void VBlankISR(void) __attribute__((interrupt("IRQ")));
void VBlankISR(void) {
    vblank_flag = 1;
    frame_count++;

    /* Acknowledge the VBlank interrupt by writing to IF */
    REG_IF = INT_VBLANK;
}

/* -------------------------------------------------------
 *  sys_init — One-time hardware setup
 *  - Mode 0: 4 text/tile background layers
 *  - Enable all 4 BGs, enable sprites, enable 1D sprite mapping
 *  - Set up VBlank interrupt
 *  - Clear OAM
 * ------------------------------------------------------- */
void sys_init(void) {
    /* Enable Mode 0 with all BGs, sprites, 1D OAM mapping */
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_BG3
                | DCNT_OBJ | DCNT_OBJ_1D;

    /* Install VBlank interrupt handler */
    sys_install_vblank_handler();

    /* Clear OAM — all sprites off-screen / disabled */
    ObjAttr *oam = (ObjAttr *)OAM_BASE;
    for (int i = 0; i < MAX_SPRITES; i++) {
        oam[i].attr0 = ATTR0_Y_MASK;  /* y = 160 (off-screen) + disable */
        oam[i].attr1 = 0;
        oam[i].attr2 = 0;
        oam[i].padding = 0;
    }

    /* Clear VRAM (96KB) to 0 */
    u16 *vram = (u16 *)VRAM_BASE;
    for (int i = 0; i < (96 * 1024) / 2; i++) {
        vram[i] = 0;
    }

    /* Clear BG palette to black */
    for (int i = 0; i < 256; i++) {
        BG_PALETTE[i] = 0x0000;
    }

    /* Clear OBJ palette to transparent black */
    for (int i = 0; i < 256; i++) {
        OBJ_PALETTE[i] = 0x0000;
    }

    /* Set first BG palette entry to black */
    BG_PALETTE[0] = 0x0000;

    /* Clear all BG scroll registers */
    REG_BG0HOFS = 0; REG_BG0VOFS = 0;
    REG_BG1HOFS = 0; REG_BG1VOFS = 0;
    REG_BG2HOFS = 0; REG_BG2VOFS = 0;
    REG_BG3HOFS = 0; REG_BG3VOFS = 0;

    /* Disable all BGs initially (will be configured by bg_manager) */
    REG_BG0CNT = 0;
    REG_BG1CNT = 0;
    REG_BG2CNT = 0;
    REG_BG3CNT = 0;

    /* Enable master interrupt */
    REG_IME = 1;
}

/* -------------------------------------------------------
 *  sys_install_vblank_handler — Set up VBlank IRQ
 * ------------------------------------------------------- */
void sys_install_vblank_handler(void) {
    /* Set VBlank ISR address at BIOS interrupt table */
    *(void (**)(void))0x03000008 = VBlankISR;

    /* Enable VBlank interrupt in DISPSTAT */
    REG_DISPSTAT = DSTAT_VBL_IRQ;

    /* Enable VBlank in IE register */
    REG_IE = INT_VBLANK;

    /* Enable interrupts globally */
    REG_IME = 1;
}

/* -------------------------------------------------------
 *  sys_wait_vblank — Spin until VBlank starts
 *  Called at the end of each frame to sync to 60fps.
 * ------------------------------------------------------- */
void sys_wait_vblank(void) {
    while (!vblank_flag) {
        /* Spin — wait for VBlank ISR to set flag */
    }
    vblank_flag = 0;
}

/* -------------------------------------------------------
 *  sys_get_frame_count — Return total frames since boot
 * ------------------------------------------------------- */
u32 sys_get_frame_count(void) {
    return frame_count;
}

/* -------------------------------------------------------
 *  sys_set_default_palette — Set up basic palette for testing
 *  Color format: BGR555 (0BBBBBGGGGGRRRRR)
 * ------------------------------------------------------- */
void sys_set_default_palette(void) {
    /* BG Palette 0 — Circuit colors */
    BG_PALETTE[0]  = 0x0000;  /* 0: Black / transparent */
    BG_PALETTE[1]  = 0x7FFF;  /* 1: White */
    BG_PALETTE[2]  = 0x4A4A;  /* 2: Dark gray (metal) */
    BG_PALETTE[3]  = 0x739C;  /* 3: Medium gray (metal highlight) */
    BG_PALETTE[4]  = 0x296B;  /* 4: Blue (circuit floor) */
    BG_PALETTE[5]  = 0x5293;  /* 5: Light blue (glow) */
    BG_PALETTE[6]  = 0x7C00;  /* 6: Red (danger/obstacle) */
    BG_PALETTE[7]  = 0x001F;  /* 7: Blue (deep) */
    BG_PALETTE[8]  = 0x03E0;  /* 8: Green (checkpoint) */
    BG_PALETTE[9]  = 0x7FE0;  /* 9: Yellow (boost) */
    BG_PALETTE[10] = 0x5EBF;  /* 10: Cyan (ship accent) */
    BG_PALETTE[11] = 0x4208;  /* 11: Dark red (wall shadow) */
    BG_PALETTE[12] = 0x8410;  /* 12: Orange (fire/explosion) */
    BG_PALETTE[13] = 0x6B4A;  /* 13: Gray-blue (floor accent) */
    BG_PALETTE[14] = 0x2124;  /* 14: Very dark blue (void) */
    BG_PALETTE[15] = 0x57E0;  /* 15: Light green (finish) */

    /* OBJ Palette 0 — Sprite colors */
    OBJ_PALETTE[0]  = 0x0000;  /* 0: Transparent */
    OBJ_PALETTE[1]  = 0x7FFF;  /* 1: White (ship body) */
    OBJ_PALETTE[2]  = 0x5EBF;  /* 2: Cyan (ship glow) */
    OBJ_PALETTE[3]  = 0x7C00;  /* 3: Red (ship thrust) */
    OBJ_PALETTE[4]  = 0x7FE0;  /* 4: Yellow (fire) */
    OBJ_PALETTE[5]  = 0x8410;  /* 5: Orange (flame) */
    OBJ_PALETTE[6]  = 0x739C;  /* 6: Silver (medal) */
    OBJ_PALETTE[7]  = 0x4210;  /* 7: Dark orange */
    OBJ_PALETTE[8]  = 0x001F;  /* 8: Deep blue */
    OBJ_PALETTE[9]  = 0x03E0;  /* 9: Green */
    OBJ_PALETTE[10] = 0x7BEF;  /* 10: Light gray (smoke) */
    OBJ_PALETTE[11] = 0x294A;  /* 11: Dark cyan */
    OBJ_PALETTE[12] = 0xFD20;  /* 12: Gold (gold medal) */
    OBJ_PALETTE[13] = 0xC618;  /* 13: Silver medal color */
    OBJ_PALETTE[14] = 0x8400;  /* 14: Bronze medal color */
    OBJ_PALETTE[15] = 0x5293;  /* 15: Light blue glow */
}