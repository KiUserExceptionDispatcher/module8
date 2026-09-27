#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <src/graphics/framework/settings/variables.h>
#include <src/cheat/features/visuals/players/players.hpp>

namespace graphics {
	struct layout_t {
		enum e_layout : std::uint8_t {
			top = 0,
			right,
			left,
			bottom
		};

		layout_t( e_layout layout ) : layout_num( layout ) {}

		ImVec2 get_position( const ImVec2& box_pos, const ImVec2& box_size, const ImVec2& text_size, float gap = 3.0f, float offset = 0.0f ) const {
			switch ( layout_num ) {
			case left:
				return ImVec2(
					std::round( box_pos.x - gap - text_size.x ),
					std::round( box_pos.y + ( box_size.y * 0.5f ) - ( text_size.y * 0.5f ) )
				);

			case right:
				return ImVec2(
					std::round( box_pos.x + box_size.x + gap ),
					std::round( box_pos.y + offset )
				);

			case top:
				return ImVec2(
					std::round( box_pos.x + ( box_size.x * 0.5f ) - ( text_size.x * 0.5f ) ),
					std::round( box_pos.y - text_size.y - gap )
				);

			case bottom:
				return ImVec2(
					std::round( box_pos.x + ( box_size.x * 0.5f ) - ( text_size.x * 0.5f ) ),
					std::round( box_pos.y + box_size.y + gap + offset )
				);
			}

			return box_pos;
		}

		e_layout layout_num;
	};

	struct drawing_t {
		static drawing_t& get( ) {
			static drawing_t instance;
			return instance;
		}

		float used[4] = {0.f, 0.f, 0.f, 0.f};

		void reset_used( ) {
			used[0] = 0.f;
			used[1] = 0.f;
			used[2] = 0.f;
			used[3] = 0.f;
		}

		void box( const ImVec2& pos, const ImVec2& size, ImU32 col, float thickness, float rounding ) {
			ImVec2 rounded_pos( std::round( pos.x ), std::round( pos.y ) );
			ImVec2 rounded_size( std::round( size.x ), std::round( size.y ) );
			ImVec2 rect_max( rounded_pos.x + rounded_size.x, rounded_pos.y + rounded_size.y );

			ImDrawList* draw = ImGui::GetBackgroundDrawList( );

			float max_rounding = (std::min)( rounded_size.x, rounded_size.y ) / 2.0f;
			rounding = (std::min)( rounding, max_rounding );
			ImU32 outline = IM_COL32( 0, 0, 0, col >> 24 );

			draw->AddRect( rounded_pos, rect_max, outline, rounding, 0, thickness );
			draw->AddRect( ImVec2( rounded_pos.x - 2.f, rounded_pos.y - 2.f ), ImVec2( rect_max.x + 2.f, rect_max.y + 2.f ), outline, rounding, 0, thickness );
			draw->AddRect( ImVec2( rounded_pos.x - 1.f, rounded_pos.y - 1.f ), ImVec2( rect_max.x + 1.f, rect_max.y + 1.f ), col, rounding, 0, thickness );
		}

