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

#include "draw_ready_print.h"
#include "draw_set.h"
#include "draw_ui.h"
#include <lv_conf.h>
//#include "../lvgl/src/lv_objx/lv_imgbtn.h"
//#include "../lvgl/src/lv_objx/lv_img.h"
//#include "../lvgl/src/lv_core/lv_disp.h"
//#include "../lvgl/src/lv_core/lv_refr.h"

#include "pic_manager.h"

#include "../../../../gcode/queue.h"
#include "../../../../inc/MarlinConfig.h"

static lv_obj_t * scr;
extern lv_group_t*  g;

#define ID_S_WIFI         1
#define ID_S_FAN          2
#define ID_S_ABOUT        3
#define ID_S_CONTINUE     4
#define ID_S_MOTOR_OFF    5
#define ID_S_LANGUAGE     6
#define ID_S_MACHINE_PARA 7
#define ID_S_EEPROM_SET   8
#define ID_S_RETURN       9

static void event_handler(lv_obj_t * obj, lv_event_t event) {
  #if ENABLED(USE_WIFI_FUNCTION)
    char buf[6] = { 0 };
  #endif
  switch (obj->mks_obj_id) {

    case ID_S_FAN:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_fan();
      }
      break;
    case ID_S_ABOUT:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_about();
      }
      break;
    case ID_S_CONTINUE:

      break;
    case ID_S_MOTOR_OFF:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        #if HAS_SUICIDE
          suicide();
        #else
          queue.enqueue_now_P(PSTR("M84"));
        #endif
      }
      break;
    case ID_S_LANGUAGE:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_language();
      }
      break;
    case ID_S_MACHINE_PARA:
      if (event == LV_EVENT_CLICKED) {

      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_machine_para();
      }
      break;
    case ID_S_EEPROM_SET:
      if (event == LV_EVENT_CLICKED) {

      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_eeprom_settings();
      }
      break;
    case ID_S_RETURN:
      if (event == LV_EVENT_CLICKED) {
        // nothing to do
      }
      else if (event == LV_EVENT_RELEASED) {
        lv_clear_set();
        lv_draw_ready_print();
      }
      break;
    #if ENABLED(USE_WIFI_FUNCTION)
      case ID_S_WIFI:
        if (event == LV_EVENT_CLICKED) {

        }
        else if (event == LV_EVENT_RELEASED) {
        if (gCfgItems.wifi_mode_sel == STA_MODEL) {
          if (wifi_link_state == WIFI_CONNECTED) {
            last_disp_state = SET_UI;
            lv_clear_set();
            lv_draw_wifi();
          }
          else {
            if (uiCfg.command_send == 1) {
              buf[0] = 0xA5;
              buf[1] = 0x07;
              buf[2] = 0x00;
              buf[3] = 0x00;
              buf[4] = 0xFC;
              raw_send_to_wifi(buf, 5);

              last_disp_state = SET_UI;
              lv_clear_set();
              lv_draw_wifi_list();
            }
            else {
              last_disp_state = SET_UI;
              lv_clear_set();
              lv_draw_dialog(WIFI_ENABLE_TIPS);
            }
          }
        }
        else {
          last_disp_state = SET_UI;
          lv_clear_set();
          lv_draw_wifi();
        }
      }
      break;
    #endif
  }
}

