#pragma once

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_set>

namespace cheat {
	enum type : int {
		disabled = 0,
		xray = 1,
		wireframe = 2,
		glow = 3,
		flat = 4,
		glass = 5,
		metal = 6,
		ghost = 7,
		invisible = 8
	};

	struct engine_chams_cfg_t {
		bool enabled{true};
		bool alpha{false};
		bool use_color{false};

		int style{type::disabled};
		int color{0};
		int alpha_value{255};

		std::uint32_t color_value{0};
	};

	struct engine_chams_t {
	private:
		struct layer_backup_t {
			std::uint8_t fill_mode;
			std::uint32_t material_flags;
			std::uint32_t param;
			std::uint32_t flags_2;
			std::uint32_t color;
		};

		struct entity_backup_t {
			std::uint32_t render_queue_id;
			std::uint8_t alpha;
		};

	public:
		void init( );
		void denit( );

	public:
		std::unique_ptr< engine_chams_cfg_t > prop = std::make_unique< engine_chams_cfg_t >( );

	private:
		void loop( );

		void material_set( std::uintptr_t address );
		void entity_locked( std::uintptr_t address );

		void reset( );
		void reload( );
		bool is_local_entity( std::uintptr_t address );

		std::uint32_t get_color_param( int index );
		std::uint32_t get_style_queue( int style );
		std::uintptr_t get_base_address( );

	private:
		std::unordered_map< std::uintptr_t, layer_backup_t > backups;
		std::unordered_map< std::uintptr_t, entity_backup_t > entity_backups;
		std::unordered_map< std::uintptr_t,std::vector< std::uintptr_t > > entity_layers;
		std::unordered_set< std::uintptr_t > known_entities;

		std::mutex mutex;
		std::atomic<bool> running{false};
		std::thread thread;
	};

	inline std::unique_ptr<engine_chams_t> engine_chams =
		std::make_unique<engine_chams_t>( );

}