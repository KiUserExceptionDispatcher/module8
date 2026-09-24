#pragma once
// <header guard>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>

namespace o {
	namespace cache {

		struct part {
			std::uint64_t address = 0;
			std::uint64_t primitive = 0;
			std::string name{};
			std::string mesh_id{};
			engine::vector3 mesh_scale{0.f, 0.f, 0.f};
			engine::vector3 mesh_offset{0.f, 0.f, 0.f};
			engine::matrix3x3 rotation{};
			engine::vector3 position{};
			engine::vector3 velocity{};
			engine::vector3 size{};
			bool is_accessory = false;
			std::vector< engine::special_mesh > meshes{};
		};

		struct entity {
			std::uint64_t address = 0;
			engine::player player{};
			engine::instance team{};
			engine::model model{};
			engine::humanoid humanoid{};
			std::string name{};
			std::string display_name{};
			bool local_player = false;
			std::vector< part > parts{};
			float health = 0.f;
			float max_health = 0.f;
			std::string tool{};
			std::uint64_t last_model = 0;
			std::uint64_t last_children_start = 0;
			std::uint64_t last_children_end = 0;
			float priority = 0.f;
			float distance = 0.f;
			bool teammate = false;

			const part* get_bone( ) const;
			const part* get_part( const std::string& part_name ) const;
			const part* get_hitbox( int bone ) const;
		};

		struct render_entity {
			std::uint64_t address = 0;
			std::uint64_t primitive = 0;
			engine::base_part part{};
			engine::vector3 position{};
			engine::vector3 size{};
			engine::matrix3x3 rotation{};
			std::vector< engine::special_mesh > meshes{};
		};

		class manager {
		public:
			manager( ) = default;
			manager( const manager& ) = delete;
			manager& operator=( const manager& ) = delete;
			~manager( );

			static manager& get( );

			bool start( );
			bool stop( );
			bool is_ready( ) const;
			void reset( );

			void init( ) { this->start( ); }
			void denit( ) { this->stop( ); }

			std::vector< entity > get_players( ) const;
			std::vector< entity > snapshot_players( );
			std::vector< render_entity > get_render_entities( ) const;
			std::size_t add_render_entities( const std::vector< render_entity >& entities );

			void static_loop( );
			void transform( );

			std::uint64_t base = 0;
			std::uint64_t data_model = 0;
			std::uint64_t fake_dm = 0;
			std::uint64_t players_service = 0;
			std::uint64_t mouse_service = 0;
			std::uint64_t workspace = 0;
			std::uint64_t camera = 0;
			std::uint64_t local_player = 0;
			std::uint64_t local_prim = 0;
			std::uint64_t local_team = 0;
			std::atomic< std::uint64_t > epoch{0};

		private:
			std::atomic< bool > active{false};
			std::thread statics{};
			std::thread transforms{};
			mutable std::shared_mutex mutex_{};
			std::vector< entity > players_{};
			std::vector< render_entity > render_entities_{};
		};

		using part_entry = part;
		using player_entry = entity;
		using c_cache = manager;
	}

	using part_entry = cache::part;
	using player_entry = cache::entity;
	using c_cache = cache::manager;
}

inline o::cache::manager* cache = &o::cache::manager::get( );