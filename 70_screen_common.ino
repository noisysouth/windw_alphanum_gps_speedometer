extern struct ctrl_screen_s *cur_screen;

// ----------------------- generic screen functions: change focus to new control ----------------------------

// Change focus to *next* control in screen structure.
//  If the last control had focus, wrap (change focus to the first control).
void ScreenTabNext(void) {
  struct ctrl_screen_s *scrn;
  int ctrl_idx;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new;
  bool did_tab;
  bool can_focus;
  
  scrn = cur_screen;
  focus_old = scrn->screen_ctrls[scrn->focus_idx];
  did_tab = CtrlTabNext(focus_old);
  if (did_tab) {
    return; // changed focus within control (e.g. list item). control keeps focus.
  }
  // change focus to another control.
  DrawCtrl (focus_old, /*has_focus*/false);

  // from scrn->focus_idx, go to focus_idx+1, if possible.
  ctrl_idx = scrn->focus_idx;
  do {
    ctrl_idx++;
    if (scrn->screen_ctrls[ctrl_idx]->ctrl_type == ctrl_type_none) {
      ctrl_idx = 0; // go back to the first control
    }
    focus_new = scrn->screen_ctrls[ctrl_idx];
    can_focus = CtrlCanFocus(focus_new);
    if (focus_new == focus_old) {
      break; // screen had no controls that could focus. break out of infinite loop.
    }
  } while (!can_focus); // skip elements that cannot receive focus, like background windows and static text labels.
  
  scrn->focus_idx = ctrl_idx; // store the new focus
  DrawCtrl (focus_new, /*has_focus*/true);
}

// Change focus to *previous* control in screen structure.
//  If the first control had focus, wrap (change focus to the last control).
void ScreenTabPrev(void) {
  struct ctrl_screen_s *scrn;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new;
  bool did_tab;
  bool can_focus;

  scrn = cur_screen;
  focus_old = scrn->screen_ctrls[scrn->focus_idx];
  did_tab = CtrlTabPrev(focus_old);
  if (did_tab) {
    return; // changed focus within control (e.g. list item). control keeps focus.
  }
  DrawCtrl (focus_old, /*has_focus*/false);

  do {
    if (scrn->focus_idx > 0) {
      scrn->focus_idx--;
    } else { // scrn->focus_idx <= 0. We are at the first control in the screen, going backwards.
      int ctrl_idx;

      ctrl_idx = 0;
      while (scrn->screen_ctrls[ctrl_idx]->ctrl_type != ctrl_type_none) {
        ctrl_idx++;
      };
      scrn->focus_idx = ctrl_idx-1; // go to the last control
    }
    focus_new = scrn->screen_ctrls[scrn->focus_idx];
    can_focus = CtrlCanFocus(focus_new);
    if (focus_new == focus_old) {
      break; // screen had no controls that could focus. break out of infinite loop.
    }
  } while (!can_focus);

  DrawCtrl (focus_new, /*has_focus*/true);
}

void ScreenMove (int x_move, int y_move) {
  static int  y_old = 0;

  // move the direction that was pressed
  if (y_move > 0 && y_old == 0) {
    ScreenTabNext();
  }
  if (y_move < 0 && y_old == 0) {
    ScreenTabPrev();
  }
  if (x_move > 0) {
    do_click = true;
    //was_click = true;
  }
  if (was_click && !do_click) {
    do_unclick = true;
  }

  y_old = y_move;
}

// On touching the screen:
// 1. Change focus to control at that location.
// 2. Click that control.
void ScreenTouch(int touch_x, int touch_y) {
  struct ctrl_screen_s *scrn;
  int old_idx;
  int ctrl_idx;
  struct tab_ctrl_s *chk_ctrl;
  struct windw_s *chk_windw;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new = NULL;

  scrn = cur_screen;
  old_idx = scrn->focus_idx;
  ctrl_idx = 0;
  do {
    if (scrn->screen_ctrls[ctrl_idx]->ctrl_type == ctrl_type_none) {
      return; // got to the end of the list, found no control being touched
    }

    chk_ctrl = scrn->screen_ctrls[ctrl_idx];
    // TODO: check CtrlCanFocus()
    chk_windw = GetCtrlWindow(chk_ctrl); // Get X and Y of control
    if (chk_windw != NULL) {
      if (touch_x >= chk_windw->wx &&
          touch_x <= chk_windw->wx+chk_windw->ww &&
          touch_y >= chk_windw->wy &&
          touch_y <= chk_windw->wy+chk_windw->wh) {
        // touch is within bounds of window
        scrn->focus_idx = ctrl_idx;
        do_click  = true;
        //was_click = true;
        break;
      }
    }
    ctrl_idx++;
  } while (1);

  if (scrn->focus_idx != old_idx) {
    focus_old = scrn->screen_ctrls[old_idx];
    DrawCtrl (focus_old, /*has_focus*/false);

    focus_new = scrn->screen_ctrls[scrn->focus_idx];
    DrawCtrl (focus_new, /*has_focus*/true);
  }
}

// redraw a control.
//  determines if the control has focus, then draws appropriately.
void ScreenRedrawCtrl(struct tab_ctrl_s *draw_ctrl) {
  struct tab_ctrl_s *focus_ctrl;
  bool has_focus;

  focus_ctrl = cur_screen->screen_ctrls[cur_screen->focus_idx];
  if (draw_ctrl == focus_ctrl) {
    has_focus = true;
  } else {
    has_focus = false;
  }
  DrawCtrl (draw_ctrl, has_focus);
}

void ScreenDraw(void) {
  struct ctrl_screen_s *scrn;
  int ctrl_idx;
  struct tab_ctrl_s *chk_ctrl;

  tft.fillScreen(GetColorBackgr());
  scrn = cur_screen;
  ctrl_idx = 0;
  do {
    if (scrn->screen_ctrls[ctrl_idx]->ctrl_type == ctrl_type_none) {
      return; // got to the end of the list, found no control being touched
    }
    chk_ctrl = scrn->screen_ctrls[ctrl_idx];
    ScreenRedrawCtrl(chk_ctrl);
    ctrl_idx++;
  } while (1);
}

// ------------------------- Send input to control that has focus --------------------------
void loop_screen(char kb_in, bool do_unclick) {
  struct tab_ctrl_s *focus_ctrl;

  focus_ctrl = cur_screen->screen_ctrls[cur_screen->focus_idx];

  if (do_click) {
    OnClickCtrl (focus_ctrl);
    do_click = false;
  }
  if (do_unclick) {
    haptic_click ();
    DrawCtrl (focus_ctrl, /*has_focus*/true);
    was_click = false;
  }

  UpdateCtrl (focus_ctrl); // e.g. for cursor blink, etc.
  if (kb_in != 0) {
    OnKeyCtrl (focus_ctrl, kb_in);
  }
}