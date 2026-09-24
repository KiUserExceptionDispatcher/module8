#include "../settings/functions.h"
#include <unordered_map>
#include <vector>
#include <string>

struct child_subtab
{
    std::string name;
};

static std::unordered_map<ImGuiID, int> child_subtab_map;
static std::unordered_map<ImGuiID, std::vector<child_subtab>> subtab_list_map;

bool begin_child_ex(const char* name, ImGuiID id, int x, int y, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags, bool warning);

int c_gui::get_child_subtab(ImGuiID id)
{
    return child_subtab_map[id];
}

static void set_child_subtab(ImGuiID id, int v)
{
    child_subtab_map[id] = v;
}

static void set_child_subtabs(ImGuiID id, const std::vector<std::string>& names)
{
    auto& tabs = subtab_list_map[id];
    if (tabs.size() != names.size())
    {
        tabs.clear();
        tabs.reserve(names.size());
        for (const auto& n : names)
            tabs.push_back({ n });
        if (child_subtab_map.find(id) == child_subtab_map.end())
            child_subtab_map[id] = 0;
    }
    else
    {
        for (size_t i = 0; i < names.size(); i++)
            tabs[i].name = names[i];
    }
}

static void draw_child_subtabs(ImGuiWindow* parent_window, ImGuiID id, const ImVec2& size_arg)
{
    auto& tabs = subtab_list_map[id];
    if (tabs.empty())
        return;

    int active = c_gui::get_child_subtab(id);
    const int count = static_cast<int>(tabs.size());
    const float tab_h = var->window.titlebar + 4.f;
    const ImVec2 base = parent_window->DC.CursorPos;
    ImDrawList* dl = parent_window->DrawList;

    const float inner_x = base.x + 1.f;
    const float inner_w = size_arg.x - 2.f;
    const float last_x = inner_x + inner_w;
    for (int i = 0; i < count; i++)
    {
        const float x0 = inner_x + IM_TRUNC(inner_w * i / count);
        const float x1 = (i == count - 1) ? last_x : inner_x + IM_TRUNC(inner_w * (i + 1) / count);
        const ImVec2 p_min(x0, base.y);
        const ImVec2 p_max(x1, base.y + tab_h);
        const bool hovered = IsMouseHoveringRect(p_min, p_max);
        const bool selected = (i == active);

        if (hovered && IsMouseClicked(0))
            active = i;

        if (selected || hovered)
            draw->fade_rect_filled(dl, p_min, p_max, draw->get_clr(clr->window.background_two), draw->get_clr(clr->window.background_one), fade_direction::vertically);
        else
            draw->rect_filled(dl, p_min, p_max, draw->get_clr(clr->window.background_one));

        if (i > 0)
            draw->line(dl, ImVec2(x0, p_min.y + 1), ImVec2(x0, p_max.y), draw->get_clr(clr->window.stroke));

        if (selected)
            draw->line(dl, ImVec2(p_min.x + 1, p_max.y - 1), ImVec2(p_max.x - 1, p_max.y - 1), draw->get_clr(clr->window.background_one));
        else
            draw->line(dl, ImVec2(p_min.x, p_max.y - 1), ImVec2(p_max.x + 1, p_max.y - 1), draw->get_clr(clr->window.stroke));

        draw->text_clipped_outline(dl, var->font.tahoma, p_min + ImVec2(0, 1), p_max + ImVec2(0, 1), selected ? draw->get_clr(clr->accent) : draw->get_clr(clr->widgets.text_inactive), tabs[i].name.c_str(), NULL, NULL, ImVec2(0.5f, 0.5f));
    }

    draw->rect(dl, base + ImVec2(1, 0), base + ImVec2(size_arg.x - 1, size_arg.y - 1), draw->get_clr(clr->window.stroke));

    const ImVec2 line_min = base + ImVec2(2, 2);
    const ImVec2 line_max = base + ImVec2(size_arg.x - 2, 4);
    if (var->window.shadow_size > 0.f && var->window.shadow_alpha > 0.f)
        draw->shadow_rect(dl, line_min, line_max, draw->get_clr(clr->glow, var->window.shadow_alpha), var->window.shadow_size, ImVec2(0, 0));
    draw->line(dl, base + ImVec2(2, 2), base + ImVec2(size_arg.x - 2, 2), draw->get_clr(clr->accent));
    draw->line(dl, base + ImVec2(2, 3), base + ImVec2(size_arg.x - 2, 3), draw->get_clr(clr->accent, 0.4f));

    set_child_subtab(id, active);
}

