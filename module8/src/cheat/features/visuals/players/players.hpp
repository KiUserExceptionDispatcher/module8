#pragma once
#include "../helper.hpp"
#include <src/graphics/drawing/drawing.hpp>

namespace cheat {
	struct esp_cfg_t {
		bool enabled{false};

		struct box_t {
			bool enable{false};
			int type{0};
			float length{20.0f};
			bool fill{false};

			ImVec4 color = ImVec4( 0.510f, 0.478f, 0.435f, 1.0f );
		};
		box_t box;

		struct info_t {
			bool health_bar{false};
			bool use_health{false};
			bool name{false};
			bool display_name{false};
			bool distance{false};

			ImVec4 name_color = ImVec4( 0.510f, 0.478f, 0.435f, 1.0f );
			ImVec4 distance_color = ImVec4( 0.510f, 0.478f, 0.435f, 1.0f );
			ImVec4 health_bar_color = ImVec4( 0.510f, 0.478f, 0.435f, 1.0f );
		};
		info_t info;
	};

	struct esp_t {
		std::unique_ptr< esp_cfg_t > prop = std::make_unique< esp_cfg_t >( );

		void render( );
	};
	inline std::unique_ptr< esp_t > esp = std::make_unique< esp_t >( );
}