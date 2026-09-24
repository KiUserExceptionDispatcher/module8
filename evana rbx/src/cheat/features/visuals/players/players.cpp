#include "players.hpp"

void cheat::esp_t::render( ) {
	if ( !prop->enabled )
		return;

	std::optional< helpers::camera_frame > camera_frame = helpers::internal{}.get_frame( );

	if ( !camera_frame )
		return;

	for ( auto& p : cache->get_players( ) ) {
		o::drawing->reset_used( );

		std::optional<helpers::bounding_box> bb = helpers::internal{}.get_bounding( p, *camera_frame );
		if ( !bb )
			continue;

		if ( prop->box.enable ) {
			if ( prop->box.type == 0 ) {
				o::drawing->box( bb->min, bb->size( ), ImGui::ColorConvertFloat4ToU32( prop->box.color ), 1.0, 0.0f );
			}
			else if ( prop->box.type == 1 ) {
				o::drawing->corner( bb->min, bb->size( ), ImGui::ColorConvertFloat4ToU32( prop->box.color ), 1.0f, prop->box.length );
			}
		}

		if ( prop->info.name ) {
			if ( prop->info.display_name ) {
				o::drawing->name( bb->min, bb->size( ), (p.name + "{" + p.display_name + "}" ), ImGui::ColorConvertFloat4ToU32( prop->info.name_color ) );
			}
			else {
				o::drawing->name( bb->min, bb->size( ), p.name.c_str( ), ImGui::ColorConvertFloat4ToU32( prop->info.name_color ) );
			}
		}

		if ( prop->info.health_bar ) {
			o::drawing->bar( bb->min, bb->size( ), p.health, p.max_health, &prop->info.health_bar_color.x, 4.0f, 2.2f, IM_COL32_BLACK, prop->info.use_health );
		}

		if ( prop->info.distance ) {
			o::drawing->distance( bb->min, bb->size( ), std::to_string( (int)p.distance ) + "m", ImGui::ColorConvertFloat4ToU32( prop->info.distance_color ) );
		}
	}
}