// Copyright (c) Maia

#include <raylib.h>
#include <raymath.h>

#include "maia/gui/imgui_extensions.h"

namespace maia {

// void DrawCameraMesh(const Camera3D& camera) {
//   auto pos = camera.target;
//   pos.z += 10;
//   DrawSphereWires(pos, 64, 64, 64, RED);
// }

void DrawCameraMesh(const Camera3D& camera) {
  Vector3 forward =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  float distance_from_target = 30.0f;
  Vector3 offset = Vector3Scale(forward, distance_from_target);
  Vector3 sphere_pos = Vector3Add(camera.target, offset);
  DrawSphereWires(sphere_pos, 15.0f, 16, 16, RED);
}

void Run() {
  RenderTexture2D tex = LoadRenderTexture(256, 256);

  static Camera3D camera{};

  gui::ImGuiInit();

  while (!WindowShouldClose()) {
    BeginTextureMode(tex);
    {
      ClearBackground(BLANK);

      DrawTriangle(
          {.x = 0, .y = 0}, {.x = 64, .y = 64}, {.x = 128, .y = 0}, RED);

      DrawRectangle(0, 0, 64, 64, BLUE);

      BeginMode3D(camera);
      {
      }
      EndMode3D();
    }
    EndTextureMode();

    BeginDrawing();
    {
      ClearBackground(BLANK);

      DrawTexture(tex.texture, 0, 0, WHITE);

      gui::ImGuiBeginFrame();
      if (ImGui::Begin("win")) {
        gui::ImGuiImage(tex.texture);
      }
      ImGui::End();
      gui::ImGuiEndFrame();

      DrawCameraMesh(camera);
    }
    EndDrawing();
  }

  gui::ImGuiTerminate();

  UnloadRenderTexture(tex);
}

}  // namespace maia

int main() {
  constexpr int kScreenWidth = 800;
  constexpr int kScreenHeight = 600;

  InitWindow(kScreenWidth, kScreenHeight, "Maia - CamCalib");

  // maia::Run();

  RenderTexture2D tex = LoadRenderTexture(256, 256);

  // Define the camera to look into our 3d world
  Camera3D camera{};
  camera.position = Vector3{.x = 0.0f, .y = 0.0f, .z = 1.0f};
  camera.target = Vector3{.x = 0.0f, .y = 0.0f, .z = 0.0f};
  camera.up = Vector3{.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  // camera.position = Vector3(0, 0, 15);

  namespace gui = maia::gui;

  gui::ImGuiInit();

  while (!WindowShouldClose()) {
    BeginTextureMode(tex);
    {
      ClearBackground(BLANK);

      // DrawTriangle(
      //     {.x = 0, .y = 0}, {.x = 64, .y = 64}, {.x = 128, .y = 0}, RED);

      // DrawRectangle(0, 0, 64, 64, BLUE);

      BeginMode3D(camera);
      {
        maia::DrawCameraMesh(camera);
      }
      EndMode3D();
    }
    EndTextureMode();

    BeginDrawing();
    {
      ClearBackground(BLANK);

      // DrawTexture(tex.texture, 0, 0, WHITE);

      gui::ImGuiBeginFrame();
      if (ImGui::Begin("win")) {
        gui::ImGuiImage(tex.texture);
      }
      ImGui::End();

      gui::ImGuiEndFrame();
    }
    EndDrawing();
  }

  gui::ImGuiTerminate();

  UnloadRenderTexture(tex);

  CloseWindow();

  return 0;
}
