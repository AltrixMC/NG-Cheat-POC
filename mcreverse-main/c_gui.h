#pragma once

#include <mutex>

#include "c_minecraft.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

class c_gui {
public:
	bool open = false;
	HWND game_window;

	void initialize(HDC dc) {
		static bool wasPressed = false;
		std::once_flag once;
		std::call_once(once, [&]() {
			game_window = WindowFromDC(dc);
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(game_window);
		ImGui_ImplOpenGL2_Init();
			});

		if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) {
			if (wasPressed == false)
			{
				wasPressed = true;
				open = !open;
			}
			wasPressed = true;
		} else wasPressed = false;

		if (open) {
			ImGuiIO& io = ImGui::GetIO();
			io.MouseDrawCursor = true;

			ImGui_ImplOpenGL2_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Chanceware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Checkbox("Fly enabled", &minecraft->fly);
			ImGui::SliderFloat("Fly Speed", &minecraft->fly_speed, 0.2f, 10.0f, "%.2f");
			ImGui::SliderFloat("Block Reach", &minecraft->block_reach, 0.2f, 10.0f, "%.2f");
			ImGui::SliderFloat("Step Height", &minecraft->step_height, 1.0f, 10.0f, "%.1f");
			ImGui::Checkbox("Aimbot", &minecraft->aimbot);
			ImGui::Checkbox("Destroy", &minecraft->destroy);

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
		}
	}
};
