//
// FGUI - feature rich graphical user interface
//

#ifndef FGUI_ALIASES_HH
#define FGUI_ALIASES_HH

// includes
#include <functional>

// library includes
#define FGUI_USE_D3D9 // tell FGUI that we are using DirectX 9
#include "definitions.hpp"
#include "helpers.hpp"

namespace FGUI
{
  using INPUT_TYPE = enum struct ESInputType_t : int
  {
    NONE = 0,
    WIN_32 = 1,
    INPUT_SYSTEM = 2,
    CUSTOM = 3
  };

  namespace DETAIL
  {
    // drawing functions pointers
    using pCreateFont = std::add_pointer_t<void(FGUI::FONT&, std::string, int, int, bool)>;
    using pGetScreenSize = std::add_pointer_t<FGUI::DIMENSION()>;
    using pGetTextSize = std::add_pointer_t<FGUI::DIMENSION(FGUI::FONT, std::string)>;
    using pDrawRectangle = std::add_pointer_t<void(int, int, int, int, FGUI::COLOR)>;
    using pDrawOutline = std::add_pointer_t<void(int, int, int, int, FGUI::COLOR)>;
    using pDrawGradient = std::add_pointer_t<void(int, int, int, int, FGUI::COLOR, FGUI::COLOR, bool)>;
    using pDrawLine = std::add_pointer_t<void(int, int, int, int, FGUI::COLOR)>;
    using pDrawText = std::add_pointer_t<void(int, int, FGUI::FONT, FGUI::COLOR, std::string)>;
    using pSetAlpha = std::add_pointer_t<void(float)>;
    using pGetAlpha = std::add_pointer_t<float()>;

    // input functions pointers
    using pPullInput = std::add_pointer_t<void()>;
    using pIsKeyHeld = std::add_pointer_t<bool(unsigned int)>;
    using pIsKeyReleased = std::add_pointer_t<bool(unsigned int)>;
    using pIsKeyPressed = std::add_pointer_t<bool(unsigned int)>;
    using pGetCursorPos = std::add_pointer_t<FGUI::POINT()>;
    using pGetCursorPosDelta = std::add_pointer_t<FGUI::POINT()>;
    using pIsCursorInArea = std::add_pointer_t<bool(FGUI::AREA)>;
    using pSetScroll = std::add_pointer_t<void(int)>;
    using pGetScroll = std::add_pointer_t<int()>;

    using RENDER_MANAGER = struct SRenderManager_t
    {
      pCreateFont CreateFont;
      pGetScreenSize GetScreenSize;
      pGetTextSize GetTextSize;
      pDrawRectangle Rectangle;
      pDrawOutline Outline;
      pDrawGradient Gradient;
      pDrawLine Line;
      pDrawText Text;
      pSetAlpha SetAlpha;
      pGetAlpha GetAlpha;
    };

    using INPUT_SYTEM = struct SInputSystem_t
    {
      pPullInput PullInput;
      pIsKeyHeld IsKeyHeld;
      pIsKeyReleased IsKeyReleased;
      pIsKeyPressed IsKeyPressed;
      pGetCursorPos GetCursorPos;
      pGetCursorPosDelta GetCursorPosDelta;
      pIsCursorInArea IsCursorInArea;
      pSetScroll SetScroll;
      pGetScroll GetScroll;

      // @brief: set the input type
      // @args: FGUI::INPUT_TYPE = input type (NONE, WIN_32, INPUT_SYSTEM or CUSTOM)
      // @note: make sure to set this when you initialize your widgets
      void SetInputType(FGUI::INPUT_TYPE type)
      {
        m_nInputType = static_cast<int>(type);
      }

      // @brief: get the current input type
      int GetInputType()
      {
        return m_nInputType;
      }
    private:
      int m_nInputType;
    };

  } // namespace DETAIL

  inline DETAIL::RENDER_MANAGER RENDER;
  inline DETAIL::INPUT_SYTEM INPUT;

} // namespace FGUI

#endif // FGUI_ALIASES_HH