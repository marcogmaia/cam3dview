// Copyright (c) Maia

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include "maia/gui/imgui_extensions.h"

namespace maia {

void DrawCameraMesh(const Camera3D& camera) {
  // Vector3 forward =
  //     Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  // float distance_from_target = 10.0f;
  // Vector3 offset = Vector3Scale(forward, distance_from_target);
  // Vector3 sphere_pos = Vector3Add(camera.target, offset);
  DrawSphereWires(camera.position, 1.0f, 16, 16, RED);
}

void DrawCamera(const Camera3D& /*camera*/) {
  rlBegin(RL_TRIANGLES);
  // Front face (Red)
  rlColor3f(1.0f, 0.0f, 0.0f);     // Set color for the next vertices
  rlVertex3f(0.0f, 1.0f, 0.0f);    // Top vertex (Apex)
  rlVertex3f(-1.0f, -1.0f, 1.0f);  // Bottom-left vertex
  rlVertex3f(1.0f, -1.0f, 1.0f);   // Bottom-right vertex

  // Right face (Green)
  rlColor3f(0.0f, 1.0f, 0.0f);
  rlVertex3f(0.0f, 1.0f, 0.0f);    // Apex
  rlVertex3f(1.0f, -1.0f, 1.0f);   // Bottom-left vertex
  rlVertex3f(1.0f, -1.0f, -1.0f);  // Bottom-right vertex

  // Back face (Blue)
  rlColor3f(0.0f, 0.0f, 1.0f);
  rlVertex3f(0.0f, 1.0f, 0.0f);     // Apex
  rlVertex3f(1.0f, -1.0f, -1.0f);   // Bottom-left vertex
  rlVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom-right vertex

  // Left face (Yellow)
  rlColor3f(1.0f, 1.0f, 0.0f);
  rlVertex3f(0.0f, 1.0f, 0.0f);     // Apex
  rlVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom-left vertex
  rlVertex3f(-1.0f, -1.0f, 1.0f);   // Bottom-right vertex

  // --- Define the square base using two triangles (Gray) ---
  rlColor3f(0.5f, 0.5f, 0.5f);

  // First triangle for the base
  rlVertex3f(-1.0f, -1.0f, -1.0f);
  rlVertex3f(1.0f, -1.0f, 1.0f);
  rlVertex3f(-1.0f, -1.0f, 1.0f);

  // Second triangle for the base
  rlVertex3f(-1.0f, -1.0f, -1.0f);
  rlVertex3f(1.0f, -1.0f, -1.0f);
  rlVertex3f(1.0f, -1.0f, 1.0f);
  rlEnd();
}

void MoveCamera(Camera3D& camera) {
  float delta = 0.1f;
  // clang-format off
  if (IsKeyDown(KEY_W)) { camera.position.y += delta; }
  if (IsKeyDown(KEY_A)) { camera.position.x -= delta; }
  if (IsKeyDown(KEY_S)) { camera.position.y -= delta; }
  if (IsKeyDown(KEY_D)) { camera.position.x += delta; }
  // clang-format on
}

}  // namespace maia

int main() {
  constexpr int kScreenWidth = 1024;
  constexpr int kScreenHeight = 768;

  InitWindow(kScreenWidth, kScreenHeight, "Maia - CamCalib");
  SetTargetFPS(120);

  RenderTexture2D tex = LoadRenderTexture(800, 600);
  RenderTexture2D tex_scene = LoadRenderTexture(800, 600);

  // Define the camera to look into our 3d world
  Camera3D camera{};
  camera.position = Vector3{.x = 0.0f, .y = 0.0f, .z = 10.0f};
  camera.target = Vector3{.x = 0.0f, .y = 0.0f, .z = 0.0f};
  camera.up = Vector3{.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera.fovy = 60.0f;
  camera.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  Camera3D camera_scene{};
  camera_scene.position = Vector3{.x = 10.0f, .y = 10.0f, .z = 20.0f};
  camera_scene.target = Vector3{.x = 0.0f, .y = 0.0f, .z = 0.0f};
  camera_scene.up = Vector3{.x = 0.0f, .y = 1.0f, .z = 0.0f};
  camera_scene.fovy = 60.0f;
  camera_scene.projection = CAMERA_PERSPECTIVE;  // Camera projection type

  // camera.position = Vector3(0, 0, 15);

  namespace gui = maia::gui;

  gui::ImGuiInit();

  while (!WindowShouldClose()) {
    maia::MoveCamera(camera);

    BeginTextureMode(tex);
    {
      ClearBackground(BLANK);

      DrawFPS(32, 32);
      BeginMode3D(camera);
      {
        maia::DrawCamera(camera);
        DrawGrid(10, 1.0f);  // Draw a grid for context
        maia::DrawCameraMesh(camera);
      }
      EndMode3D();
    }
    EndTextureMode();

    // The scene fixed camera
    BeginTextureMode(tex_scene);
    {
      ClearBackground(BLANK);
      BeginMode3D(camera_scene);
      {
        maia::DrawCamera(camera);
        DrawGrid(10, 1.0f);  // Draw a grid for context
        maia::DrawCameraMesh(camera);
      }
      EndMode3D();
    }
    EndTextureMode();

    BeginDrawing();
    {
      ClearBackground(WHITE);

      maia::DrawCamera(camera);

      gui::ImGuiBeginFrame();
      {
        ImGui::DockSpaceOverViewport();
        if (ImGui::Begin("win")) {
          gui::ImGuiImageRect(tex.texture,
                              {.x = 0, .y = 0, .width = 800, .height = -600});
          // auto delta = ImGui::GetMouseDragDelta();
          // camera.position.x = delta.x;
          // camera.position.y = -delta.y;
        }
        ImGui::End();

        if (ImGui::Begin("win scene")) {
          gui::ImGuiImageRect(tex_scene.texture,
                              {.x = 0, .y = 0, .width = 800, .height = -600});
          // auto delta = ImGui::GetMouseDragDelta();
          // camera.position.x = delta.x;
          // camera.position.y = -delta.y;
        }
        ImGui::End();
      }
      gui::ImGuiEndFrame();
    }
    EndDrawing();
  }

  gui::ImGuiTerminate();

  UnloadRenderTexture(tex);

  CloseWindow();

  return 0;
}