void c_gui::begin_multi_subtab(std::string_view name, int x, int y, int tab_count, const ImVec2& size, const std::vector<std::string>& tab_names)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    set_child_subtabs(id, tab_names.empty() ? std::vector<std::string>(tab_count) : tab_names);

    gui->push_style_var(ImGuiStyleVar_WindowPadding, elements->widgets.padding);
    begin_child_ex(name.data(), id, x, y, size, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove, false);
    gui->push_style_var(ImGuiStyleVar_ItemSpacing, elements->widgets.spacing);
}

bool c_gui::is_subtab_open(std::string_view name, int tab_index)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());
    auto& tabs = subtab_list_map[id];
    if (tab_index < 0 || tab_index >= static_cast<int>(tabs.size()))
        return false;
    return child_subtab_map[id] == tab_index;
}

bool begin_child_ex(const char* name, ImGuiID id, int x, int y, const ImVec2& size_arg, ImGuiChildFlags child_flags, ImGuiWindowFlags window_flags, bool warning)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    IM_ASSERT(id != 0);

    const ImGuiChildFlags ImGuiChildFlags_SupportedMask_ = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY | ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_FrameStyle;
    IM_UNUSED(ImGuiChildFlags_SupportedMask_);
    IM_ASSERT((child_flags & ~ImGuiChildFlags_SupportedMask_) == 0 && "Illegal ImGuiChildFlags value. Did you pass ImGuiWindowFlags values instead of ImGuiChildFlags?");
    IM_ASSERT((window_flags & ImGuiWindowFlags_AlwaysAutoResize) == 0 && "Cannot specify ImGuiWindowFlags_AlwaysAutoResize for BeginChild(). Use ImGuiChildFlags_AlwaysAutoResize!");
    if (child_flags & ImGuiChildFlags_AlwaysAutoResize)
    {
        IM_ASSERT((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0 && "Cannot use ImGuiChildFlags_ResizeX or ImGuiChildFlags_ResizeY with ImGuiChildFlags_AlwaysAutoResize!");
        IM_ASSERT((child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY)) != 0 && "Must use ImGuiChildFlags_AutoResizeX or ImGuiChildFlags_AutoResizeY with ImGuiChildFlags_AlwaysAutoResize!");
    }
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    if (window_flags & ImGuiWindowFlags_AlwaysUseWindowPadding)
        child_flags |= ImGuiChildFlags_AlwaysUseWindowPadding;
