#pragma once
#include "../offsets.hpp"

namespace engine {
	struct vector2 {
	public:
		float x, y;

		auto operator+( const vector2& other ) const -> vector2 {
			return {this->x + other.x, this->y + other.y};
		}

		auto operator-( const vector2& other ) const -> vector2 {
			return {this->x - other.x, this->y - other.y};
		}

		auto operator*( float scalar ) const -> vector2 {
			return {this->x * scalar, this->y * scalar};
		}

		auto operator/( float scalar ) const -> vector2 {
			return {this->x / scalar, this->y / scalar};
		}

		auto operator-( ) const -> vector2 {
			return {-this->x, -this->y};
		}

		auto operator+=( const vector2& other ) -> vector2& {
			this->x += other.x;
			this->y += other.y;
			return *this;
		}

		auto operator-=( const vector2& other ) -> vector2& {
			this->x -= other.x;
			this->y -= other.y;
			return *this;
		}

		auto operator*=( float scalar ) -> vector2& {
			this->x *= scalar;
			this->y *= scalar;
			return *this;
		}

		auto operator/=( float scalar ) -> vector2& {
			this->x /= scalar;
			this->y /= scalar;
			return *this;
		}

		auto operator==( const vector2& other ) const -> bool {
			return this->x == other.x && this->y == other.y;
		}

		auto operator!=( const vector2& other ) const -> bool {
			return !( *this == other );
		}


		[[nodiscard]] auto magnitude( ) -> float {
			return std::sqrt( this->x * this->x + this->y * this->y );
		}

		[[nodiscard]] auto distance_to( vector2 more ) -> float {
			return ( *this - more ).magnitude( );
		}

		[[nodiscard]] auto dot( vector2 more ) -> float {
			return this->x * more.x + this->y * more.y;
		}
	};

	struct vector3 {
		float x, y, z;

		auto operator+( const vector3& other ) const -> vector3 {
			return {this->x + other.x, this->y + other.y, this->z + other.z};
		}

		auto operator-( const vector3& other ) const -> vector3 {
			return {this->x - other.x, this->y - other.y, this->z - other.z};
		}

		auto operator*( float scalar ) const -> vector3 {
			return {this->x * scalar, this->y * scalar, this->z * scalar};
		}

		auto operator/( float scalar ) const -> vector3 {
			return {this->x / scalar, this->y / scalar, this->z / scalar};
		}

		auto operator-( ) const -> vector3 {
			return {-this->x, -this->y, -this->z};
		}

		auto operator+=( const vector3& other ) -> vector3& {
			this->x += other.x;
			this->y += other.y;
			this->z += other.z;
			return *this;
		}

		auto operator-=( const vector3& other ) -> vector3& {
			this->x -= other.x;
			this->y -= other.y;
			this->z -= other.z;
			return *this;
		}

		auto operator*=( float scalar ) -> vector3& {
			this->x *= scalar;
			this->y *= scalar;
			this->z *= scalar;
			return *this;
		}

		auto operator/=( float scalar ) -> vector3& {
			this->x /= scalar;
			this->y /= scalar;
			this->z /= scalar;
			return *this;
		}

		auto operator==( const vector3& other ) const -> bool {
			return this->x == other.x && this->y == other.y && this->z == other.z;
		}

		auto operator!=( const vector3& other ) const -> bool {
			return !( *this == other );
		}

		auto magnitude( ) const -> float {
			return std::sqrt( this->x * this->x + this->y * this->y + this->z * this->z );
		}

		auto distance_to( const vector3& other ) const -> float {
			return ( *this - other ).magnitude( );
		}

		auto dot( const vector3& other ) const -> float {
			return this->x * other.x + this->y * other.y + this->z * other.z;
		}

		auto cross( const vector3& other ) const -> vector3 {
			return {
				this->y * other.z - this->z * other.y,
				this->z * other.x - this->x * other.z,
				this->x * other.y - this->y * other.x
			};
		}

		auto unit( ) const -> vector3 {
			auto mag = this->magnitude( );
			if ( mag == 0.f )
				return {};

			return *this / mag;
		}
	};

	struct matrix3x3 {
		float m[9]{};

		auto operator*( const vector3& vec ) const -> vector3 {
			return {
				this->m[0] * vec.x + this->m[1] * vec.y + this->m[2] * vec.z,
				this->m[3] * vec.x + this->m[4] * vec.y + this->m[5] * vec.z,
				this->m[6] * vec.x + this->m[7] * vec.y + this->m[8] * vec.z
			};
		}