		void corner( const ImVec2& pos, const ImVec2& size, ImU32 col, float rounding = 0.f, float length = 20.0f ) {
			auto draw = ImGui::GetBackgroundDrawList( );

			float X = pos.x;
			float Y = pos.y;
			float W = size.x;
			float H = size.y;

			float lineW = size.x / length;
			float lineH = size.y / length;
			float lineT = 1;
			float topOffset = 1.0f;

			auto outline = IM_COL32( 0, 0, 0, col >> 24 );

			draw->AddLine( {roundf( X - lineT + 1.f ), roundf( Y - lineT - topOffset )}, {roundf( X + lineW ), roundf( Y - lineT - topOffset )}, outline );
			draw->AddLine( {roundf( X - lineT ), roundf( Y - lineT - topOffset )}, {roundf( X - lineT ), roundf( Y + lineH )}, outline );

			draw->AddLine( {roundf( X + W - lineW ), roundf( Y - lineT - topOffset )}, {roundf( X + W + lineT ), roundf( Y - lineT - topOffset )}, outline );
			draw->AddLine( {roundf( X + W + lineT ), roundf( Y - lineT - topOffset )}, {roundf( X + W + lineT ), roundf( Y + lineH )}, outline );

			draw->AddLine( {roundf( X + W + lineT ), roundf( Y + H - lineH )}, {roundf( X + W + lineT ), roundf( Y + H + lineT )}, outline );
			draw->AddLine( {roundf( X + W - lineW ), roundf( Y + H + lineT )}, {roundf( X + W + lineT ), roundf( Y + H + lineT )}, outline );

			draw->AddLine( {roundf( X - lineT ), roundf( Y + H - lineH )}, {roundf( X - lineT ), roundf( Y + H + lineT )}, outline );
			draw->AddLine( {roundf( X - lineT ), roundf( Y + H + lineT )}, {roundf( X + lineW ), roundf( Y + H + lineT )}, outline );

			draw->AddLine( {roundf( X - ( lineT - 3 ) ), roundf( Y - ( lineT - 2 ) - topOffset )}, {roundf( X + lineW ), roundf( Y - ( lineT - 2 ) - topOffset )}, outline );
			draw->AddLine( {roundf( X - ( lineT - 2 ) ), roundf( Y - ( lineT - 2 ) - topOffset )}, {roundf( X - ( lineT - 2 ) ), roundf( Y + lineH )}, outline );

			draw->AddLine( {roundf( X - ( lineT - 2 ) ), roundf( Y + H - lineH )}, {roundf( X - ( lineT - 2 ) ), roundf( Y + H + ( lineT - 2 ) )}, outline );
			draw->AddLine( {roundf( X - ( lineT - 2 ) ), roundf( Y + H + ( lineT - 2 ) )}, {roundf( X + lineW ), roundf( Y + H + ( lineT - 2 ) )}, outline );

			draw->AddLine( {roundf( X + W - lineW ), roundf( Y - ( lineT - 2 ) - topOffset )}, {roundf( X + W + ( lineT - 2 ) ), roundf( Y - ( lineT - 2 ) - topOffset )}, outline );
			draw->AddLine( {roundf( X + W + ( lineT - 2 ) ), roundf( Y - ( lineT - 2 ) - topOffset )}, {roundf( X + W + ( lineT - 2 ) ), roundf( Y + lineH )}, outline );

			draw->AddLine( {roundf( X + W + ( lineT - 2 ) ), roundf( Y + H - lineH )}, {roundf( X + W + ( lineT - 2 ) ), roundf( Y + H + ( lineT - 2 ) )}, outline );
			draw->AddLine( {roundf( X + W - lineW ), roundf( Y + H + ( lineT - 2 ) )}, {roundf( X + W + ( lineT - 2 ) ), roundf( Y + H + ( lineT - 2 ) )}, outline );

			draw->AddLine( {roundf( X ), roundf( Y - topOffset )}, {roundf( X ), roundf( Y + lineH )}, col );
			draw->AddLine( {roundf( X + 1.f ), roundf( Y - topOffset )}, {roundf( X + lineW ), roundf( Y - topOffset )}, col );

			draw->AddLine( {roundf( X + W - lineW ), roundf( Y - topOffset )}, {roundf( X + W ), roundf( Y - topOffset )}, col );
			draw->AddLine( {roundf( X + W ), roundf( Y - topOffset )}, {roundf( X + W ), roundf( Y + lineH )}, col );

			draw->AddLine( {roundf( X ), roundf( Y + H - lineH )}, {roundf( X ), roundf( Y + H )}, col );
			draw->AddLine( {roundf( X ), roundf( Y + H )}, {roundf( X + lineW ), roundf( Y + H )}, col );

			draw->AddLine( {roundf( X + W - lineW ), roundf( Y + H )}, {roundf( X + W ), roundf( Y + H )}, col );
			draw->AddLine( {roundf( X + W ), roundf( Y + H - lineH )}, {roundf( X + W ), roundf( Y + H )}, col );
		}

		static void bar( const ImVec2& box_pos, const ImVec2& box_size, float health, float max_health, const float color[4], float gap = 4.0f, float thickness = 2.2f, ImU32 outline_col = IM_COL32( 0, 0, 0, 255 ), bool use_health = true ) {
			ImDrawList* draw = ImGui::GetBackgroundDrawList( );
			draw->Flags &= ~ImDrawListFlags_AntiAliasedLines;

			float ratio = max_health > 0.f ? health / max_health : 0.f;
			ratio = std::clamp( ratio, 0.f, 1.f );

			layout_t left_side( layout_t::left );
			ImVec2 bar_base_pos = left_side.get_position( box_pos, box_size, ImVec2( thickness, box_size.y ), gap );

			float y_min = std::round( box_pos.y );
			float y_max = std::round( box_pos.y + box_size.y );
			float x_hp = std::round( bar_base_pos.x );

			ImVec2 bg_min( x_hp - 1.f, y_min - 2.f );
			ImVec2 bg_max( x_hp + thickness + 1.f, y_max + 2.f );
			draw->AddRectFilled( bg_min, bg_max, outline_col );

			ImVec2 empty_min( x_hp, y_min - 1.f );
			ImVec2 empty_max( x_hp + thickness, y_max + 1.f );
			draw->AddRectFilled( empty_min, empty_max, IM_COL32( 130, 130, 130, 150 ) );

			float height = ( y_max - y_min ) * ratio;

			ImVec2 fg_min( x_hp, y_max - height - 1.f );
			ImVec2 fg_max( x_hp + thickness, y_max + 1.f );

			ImVec4 final_color;

			if ( use_health ) {
				if ( ratio < 0.2f )
					final_color = ImVec4( 235.f / 255.f, 52.f / 255.f, 52.f / 255.f, color[3] );
				else if ( ratio < 0.4f )
					final_color = ImVec4( 235.f / 255.f, 168.f / 255.f, 52.f / 255.f, color[3] );
				else if ( ratio < 0.7f )
					final_color = ImVec4( 192.f / 255.f, 235.f / 255.f, 52.f / 255.f, color[3] );
				else
					final_color = ImVec4( 94.f / 255.f, 235.f / 255.f, 52.f / 255.f, color[3] );
			}
			else {
				final_color = ImVec4( color[0], color[1], color[2], color[3] );
			}

			draw->AddRectFilled(
				fg_min,
				fg_max,
				ImGui::ColorConvertFloat4ToU32( final_color )
			);
		}

