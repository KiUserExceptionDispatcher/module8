#include "players.hpp"

void cheat::esp_t::render( ) {
	if ( !prop->enabled )
		return;

	std::optional< helpers::camera_frame > camera_frame = helpers::internal{}.get_frame( );

	if ( !camera_frame )
		return;

	for ( auto& p : cache->get_players( ) ) {
		graphics::drawing->reset_used( );

		std::optional<helpers::bounding_box> bb = helpers::internal{}.get_bounding( p, *camera_frame );
		if ( !bb )
			continue;

		if ( prop->box.enable ) {
			if ( prop->box.type == 0 ) {
				graphics::drawing->box( bb->min, bb->size( ), ImGui::ColorConvertFloat4ToU32( prop->box.color ), 1.0, 0.0f );
			}
			else if ( prop->box.type == 1 ) {
				graphics::drawing->corner( bb->min, bb->size( ), ImGui::ColorConvertFloat4ToU32( prop->box.color ), 1.0f, prop->box.length );
			}
		}

		if ( prop->info.name ) {
			if ( prop->info.display_name ) {
				graphics::drawing->name( bb->min, bb->size( ), (p.name + "{" + p.display_name + "}" ), ImGui::ColorConvertFloat4ToU32( prop->info.name_color ) );
			}
			else {
				graphics::drawing->name( bb->min, bb->size( ), p.name.c_str( ), ImGui::ColorConvertFloat4ToU32( prop->info.name_color ) );
			}
		}

		if ( prop->info.health_bar ) {
			graphics::drawing->bar( bb->min, bb->size( ), p.health, p.max_health, &prop->info.health_bar_color.x, 4.0f, 2.2f, IM_COL32_BLACK, prop->info.use_health );
		}

		if ( prop->info.distance ) {
			graphics::drawing->distance( bb->min, bb->size( ), std::to_string( (int)p.distance ) + "m", ImGui::ColorConvertFloat4ToU32( prop->info.distance_color ) );
		}
	}
}