		auto operator*( const matrix3x3& other ) const -> matrix3x3 {
			matrix3x3 out{};
			for ( auto row = 0; row < 3; row++ ) {
				for ( auto col = 0; col < 3; col++ ) {
					out.m[row * 3 + col] =
						this->m[row * 3 + 0] * other.m[0 * 3 + col] +
						this->m[row * 3 + 1] * other.m[1 * 3 + col] +
						this->m[row * 3 + 2] * other.m[2 * 3 + col];
				}
			}
			return out;
		}

		auto get_column( int index ) const -> vector3 {
			return {this->m[index], this->m[index + 3], this->m[index + 6]};
		}

		auto get_right( ) const -> vector3 {
			return this->get_column( 0 );
		}

		auto get_up( ) const -> vector3 {
			return this->get_column( 1 );
		}

		auto get_look( ) const -> vector3 {
			return this->get_column( 2 );
		}
	};

	struct matrix4x4 {
		float m[16]{};

		auto operator*( const matrix4x4& other ) const -> matrix4x4 {
			matrix4x4 out{};
			for ( auto row = 0; row < 4; row++ ) {
				for ( auto col = 0; col < 4; col++ ) {
					out.m[row * 4 + col] =
						this->m[row * 4 + 0] * other.m[0 * 4 + col] +
						this->m[row * 4 + 1] * other.m[1 * 4 + col] +
						this->m[row * 4 + 2] * other.m[2 * 4 + col] +
						this->m[row * 4 + 3] * other.m[3 * 4 + col];
				}
			}
			return out;
		}

		auto multiply_point( const vector3& vec ) const -> vector3 {
			auto w = this->m[12] * vec.x + this->m[13] * vec.y + this->m[14] * vec.z + this->m[15];
			auto inv_w = ( w == 0.f ) ? 1.f : 1.f / w;

			return {
				( this->m[0] * vec.x + this->m[1] * vec.y + this->m[2] * vec.z + this->m[3] ) * inv_w,
				( this->m[4] * vec.x + this->m[5] * vec.y + this->m[6] * vec.z + this->m[7] ) * inv_w,
				( this->m[8] * vec.x + this->m[9] * vec.y + this->m[10] * vec.z + this->m[11] ) * inv_w
			};
		}
	};

	struct cframe {
		matrix3x3 rotation{};
		vector3 position{};
	};

	struct screen_point {
		vector2 position{};
		bool on_screen = false;
	};

	inline auto world_to_screen( const matrix4x4& view_projection, const vector3& world_position, const vector2& viewport_size ) -> screen_point {
		const auto& m = view_projection.m;

		auto clip_x = m[0] * world_position.x + m[1] * world_position.y + m[2] * world_position.z + m[3];
		auto clip_y = m[4] * world_position.x + m[5] * world_position.y + m[6] * world_position.z + m[7];
		auto clip_w = m[12] * world_position.x + m[13] * world_position.y + m[14] * world_position.z + m[15];

		if ( clip_w < 0.01f )
			return {};

		auto inv_w = 1.0f / clip_w;
		auto ndc_x = clip_x * inv_w;
		auto ndc_y = clip_y * inv_w;

		screen_point result;
		result.position.x = ( ndc_x * 0.5f + 0.5f ) * viewport_size.x;
		result.position.y = ( 1.0f - ( ndc_y * 0.5f + 0.5f ) ) * viewport_size.y;
		result.on_screen = true;
		return result;
	}

	struct color3 {
		float r, g, b, a;
	};

	struct udim2 {
		float x_scale = 0.f;
		float x_offset = 0.f;
		float y_scale = 0.f;
		float y_offset = 0.f;
	};

	struct addressable {
		std::uint64_t address = 0;

		addressable( ) = default;
		addressable( std::uint64_t address ) : address( address ) {}

		bool is_valid( ) {
			return this->address != 0;
		}
	};

	struct instance : addressable {
		using addressable::addressable;

		std::string get_name( );
		std::string get_class_name( );
		std::vector< instance > get_children( );
		instance find_child( std::string child_name );
		instance find_class( std::string class_name );
		instance get_parent( );
		// < no set parent < //
	};

	struct air_properties : addressable {
		using addressable::addressable;

		float get_air_density( );
		vector3 get_global_wind( );
	};

	struct world : addressable {
		using addressable::addressable;

		float get_gravity( );
		float get_fallen_parts_destroy_height( );
		float get_world_steps_per_sec( );
		air_properties get_air_properties( );
		std::uint64_t get_primitives( );
	};

