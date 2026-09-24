#include "settings/functions.h"
#include "elements/model_preview.h"
#include <Windows.h>
#include <src/cheat/features/visuals/players/players.hpp>
#include <src/cheat/features/visuals/chams/engine/engine_chams.hpp>

void c_gui::render( )
{
	if ( GetAsyncKeyState( var->gui.menu_key ) & 0x1 )
		var->gui.menu_opened = !var->gui.menu_opened;

	var->gui.menu_alpha = ImClamp( var->gui.menu_alpha + ( gui->fixed_speed( 8.f ) * ( var->gui.menu_opened ? 1.f : -1.f ) ), 0.f, 1.f );
	notify->render( );
	draw->watermark( "O_o" );

	if ( var->gui.menu_alpha <= 0.01f )
		return;

	gui->set_next_window_pos( ImVec2( GetIO( ).DisplaySize.x / 2 - var->window.width / 2, 20 ) );
	gui->set_next_window_size( ImVec2( var->window.width, elements->section.size.y + var->window.spacing.y * 2 - 1 ) );
	gui->push_style_var( ImGuiStyleVar_Alpha, var->gui.menu_alpha );
	gui->begin( "evana", nullptr, var->window.main_flags );
	{
		const ImVec2 pos = GetWindowPos( );
		const ImVec2 size = GetWindowSize( );
		ImDrawList* draw_list = GetWindowDrawList( );
		ImGuiStyle* style = &GetStyle( );

		{
			style->WindowPadding = var->window.padding;
			style->PopupBorderSize = var->window.border_size;
			style->WindowBorderSize = var->window.border_size;
			style->ItemSpacing = var->window.spacing;
			style->WindowShadowSize = var->window.shadow_size;
			style->ScrollbarSize = var->window.scrollbar_size;
			style->Colors[ImGuiCol_WindowShadow] = { clr->glow.Value.x, clr->glow.Value.y, clr->glow.Value.z, var->window.shadow_alpha };
		}

		{
			draw->rect( GetBackgroundDrawList( ), pos - ImVec2( 1, 1 ), pos + size + ImVec2( 1, 1 ), draw->get_clr( { 0, 0, 0, 0.5f } ) );
			draw->rect_filled( draw_list, pos, pos + size, draw->get_clr( clr->window.background_one ) );
			draw->line( draw_list, pos + ImVec2( 1, 1 ), pos + ImVec2( size.x - 1, 1 ), draw->get_clr( clr->accent ), 1 );
			draw->line( draw_list, pos + ImVec2( 1, 2 ), pos + ImVec2( size.x - 1, 2 ), draw->get_clr( clr->accent, 0.4f ), 1 );
			draw->rect( draw_list, pos, pos + size, draw->get_clr( clr->window.stroke ) );
		}

		{
			gui->set_cursor_pos( style->ItemSpacing );
			gui->begin_group( );
			{
				for ( int i = 0; i < IM_ARRAYSIZE( var->gui.current_section ); i++ )
					gui->section( var->gui.section_icons[i], &var->gui.current_section[i] );
			}
			gui->end_group( );
		}

		{
			const ImVec2 size_max = var->window.clamp_width ? ImVec2( var->window.max_width, GetIO( ).DisplaySize.y ) : GetIO( ).DisplaySize;

			if ( var->gui.current_section[0] )
			{
				gui->set_next_window_size_constraints( ImVec2( 535, 616 ), size_max );
				gui->begin( "Home", nullptr, var->window.flags );
				{
					draw->window_decorations( );

					{
						static int subtabs;
						gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar ) );
						gui->begin_group( );
						{
							gui->sub_section( "Legit", 0, subtabs, 4 );
							gui->sub_section( "Players", 1, subtabs, 4 );
							gui->sub_section( "Visuals", 2, subtabs, 4 );
							gui->sub_section( "Misc", 3, subtabs, 4 );
						}
						gui->end_group( );

						gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar + elements->section.height - 1 ) );
						gui->begin_content( );
						{
							const ImVec2 content_padding = elements->content.padding;
							const ImVec2 content_spacing = elements->content.spacing;
							const float avail_w = GetWindowWidth( ) - content_padding.x * 2.f;
							const float avail_h = GetWindowHeight( ) - content_padding.y * 2.f;
							const float col_w = ( avail_w - content_spacing.x ) / 2.f;
							const float left_main_h = avail_h * 0.6f;
							const float left_small_h = ( avail_h - left_main_h - content_spacing.y * 2.f ) / 2.f;
							const float right_h = ( avail_h - content_spacing.y ) / 2.f;

							if ( subtabs == 0 ) {
								gui->begin_group( );
								{
									gui->begin_child( "Main", 1, 1, ImVec2( col_w, left_main_h ) );
									{
										static bool enabled = true;
										static int key;
										static int mode;
										gui->checkbox( "Enabled", &enabled, &key, &mode );

										static bool unsafe = false;
										gui->checkbox( "Unsafe", &unsafe, true );

										static int fov = 90;
										gui->slider_int( "Field Of View", &fov, 0, 100, false, "%d", true );

										static int fov_type = 0;
										const char* fov_type_items[2] = {"Static", "Dynamic"};
										gui->dropdown( "fov type", &fov_type, fov_type_items, IM_ARRAYSIZE( fov_type_items ), true );

										static float horizontal = 30;
										gui->slider_float( "Horizontal Smoothing", &horizontal, 0, 100 );

										static float vertical = 12;
										gui->slider_float( "Vertical Smoothing", &vertical, 0, 100 );

										static std::vector<int> checks = {1, 1, 1};
										const char* checks_items[3] = {"Team Check", "Alive Check", "Enemy Check"};
										gui->multi_dropdown( "Checks", checks, checks_items, IM_ARRAYSIZE( checks_items ) );

										static std::vector<int> hitboxes = {1, 0, 1, 0, 1, 0};
										const char* hitboxes_items[6] = {"Head", "Neck", "Stomach", "Body", "Arms", "Legs"};
										gui->multi_dropdown( "Hitboxes", hitboxes, hitboxes_items, IM_ARRAYSIZE( hitboxes_items ) );
									}
									gui->end_child( );

									gui->begin_child( "Other", 1, 1, ImVec2( col_w, left_small_h ) );
									{
										static bool randomize = false;
										gui->checkbox( "Randomize Position", &randomize );

										static int hitscan_type = 0;
										const char* hitscan_type_items[2] = {"Mouse", "Distance"};
										gui->dropdown( "Hitscan Type", &hitscan_type, hitscan_type_items, IM_ARRAYSIZE( hitscan_type_items ) );

										static bool readjustment = false;
										static int rkey;
										static int rmode;
										gui->checkbox( "Readjustment", &readjustment, &rkey, &rmode );
									}
									gui->end_child( );

									gui->begin_child( "xdd", 1, 1, ImVec2( col_w, left_small_h ) );
									{
										static bool deadzone = true;
										gui->checkbox( "Dead Zone", &deadzone );

										static float dzone = 44;
										gui->slider_float( "dzone", &dzone, 0, 100, true );

										static bool stutter = true;
										gui->checkbox( "Stutter", &stutter );

										static float stslider = 25;
										gui->slider_float( "stslider", &stslider, 0, 100, true, "%.1ft" );
									}
									gui->end_child( );
								}
								gui->end_group( );

								gui->sameline( );

								gui->begin_group( );
								{
									gui->begin_child( "New", 1, 1, ImVec2( col_w, right_h ) );
									{
										static bool enabled = true;
										static int key;
										static int mode;
										gui->checkbox( "Enabled", &enabled, &key, &mode );

										static int delay = 15;
										gui->slider_int( "Delay", &delay, 0, 500, false, "%dms" );

										static int interval = 75;
										gui->slider_int( "Interval", &interval, 0, 1000, false, "%dms" );

										static std::vector<int> checks = {1, 1, 1};
										const char* checks_items[3] = {"Team Check", "Alive Check", "Enemy Check"};
										gui->multi_dropdown( "Checks", checks, checks_items, IM_ARRAYSIZE( checks_items ) );

										static std::vector<int> hitboxes = {1, 0, 1, 0, 1, 0};
										const char* hitboxes_items[6] = {"Head", "Neck", "Stomach", "Body", "Arms", "Legs"};
										gui->multi_dropdown( "Hitboxes", hitboxes, hitboxes_items, IM_ARRAYSIZE( hitboxes_items ) );

										static bool readjustment = false;
										static int rkey;
										static int rmode;
										gui->checkbox( "Readjustment", &readjustment, &rkey, &rmode );
									}
									gui->end_child( );

									gui->begin_child( "Avaws", 1, 1, ImVec2( col_w, right_h ) );
									{
										static bool cursor = true;
										gui->checkbox( "Cursor Offset", &cursor );

										static int x = 50;
										gui->slider_int( "offsetx", &x, 0, 100, true, "%dpx" );

										static int y = 50;
										gui->slider_int( "offsety", &y, 0, 100, true, "%dpx" );

										gui->label_keybind( "MENU KEY", &var->gui.menu_key, 0 );
									}
									gui->end_child( );
								}
								gui->end_group( );
							}
							else if ( subtabs == 1 ) {
								gui->begin_group( );
							{
								gui->begin_child( "Players", 1, 1, ImVec2( col_w, left_main_h ) );
								{
									auto& prop = cheat::esp->prop;
									gui->checkbox( "Enabled", &prop->enabled );
									
									gui->checkbox( "Bounding Box", &prop->box.enable );
									gui->checkbox( "Name", &prop->info.name );
									if ( prop->info.name ) {
										gui->checkbox( "Display Name", &prop->info.display_name );
									}

									gui->checkbox( "Health Bar", &prop->info.health_bar );
									if ( prop->info.health_bar ) {
										gui->checkbox( "Use Health", &prop->info.use_health );
									}

									gui->checkbox( "Distance", &prop->info.distance );

									auto* prop2 = cheat::engine_chams->prop.get( );

									ImGui::Checkbox( "Enabled", &prop2->enabled );
									gui->checkbox( "Alpha", &prop2->alpha );

									if ( prop2->alpha ) {
										gui->slider_int( "Alpha Value", &prop2->alpha_value, 0, 255 );
									}

									gui->checkbox( "Use Color", &prop2->use_color );

									if ( prop2->use_color ) {
										gui->slider_int( "Color", &prop2->color, 0, 6 );
									}

									gui->slider_int( "Style", &prop2->style, 1, 8 );
								}
								gui->end_child( );

								gui->begin_child( "Settings", 1, 1, ImVec2( col_w, left_small_h ) );
								{
									static bool deadzone = true;
									gui->checkbox( "Dead Zone", &deadzone );

									static float dzone = 44;
									gui->slider_float( "dzone", &dzone, 0, 100, true );

									static bool stutter = true;
									gui->checkbox( "Stutter", &stutter );

									static float stslider = 25;
									gui->slider_float( "stslider", &stslider, 0, 100, true, "%.1ft" );
								}
								gui->end_child( );
							}
							gui->end_group( );

							gui->sameline( );

							gui->begin_group( );
							{
								gui->begin_child( "Chams", 1, 1, ImVec2( col_w, right_h ) );
								{

								}
								gui->end_child( );

								gui->begin_child( "Misc", 1, 1, ImVec2( col_w, right_h ) );
								{
									
								}
								gui->end_child( );
							}
							gui->end_group( );
							}
							
						}
						gui->end_content( );
					}

				}
				gui->end( );
			}

			if ( var->gui.current_section[1] )
			{
				gui->set_next_window_size_constraints( ImVec2( 314, 540 ), GetIO( ).DisplaySize );
				gui->begin( "Preview", nullptr, var->window.flags );
				{
					draw->window_decorations( );

					gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar + 1 ) );

					gui->begin_content( );
					{
						gui->begin_group( );
						{
							static int cond = 0;

							const char* items[] = {
								"None",
								"Visible",
								"Distance"
							};

							gui->dropdown( "Conditions", &cond, items, IM_ARRAYSIZE( items ) );
						}
						gui->end_group( );
					}
					gui->end_content( );
				}
				gui->end( );
			}

			if ( var->gui.current_section[2] )
			{


			}

			if ( var->gui.current_section[2] )
			{
				gui->set_next_window_size_constraints( ImVec2( 400, 400 ), size_max );
				gui->begin( "menu", nullptr, var->window.flags );
				{
					draw->window_decorations( );

				}
				gui->end( );
			}

			if ( var->gui.current_section[3] )
			{
				gui->set_next_window_size_constraints( ImVec2( 564, 356 ), size_max );
				gui->begin( "Lua", nullptr, var->window.flags );
				{
					draw->window_decorations( );
					gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar + 1 ) );

					gui->begin_content( );
					{
						gui->begin_group( );
						{
							gui->begin_child( "Main", 2, 2, ImVec2( 519, 298 ) );
							{
								
							}
							gui->end_child( );

						}
						gui->end_group( );
					}
					gui->end_content( );
				}
				gui->end( );
			}

			if ( var->gui.current_section[4] )
			{
				gui->set_next_window_size_constraints( ImVec2( 400, 470 ), size_max );
				gui->begin( "style", nullptr, var->window.flags );
				{
					draw->window_decorations( );

					{
						gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar + 1 ) );
						gui->begin_content( );
						{
							ImGuiID theme_id = GetCurrentWindow( )->GetID( "Theme" );
							const int style_subtab = gui->get_child_subtab( theme_id );
							gui->begin_multi_subtab( "Theme", 1, 2, 2, ImVec2( 0, 0 ), { "Theme", "Presets" } );
							{
								static float menu_accent[4] = { clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f };
								static float menu_accent_two[4] = { clr->accent_two.Value.x, clr->accent_two.Value.y, clr->accent_two.Value.z, 1.f };
								static float contrast_one[4] = { clr->window.background_one.Value.x, clr->window.background_one.Value.y, clr->window.background_one.Value.z, 1.f };
								static float contrast_two[4] = { clr->window.background_two.Value.x, clr->window.background_two.Value.y, clr->window.background_two.Value.z, 1.f };
								static float inline_c[4] = { clr->window.stroke.Value.x, clr->window.stroke.Value.y, clr->window.stroke.Value.z, 1.f };
								static float outline_c[4] = { clr->widgets.stroke_two.Value.x, clr->widgets.stroke_two.Value.y, clr->widgets.stroke_two.Value.z, 1.f };
								static float glow_c[4] = { clr->glow.Value.x, clr->glow.Value.y, clr->glow.Value.z, 1.f };
								static float text_c[4] = { clr->widgets.text.Value.x, clr->widgets.text.Value.y, clr->widgets.text.Value.z, 1.f };
								static float text_outline_c[4] = { clr->widgets.text_outline.Value.x, clr->widgets.text_outline.Value.y, clr->widgets.text_outline.Value.z, 1.f };
								static float text_outline_two_c[4] = { clr->widgets.text_outline_two.Value.x, clr->widgets.text_outline_two.Value.y, clr->widgets.text_outline_two.Value.z, 1.f };
								static float text_warning[4] = { clr->widgets.text_warning.Value.x, clr->widgets.text_warning.Value.y, clr->widgets.text_warning.Value.z, 1.f };

								if ( style_subtab == 0 )
								{
									if ( gui->label_color_edit( "Menu Accent", menu_accent, menu_accent_two, false ) )
									{
										const bool fade_changed =
											menu_accent_two[0] != clr->accent_two.Value.x ||
											menu_accent_two[1] != clr->accent_two.Value.y ||
											menu_accent_two[2] != clr->accent_two.Value.z;

										clr->accent.Value.x = menu_accent[0];
										clr->accent.Value.y = menu_accent[1];
										clr->accent.Value.z = menu_accent[2];

										if ( !fade_changed )
										{
											menu_accent_two[0] = ImMax( 0.f, menu_accent[0] - 0.2f );
											menu_accent_two[1] = ImMax( 0.f, menu_accent[1] - 0.2f );
											menu_accent_two[2] = ImMax( 0.f, menu_accent[2] - 0.2f );
										}

										clr->accent_two.Value.x = menu_accent_two[0];
										clr->accent_two.Value.y = menu_accent_two[1];
										clr->accent_two.Value.z = menu_accent_two[2];
									}

									if ( gui->label_color_edit( "Contrast", contrast_one, contrast_two, false ) )
									{
										clr->window.background_one.Value.x = contrast_one[0];
										clr->window.background_one.Value.y = contrast_one[1];
										clr->window.background_one.Value.z = contrast_one[2];
										clr->window.background_two.Value.x = contrast_two[0];
										clr->window.background_two.Value.y = contrast_two[1];
										clr->window.background_two.Value.z = contrast_two[2];
									}

									if ( gui->label_color_edit( "Inline", inline_c, false ) )
									{
										clr->window.stroke.Value.x = inline_c[0];
										clr->window.stroke.Value.y = inline_c[1];
										clr->window.stroke.Value.z = inline_c[2];
									}

									if ( gui->label_color_edit( "Outline", outline_c, false ) )
									{
										clr->widgets.stroke_two.Value.x = outline_c[0];
										clr->widgets.stroke_two.Value.y = outline_c[1];
										clr->widgets.stroke_two.Value.z = outline_c[2];
									}

									if ( gui->label_color_edit( "Glow", glow_c, false ) )
									{
										clr->glow.Value.x = glow_c[0];
										clr->glow.Value.y = glow_c[1];
										clr->glow.Value.z = glow_c[2];
									}

									if ( gui->label_color_edit( "Text", text_c, false ) )
									{
										clr->widgets.text.Value.x = text_c[0];
										clr->widgets.text.Value.y = text_c[1];
										clr->widgets.text.Value.z = text_c[2];
									}

									if ( gui->label_color_edit( "Text Outline", text_outline_c, text_outline_two_c, false ) )
									{
										clr->widgets.text_outline.Value.x = text_outline_c[0];
										clr->widgets.text_outline.Value.y = text_outline_c[1];
										clr->widgets.text_outline.Value.z = text_outline_c[2];
										clr->widgets.text_outline_two.Value.x = text_outline_two_c[0];
										clr->widgets.text_outline_two.Value.y = text_outline_two_c[1];
										clr->widgets.text_outline_two.Value.z = text_outline_two_c[2];
									}

									if ( gui->label_color_edit( "Unsafe", text_warning, false, true ) )
									{
										clr->widgets.text_warning.Value.x = text_warning[0];
										clr->widgets.text_warning.Value.y = text_warning[1];
										clr->widgets.text_warning.Value.z = text_warning[2];
									}
								}

								if ( style_subtab == 1 )
								{
									if ( gui->button( "Notify" ) )
										notify->add( "Player killed", "Test123 has been killed." );

									static int theme = 0;
									const char* theme_items[] = { "Default", "Blue", "Cherry", "Assembly", "GameSense", "OneTap", "Violet",
										"Crimson", "Emerald", "Pearl", "Cream", "Obsidian", "Artic" };
									if ( gui->dropdown( "Theme", &theme, theme_items, IM_ARRAYSIZE( theme_items ) ) )
									{
										if ( theme == 0 )
										{
											clr->accent = ImColor( 154, 127, 172 );
											clr->accent_two = ImColor( 103, 76, 121 );
											clr->glow = ImColor( 154, 127, 172 );
											clr->window.background_one = ImColor( 36, 36, 47 );
											clr->window.background_two = ImColor( 42, 42, 56 );
											clr->window.stroke = ImColor( 53, 53, 67 );
											clr->widgets.stroke_two = ImColor( 36, 36, 48 );
											clr->widgets.text = ImColor( 200, 200, 200 );
											clr->widgets.text_inactive = ImColor( 136, 136, 136 );
											clr->widgets.text_warning = ImColor( 255, 165, 50 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 1 )
										{
											clr->accent = ImColor( 62, 93, 241 );
											clr->accent_two = ImColor( 11, 42, 190 );
											clr->glow = ImColor( 62, 93, 241 );
											clr->window.background_one = ImColor( 12, 12, 12 );
											clr->window.background_two = ImColor( 18, 18, 18 );
											clr->window.stroke = ImColor( 32, 32, 32 );
											clr->widgets.stroke_two = ImColor( 12, 12, 12 );
										}
										else if ( theme == 2 )
										{
											clr->accent = ImColor( 175, 50, 100 );
											clr->accent_two = ImColor( 130, 35, 75 );
											clr->glow = ImColor( 175, 50, 100 );

											clr->window.background_one = ImColor( 22, 4, 12 );
											clr->window.background_two = ImColor( 30, 6, 16 );
											clr->window.stroke = ImColor( 38, 9, 21 );

											clr->widgets.stroke_two = ImColor( 15, 2, 7 );
											clr->widgets.text = ImColor( 180, 180, 180 );
											clr->widgets.text_inactive = ImColor( 136, 136, 136 );
											clr->widgets.text_warning = ImColor( 255, 165, 50 );

											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 3 )
										{
											clr->accent = ImColor( 139, 152, 199 );
											clr->accent_two = ImColor( 105, 118, 165 );
											clr->glow = ImColor( 139, 152, 199 );

											clr->window.background_one = ImColor( 10, 11, 16 );
											clr->window.background_two = ImColor( 25, 28, 37 );
											clr->window.stroke = ImColor( 43, 48, 64 );

											clr->widgets.stroke_two = ImColor( 10, 11, 16 );
											clr->widgets.text = ImColor( 221, 234, 246 );
											clr->widgets.text_inactive = ImColor( 139, 142, 160 );
											clr->widgets.text_warning = ImColor( 255, 165, 50 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 4 )
										{
											clr->accent = ImColor( 156, 199, 40 );
											clr->accent_two = ImColor( 112, 148, 24 );
											clr->glow = ImColor( 156, 199, 40 );

											clr->window.background_one = ImColor( 12, 12, 12 );
											clr->window.background_two = ImColor( 20, 20, 20 );
											clr->window.stroke = ImColor( 48, 48, 48 );

											clr->widgets.stroke_two = ImColor( 12, 12, 12 );
											clr->widgets.text = ImColor( 205, 205, 205 );
											clr->widgets.text_inactive = ImColor( 195, 195, 195 );
											clr->widgets.text_warning = ImColor( 172, 177, 123 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 5 )
										{
											clr->accent = ImColor( 252, 154, 29 );
											clr->accent_two = ImColor( 190, 105, 12 );
											clr->glow = ImColor( 252, 154, 29 );

											clr->window.background_one = ImColor( 18, 17, 22 );
											clr->window.background_two = ImColor( 30, 29, 34 );
											clr->window.stroke = ImColor( 68, 67, 72 );

											clr->widgets.stroke_two = ImColor( 13, 12, 17 );
											clr->widgets.text = ImColor( 233, 232, 237 );
											clr->widgets.text_inactive = ImColor( 184, 183, 188 );
											clr->widgets.text_warning = ImColor( 250, 161, 33 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 6 )
										{
											clr->accent = ImColor( 154, 112, 201 );
											clr->accent_two = ImColor( 106, 72, 151 );
											clr->glow = ImColor( 154, 112, 201 );

											clr->window.background_one = ImColor( 18, 12, 25 );
											clr->window.background_two = ImColor( 28, 20, 38 );
											clr->window.stroke = ImColor( 51, 38, 67 );

											clr->widgets.stroke_two = ImColor( 14, 9, 20 );
											clr->widgets.text = ImColor( 210, 204, 219 );
											clr->widgets.text_inactive = ImColor( 145, 136, 157 );
											clr->widgets.text_warning = ImColor( 196, 144, 220 );

											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 7 )
										{
											clr->accent = ImColor( 210, 65, 85 );
											clr->accent_two = ImColor( 145, 38, 58 );
											clr->glow = ImColor( 210, 65, 85 );

											clr->window.background_one = ImColor( 24, 10, 14 );
											clr->window.background_two = ImColor( 36, 16, 21 );
											clr->window.stroke = ImColor( 63, 31, 39 );

											clr->widgets.stroke_two = ImColor( 15, 6, 9 );
											clr->widgets.text = ImColor( 215, 205, 208 );
											clr->widgets.text_inactive = ImColor( 150, 134, 140 );
											clr->widgets.text_warning = ImColor( 225, 110, 125 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 8 )
										{
											clr->accent = ImColor( 72, 195, 132 );
											clr->accent_two = ImColor( 37, 130, 87 );
											clr->glow = ImColor( 72, 195, 132 );

											clr->window.background_one = ImColor( 8, 19, 14 );
											clr->window.background_two = ImColor( 15, 29, 22 );
											clr->window.stroke = ImColor( 31, 55, 42 );

											clr->widgets.stroke_two = ImColor( 5, 12, 8 );
											clr->widgets.text = ImColor( 205, 218, 211 );
											clr->widgets.text_inactive = ImColor( 134, 153, 143 );
											clr->widgets.text_warning = ImColor( 91, 201, 145 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 9 )
										{
											clr->accent = ImColor( 108, 117, 191 );
											clr->accent_two = ImColor( 75, 82, 151 );
											clr->glow = ImColor( 108, 117, 191 );

											clr->window.background_one = ImColor( 232, 234, 240 );
											clr->window.background_two = ImColor( 245, 246, 250 );
											clr->window.stroke = ImColor( 190, 193, 204 );

											clr->widgets.stroke_two = ImColor( 215, 217, 226 );
											clr->widgets.text = ImColor( 45, 47, 55 );
											clr->widgets.text_inactive = ImColor( 105, 108, 120 );
											clr->widgets.text_warning = ImColor( 193, 133, 45 );
											clr->widgets.text_outline = ImColor( 255, 255, 255 );
											clr->widgets.text_outline_two = ImColor( 255, 255, 255 );

											var->window.outline_style = 1;
										}
										else if ( theme == 10 )
										{
											clr->accent = ImColor( 188, 137, 59 );
											clr->accent_two = ImColor( 136, 94, 35 );
											clr->glow = ImColor( 188, 137, 59 );

											clr->window.background_one = ImColor( 239, 234, 223 );
											clr->window.background_two = ImColor( 249, 246, 239 );
											clr->window.stroke = ImColor( 202, 194, 177 );

											clr->widgets.stroke_two = ImColor( 221, 214, 198 );
											clr->widgets.text = ImColor( 61, 57, 49 );
											clr->widgets.text_inactive = ImColor( 118, 111, 98 );
											clr->widgets.text_warning = ImColor( 185, 113, 45 );
											clr->widgets.text_outline = ImColor( 255, 255, 255 );
											clr->widgets.text_outline_two = ImColor( 255, 255, 255 );

											var->window.outline_style = 1;
										}
										else if ( theme == 11 )
										{
											clr->accent = ImColor( 185, 190, 202 );
											clr->accent_two = ImColor( 120, 126, 142 );
											clr->glow = ImColor( 185, 190, 202 );

											clr->window.background_one = ImColor( 9, 10, 13 );
											clr->window.background_two = ImColor( 17, 18, 23 );
											clr->window.stroke = ImColor( 38, 40, 47 );

											clr->widgets.stroke_two = ImColor( 6, 7, 9 );
											clr->widgets.text = ImColor( 215, 217, 223 );
											clr->widgets.text_inactive = ImColor( 130, 133, 143 );
											clr->widgets.text_warning = ImColor( 205, 171, 91 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}
										else if ( theme == 12 )
										{
											clr->accent = ImColor( 184, 218, 52 );
											clr->accent_two = ImColor( 113, 146, 22 );
											clr->glow = ImColor( 184, 218, 52 );

											clr->window.background_one = ImColor( 12, 16, 9 );
											clr->window.background_two = ImColor( 20, 27, 13 );
											clr->window.stroke = ImColor( 40, 52, 24 );

											clr->widgets.stroke_two = ImColor( 7, 10, 5 );
											clr->widgets.text = ImColor( 210, 218, 195 );
											clr->widgets.text_inactive = ImColor( 143, 153, 123 );
											clr->widgets.text_warning = ImColor( 201, 216, 67 );
											clr->widgets.text_outline = ImColor( 0, 0, 0 );
											clr->widgets.text_outline_two = ImColor( 0, 0, 0 );
										}

										menu_accent[0] = clr->accent.Value.x; menu_accent[1] = clr->accent.Value.y; menu_accent[2] = clr->accent.Value.z;
										menu_accent_two[0] = clr->accent_two.Value.x; menu_accent_two[1] = clr->accent_two.Value.y; menu_accent_two[2] = clr->accent_two.Value.z;
										contrast_one[0] = clr->window.background_one.Value.x; contrast_one[1] = clr->window.background_one.Value.y; contrast_one[2] = clr->window.background_one.Value.z;
										contrast_two[0] = clr->window.background_two.Value.x; contrast_two[1] = clr->window.background_two.Value.y; contrast_two[2] = clr->window.background_two.Value.z;
										inline_c[0] = clr->window.stroke.Value.x; inline_c[1] = clr->window.stroke.Value.y; inline_c[2] = clr->window.stroke.Value.z;
										outline_c[0] = clr->widgets.stroke_two.Value.x; outline_c[1] = clr->widgets.stroke_two.Value.y; outline_c[2] = clr->widgets.stroke_two.Value.z;
										glow_c[0] = clr->glow.Value.x; glow_c[1] = clr->glow.Value.y; glow_c[2] = clr->glow.Value.z;
										text_c[0] = clr->widgets.text.Value.x; text_c[1] = clr->widgets.text.Value.y; text_c[2] = clr->widgets.text.Value.z;
										text_outline_c[0] = clr->widgets.text_outline.Value.x; text_outline_c[1] = clr->widgets.text_outline.Value.y; text_outline_c[2] = clr->widgets.text_outline.Value.z;
										text_outline_two_c[0] = clr->widgets.text_outline_two.Value.x; text_outline_two_c[1] = clr->widgets.text_outline_two.Value.y; text_outline_two_c[2] = clr->widgets.text_outline_two.Value.z;
										text_warning[0] = clr->widgets.text_warning.Value.x; text_warning[1] = clr->widgets.text_warning.Value.y; text_warning[2] = clr->widgets.text_warning.Value.z;
									}
								}
							}
							gui->end_child( );

							gui->begin_child( "Style", 1, 2 );
							{
								gui->checkbox( "Hover Highlight", &var->window.hover_hightlight );

								static bool window_glow = var->window.shadow_size > 0.f;
								gui->checkbox( "Window Glow", &window_glow );
								if ( window_glow )
									gui->slider_float( "Glow Thickness", &var->window.shadow_size, 1, 100 );
								else
									var->window.shadow_size = 0;

								gui->checkbox( "Clamp Width", &var->window.clamp_width );
								gui->checkbox( "Clamp Content", &var->window.clamp_content );
								if ( var->window.clamp_width )
								{
									static int max_width_item = 1;
									const char* max_width_items[4] = { "600", "800", "1200", "1600" };
									if ( gui->dropdown( "Max Width", &max_width_item, max_width_items, IM_ARRAYSIZE( max_width_items ) ) )
										var->window.max_width = ( float ) atoi( max_width_items[max_width_item] );
								}

								gui->checkbox( "Debug Boxes", &var->window.debug_boxes );

								static int outline_style = var->window.outline_style;
								const char* outline_items[3] = { "None", "4-way", "8-way" };
								if ( gui->dropdown( "Outline Style", &outline_style, outline_items, IM_ARRAYSIZE( outline_items ) ) )
									var->window.outline_style = outline_style;
							}
							gui->end_child( );
						}
						gui->end_content( );
					}

				}
				gui->end( );
			}

			if ( var->gui.current_section[5] )
			{
				gui->set_next_window_size_constraints( ImVec2( 386, 454 ), size_max );
				gui->begin( "Configurations", nullptr, var->window.flags );
				{
					draw->window_decorations( );
					gui->set_cursor_pos( elements->content.window_padding + ImVec2( 0, var->window.titlebar + 1 ) );

					gui->begin_content( );
					{
						gui->begin_group( );
						{
							ImGuiID cfg_id = GetCurrentWindow( )->GetID( "Configurations2" );
							const int sub = gui->get_child_subtab( cfg_id );
							gui->begin_multi_subtab( "Configurations2", 1, 1, 2, ImVec2( 0, 0 ), { "Options", "Others" } );
							{
								if ( sub == 0 ) {

								}
								else if ( sub == 1 ) {
									// cloud configs?
								}
							}
							gui->end_child( );
						}
						gui->end_group( );
					}
					gui->end_content( );
				}
				gui->end( );
			}

			var->window.width = GetCurrentWindow( )->ContentSize.x + style->ItemSpacing.x;

			if ( IsMouseHoveringRect( pos, pos + size ) )
				SetWindowFocus( );
		}
		gui->end( );
		gui->pop_style_var( );
	}
}