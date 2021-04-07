#pragma once
#include "../internal/definitions.hpp"

#ifndef CUSTOM_THEME
#define COLOR1 {20, 20, 20}
#define COLOR2 {27, 27, 27}
#define COLOR3 {35, 35, 35}
#define COLOR4 {42, 42, 42}
#define COLOR5 {49, 49, 49}
#define COLOR6 {56, 56, 56}

#define TEXTDARK { 10, 10, 10 }
#define TEXTLIGHT { 240, 240, 240}

//#define COLOR1     { 220, 220, 220 }
//#define COLOR2     { 50, 50, 50 }
//#define COLOR3     { 40, 40, 40 }
//#define COLOR4     { 30, 30, 30 }
//#define COLOR5     { 25, 25, 25 }
//#define COLOR6     { 15, 15, 15 }

#define COMBOARROW { 240, 240, 240 }
#define ACCENT     { 255, 40, 40 }
#define SCROLLBAR  { 80, 80, 80 }
#define CBHOVER    { 115, 115, 115 }

#endif

// Tab Panel Colors
inline FGUI::COLOR COLOR_CTABPANEL_BG            = COLOR2;
inline FGUI::COLOR COLOR_CTABPANEL_BG_HOVER      = COLOR4;
inline FGUI::COLOR COLOR_CTABPANEL_INDICATOR     = ACCENT;

// Container Colors
inline FGUI::COLOR COLOR_CCONTAINER_BG           = COLOR3;
inline FGUI::COLOR COLOR_CCONTAINER_TITLE_BG     = COLOR1;
inline FGUI::COLOR COLOR_CCONTAINER_BORDER       = COLOR1;
inline FGUI::COLOR COLOR_CCONTAINER_TEXT         = COMBOARROW;

// Combo Box Colors
inline FGUI::COLOR COLOR_CCOMBOBOX_BG            = COLOR3;
inline FGUI::COLOR COLOR_CCOMBOBOX_BG_HOVER      = COLOR5;
inline FGUI::COLOR COLOR_CCOMBOBOX_BG_ITEM_HOVER = COLOR5;
inline FGUI::COLOR COLOR_CCOMBOBOX_BORDER        = COLOR2;
inline FGUI::COLOR COLOR_CCOMBOBOX_LABEL         = COMBOARROW;
inline FGUI::COLOR COLOR_CCOMBOBOX_ARROW         = COMBOARROW;

// List Box Colors
inline FGUI::COLOR COLOR_CLISTBOX_BG             = COLOR4;
inline FGUI::COLOR COLOR_CLISTBOX_BORDER         = COLOR2;
inline FGUI::COLOR COLOR_CLISTBOX_SCROLL         = SCROLLBAR;
inline FGUI::COLOR COLOR_CLISTBOX_LABEL          = COLOR1;

// Button Colors
inline FGUI::COLOR COLOR_CBUTTON_BG              = COLOR2;
inline FGUI::COLOR COLOR_CBUTTON_BG_HOVER        = COLOR4;
inline FGUI::COLOR COLOR_CBUTTON_BORDER          = COLOR1;
inline FGUI::COLOR COLOR_CBUTTON_LABEL           = COMBOARROW;

// Checkbox Colors
inline FGUI::COLOR COLOR_CCHECKBOX_BG            = COLOR2;
inline FGUI::COLOR COLOR_CCHECKBOX_BG_HOVER		 = COLOR4;
inline FGUI::COLOR COLOR_CCHECKBOX_BORDER        = COLOR1;
inline FGUI::COLOR COLOR_CCHECKBOX_LABEL         = COMBOARROW;