	struct workspace : instance {
		using instance::instance;

		instance get_current_camera( );
		world get_world( );
		float get_distributed_game_time( );
		float get_gravity( );
	};

	struct camera : instance {
		using instance::instance;

		vector3 get_position( );
		matrix3x3 get_rotation( );
		void set_rotation( const matrix3x3& value );
		vector2 get_viewport_size( );
		float get_field_of_view( );
		int get_camera_type( );
		instance get_subject( );
	};

	struct visual_engine : instance {
		using instance::instance;

		static visual_engine get( );

		vector2 get_dimensions( );
		matrix4x4 get_matrix( );
	};

	struct model : instance {
		using instance::instance;

		instance get_primary_part( );
		vector3 get_scale( );
	};

	struct player : instance {
		using instance::instance;

		std::uint64_t get_user_id( );
		int get_account_age( );
		std::string get_display_name( );
		bool is_local_player( );
		engine::instance get_client( );
		instance get_local_player( );
		instance get_character( );
		instance get_team( );
		instance get_mouse( );
	};

	struct team : player {
		using player::player;

		engine::vector3 get_color( );
	};

	struct humanoid : instance {
		using instance::instance;

		float get_health( );
		float get_max_health( );
		float get_walkspeed( );
		float get_jump_power( );
		float get_jump_height( );
		float get_hip_height( );
		std::string get_display_name( );
		int get_state_id( );
		bool is_walking( );
		instance get_root_part( );
		instance get_seat_part( );
		instance get_move_to_part( );
		vector3 get_move_direction( );
		vector3 get_move_to_point( );
		vector3 get_target_point( );
		vector3 get_camera_offset( );
		int get_floor_material( );
		int get_rig_type( );
		float get_walk_timer( );
		float get_walkspeed_check( );
		bool is_sitting( );

		void set_health( float value );
		void set_max_health( float value );
		void set_walkspeed( float value );
		void set_jump_power( float value );
		void set_auto_jump_enabled( bool value );
		void set_auto_rotate( bool value );
		void set_jump( bool value );
	};

	struct base_part : instance {
		using instance::instance;

		std::uint64_t get_primitive( );
		vector3 get_position( );
		vector3 get_size( );
		matrix3x3 get_rotation( );
		vector3 get_linear_velocity( );
		vector3 get_angular_velocity( );
		color3 get_color( );
		float get_reflectance( );
		float get_transparency( );
		bool get_cast_shadow( );
		bool is_locked( );
		bool is_massless( );
		int get_shape( );
		int get_material( );
		bool is_anchored( );
		bool is_can_collide( );
		bool is_can_query( );
		bool is_can_touch( );

		void set_position( const vector3& value );
		void set_size( const vector3& value );
		void set_linear_velocity( const vector3& value );
		void set_anchored( bool value );
	};

	struct mesh_data : addressable {
		using addressable::addressable;

		std::uint64_t get_vertex_start( );
		std::uint64_t get_vertex_end( );
		std::uint64_t get_face_start( );
		std::uint64_t get_face_end( );
	};

	struct mesh_content_provider : instance {
		using instance::instance;

		std::uint64_t get_asset_id( );
		std::uint64_t get_cache( );
		std::uint64_t get_lru_cache( );
		std::uint64_t get_mesh_data( );
		std::uint64_t get_to_mesh_data( );
		std::uint64_t get_head( std::uint64_t lru );
		std::string get_node_asset_id( std::uint64_t node );
		mesh_data get_node_mesh_data( std::uint64_t node );

		static mesh_content_provider locate( instance& dm );
	};

	struct special_mesh : instance {
		using instance::instance;

		std::string get_mesh_id( );
		vector3 get_scale( );
	};

	struct character_mesh : instance {
		using instance::instance;

		std::string get_base_texture_id( );
		std::string get_mesh_id( );
		std::string get_overlay_texture_id( );
		int get_body_part( );
	};

	struct clothing : instance {
		using instance::instance;

		color3 get_color( );
		std::string get_template( );
	};

	struct mesh_part : base_part {
		using base_part::base_part;

		std::string get_mesh_id( );
		std::string get_texture( );
	};

	struct union_operation : base_part {
		using base_part::base_part;

		std::string get_asset_id( );
	};

	struct seat : base_part {
		using base_part::base_part;

		instance get_occupant( );
	};

	struct vehicle_seat : seat {
		using seat::seat;

