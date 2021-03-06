#pragma once

#include "common.hpp"

POLYBAR_NS

class signal_emitter;
class signal_receiver_interface;
template <uint8_t Priority, typename Signal, typename... Signals>
class signal_receiver;

namespace signals {
  namespace detail {
    class signal;
  }

  namespace eventqueue {
    struct start;
    struct exit_terminate;
    struct exit_reload;
    struct notify_change;
    struct notify_forcechange;
    struct check_state;
  }
  namespace ipc {
    struct command;
    struct hook;
    struct action;
  }
  namespace ui {
    struct tick;
    struct button_press;
    struct visibility_change;
    struct dim_window;
    struct shade_window;
    struct unshade_window;
  }
  namespace ui_tray {
    struct mapped_clients;
  }
  namespace parser {
    struct change_background;
    struct change_foreground;
    struct change_underline;
    struct change_overline;
    struct change_font;
    struct change_alignment;
    struct offset_pixel;
    struct attribute_set;
    struct attribute_unset;
    struct attribute_toggle;
    struct action_begin;
    struct action_end;
    struct write_text_ascii;
    struct write_text_unicode;
    struct write_text_string;
  }
}

POLYBAR_NS_END
