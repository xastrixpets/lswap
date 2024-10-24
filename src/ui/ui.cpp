#include "ui.h"
#include "ui_resources.h"

#include "../common.h"
#include "../globals.h"
#include "../utils/utils.h"

#include <locale>
#include <curl/curl.h>

static LPDIRECT3D9           g_d3d = {};
static LPDIRECT3DDEVICE9     g_device = {};
static D3DPRESENT_PARAMETERS g_d3d_parameters = {};
static HWND                  g_window = {};
static WNDCLASSEX            g_wc = {};
static bool                  g_hold_proc = {};
static bool                  g_service_enabled = true;
static ImFont*               g_verdanab_font = {};
static ImFont*               g_big_font = {};
static IDirect3DTexture9*    g_info_texture = {};
static IDirect3DTexture9*    g_file_texture = {};
static IDirect3DTexture9*    g_home_texture = {};
static IDirect3DTexture9*    g_logo_texture = {};
static int                   g_tab_index = {};
static int                   g_rect_pos = {};

static LRESULT CALLBACK wnd_proc_h(HWND h, UINT m, WPARAM w, LPARAM l);
static bool create_device();
static void cleanup_device();
static void curl_escape_url(CURL* curl, const std::wstring& in, std::string& url);

ui_stat ui::init()
{
	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		wnd_proc_h, 0L, 0L,
		GetModuleHandle(NULL),
		0, 0, 0, 0,
		LSWAP_APPLICATION_CLASS_NAME, 0
	};
	wc.style = WS_EX_TOOLWINDOW;

	RegisterClassEx(&wc);

	g_window = CreateWindow(wc.lpszClassName, LSWAP_APPLICATION_NAME, WS_POPUP, 0, 0, 1, 1, NULL, NULL, wc.hInstance, NULL);

	if (!create_device()) {
		cleanup_device();
		return UI_FAIL;
	}

	ShowWindow(g_window, SW_HIDE);
	UpdateWindow(g_window);

	ImGui::CreateContext();

	ImGui::GetIO().IniFilename = {};
	ImGui::GetIO().LogFilename = {};

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguihis.ttf", 14.0f);
	
	g_verdanab_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanab.ttf", 11.0f);
	g_big_font = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\seguihis.ttf", 24.0f);
	
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_window);
	ImGui_ImplDX9_Init(g_device);

	AddClipboardFormatListener(g_window);

	D3DXCreateTextureFromFileInMemoryEx(g_device, &info_ico, sizeof(info_ico),
		21, 21, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &g_info_texture);

	D3DXCreateTextureFromFileInMemoryEx(g_device, &file_ico, sizeof(file_ico),
		16, 20, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &g_file_texture);

	D3DXCreateTextureFromFileInMemoryEx(g_device, &home_ico, sizeof(home_ico),
		21, 21, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &g_home_texture);

	D3DXCreateTextureFromFileInMemoryEx(g_device, &logo, sizeof(logo),
		194, 66, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &g_logo_texture);

	return UI_OK;
}

