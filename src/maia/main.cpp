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

void DrawCamera(const Camera3D& camera,
                float size = 1.f,
                float distance = 2.f) {
  // Camera's Orientation Vectors.
  const Eigen::Vector3f cam_pos = ToEigenVec(camera.position);
  const Eigen::Vector3f cam_target = ToEigenVec(camera.target);
  const Eigen::Vector3f forward = (cam_target - cam_pos).normalized();
  const Eigen::Vector3f right =
      Eigen::Vector3f(0.0f, 1.0f, 0.0f).cross(forward).normalized();
  Eigen::Vector3f up = forward.cross(right).normalized();

  // Transformation Matrix.
  Eigen::Affine3f transform = Eigen::Affine3f::Identity();
  transform.matrix().col(0).head<3>() = right;
  transform.matrix().col(1).head<3>() = up;
  transform.matrix().col(2).head<3>() = forward;
  transform.matrix().col(3).head<3>() = cam_pos;

  // Pyramid's vertices in LOCAL space.
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

  // Transform local vertices into WORLD space.
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

  rlBegin(RL_TRIANGLES);

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

  // Pyramid base (Gray).
  rlColor3f(0.5f, 0.5f, 0.5f);
  create_triangle(world_tl, world_bl, world_br);
  create_triangle(world_tl, world_br, world_tr);

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

// Helper to convert Eigen Vector3f to Raylib Vector3
::Vector3 ToRayVec(const Eigen::Vector3f& vec) {
  return ::Vector3{.x = vec.x(), .y = vec.y(), .z = vec.z()};
}

void UpdateBlenderCamera(Camera3D& camera) {
  constexpr float kRotateSpeed = 0.005f;
  constexpr float kPanSpeed = 0.02f;
  constexpr float kZoomSpeed = 2.0f;
  const ::Vector2 mouse_delta = GetMouseDelta();

  Eigen::Vector3f camera_pos = ToEigenVec(camera.position);
  Eigen::Vector3f camera_target = ToEigenVec(camera.target);
  Eigen::Vector3f world_up(0.0f, 1.0f, 0.0f);
  bool is_middle_down = IsMouseButtonDown(MOUSE_MIDDLE_BUTTON);

  float wheel_move = GetMouseWheelMove();
  if (wheel_move != 0) {
    // Zoom. Move camera along the view vector.
    Eigen::Vector3f view_vec = (camera_pos - camera_target).normalized();
    camera_pos -= view_vec * wheel_move * kZoomSpeed;
  } else if (is_middle_down && IsKeyDown(KEY_LEFT_SHIFT)) {
    // Pan (Shift + MMB).
    Eigen::Vector3f forward = (camera_target - camera_pos).normalized();
    Eigen::Vector3f right = world_up.cross(forward).normalized();
    Eigen::Vector3f up = forward.cross(right);

    Eigen::Vector3f pan_vec =
        (right * mouse_delta.x * kPanSpeed) + (up * mouse_delta.y * kPanSpeed);

    camera_pos += pan_vec;
    camera_target += pan_vec;
  } else if (is_middle_down) {
    // Orbit (MMB).
    Eigen::Vector3f view_vec = camera_pos - camera_target;

    Eigen::Quaternionf yaw_rot(
        Eigen::AngleAxisf(-mouse_delta.x * kRotateSpeed, world_up));

    Eigen::Vector3f right = world_up.cross(view_vec.normalized()).normalized();

    Eigen::Quaternionf pitch_rot(
        Eigen::AngleAxisf(-mouse_delta.y * kRotateSpeed, right));

    Eigen::Quaternionf total_rotation = yaw_rot * pitch_rot;

    view_vec = total_rotation * view_vec;
    camera_pos = camera_target + view_vec;
  }

  camera.position = ToRayVec(camera_pos);
  camera.target = ToRayVec(camera_target);
}

}  // namespace maia

int main() {
  namespace gui = maia::gui;

  constexpr int kScreenWidth = 1600;
  constexpr int kScreenHeight = 900;

  // SetConfigFlags(ConfigFlags::FLAG_MSAA_4X_HINT);
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

  gui::ImGuiInit();

  while (!WindowShouldClose()) {
    maia::MoveCamera(camera);
    maia::UpdateBlenderCamera(camera_scene);

    BeginTextureMode(tex);
    {
      ClearBackground(BLANK);

      DrawFPS(32, 32);
      BeginMode3D(camera);
      {
        maia::DrawCamera(camera);
        DrawGrid(10, 1.0f);  // Draw a grid for context
        maia::DrawCameraMesh(camera);
        maia::DrawCamera(camera_scene);
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
        ImGui::SetNextWindowContentSize({800, 600});
        if (ImGui::Begin("win")) {
          gui::ImGuiImageRect(tex.texture,
                              {.x = 0, .y = 0, .width = 800, .height = -600});
        }
        ImGui::End();

        ImGui::SetNextWindowContentSize({800, 600});
        if (ImGui::Begin("win scene")) {
          gui::ImGuiImageRect(tex_scene.texture,
                              {.x = 0, .y = 0, .width = 800, .height = -600});
          // if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
          //   auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
          //   float speed = 0.02f;
          //   camera_scene.target.x += delta.x * speed;
          //   camera_scene.target.y -= delta.y * speed;
          // }
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