		float get_max_speed( );
		float get_steer_float( );
		float get_throttle_float( );
		float get_torque( );
		float get_turn_speed( );
	};

	struct spawn_location : base_part {
		using base_part::base_part;

		bool get_allow_team_change_on_touch( );
		bool is_enabled( );
		float get_forcefield_duration( );
		bool is_neutral( );
		color3 get_team_color( );

		void set_enabled( bool value );
		void set_forcefield_duration( float value );
	};

	struct terrain : base_part {
		using base_part::base_part;

		float get_grass_length( );
		std::uint64_t get_material_colors( );
		color3 get_water_color( );
		float get_water_reflectance( );
		float get_water_transparency( );
		float get_water_wave_size( );
		float get_water_wave_speed( );
	};

	struct tool : instance {
		using instance::instance;

		cframe get_grip( );
		bool can_be_dropped( );
		bool is_enabled( );
		bool get_manual_activation_only( );
		bool get_requires_handle( );
		std::string get_texture_id( );
		std::string get_tooltip( );

		void set_enabled( bool value );
		void set_grip( const cframe& value );
	};

	struct weld : instance {
		using instance::instance;

		instance get_part0( );
		instance get_part1( );
	};

	struct weld_constraint : instance {
		using instance::instance;

		instance get_part0( );
		instance get_part1( );
	};

	struct animation_track : instance {
		using instance::instance;

		instance get_animation( );
		instance get_animator( );
		bool is_playing( );
		bool is_looped( );
		float get_speed( );
		float get_time_position( );

		void set_speed( float value );
		void set_time_position( float value );
	};

	struct lighting : instance {
		using instance::instance;

		float get_brightness( );
		float get_clock_time( );
		color3 get_ambient( );
		color3 get_color_shift_top( );
		color3 get_color_shift_bottom( );
		color3 get_outdoor_ambient( );
		color3 get_fog_color( );
		float get_fog_start( );
		float get_fog_end( );
		float get_geographic_latitude( );
		bool get_global_shadows( );
		float get_environment_diffuse_scale( );
		float get_environment_specular_scale( );
		float get_exposure_compensation( );
		color3 get_gradient_top( );
		color3 get_gradient_bottom( );
		color3 get_light_color( );
		vector3 get_light_direction( );
		vector3 get_moon_position( );
		vector3 get_sun_position( );
		std::string get_source( );
		instance get_sky( );

		void set_brightness( float value );
		void set_clock_time( float value );
		void set_fog_end( float value );
		void set_fog_start( float value );
		void set_global_shadows( bool value );
	};

	struct sky : instance {
		using instance::instance;

		std::string get_skybox_bk( );
		std::string get_skybox_dn( );
		std::string get_skybox_ft( );
		std::string get_skybox_lf( );
		std::string get_skybox_rt( );
		std::string get_skybox_up( );
		std::string get_sun_texture_id( );
		std::string get_moon_texture_id( );
		float get_skybox_orientation( );
		float get_sun_angular_size( );
		float get_moon_angular_size( );
		int get_star_count( );
	};

	struct atmosphere : instance {
		using instance::instance;

		color3 get_color( );
		float get_decay( );
		float get_density( );
		float get_glare( );
		float get_haze( );
		float get_offset( );
	};

	struct sound : instance {
		using instance::instance;

		std::string get_sound_id( );
		float get_volume( );
		float get_playback_speed( );
		bool is_playing( );
		bool is_looped( );
		float get_roll_off_max_distance( );
		float get_roll_off_min_distance( );
		instance get_sound_group( );

		void set_volume( float value );
		void set_playback_speed( float value );
	};

	struct attachment : instance {
		using instance::instance;

		vector3 get_position( );
	};

	struct beam : instance {
		using instance::instance;

		instance get_attachment0( );
		instance get_attachment1( );
		float get_brightness( );
		float get_curve_size0( );
		float get_curve_size1( );
		float get_light_emission( );
		float get_light_influence( );
		std::string get_texture( );
		float get_texture_length( );
		float get_texture_speed( );
		float get_width0( );
		float get_width1( );
		float get_z_offset( );
	};

	struct particle_emitter : instance {
		using instance::instance;

		vector3 get_acceleration( );
		float get_brightness( );
		float get_drag( );
		float get_lifetime( );
		float get_light_emission( );
		float get_light_influence( );
		float get_rate( );
		vector3 get_rot_speed( );
		vector3 get_rotation( );
		float get_speed( );
		vector2 get_spread_angle( );
		std::string get_texture( );
		float get_time_scale( );
		float get_velocity_inheritance( );
		float get_z_offset( );
	};

