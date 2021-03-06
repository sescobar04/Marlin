/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "../../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>
//#include "../lvgl/src/lv_objx/lv_imgbtn.h"
//#include "../lvgl/src/lv_objx/lv_img.h"
//#include "../lvgl/src/lv_core/lv_disp.h"
//#include "../lvgl/src/lv_core/lv_refr.h"

#include "../../../../module/temperature.h"
#include "../../../../inc/MarlinConfig.h"

static lv_obj_t * scr;
extern lv_group_t*  g;
static lv_obj_t *buttonType, *buttonStep;
static lv_obj_t *labelType;
static lv_obj_t *labelStep;
static lv_obj_t * tempText1;

#define ID_P_ADD    1
#define ID_P_DEC    2
#define ID_P_TYPE   3
#define ID_P_STEP   4
#define ID_P_OFF    5
#define ID_P_RETURN 6

static void event_handler(lv_obj_t * obj, lv_event_t event) {
  switch (obj->mks_obj_id) {
    case ID_P_ADD:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        if (uiCfg.curTempType == 0) {
          thermalManager.temp_hotend[uiCfg.curSprayerChoose].target += uiCfg.stepHeat;
          if (uiCfg.curSprayerChoose == 0) {
            if ((int)thermalManager.temp_hotend[uiCfg.curSprayerChoose].target > (HEATER_0_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1))) {
              thermalManager.temp_hotend[uiCfg.curSprayerChoose].target = (float)HEATER_0_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1);
            }
          }
          #if !defined(SINGLENOZZLE) && HAS_MULTI_EXTRUDER
            else if ((int)thermalManager.temp_hotend[uiCfg.curSprayerChoose].target > (HEATER_1_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1))) {
              thermalManager.temp_hotend[uiCfg.curSprayerChoose].target = (float)HEATER_1_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1);
            }
          #endif
          thermalManager.start_watching_hotend(uiCfg.curSprayerChoose);
        }
        #if HAS_HEATED_BED
          else {
            thermalManager.temp_bed.target += uiCfg.stepHeat;
            if ((int)thermalManager.temp_bed.target > BED_MAXTEMP - (WATCH_BED_TEMP_INCREASE + TEMP_BED_HYSTERESIS + 1)) {
              thermalManager.temp_bed.target = (float)BED_MAXTEMP - (WATCH_BED_TEMP_INCREASE + TEMP_BED_HYSTERESIS + 1);
            }
            thermalManager.start_watching_bed();
          }
        #endif
        disp_desire_temp();
      }
      break;
    case ID_P_DEC:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        if (uiCfg.curTempType == 0) {
          if ((int)thermalManager.temp_hotend[uiCfg.curSprayerChoose].target > uiCfg.stepHeat) {
            thermalManager.temp_hotend[uiCfg.curSprayerChoose].target -= uiCfg.stepHeat;
          }
          else {
            thermalManager.temp_hotend[uiCfg.curSprayerChoose].target = (float)0;
          }
          thermalManager.start_watching_hotend(uiCfg.curSprayerChoose);
        }
        #if HAS_HEATED_BED
          else {
            if ((int)thermalManager.temp_bed.target > uiCfg.stepHeat) {
              thermalManager.temp_bed.target -= uiCfg.stepHeat;
            }
            else {
              thermalManager.temp_bed.target = (float)0;
            }
            thermalManager.start_watching_bed();
          }
        #endif
        disp_desire_temp();
      }

      break;
    case ID_P_TYPE:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        if (uiCfg.curTempType == 0) {
          if (ENABLED(HAS_MULTI_EXTRUDER)) {
            if (uiCfg.curSprayerChoose == 0) {
              uiCfg.curSprayerChoose = 1;
            }
            else if (uiCfg.curSprayerChoose == 1) {
              if (TEMP_SENSOR_BED != 0) {
                uiCfg.curTempType = 1;
              }
              else {
                uiCfg.curTempType      = 0;
                uiCfg.curSprayerChoose = 0;
              }
            }
          }
          else if (uiCfg.curSprayerChoose == 0) {
            if (TEMP_SENSOR_BED != 0)
              uiCfg.curTempType = 1;
            else
              uiCfg.curTempType = 0;
          }
        }
        else if (uiCfg.curTempType == 1) {
          uiCfg.curSprayerChoose = 0;
          uiCfg.curTempType      = 0;
        }
        disp_temp_type();
      }
      break;
    case ID_P_STEP:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        switch (uiCfg.stepHeat) {
          case  1: uiCfg.stepHeat =  5; break;
          case  5: uiCfg.stepHeat = 10; break;
          case 10: uiCfg.stepHeat =  1; break;
          default: break;
        }
        disp_step_heat();
      }
      break;
    case ID_P_OFF:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        if (uiCfg.curTempType == 0) {
          thermalManager.temp_hotend[uiCfg.curSprayerChoose].target = (float)0;
          thermalManager.start_watching_hotend(uiCfg.curSprayerChoose);
        }
        #if HAS_HEATED_BED
          else {
            thermalManager.temp_bed.target = (float)0;
            thermalManager.start_watching_bed();
          }
        #endif
        disp_desire_temp();
      }
      break;
    case ID_P_RETURN:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        clear_cur_ui();
        draw_return_ui();
      }
      break;
  }
}

