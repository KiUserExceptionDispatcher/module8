#pragma once
// <header guard>
#include <imgui.h>
#include <optional>
#include <cmath>
#include <algorithm>

namespace helpers { // <i'll finish this tonight, or tmr>
	struct camera_frame {
		engine::vector2 dimensions;
		engine::matrix4x4 matrix;
	};

	struct bounding_box {
		ImVec2 min{};
		ImVec2 max{};

		float width( ) {
			return max.x - min.x;
		}

		float height( ) {
			return max.y - min.y;
		}

		ImVec2 size( ) {
			return {width( ), height( )};
		}

		ImVec2 center( ) {
			return {min.x + width( ) * 0.5f, min.y + height( ) * 0.5f};
		}
	};

	struct internal {
		std::optional<camera_frame> get_frame( ) {
			engine::visual_engine engine = engine::visual_engine::get( );
			if ( !engine.is_valid( ) ) {
				engine = g->visual_engine;
			}
			if ( !engine.is_valid( ) ) {
				return std::nullopt;
			}

			camera_frame frame;
			frame.matrix = engine.get_matrix( );
			frame.dimensions = engine.get_dimensions( );

			if ( frame.dimensions.x <= 0.f || frame.dimensions.y <= 0.f ) {
				ImGuiIO& io = ImGui::GetIO( );
				frame.dimensions = engine::vector2( io.DisplaySize.x, io.DisplaySize.y );
			}

			return frame;
		}

		static internal& get( ) {
			static internal g;
			return g;
		}

		std::optional<ImVec2> w2s( const engine::vector3& world, const camera_frame& frame ) {
			engine::screen_point screen = engine::world_to_screen( frame.matrix, world, frame.dimensions );
			if ( !screen.on_screen ) {
				return std::nullopt;
			}

			return ImVec2{screen.position.x, screen.position.y};
		}

	private:
		void project_part(
			const engine::vector3& position, const engine::vector3& size, const engine::matrix3x3& rotation,
			const camera_frame& frame,
			float& min_x, float& min_y, float& max_x, float& max_y, bool& has_point
		) {
			auto accumulate = [&]( const ImVec2& screen ) {
				has_point = true;
				min_x = (std::min)( min_x, screen.x );
				min_y = (std::min)( min_y, screen.y );
				max_x = (std::max)( max_x, screen.x );
				max_y = (std::max)( max_y, screen.y );
				};

			if ( !std::isfinite( position.x ) || !std::isfinite( position.y ) || !std::isfinite( position.z ) ) {
				return;
			}

			const bool has_size = std::isfinite( size.x ) && std::isfinite( size.y ) && std::isfinite( size.z )
				&& size.x > 0.f && size.y > 0.f && size.z > 0.f;

			if ( !has_size ) {
				if ( const auto screen = w2s( position, frame ) ) {
					accumulate( *screen );
				}
				return;
			}

			const auto right = rotation.get_right( );
			const auto up = rotation.get_up( );
			const auto look = rotation.get_look( );
			const engine::vector3 half = size * 0.5f;

			for ( int xi : { -1, 1 } ) {
				for ( int yi : { -1, 1 } ) {
					for ( int zi : { -1, 1 } ) {
						const engine::vector3 corner =
							position +
							right * ( half.x * static_cast<float>( xi ) ) +
							up * ( half.y * static_cast<float>( yi ) ) +
							look * ( half.z * static_cast<float>( zi ) );

						if ( const auto screen = w2s( corner, frame ) ) {
							accumulate( *screen );
						}
					}
				}
			}
		}

		std::optional<bounding_box> finalize_bounding(
			float min_x, float min_y, float max_x, float max_y, bool has_point, const camera_frame& frame
		) {
			if ( !has_point ) {
				return std::nullopt;
			}

			if ( max_x <= 0.f || min_x >= frame.dimensions.x || max_y <= 0.f || min_y >= frame.dimensions.y ) {
				return std::nullopt;
			}

			if ( min_x >= max_x || min_y >= max_y ) {
				return std::nullopt;
			}

			if ( ( max_x - min_x ) > frame.dimensions.x * 2.0f || ( max_y - min_y ) > frame.dimensions.y * 2.0f ) {
				return std::nullopt;
			}

			return bounding_box{ImVec2( min_x, min_y ), ImVec2{max_x, max_y}};
		}

	public:
		std::optional<bounding_box> get_bounding( o::cache::render_entity& entry, camera_frame& frame ) {
			constexpr float max_val = ( std::numeric_limits<float>::max )( );
			float min_x = max_val, min_y = max_val, max_x = -max_val, max_y = -max_val;
			bool has_point = false;

			project_part( entry.position, entry.size, entry.rotation, frame, min_x, min_y, max_x, max_y, has_point );

			return finalize_bounding( min_x, min_y, max_x, max_y, has_point, frame );
		}

		std::optional<bounding_box> get_bounding( o::cache::entity& entry, camera_frame& frame ) {
			constexpr float max_val = ( std::numeric_limits<float>::max )( );
			constexpr float max_part_distance = 10.f;

			const engine::vector3* anchor = nullptr;
			for ( const auto& part : entry.parts ) {
				if ( part.name == "HumanoidRootPart" || part.name == "UpperTorso" || part.name == "Torso" ) {
					anchor = &part.position;
					break;
				}
			}
			if ( !anchor && !entry.parts.empty( ) ) {
				anchor = &entry.parts[0].position;
			}

			float min_x = max_val, min_y = max_val, max_x = -max_val, max_y = -max_val;
			bool has_point = false;

			for ( const auto& part : entry.parts ) {
				if ( part.is_accessory || part.name == "Handle" ) {
					continue;
				}
				if ( anchor && part.position.distance_to( *anchor ) > max_part_distance ) {
					continue;
				}
				project_part( part.position, part.size, part.rotation, frame, min_x, min_y, max_x, max_y, has_point );
			}

			if ( !has_point ) {
				for ( const auto& part : entry.parts ) {
					if ( anchor && part.position.distance_to( *anchor ) > max_part_distance ) {
						continue;
					}
					project_part( part.position, part.size, part.rotation, frame, min_x, min_y, max_x, max_y, has_point );
				}
			}

			return finalize_bounding( min_x, min_y, max_x, max_y, has_point, frame );
		}
	};
}