void lv_draw_set(void) {
  lv_obj_t *buttonFan, *buttonAbout;
  lv_obj_t *buMotorOff, *buttonBack;
  #if HAS_LANG_SELECT_SCREEN
    lv_obj_t *buttonLanguage;
  #endif
  lv_obj_t *buttonMachinePara;
  lv_obj_t *buttonEepromSet;
  #if ENABLED(USE_WIFI_FUNCTION)
    lv_obj_t *buttonWifi;
  #endif

  if (disp_state_stack._disp_state[disp_state_stack._disp_index] != SET_UI) {
    disp_state_stack._disp_index++;
    disp_state_stack._disp_state[disp_state_stack._disp_index] = SET_UI;
  }
  disp_state = SET_UI;

  scr = lv_obj_create(NULL, NULL);

  //static lv_style_t tool_style;

  lv_obj_set_style(scr, &tft_style_scr);
  lv_scr_load(scr);
  lv_obj_clean(scr);

  (void)lv_label_create(scr, TITLE_XPOS, TITLE_YPOS, creat_title_text());

  lv_refr_now(lv_refr_get_disp_refreshing());

  // Create image buttons
  buttonEepromSet = lv_imgbtn_create(scr, "F:/bmp_eeprom_settings.bin", INTERVAL_V, titleHeight, event_handler, ID_S_EEPROM_SET);
  //buttonWifi = lv_imgbtn_create(scr, NULL);
  buttonFan = lv_imgbtn_create(scr, "F:/bmp_fan.bin", BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_S_FAN);
  buttonAbout = lv_imgbtn_create(scr, "F:/bmp_about.bin", BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_S_ABOUT);
  //buttonContinue = lv_imgbtn_create(scr, NULL);
  buMotorOff = lv_imgbtn_create(scr, ENABLED(HAS_SUICIDE) ? "F:/bmp_manual_off.bin" : "F:/bmp_function1.bin", BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_S_MOTOR_OFF);
  buttonMachinePara = lv_imgbtn_create(scr, "F:/bmp_machine_para.bin", INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_S_MACHINE_PARA);
  #if HAS_LANG_SELECT_SCREEN
    buttonLanguage = lv_imgbtn_create(scr, "F:/bmp_language.bin", BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_S_LANGUAGE);
  #endif
  #if ENABLED(USE_WIFI_FUNCTION)
    buttonWifi = lv_imgbtn_create(scr, "F:/bmp_wifi.bin", BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_S_WIFI);
  #endif
  buttonBack = lv_imgbtn_create(scr, "F:/bmp_return.bin", BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_S_RETURN);

  /*lv_obj_set_pos(buttonWifi,INTERVAL_V,titleHeight);
  lv_obj_set_pos(buttonFan,BTN_X_PIXEL+INTERVAL_V*2,titleHeight);
  lv_obj_set_pos(buttonAbout,BTN_X_PIXEL*2+INTERVAL_V*3,titleHeight);
  lv_obj_set_pos(buttonContinue,BTN_X_PIXEL*3+INTERVAL_V*4,titleHeight);
  lv_obj_set_pos(buMotorOff,INTERVAL_V, BTN_Y_PIXEL+INTERVAL_H+titleHeight);
  lv_obj_set_pos(buttonLanguage,BTN_X_PIXEL+INTERVAL_V*2,BTN_Y_PIXEL+INTERVAL_H+titleHeight);
  lv_obj_set_pos(buttonBack,BTN_X_PIXEL*3+INTERVAL_V*4, BTN_Y_PIXEL+INTERVAL_H+titleHeight);*/

  //lv_obj_set_pos(buttonWifi,INTERVAL_V,titleHeight);
  //lv_obj_set_pos(buttonContinue,BTN_X_PIXEL*3+INTERVAL_V*4,titleHeight);

  /// Create labels on the buttons
  //lv_obj_t *labelWifi= lv_label_create_empty(buttonWifi);
  lv_obj_t *label_EepromSet   = lv_label_create_empty(buttonEepromSet);
  lv_obj_t *labelFan          = lv_label_create_empty(buttonFan);
  lv_obj_t *label_About       = lv_label_create_empty(buttonAbout);
  //lv_obj_t *label_Continue  = lv_label_create_empty(buttonContinue);
  lv_obj_t *label_MotorOff    = lv_label_create_empty(buMotorOff);
  lv_obj_t *label_MachinePara = lv_label_create_empty(buttonMachinePara);
  #if HAS_LANG_SELECT_SCREEN
    lv_obj_t *label_Language  = lv_label_create_empty(buttonLanguage);
  #endif
  #if ENABLED(USE_WIFI_FUNCTION)
    lv_obj_t *label_Wifi      = lv_label_create_empty(buttonWifi);
  #endif
  lv_obj_t *label_Back        = lv_label_create_empty(buttonBack);

  if (gCfgItems.multiple_language) {

    lv_label_set_text(label_EepromSet, set_menu.eepromSet);
    lv_obj_align(label_EepromSet, buttonEepromSet, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(labelFan, set_menu.fan);
    lv_obj_align(labelFan, buttonFan, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(label_About, set_menu.about);
    lv_obj_align(label_About, buttonAbout, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    //lv_label_set_text(label_Continue, set_menu.breakpoint);
    //lv_obj_align(label_Continue, buttonContinue, LV_ALIGN_IN_BOTTOM_MID,0, BUTTON_TEXT_Y_OFFSET);
    lv_label_set_text(label_MotorOff, set_menu.TERN(HAS_SUICIDE, shutdown, motoroff));
    lv_obj_align(label_MotorOff, buMotorOff, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    lv_label_set_text(label_MachinePara, set_menu.machine_para);
    lv_obj_align(label_MachinePara, buttonMachinePara, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);

    #if HAS_LANG_SELECT_SCREEN
      lv_label_set_text(label_Language, set_menu.language);
      lv_obj_align(label_Language, buttonLanguage, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    #endif

    #if ENABLED(USE_WIFI_FUNCTION)
      lv_label_set_text(label_Wifi, set_menu.wifi);
      lv_obj_align(label_Wifi, buttonWifi, LV_ALIGN_IN_BOTTOM_MID,0, BUTTON_TEXT_Y_OFFSET);
    #endif

    lv_label_set_text(label_Back, common_menu.text_back);
    lv_obj_align(label_Back, buttonBack, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  }
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) {
      lv_group_add_obj(g, buttonEepromSet);
      lv_group_add_obj(g, buttonFan);
      lv_group_add_obj(g, buttonAbout);
      lv_group_add_obj(g, buMotorOff);
      lv_group_add_obj(g, buttonMachinePara);
      lv_group_add_obj(g, buttonLanguage);
      #if ENABLED(USE_WIFI_FUNCTION)
        lv_group_add_obj(g, buttonWifi);
      #endif
      lv_group_add_obj(g, buttonBack);
    }
  #endif
}

void lv_clear_set() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