void ui::handler()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	g_rect_pos = 36;

	std::string src_lang{ g::cfg.source_lang };
	std::string target_lang{ g::cfg.target_lang };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

			ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));

			ImGui::SetNextWindowPos(ImVec2((g::monitor_width - g::window_width) * 0.5f,
				(g::monitor_height - g::window_height) * 0.5f), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(g::window_width, g::window_height), ImGuiCond_Once);

			if (ImGui::Begin("##", 0,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoResize))
			{
				ImGui::SetCursorPos(ImVec2(10.0f, 5.5f));
				ImGui::Text(g_service_enabled ? LSWAP_APPLICATION_NAME " (Running)" : LSWAP_APPLICATION_NAME);

				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + 30),
					ImVec2(ImGui::GetWindowPos().x + 30, ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
					IM_COL32(110, 110, 110, 120),
					0, 0);

				ImGui::GetWindowDrawList()->AddRectFilled(
					ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + g_rect_pos),
					ImVec2(ImGui::GetWindowPos().x + 3, ImGui::GetWindowPos().y + g_rect_pos + 30),
					IM_COL32(0, 120, 215, 255),
					0, 0);

				{
					ImGui::PushFont(g_verdanab_font);

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.51f, 0.20f, 0.20f, 1.00f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.71f, 0.20f, 0.20f, 1.00f));

					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 30, 0));

					if (ImGui::Button("X", ImVec2(30, 30)))
						PostMessage(g_window, WM_QUIT, 0, 0);

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.00f, 1.00f, 1.00f, 0.04f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.00f, 1.00f, 1.00f, 0.08f));

					ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 60, 0));

					if (ImGui::Button("-", ImVec2(30, 30)))
						ShowWindow(g_window, SW_MINIMIZE);

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();

					ImGui::PopFont();
				}

				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));

					ImGui::SetCursorPos(ImVec2(3, 40));
					if (ImGui::ImageButton("##HOME_ICO", g_home_texture, ImVec2(16, 16)))
						g_tab_index = 0, g_rect_pos = 36;

					ImGui::SetCursorPos(ImVec2(3, 75));
					if (ImGui::ImageButton("##FILE_ICO", g_file_texture, ImVec2(16, 20)))
						g_tab_index = 1, g_rect_pos = 73;

					ImGui::SetCursorPos(ImVec2(3, ImGui::GetWindowHeight() - 27));
					if (ImGui::ImageButton("##INFO_ICO", g_info_texture, ImVec2(16, 16)))
						g_tab_index = 2, g_rect_pos = ImGui::GetWindowHeight() - 32;

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
				}

				ImGui::SetCursorPos(ImVec2(35, 30));

				switch (g_tab_index) {
				case 0: {
					if (ImGui::BeginChild("##HomeChild"))
					{
						ImGui::PushFont(g_big_font);

						ImGui::SetCursorPos(ImVec2(40, 20));
						ImGui::Text("Home");
						
						ImGui::PopFont();

						ImGui::SetCursorPos(ImVec2(40, 47));
						ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 0.43f), "Running the program");

						ImGui::SetCursorPos(ImVec2(40, 80));
						if (ImGui::WinButton(g_service_enabled ? "Stop Service" : "Run Service", ImVec2(120, 25)))
							g_service_enabled = !g_service_enabled;

						ImGui::SetCursorPos(ImVec2(176, 85));
						if (ImGui::WinToggle("In autorun", &g::in_autorun))
							utils::add_reg_to_autorun(LSWAP_APPLICATION_NAME, g::in_autorun);

						ImGui::EndChild();
					}
					break;
				}
				case 1: {
					if (ImGui::BeginChild("##ConfigurationChild"))
					{
						ImGui::PushFont(g_big_font);

						ImGui::SetCursorPos(ImVec2(40, 20));
						ImGui::Text("Config");

						ImGui::PopFont();

						ImGui::SetCursorPos(ImVec2(40, 47));
						ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 0.43f), "Changing configuration settings");

						ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.32f, 0.32f, 0.32f, 0.54f));
						ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(1.00f, 1.00f, 1.00f, 0.08f));

						ImGui::SetCursorPos(ImVec2(40, 80));
						ImGui::InputText("Source lang", utils::X(src_lang), utils::X(src_lang).len());

						ImGui::SetCursorPos(ImVec2(40, 105));
						ImGui::InputText("Target lang", utils::X(target_lang), utils::X(target_lang).len());

						ImGui::SetCursorPos(ImVec2(190, 140));
						if (ImGui::WinButton("Update config", ImVec2(120, 25))) {
							if (config::change_cfg_values(src_lang, target_lang)) {
								g::cfg = config::init();
							}
						}

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();

						ImGui::EndChild();
					}
					break;
				}
				case 2: {
					if (ImGui::BeginChild("##InfoChild"))
					{
						ImGui::SetCursorPos(ImVec2(67, 40));
						ImGui::Image(g_logo_texture, ImVec2(194, 66));

						ImGui::SetCursorPos(ImVec2(175, 115));
						ImGui::Text("%s %s", LSWAP_APPLICATION_NAME, LSWAP_VERSION_STRING);

						ImGui::SetCursorPos(ImVec2(67, 145));
						if (ImGui::WinTextLinkButton("Open repository")) {
							ShellExecute(0, "open", "https://www.github.com/xastrix/lswap", 0, 0, 5);
						}

						ImGui::SetCursorPos(ImVec2(67, 165));
						if (ImGui::WinTextLinkButton("Open author github profile")) {
							ShellExecute(0, "open", "https://www.github.com/xastrix", 0, 0, 5);
						}

						ImGui::EndChild();
					}
					break;
				}
				}

				ImGui::End();
			}

			ImGui::PopStyleColor();

			ImGui::PopStyleVar();
		}
		ImGui::EndFrame();

		g_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		if (g_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

			g_device->EndScene();
		}

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		const HRESULT result = g_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && g_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();

			const HRESULT hr = g_device->Reset(&g_d3d_parameters);

			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);

			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
}