#endif
    if (child_flags & ImGuiChildFlags_AutoResizeX)
        child_flags &= ~ImGuiChildFlags_ResizeX;
    if (child_flags & ImGuiChildFlags_AutoResizeY)
        child_flags &= ~ImGuiChildFlags_ResizeY;

    window_flags |= ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar;
    window_flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);
    if (child_flags & (ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AlwaysAutoResize))
        window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    if ((child_flags & (ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY)) == 0)
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PushStyleColor(ImGuiCol_ChildBg, g.Style.Colors[ImGuiCol_FrameBg]);
        PushStyleVar(ImGuiStyleVar_ChildRounding, g.Style.FrameRounding);
        PushStyleVar(ImGuiStyleVar_ChildBorderSize, g.Style.FrameBorderSize);
        PushStyleVar(ImGuiStyleVar_WindowPadding, g.Style.FramePadding);
        child_flags |= ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding;
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasChildFlags;
    g.NextWindowData.ChildFlags = child_flags;

    ImVec2 size = size_arg;
    if (size.x <= 0)
        size.x = (GetWindowWidth() - elements->content.padding.x * 2.f - elements->content.spacing.x * (x - 1)) / x;
    if (size.y <= 0)
        size.y = (GetWindowHeight() - elements->content.padding.y * 2 - elements->content.spacing.y * (y - 1)) / y;

    gui->set_next_window_size(size - ImVec2(0, var->window.titlebar));
    gui->set_next_window_pos(parent_window->DC.CursorPos + ImVec2(0, var->window.titlebar));

    const char* temp_window_name;

    if (name)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, name, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    const ImVec2 child_pos = parent_window->DC.CursorPos;
    const ImVec2 line_min = child_pos + ImVec2(2, 2);
    const ImVec2 line_max = child_pos + ImVec2(size.x - 2, 4);
    const bool has_subtabs = !subtab_list_map[id].empty();

    draw->rect_filled(parent_window->DrawList, child_pos, child_pos + size, draw->get_clr(clr->window.background_one));
    if (!has_subtabs)
    {
        if (var->window.shadow_size > 0.f && var->window.shadow_alpha > 0.f)
            draw->shadow_rect(parent_window->DrawList, line_min, line_max, draw->get_clr(clr->glow, var->window.shadow_alpha), var->window.shadow_size, ImVec2(0, 0));
        draw->line(parent_window->DrawList, child_pos + ImVec2(2, 2), child_pos + ImVec2(size.x - 2, 2), draw->get_clr(clr->accent));
        draw->line(parent_window->DrawList, child_pos + ImVec2(2, 3), child_pos + ImVec2(size.x - 2, 3), draw->get_clr(clr->accent, 0.4f));
    }
    draw->rect(parent_window->DrawList, parent_window->DC.CursorPos, parent_window->DC.CursorPos + size, draw->get_clr(clr->window.stroke));
    if (has_subtabs)
        draw_child_subtabs(parent_window, id, size);
    else
        draw->text_outline(parent_window->DrawList, var->font.tahoma, var->font.tahoma->FontSize, parent_window->DC.CursorPos + ImVec2(6, 6), gui->get_text_col(warning), name);

    const float backup_border_size = g.Style.ChildBorderSize;
    if ((child_flags & ImGuiChildFlags_Border) == 0)
        g.Style.ChildBorderSize = 0.0f;

    const bool ret = gui->begin(temp_window_name, NULL, window_flags);

    g.Style.ChildBorderSize = backup_border_size;
    if (child_flags & ImGuiChildFlags_FrameStyle)
    {
        PopStyleVar(3);
        PopStyleColor();
    }

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;

    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;

    const ImGuiID temp_id_for_activation = ImHashStr("##Child", 0, id);
    if (g.ActiveId == temp_id_for_activation)
        ClearActiveID();
    if (g.NavActivateId == id && !(window_flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(temp_id_for_activation, child_window);
        g.ActiveIdSource = g.NavInputSource;
    }
    return ret;
}

void c_gui::begin_child(std::string_view name, int x, int y, const ImVec2& size, bool warning)
{
    ImGuiID id = GetCurrentWindow()->GetID(name.data());

    gui->push_style_var(ImGuiStyleVar_WindowPadding, elements->widgets.padding);
    begin_child_ex(name.data(), id, x, y, size, ImGuiChildFlags_None, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove, warning);
    gui->push_style_var(ImGuiStyleVar_ItemSpacing, elements->widgets.spacing);
}

void c_gui::end_child()
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* child_window = g.CurrentWindow;

    IM_ASSERT(g.WithinEndChild == false);
    IM_ASSERT(child_window->Flags & ImGuiWindowFlags_ChildWindow);

    gui->pop_style_var();

    g.WithinEndChild = true;
    ImVec2 child_size = child_window->Size;
    gui->end();
    if (child_window->BeginCount == 1)
    {
        ImGuiWindow* parent_window = g.CurrentWindow;
        ImRect bb(parent_window->DC.CursorPos, parent_window->DC.CursorPos + child_size);
        ItemSize(child_size);
        if ((child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavWindowHasScrollY) && !(child_window->Flags & ImGuiWindowFlags_NavFlattened))
        {
            ItemAdd(bb, child_window->ChildId);
            RenderNavHighlight(bb, child_window->ChildId);

            if (child_window->DC.NavLayersActiveMask == 0 && child_window == g.NavWindow)
                RenderNavHighlight(ImRect(bb.Min - ImVec2(2, 2), bb.Max + ImVec2(2, 2)), g.NavId, ImGuiNavHighlightFlags_Compact);
        }
        else
        {

            ItemAdd(bb, 0);

            if (child_window->Flags & ImGuiWindowFlags_NavFlattened)
                parent_window->DC.NavLayersActiveMaskNext |= child_window->DC.NavLayersActiveMaskNext;
        }
        if (g.HoveredWindow == child_window)
            g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_HoveredWindow;
    }
    gui->pop_style_var();
    g.WithinEndChild = false;
    g.LogLinePosY = -FLT_MAX;
}
