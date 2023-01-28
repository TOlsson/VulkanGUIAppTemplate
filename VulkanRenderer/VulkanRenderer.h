#pragma once

#include "Application.h"

class ExampleLayer : public GUI::Layer
{
public:
  virtual void OnUIRender() override
  {
    ImGui::Begin("Hello");
    ImGui::Button("Button");
    ImGui::End();

    ImGui::ShowDemoWindow();
  }

  virtual void OnUpdate(float ts)
  {
    
  }
};