void ui::undo()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_device();

	RemoveClipboardFormatListener(g_window);
	UnregisterClass(g_wc.lpszClassName, g_wc.hInstance);
}

static LRESULT CALLBACK wnd_proc_h(HWND h, UINT m, WPARAM w, LPARAM l)
{
	LRESULT ImGui_ImplWin32_WndProcHandler(HWND h, UINT m, WPARAM w, LPARAM l);
	ImGui_ImplWin32_WndProcHandler(h, m, w, l);

	auto ret = DefWindowProc(h, m, w, l);
	if (!g_service_enabled)
		return ret;

	switch (m) {
	case WM_CLIPBOARDUPDATE: {
		if (g_hold_proc) {
			g_hold_proc = false;
			break;
		}

		CURL* curl = curl_easy_init();
		CURLcode res;

		if (curl)
		{
			std::wstring buffer;
			std::wstring clipboard = utils::remove_chars(utils::get_current_clipboard(g_window), FORBIDDEN_CHARS);

			std::string url;
			curl_escape_url(curl, clipboard, url = std::string{
				"https://translate.googleapis.com/translate_a/single?client=gtx&sl=" +
				g::cfg.source_lang + "&tl=" + g::cfg.target_lang + "&dt=t&q="
				});

			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(curl, CURLOPT_USERAGENT, LSWAP_APPLICATION_NAME "/" LSWAP_VERSION_STRING);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
			curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

			res = curl_easy_perform(curl);

			if (res == CURLE_OK)
			{
				std::wstring text = utils::parse_json(buffer);

				if (!text.empty())
				{
					utils::put_in_clipboard(g_window, text);
					g_hold_proc = true;
				}
			}
		}

		curl_global_cleanup();
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}
	
	return ret;
}

static bool create_device()
{
	if ((g_d3d = Direct3DCreate9(D3D_SDK_VERSION)) == 0)
		return false;

	ZeroMemory(&g_d3d_parameters, sizeof(g_d3d_parameters));

	g_d3d_parameters.Windowed = TRUE;
	g_d3d_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3d_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3d_parameters.EnableAutoDepthStencil = TRUE;
	g_d3d_parameters.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3d_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (g_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3d_parameters, &g_device) < 0)
		return false;

	return true;
}

static void cleanup_device()
{
	if (g_device)
	{
		g_device->Release();
		g_device = nullptr;
	}

	if (g_d3d)
	{
		g_d3d->Release();
		g_d3d = nullptr;
	}
}

static void curl_escape_url(CURL* curl, const std::wstring& in, std::string& url)
{
	char* esc = curl_easy_escape(curl, utils::to_utf8(in).c_str(), 0);

	if (esc) {
		url += esc;
		curl_free(esc);
	}
}