// Copyright (c) Maia

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <Eigen/Geometry>

#include "maia/gui/imgui_extensions.h"

namespace maia {

void DrawCameraMesh(const Camera3D& camera) {
  DrawSphereWires(camera.position, 1.0f, 16, 16, RED);
}

Eigen::Vector3f ToEigenVec(const ::Vector3& vec) {
  return Eigen::Vector3f{vec.x, vec.y, vec.z};
}

void DrawCamera(const Camera3D& camera, float size = 1, float distance = 2) {
  // 1. Define the Camera's Orientation Vectors.
  Eigen::Vector3f cam_pos = ToEigenVec(camera.position);
  Eigen::Vector3f cam_target = ToEigenVec(camera.target);
  Eigen::Vector3f forward = (cam_target - cam_pos).normalized();
  Eigen::Vector3f right =
      Eigen::Vector3f(0.0f, 1.0f, 0.0f).cross(forward).normalized();
  Eigen::Vector3f up = forward.cross(right).normalized();

  // 2. Construct the Transformation Matrix.
  Eigen::Affine3f transform = Eigen::Affine3f::Identity();
  transform.matrix().col(0).head<3>() = right;
  transform.matrix().col(1).head<3>() = up;
  transform.matrix().col(2).head<3>() = forward;
  transform.matrix().col(3).head<3>() = cam_pos;

  // 3. Define the Pyramid's vertices in LOCAL space.
  // The tip is at the origin (0,0,0) and the base is 'distance' units forward
  // along the local Z-axis.
  constexpr float kAspect = 16.0f / 9.0f;
  const float width = size * kAspect;
  const float height = size;

  Eigen::Vector3f local_tip(0.0f, 0.0f, 0.0f);
  Eigen::Vector3f local_base_tl(-width, height, distance);
  Eigen::Vector3f local_base_tr(width, height, distance);
  Eigen::Vector3f local_base_bl(-width, -height, distance);
  Eigen::Vector3f local_base_br(width, -height, distance);

  // 4. Transform local vertices into WORLD space.
  Eigen::Vector3f world_tip = transform * local_tip;
  Eigen::Vector3f world_tl = transform * local_base_tl;
  Eigen::Vector3f world_tr = transform * local_base_tr;
  Eigen::Vector3f world_bl = transform * local_base_bl;
  Eigen::Vector3f world_br = transform * local_base_br;

  const auto create_triangle = [](const Eigen::Vector3f& a,
                                  const Eigen::Vector3f& b,
                                  const Eigen::Vector3f& c) {
    rlVertex3f(a.x(), a.y(), a.z());
    rlVertex3f(b.x(), b.y(), b.z());
    rlVertex3f(c.x(), c.y(), c.z());
  };

  rlEnableWireMode();
  // rlBegin(RL_TRIANGLES);
  rlBegin(RL_LINES);

  // Front face (Red).
  rlColor3f(1.0f, 0.0f, 0.0f);
  create_triangle(world_tip, world_bl, world_tl);

  // Right face (Green).
  rlColor3f(0.0f, 1.0f, 0.0f);
  create_triangle(world_tip, world_br, world_bl);

  // Back face (Blue)
  rlColor3f(0.0f, 0.0f, 1.0f);
  create_triangle(world_tip, world_tr, world_br);

  // Left face (Yellow)
  rlColor3f(1.0f, 1.0f, 0.0f);
  create_triangle(world_tip, world_tl, world_tr);

  // Pyramid base (Gray) - Note the corrected vertex order for two triangles
  rlColor3f(0.5f, 0.5f, 0.5f);
  create_triangle(world_tl, world_bl, world_br);
  create_triangle(world_tl, world_br, world_tr);

  rlEnd();
  rlDisableWireMode();
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
        }
        ImGui::End();

        if (ImGui::Begin("win scene")) {
          gui::ImGuiImageRect(tex_scene.texture,
                              {.x = 0, .y = 0, .width = 800, .height = -600});
          if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
            float speed = 0.02f;
            camera_scene.target.x += delta.x * speed;
            camera_scene.target.y -= delta.y * speed;
          }
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