		static void add_text( ImDrawList* draw, ImFont* font, float font_size, const ImVec2& pos, ImU32 color, const std::string& text_str, bool draw_outline = false ) {
			if ( !draw || text_str.empty( ) )
				return;

			if ( font && font_size > 0.0f )
				draw->AddText( font, font_size, pos, color, text_str.c_str( ) );
			else
				draw->AddText( pos, color, text_str.c_str( ) );
		}

		void text( ImDrawList* draw, const ImVec2& box_pos, const ImVec2& box_size, const std::string& text_str, const layout_t& side, ImU32 color, ImU32 outline_color, ImFont* font = nullptr, float font_size = 0.f, float gap = 3.2f, float stack_gap = 2.0f ) {
			if ( !draw || text_str.empty( ) )
				return;

			if ( !font )
				font = ImGui::GetFont( );

			if ( font_size <= 0.f )
				font_size = ImGui::GetFontSize( );

			if ( font && font_size <= 0.f )
				font_size = font->FontSize > 0.f ? font->FontSize : 13.f;

			if ( !font || font_size <= 0.f )
				return;

			const ImVec2 text_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.f, text_str.c_str( ) );

			if ( text_size.x <= 0.f || text_size.y <= 0.f )
				return;

			ImVec2 pos = side.get_position( box_pos, box_size, text_size, gap, 0.f );

			const int idx = static_cast<int>( side.layout_num );
			const float offset = used[idx];

			if ( side.layout_num == layout_t::top )
				pos.y -= offset;
			else
				pos.y += offset;

			if ( side.layout_num == layout_t::top && pos.y < 1.f )
				pos.y = box_pos.y + gap;

			pos.x = std::round( pos.x );
			pos.y = std::round( pos.y );

			for ( int x = -1; x <= 1; x++ ) {
				for ( int y = -1; y <= 1; y++ ) {
					if ( !( x || y ) )
						continue;

					draw->AddText( font, font_size, ImVec2( pos.x + static_cast<float>( x ), pos.y + static_cast<float>( y ) ), outline_color, text_str.c_str( ) );
				}
			}

			draw->AddText( font, font_size, pos, color, text_str.c_str( ) );

			used[idx] += text_size.y + stack_gap;
		}

		void name( const ImVec2& pos, const ImVec2& size, const std::string& text_str, ImU32 col ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );
			this->text( draw_list, pos, size, text_str, layout_t( layout_t::top ), col, IM_COL32( 0, 0, 0, 255 ), var->font.tahoma, 0.f, 2.0f );
		}

		void team( const ImVec2& pos, const ImVec2& size, const std::string& text_str, ImU32 col ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );
			this->text( draw_list, pos, size, text_str, layout_t( layout_t::right ), col, IM_COL32( 0, 0, 0, 255 ), var->font.tahoma, 0.f, 2.0f );
		}

		void weapon( const ImVec2& pos, const ImVec2& size, const std::string& text_str, ImU32 col ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );
			this->text( draw_list, pos, size, text_str, layout_t( layout_t::bottom ), col, IM_COL32( 0, 0, 0, 255 ), var->font.tahoma, 0.f, 2.0f );
		}

		void distance( const ImVec2& pos, const ImVec2& size, const std::string& text_str, ImU32 col ) {
			auto draw_list = ImGui::GetBackgroundDrawList( );
			this->text( draw_list, pos, size, text_str, layout_t( layout_t::bottom ), col, IM_COL32( 0, 0, 0, 255 ), var->font.tahoma, 0.f, 2.0f );
		}
	};

	inline std::unique_ptr< drawing_t > drawing = std::make_unique< drawing_t >( );
}