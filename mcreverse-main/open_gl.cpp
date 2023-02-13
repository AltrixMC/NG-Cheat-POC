#include <mutex>

#include "c_minecraft.h"
#include "c_gameinstance.h"
#include "c_glrender.h"
#include "c_gltext.h"

#include "c_visuals.h"
#include "c_esp.h"
#include "c_gui.h"
#include "wrapper.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl2.h"
#include "imgui/imgui_impl_win32.h"

fn_wgl_swap_buffers original_wgl_swap_buffers;

bool isOpen = false;
c_gui* gui = new c_gui();

bool __stdcall hooks::wgl_swap_buffers(_In_ HDC hdc) {
	gui->initialize(hdc);

	/*
	HDC m_curhdc = hdc;
	if (!c_gltext::get().m_fontbuilt || m_curhdc != c_gltext::get().m_fonthdc) {
		c_gltext::get().build_fonts();
		return original_wgl_swap_buffers(hdc);
	}

	c_glrender::get().setup_ortho();

	unsigned char color[3]{ 0, 128, 255 };
	c_gltext::get().print(2, 15, color, xor ("reverse-minecraft"));

	// run our visuals here.
	//c_esp::get( ).handle( );
	c_visuals::get().handle();

	c_glrender::get().restore_gl();



	if (GetAsyncKeyState(VK_INSERT) & 0x8000) isOpen = !isOpen;
	/*if (isOpen)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;

		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Chanceware", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Checkbox("Fly enabled", &minecraft->fly);
		ImGui::SliderFloat("Speed", &minecraft->speed, 1.0f, 10.0f, "%.1f");
		

		ImGui::End();

		ImGui::Render();
		glViewport(0, 0, window_rect.right, window_rect.top);
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}#1#

	ImGuiIO& io = ImGui::GetIO();
	io.MouseDrawCursor = false;

	wglMakeCurrent(gl_context->m_hdc_devicectx, gl_context->m_oglrenderctx);
	*/

	return original_wgl_swap_buffers(hdc);
}

LRESULT imgui_wndproc(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg) {
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_XBUTTONDOWN:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = true;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = true;
		return true;
	case WM_XBUTTONUP:
		if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON1) == MK_XBUTTON1)
			io.MouseDown[3] = false;
		else if ((GET_KEYSTATE_WPARAM(wParam) & MK_XBUTTON2) == MK_XBUTTON2)
			io.MouseDown[4] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

long __stdcall hooks::wndproc( const HWND window_handle, unsigned int user_msg, uintptr_t wparam, long lparam ) {

	if ( user_msg == WM_LBUTTONDOWN ) {
		c_visuals::get( ).add_click( );
	}

	if (gui->open && imgui_wndproc(window_handle, user_msg, wparam, lparam))
		return 1l;

	return CallWindowProcA( c_main::get( ).m_wndproc, window_handle, user_msg, wparam, lparam );
}