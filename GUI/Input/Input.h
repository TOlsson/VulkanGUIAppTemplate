#pragma once

#include "KeyCodes.h"

#include <glm/glm.hpp>

namespace GUI {
  class Input
  {
  public:
    static bool IsKeyDown(KeyCode keyCode);
    static bool IsMouseButtonDown(MouseButton mouseButton);

    static glm::vec2 GetMousePosition();

    static void SetCursorMode(CursorMode mode);
  };
}