#include "../settings/functions.h"
#include <vector>

struct notify_item
{
    std::string title;
    std::string text;
    notification_type type;
    float time;
    float duration;
    float anim;
};

static std::vector<notify_item> g_notifies;

void c_notify::add(std::string_view title, std::string_view text, notification_type type, float duration)
{
    g_notifies.push_back({ std::string(title), std::string(text), type, 0.f, duration, 0.f });
}

void c_notify::render()
{
    ImDrawList* dl = GetForegroundDrawList();
    const float pad = 12.f;
    const float text_pad = 5.f;
    float y = GetIO().DisplaySize.y - pad;

    for (int i = 0; i < (int)g_notifies.size(); )
    {
        notify_item& n = g_notifies[i];
        n.time += GetIO().DeltaTime;

        const bool closing = n.time > n.duration;
        n.anim = ImClamp(n.anim + gui->fixed_speed(10.f) * (closing ? -1.f : 1.f), 0.f, 1.f);

        if (closing && n.anim <= 0.01f)
        {
            g_notifies.erase(g_notifies.begin() + i);
            continue;
        }

        const float title_w = var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, n.title.c_str()).x;
        const float body_w = n.text.empty() ? 0.f : var->font.tahoma->CalcTextSizeA(var->font.tahoma->FontSize, FLT_MAX, 0.f, n.text.c_str()).x;
        const ImVec2 size(ImMax(title_w, body_w) + text_pad * 2.f + 2.f, n.text.empty() ? 22.f : 36.f);
        y -= size.y;

        const float t = n.anim;
        const ImVec2 pos(-size.x + (size.x + pad) * t, y);
        ImColor accent = clr->accent;
        if (n.type == notif_success) accent = ImColor(80, 200, 120);
        else if (n.type == error) accent = ImColor(220, 70, 70);
        else if (n.type == warning) accent = clr->widgets.text_warning;

        draw->rect_filled(dl, pos, pos + size, draw->get_clr(clr->window.background_one, t));
        draw->line(dl, pos + ImVec2(1, 1), pos + ImVec2(size.x - 1, 1), draw->get_clr(accent, t));
        draw->line(dl, pos + ImVec2(1, 2), pos + ImVec2(size.x - 1, 2), draw->get_clr(accent, t * 0.4f));
        draw->rect(dl, pos, pos + size, draw->get_clr(clr->window.stroke, t));
        draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize, pos + ImVec2(text_pad, 3), draw->get_clr(clr->accent, t), n.title.c_str());
        if (!n.text.empty())
            draw->text_outline(dl, var->font.tahoma, var->font.tahoma->FontSize, pos + ImVec2(text_pad, 16), draw->get_clr(clr->widgets.text, t), n.text.c_str());

        y -= 6.f;
        i++;
    }
}
