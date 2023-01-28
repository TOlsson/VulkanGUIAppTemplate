// VulkanRenderer.cpp : Defines the entry point for the application.
//

#include "VulkanRenderer.h"
#include "Entrypoint.h"

#include "Image.h"

GUI::Application* GUI::CreateApplication(int argc, char** argv)
{
  GUI::ApplicationSpecification spec;
  spec.Name = "GUI Example";

  GUI::Application* app = new GUI::Application(spec);
  app->PushLayer<ExampleLayer>();
  app->SetMenubarCallback([app]()
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("Exit"))
        {
          app->Close();
        }
        ImGui::EndMenu();
      }
    });
  return app;
}

