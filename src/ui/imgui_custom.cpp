#include "imgui_custom.h"

bool ImGui::WinButton(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.43f, 0.43f, 0.43f, 0.47f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.43f, 0.43f, 0.43f, 0.47f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.43f, 0.43f, 0.43f, 0.47f));

	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");

	if (hovered)
	{
		if (pressed || held)
		{
			window->DrawList->AddRectFilled(bb.Min, bb.Max, IM_COL32(154, 154, 154, 255));
			
			ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.6f));
			RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
			ImGui::PopStyleVar();
		}
		else
		{
			window->DrawList->AddRect(bb.Min, bb.Max, IM_COL32(154, 154, 154, 255), 0.0f, ImDrawFlags_None, 2.0f);
			RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
		}
	}
	else
	{
		RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}

bool ImGui::WinTextLinkButton(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.47f, 0.84f, 1.00f));
	RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	ImGui::PopStyleColor();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}

bool ImGui::WinToggle(const char* label, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw = ImGui::GetWindowDrawList();

	float height = 13.2f;
	float width = height * 1.5f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(label, ImVec2(width + 8.f, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.2f;
	if (g.LastActiveId == g.CurrentWindow->GetID(label))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}
	ImGui::ItemSize(ImVec2(0, 0));

	draw->AddRect(p, ImVec2(p.x + width + 8.f, p.y + height + 3.f), IM_COL32(153, 153, 153, 255), height * 0.5f);
	draw->AddRectFilled(p, ImVec2(p.x + width + 8.f, p.y + height + 3.f), ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 0.0f), ImVec4(0.f, 120 / 255.f, 215 / 255.f, 1.f), t)), height * 0.5f);
	draw->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 1.2f) + 2.f, p.y + radius + 1.5f), radius - 2.5f, IM_COL32_WHITE);

	draw->AddText(p + ImVec2(35, 0), IM_COL32(255, 255, 255, 255), label);

	return true;
}