void lv_draw_preHeat(void) {
  lv_obj_t *buttonAdd, *buttonDec;
  lv_obj_t *buttonOff, *buttonBack;

  if (disp_state_stack._disp_state[disp_state_stack._disp_index] != PRE_HEAT_UI) {
    disp_state_stack._disp_index++;
    disp_state_stack._disp_state[disp_state_stack._disp_index] = PRE_HEAT_UI;
  }
  disp_state = PRE_HEAT_UI;

  scr = lv_obj_create(NULL, NULL);

  lv_obj_set_style(scr, &tft_style_scr);
  lv_scr_load(scr);
  lv_obj_clean(scr);

  (void)lv_label_create(scr, TITLE_XPOS, TITLE_YPOS, creat_title_text());

  lv_refr_now(lv_refr_get_disp_refreshing());

  // Create image buttons
  buttonAdd = lv_imgbtn_create(scr, "F:/bmp_Add.bin", INTERVAL_V, titleHeight, event_handler, ID_P_ADD);
  lv_obj_clear_protect(buttonAdd, LV_PROTECT_FOLLOW);
  buttonDec = lv_imgbtn_create(scr, "F:/bmp_Dec.bin", BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_P_DEC);
  buttonType = lv_imgbtn_create(scr, NULL, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_P_TYPE);
  buttonStep = lv_imgbtn_create(scr, NULL, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_P_STEP);
  buttonOff = lv_imgbtn_create(scr, "F:/bmp_speed0.bin", BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_P_OFF);
  buttonBack = lv_imgbtn_create(scr, "F:/bmp_return.bin", BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_P_RETURN);

  // Create labels on the image buttons
  lv_obj_t *labelAdd = lv_label_create_empty(buttonAdd);
  lv_obj_t *labelDec = lv_label_create_empty(buttonDec);
  labelType = lv_label_create_empty(buttonType);
  labelStep = lv_label_create_empty(buttonStep);
  lv_obj_t *labelOff   = lv_label_create_empty(buttonOff);
  lv_obj_t *label_Back = lv_label_create_empty(buttonBack);

  if (gCfgItems.multiple_language) {
    lv_label_set_text(labelAdd, preheat_menu.add);
    lv_obj_align(labelAdd, buttonAdd, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(labelDec, preheat_menu.dec);
    lv_obj_align(labelDec, buttonDec, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(labelOff, preheat_menu.off);
    lv_obj_align(labelOff, buttonOff, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(label_Back, common_menu.text_back);
    lv_obj_align(label_Back, buttonBack, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  }
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) {
      lv_group_add_obj(g, buttonAdd);
      lv_group_add_obj(g, buttonDec);
      lv_group_add_obj(g, buttonType);
      lv_group_add_obj(g, buttonStep);
      lv_group_add_obj(g, buttonOff);
      lv_group_add_obj(g, buttonBack);
    }
  #endif

  disp_temp_type();
  disp_step_heat();

  tempText1 = lv_label_create_empty(scr);
  lv_obj_set_style(tempText1, &tft_style_label_rel);
  disp_desire_temp();
}

void disp_temp_type() {
  if (uiCfg.curTempType == 0) {
    if (uiCfg.curSprayerChoose == 1) {
    lv_imgbtn_set_src_both(buttonType, "F:/bmp_extru2.bin");
      if (gCfgItems.multiple_language) {
        lv_label_set_text(labelType, preheat_menu.ext2);
        lv_obj_align(labelType, buttonType, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
      }
    }
    else {
    lv_imgbtn_set_src_both(buttonType, "F:/bmp_extru1.bin");
      if (gCfgItems.multiple_language) {
        lv_label_set_text(labelType, preheat_menu.ext1);
        lv_obj_align(labelType, buttonType, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
      }
    }

  }
  else {
  lv_imgbtn_set_src_both(buttonType, "F:/bmp_bed.bin");
    if (gCfgItems.multiple_language) {
      lv_label_set_text(labelType, preheat_menu.hotbed);
      lv_obj_align(labelType, buttonType, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
}

void disp_desire_temp() {
  char buf[20] = {0};

  public_buf_l[0] = '\0';

  if (uiCfg.curTempType == 0) {
    if (uiCfg.curSprayerChoose < 1)
      strcat(public_buf_l, preheat_menu.ext1);
    else
      strcat(public_buf_l, preheat_menu.ext2);
    sprintf(buf, preheat_menu.value_state, (int)thermalManager.temp_hotend[uiCfg.curSprayerChoose].celsius,  (int)thermalManager.temp_hotend[uiCfg.curSprayerChoose].target);
  }
  #if HAS_HEATED_BED
    else {
      strcat(public_buf_l, preheat_menu.hotbed);
      sprintf(buf, preheat_menu.value_state, (int)thermalManager.temp_bed.celsius,  (int)thermalManager.temp_bed.target);
    }
  #endif
  strcat_P(public_buf_l, PSTR(": "));
  strcat(public_buf_l, buf);
  lv_label_set_text(tempText1, public_buf_l);
  lv_obj_align(tempText1, NULL, LV_ALIGN_CENTER, 0, -50);
}

void disp_step_heat() {
  if (uiCfg.stepHeat == 1) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step1_degree.bin");
  }
  else if (uiCfg.stepHeat == 5) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step5_degree.bin");
  }
  else if (uiCfg.stepHeat == 10) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step10_degree.bin");
  }

  if (gCfgItems.multiple_language) {
    if (uiCfg.stepHeat == 1) {
      lv_label_set_text(labelStep, preheat_menu.step_1c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if (uiCfg.stepHeat == 5) {
      lv_label_set_text(labelStep, preheat_menu.step_5c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if (uiCfg.stepHeat == 10) {
      lv_label_set_text(labelStep, preheat_menu.step_10c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
}

void lv_clear_preHeat() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