	struct surface_appearance : instance {
		using instance::instance;

		int get_alpha_mode( );
		color3 get_color( );
		std::string get_color_map( );
		std::string get_emissive_mask_content( );
		float get_emissive_strength( );
		color3 get_emissive_tint( );
		std::string get_metalness_map( );
		std::string get_normal_map( );
		std::string get_roughness_map( );
	};

	struct proximity_prompt : instance {
		using instance::instance;

		std::string get_action_text( );
		bool is_enabled( );
		int get_gamepad_key_code( );
		float get_hold_duration( );
		int get_key_code( );
		float get_max_activation_distance( );
		std::string get_object_text( );
		bool get_requires_line_of_sight( );
	};

	struct click_detector : instance {
		using instance::instance;

		float get_max_activation_distance( );
		std::string get_mouse_icon( );
	};

	struct drag_detector : instance {
		using instance::instance;

		std::string get_activated_cursor_icon( );
		std::string get_cursor_icon( );
		float get_max_activation_distance( );
		float get_max_drag_angle( );
		vector3 get_max_drag_translation( );
		float get_max_force( );
		float get_max_torque( );
		float get_min_drag_angle( );
		vector3 get_min_drag_translation( );
		instance get_reference_instance( );
		float get_responsiveness( );
	};

	struct script : instance {
		using instance::instance;

		std::uint64_t get_bytecode( );
		std::uint64_t get_bytecode_size( );
		std::string get_guid( );
		std::string get_hash( );
	};

	struct local_script : script {
		using script::script;
	};

	struct module_script : script {
		using script::script;

		std::string get_hash( );
	};

	struct mouse_service : instance {
		using instance::instance;

		vector2 get_mouse_position( );
		instance get_input_object( );
		instance get_input_object2( );
	};

	struct player_mouse : instance {
		using instance::instance;

		std::string get_icon( );
		instance get_workspace( );
	};

	struct gui_base2d : instance {
		using instance::instance;

		vector2 get_absolute_position( );
		float get_absolute_rotation( );
		vector2 get_absolute_size( );
	};

	struct gui_object : gui_base2d {
		using gui_base2d::gui_base2d;

		color3 get_background_color3( );
		float get_background_transparency( );
		std::string get_image( );
		int get_layout_order( );
		udim2 get_position( );
		bool get_rich_text( );
		float get_rotation( );
		bool is_screen_gui_enabled( );
		udim2 get_size( );
		std::string get_text( );
		color3 get_text_color3( );
		bool is_visible( );
		int get_z_index( );

		void set_background_transparency( float value );
		void set_visible( bool value );
	};

	struct user_input_service : instance {
		using instance::instance;

		instance get_window_input_state( );
	};

	struct stats_item : instance {
		using instance::instance;

		float get_value( );
	};

	struct textures : instance {
		using instance::instance;

		std::string get_texture( );
	};

	struct data_model : instance {
		using instance::instance;

		static engine::data_model get( );
		std::uint64_t get_game_id( );
		std::uint64_t get_creator_id( );
		bool is_game_loaded( );
		std::uint64_t get_place_id( );
		int get_place_version( );
		std::string get_job_id( );
		std::string get_server_ip( );
		engine::workspace get_workspace( );
		instance get_script_context( );
		engine::instance get_players( );
	};

	struct fast_cluster_entity : addressable {
		using addressable::addressable;

		std::uint32_t get_render_queue_id( );
		void set_render_queue_id( std::uint32_t value );

		std::uint8_t get_alpha( );
		void set_alpha( std::uint8_t value );

		std::uintptr_t get_technique_array( );
	};

	struct technique_array : addressable {
		using addressable::addressable;

		std::uintptr_t get_begin( );
		std::uintptr_t get_end( );

		std::size_t get_size( );
		std::size_t get_count( );
	};

	struct material_layer : addressable {
		using addressable::addressable;

		std::uint8_t get_fill_mode( );
		void set_fill_mode( std::uint8_t value );

		std::uint32_t get_material_flags( );
		void set_material_flags( std::uint32_t value );

		std::uint32_t get_param( );
		void set_param( std::uint32_t value );

		std::uint32_t get_flags_2( );
		void set_flags_2( std::uint32_t value );

		std::uint32_t get_color_data( );
		void set_color_data( std::uint32_t value );

		std::uintptr_t get_owner( );
	};
}