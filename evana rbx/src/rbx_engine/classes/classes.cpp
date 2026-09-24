#include "classes.hpp"

namespace O_o { // ~> helpers
	static std::string read_string_prop( std::uint64_t address ) {
		auto pointer = memory->read<std::uint64_t>( address );

		if ( pointer )
			return memory->read_string( pointer );

		return "";
	}

	static engine::color3 read_color3_prop( std::uint64_t address ) {
		engine::color3 out{};

		out.r = memory->read<float>( address );
		out.g = memory->read<float>( address + 0x4 );
		out.b = memory->read<float>( address + 0x8 );

		return out;
	}
}

std::string engine::instance::get_name( ) {
	std::uint64_t name = memory->read<std::uint64_t>( this->address + offsets::Instance::NameContainer );
	if ( !name )
		return "unknown";

	std::uint64_t length = memory->read<std::uint64_t>( name + 0x18 );
	if ( length == 0 || length > 255 )
		return "";

	std::uint64_t data = ( length >= 16 ) ? memory->read<std::uint64_t>( name + 0x8 ) : name + 0x8;
	if ( !data )
		return "";

	char buffer[256] = {};
	if ( !memory->read_buf( data, buffer, static_cast<size_t>( length ) ) )
		return "";

	return std::string( buffer, static_cast<size_t>( length ) );
}

std::string engine::instance::get_class_name( ) {
	std::uint64_t descriptor = memory->read<std::uint64_t>( this->address + offsets::Instance::ClassDescriptor );
	std::uint64_t name = memory->read<std::uint64_t>( descriptor + offsets::Instance::ClassName );

	if ( name ) {
		return memory->read_string( name );
	}

	return "unknown";
}

std::vector< engine::instance > engine::instance::get_children( ) {
	std::vector< instance > children;

	std::uint64_t container = memory->read<std::uint64_t>( this->address + offsets::Instance::ChildrenStart );
	if ( !container )
		return children;

	std::uint64_t start = memory->read<std::uint64_t>( container );
	std::uint64_t end = memory->read<std::uint64_t>( container + offsets::Instance::ChildrenEnd );

	if ( end <= start || end - start > 0x80000 )
		return children;

	for ( auto i = start; i < end; i += 0x10 ) {
		auto child = memory->read<std::uint64_t>( i );
		if ( !child )
			break;

		children.emplace_back( child );
	}

	return children;
}

engine::instance engine::instance::find_child( std::string child_name ) {
	for ( auto child : get_children( ) ) {
		if ( child.get_name( ) == child_name )
			return child;
	}

	return {0};
}

engine::instance engine::instance::find_class( std::string class_name ) {
	for ( auto child : get_children( ) ) {
		if ( child.get_class_name( ) == class_name )
			return child;
	}

	return {0};
}

engine::instance engine::instance::get_parent( ) {
	return memory->read< engine::instance >( this->address + offsets::Instance::Parent );
}

engine::data_model engine::data_model::get( ) {
	auto fake_datamodel = memory->read<std::uint64_t>( memory->base + offsets::FakeDataModel::Pointer );
	auto real_datamodel = memory->read<std::uint64_t>( fake_datamodel + offsets::FakeDataModel::RealDataModel );

	return data_model( real_datamodel );
}

std::uint64_t engine::data_model::get_game_id( ) {
	return memory->read<std::uint64_t>( this->address + offsets::DataModel::GameId );
}

std::uint64_t engine::data_model::get_creator_id( ) {
	return memory->read<std::uint64_t>( this->address + offsets::DataModel::CreatorId );
}

bool engine::data_model::is_game_loaded( ) {
	return memory->read<bool>( this->address + offsets::DataModel::GameLoaded );
}

std::uint64_t engine::data_model::get_place_id( ) {
	return memory->read<std::uint64_t>( this->address + offsets::DataModel::PlaceId );
}

int engine::data_model::get_place_version( ) {
	return memory->read<int>( this->address + offsets::DataModel::PlaceVersion );
}

std::string engine::data_model::get_job_id( ) {
	auto job_id = memory->read<std::uint64_t>( this->address + offsets::DataModel::JobId );
	if ( job_id )
		return memory->read_string( job_id );

	return "";
}

std::string engine::data_model::get_server_ip( ) {
	auto server_ip = memory->read<std::uint64_t>( this->address + offsets::DataModel::ServerIP );
	if ( server_ip )
		return memory->read_string( server_ip );

	return "";
}

engine::workspace engine::data_model::get_workspace( ) {
	return memory->read<engine::workspace>( this->address + offsets::DataModel::Workspace );
}

engine::instance engine::data_model::get_script_context( ) {
	return memory->read<engine::instance>( this->address + offsets::DataModel::ScriptContext );
}

engine::instance engine::data_model::get_players( ) {
	return this->find_child( "Players" );
}

engine::instance engine::workspace::get_current_camera( ) {
	return memory->read<engine::instance>( this->address + offsets::Workspace::CurrentCamera );
}

float engine::workspace::get_distributed_game_time( ) {
	return memory->read<float>( this->address + offsets::Workspace::DistributedGameTime );
}

float engine::workspace::get_gravity( ) {
	return memory->read<float>( this->address + offsets::Workspace::ReadOnlyGravity );
}

engine::vector3 engine::camera::get_position( ) {
	return memory->read<engine::vector3>( this->address + offsets::Camera::Position );
}

engine::matrix3x3 engine::camera::get_rotation( ) {
	return memory->read<engine::matrix3x3>( this->address + offsets::Camera::Rotation );
}

void engine::camera::set_rotation( const matrix3x3& value ) {
	memory->write<engine::matrix3x3>( this->address + offsets::Camera::Rotation, value );
}

engine::vector2 engine::camera::get_viewport_size( ) {
	return memory->read<engine::vector2>( this->address + offsets::Camera::ViewportSize );
}

float engine::camera::get_field_of_view( ) {
	return memory->read<float>( this->address + offsets::Camera::FieldOfView );
}

int engine::camera::get_camera_type( ) {
	return memory->read<int>( this->address + offsets::Camera::CameraType );
}

engine::instance engine::camera::get_subject( ) {
	return memory->read<engine::instance>( this->address + offsets::Camera::CameraSubject );
}

engine::visual_engine engine::visual_engine::get( ) {
	return memory->read<engine::visual_engine>( memory->base + offsets::VisualEngine::Pointer );
}

engine::vector2 engine::visual_engine::get_dimensions( ) {
	return memory->read<engine::vector2>( this->address + offsets::VisualEngine::Dimensions );
}

engine::matrix4x4 engine::visual_engine::get_matrix( ) {
	return memory->read<engine::matrix4x4>( this->address + offsets::VisualEngine::ViewMatrix );
}

engine::instance engine::model::get_primary_part( ) {
	return memory->read<engine::instance>( this->address + offsets::Model::PrimaryPart );
}

engine::vector3 engine::model::get_scale( ) {
	return memory->read<engine::vector3>( this->address + offsets::Model::Scale );
}

std::uint64_t engine::player::get_user_id( ) {
	return memory->read<std::uint64_t>( this->address + offsets::Player::UserId );
}

int engine::player::get_account_age( ) {
	return memory->read<int>( this->address + offsets::Player::AccountAge );
}

std::string engine::player::get_display_name( ) {
	auto pointer = memory->read<std::uint64_t>( this->address + offsets::Player::DisplayName );
	if ( pointer ) {
		auto value = memory->read_string( pointer );
		if ( !value.empty( ) && value != "Unknown" && value != "unknown" )
			return value;
	}

	auto embedded = memory->read_string( this->address + offsets::Player::DisplayName );
	if ( !embedded.empty( ) && embedded != "Unknown" && embedded != "unknown" )
		return embedded;

	return {};
}

bool engine::player::is_local_player( ) {
	return memory->read<bool>( this->address + offsets::Player::LocalPlayer );
}

engine::instance engine::player::get_client( ) {
	return memory->read<engine::instance>( this->address + offsets::Player::LocalPlayer );
}

engine::instance engine::player::get_local_player( )
{
	return memory->read<engine::instance>( this->address + offsets::Player::LocalPlayer );
}

engine::instance engine::player::get_character( ) {
	return memory->read<engine::instance>( this->address + offsets::Player::ModelInstance );
}

engine::instance engine::player::get_team( ) {
	return memory->read<engine::instance>( this->address + offsets::Player::Team );
}

engine::instance engine::player::get_mouse( ) {
	return memory->read<engine::instance>( this->address + offsets::Player::Mouse );
}

engine::vector3 engine::team::get_color( ) {
	return memory->read<engine::vector3>( this->address + offsets::Team::BrickColor );
}

float engine::humanoid::get_health( ) {
	return memory->read<float>( this->address + offsets::Humanoid::Health );
}

float engine::humanoid::get_max_health( ) {
	return memory->read<float>( this->address + offsets::Humanoid::MaxHealth );
}

float engine::humanoid::get_walkspeed( ) {
	return memory->read<float>( this->address + offsets::Humanoid::Walkspeed );
}

float engine::humanoid::get_jump_power( ) {
	return memory->read<float>( this->address + offsets::Humanoid::JumpPower );
}

float engine::humanoid::get_jump_height( ) {
	return memory->read<float>( this->address + offsets::Humanoid::JumpHeight );
}

float engine::humanoid::get_hip_height( ) {
	return memory->read<float>( this->address + offsets::Humanoid::HipHeight );
}

std::string engine::humanoid::get_display_name( ) {
	auto name = memory->read<std::uint64_t>( this->address + offsets::Humanoid::DisplayName );
	if ( name )
		return memory->read_string( name );

	return "";
}

int engine::humanoid::get_state_id( ) {
	auto state = memory->read<std::uint64_t>( this->address + offsets::Humanoid::HumanoidState );
	if ( !state )
		return -1;

	return memory->read<int>( state + offsets::Humanoid::HumanoidStateID );
}

bool engine::humanoid::is_walking( ) {
	return memory->read<bool>( this->address + offsets::Humanoid::IsWalking );
}

engine::instance engine::humanoid::get_root_part( ) {
	return memory->read<engine::instance>( this->address + offsets::Humanoid::HumanoidRootPart );
}

engine::vector3 engine::humanoid::get_move_direction( ) {
	return memory->read<engine::vector3>( this->address + offsets::Humanoid::MoveDirection );
}

engine::vector3 engine::humanoid::get_camera_offset( ) {
	return memory->read<engine::vector3>( this->address + offsets::Humanoid::CameraOffset );
}

int engine::humanoid::get_floor_material( ) {
	return memory->read<int>( this->address + offsets::Humanoid::FloorMaterial );
}

int engine::humanoid::get_rig_type( ) {
	return memory->read<int>( this->address + offsets::Humanoid::RigType );
}

void engine::humanoid::set_health( float value ) {
	memory->write<float>( this->address + offsets::Humanoid::Health, value );
}

void engine::humanoid::set_max_health( float value ) {
	memory->write<float>( this->address + offsets::Humanoid::MaxHealth, value );
}

void engine::humanoid::set_walkspeed( float value ) {
	memory->write<float>( this->address + offsets::Humanoid::Walkspeed, value );
}

void engine::humanoid::set_jump_power( float value ) {
	memory->write<float>( this->address + offsets::Humanoid::JumpPower, value );
}

std::uint64_t engine::base_part::get_primitive( ) {
	return memory->read<std::uint64_t>( this->address + offsets::BasePart::Primitive );
}

engine::vector3 engine::base_part::get_position( ) {
	return memory->read<engine::vector3>( this->get_primitive( ) + offsets::Primitive::Position );
}

engine::vector3 engine::base_part::get_size( ) {
	return memory->read<engine::vector3>( this->get_primitive( ) + offsets::Primitive::Size );
}

engine::matrix3x3 engine::base_part::get_rotation( ) {
	return memory->read<engine::matrix3x3>( this->get_primitive( ) + offsets::Primitive::Rotation );
}

engine::vector3 engine::base_part::get_linear_velocity( ) {
	return memory->read<engine::vector3>( this->get_primitive( ) + offsets::Primitive::AssemblyLinearVelocity );
}

engine::vector3 engine::base_part::get_angular_velocity( ) {
	return memory->read<engine::vector3>( this->get_primitive( ) + offsets::Primitive::AssemblyAngularVelocity );
}

engine::color3 engine::base_part::get_color( ) {
	engine::color3 out{};
	auto color = this->address + offsets::BasePart::Color3;
	out.r = memory->read<float>( color );
	out.g = memory->read<float>( color + 0x4 );
	out.b = memory->read<float>( color + 0x8 );
	return out;
}

float engine::base_part::get_transparency( ) {
	return memory->read<float>( this->address + offsets::BasePart::Transparency );
}

int engine::base_part::get_material( ) {
	return memory->read<int>( this->get_primitive( ) + offsets::Primitive::Material );
}

bool engine::base_part::is_anchored( ) {
	auto flags = memory->read<std::uint8_t>( this->get_primitive( ) + offsets::Primitive::Flags );
	return ( flags & offsets::PrimitiveFlags::Anchored ) != 0;
}

bool engine::base_part::is_can_collide( ) {
	auto flags = memory->read<std::uint8_t>( this->get_primitive( ) + offsets::Primitive::Flags );
	return ( flags & offsets::PrimitiveFlags::CanCollide ) != 0;
}

bool engine::base_part::is_can_query( ) {
	auto flags = memory->read<std::uint8_t>( this->get_primitive( ) + offsets::Primitive::Flags );
	return ( flags & offsets::PrimitiveFlags::CanQuery ) != 0;
}

bool engine::base_part::is_can_touch( ) {
	auto flags = memory->read<std::uint8_t>( this->get_primitive( ) + offsets::Primitive::Flags );
	return ( flags & offsets::PrimitiveFlags::CanTouch ) != 0;
}

float engine::base_part::get_reflectance( ) {
	return memory->read<float>( this->address + offsets::BasePart::Reflectance );
}

bool engine::base_part::get_cast_shadow( ) {
	return memory->read<bool>( this->address + offsets::BasePart::CastShadow );
}

bool engine::base_part::is_locked( ) {
	return memory->read<bool>( this->address + offsets::BasePart::Locked );
}

bool engine::base_part::is_massless( ) {
	return memory->read<bool>( this->address + offsets::BasePart::Massless );
}

int engine::base_part::get_shape( ) {
	return memory->read<int>( this->address + offsets::BasePart::Shape );
}

void engine::base_part::set_position( const vector3& value ) {
	auto primitive = this->get_primitive( );
	memory->write<engine::vector3>( primitive + offsets::Primitive::Position, value );
}

void engine::base_part::set_size( const vector3& value ) {
	auto primitive = this->get_primitive( );
	memory->write<engine::vector3>( primitive + offsets::Primitive::Size, value );
}

void engine::base_part::set_linear_velocity( const vector3& value ) {
	auto primitive = this->get_primitive( );
	memory->write<engine::vector3>( primitive + offsets::Primitive::AssemblyLinearVelocity, value );
}

void engine::base_part::set_anchored( bool value ) {
	auto primitive = this->get_primitive( );
	auto flags = memory->read<std::uint8_t>( primitive + offsets::Primitive::Flags );
	if ( value )
		flags |= offsets::PrimitiveFlags::Anchored;
	else
		flags &= ~offsets::PrimitiveFlags::Anchored;
	memory->write<std::uint8_t>( primitive + offsets::Primitive::Flags, flags );
}

engine::instance engine::humanoid::get_seat_part( ) {
	return memory->read<engine::instance>( this->address + offsets::Humanoid::SeatPart );
}

engine::instance engine::humanoid::get_move_to_part( ) {
	return memory->read<engine::instance>( this->address + offsets::Humanoid::MoveToPart );
}

engine::vector3 engine::humanoid::get_move_to_point( ) {
	return memory->read<engine::vector3>( this->address + offsets::Humanoid::MoveToPoint );
}

engine::vector3 engine::humanoid::get_target_point( ) {
	return memory->read<engine::vector3>( this->address + offsets::Humanoid::TargetPoint );
}

float engine::humanoid::get_walk_timer( ) {
	return memory->read<float>( this->address + offsets::Humanoid::WalkTimer );
}

float engine::humanoid::get_walkspeed_check( ) {
	return memory->read<float>( this->address + offsets::Humanoid::WalkspeedCheck );
}

bool engine::humanoid::is_sitting( ) {
	return memory->read<bool>( this->address + offsets::Humanoid::Sit );
}

void engine::humanoid::set_auto_jump_enabled( bool value ) {
	memory->write<bool>( this->address + offsets::Humanoid::AutoJumpEnabled, value );
}

void engine::humanoid::set_auto_rotate( bool value ) {
	memory->write<bool>( this->address + offsets::Humanoid::AutoRotate, value );
}

void engine::humanoid::set_jump( bool value ) {
	memory->write<bool>( this->address + offsets::Humanoid::Jump, value );
}

engine::world engine::workspace::get_world( ) {
	return memory->read<engine::world>( this->address + offsets::Workspace::World );
}

float engine::air_properties::get_air_density( ) {
	return memory->read<float>( this->address + offsets::AirProperties::AirDensity );
}

engine::vector3 engine::air_properties::get_global_wind( ) {
	return memory->read<engine::vector3>( this->address + offsets::AirProperties::GlobalWind );
}

float engine::world::get_gravity( ) {
	return memory->read<float>( this->address + offsets::World::Gravity );
}

float engine::world::get_fallen_parts_destroy_height( ) {
	return memory->read<float>( this->address + offsets::World::FallenPartsDestroyHeight );
}

float engine::world::get_world_steps_per_sec( ) {
	return memory->read<float>( this->address + offsets::World::worldStepsPerSec );
}

engine::air_properties engine::world::get_air_properties( ) {
	return memory->read<engine::air_properties>( this->address + offsets::World::AirProperties );
}

engine::mesh_content_provider engine::mesh_content_provider::locate( instance& dm ) {
	if ( !dm.address )
		return {};

	return mesh_content_provider( dm.find_class( "MeshContentProvider" ).address );
}

std::string engine::special_mesh::get_mesh_id( ) {
	return O_o::read_string_prop( this->address + offsets::SpecialMesh::MeshId );
}

engine::vector3 engine::special_mesh::get_scale( ) {
	return memory->read<engine::vector3>( this->address + offsets::SpecialMesh::Scale );
}

std::string engine::character_mesh::get_base_texture_id( ) {
	return O_o::read_string_prop( this->address + offsets::CharacterMesh::BaseTextureId );
}

std::string engine::character_mesh::get_mesh_id( ) {
	return O_o::read_string_prop( this->address + offsets::CharacterMesh::MeshId );
}

std::string engine::character_mesh::get_overlay_texture_id( ) {
	return O_o::read_string_prop( this->address + offsets::CharacterMesh::OverlayTextureId );
}

int engine::character_mesh::get_body_part( ) {
	return memory->read<int>( this->address + offsets::CharacterMesh::BodyPart );
}

engine::color3 engine::clothing::get_color( ) {
	return O_o::read_color3_prop( this->address + offsets::Clothing::Color3 );
}

std::string engine::clothing::get_template( ) {
	return O_o::read_string_prop( this->address + offsets::Clothing::Template );
}

std::string engine::mesh_part::get_mesh_id( ) {
	return O_o::read_string_prop( this->address + offsets::MeshPart::MeshId );
}

std::string engine::mesh_part::get_texture( ) {
	return O_o::read_string_prop( this->address + offsets::MeshPart::Texture );
}

std::string engine::union_operation::get_asset_id( ) {
	return O_o::read_string_prop( this->address + offsets::UnionOperation::AssetId );
}

engine::instance engine::seat::get_occupant( ) {
	return memory->read<engine::instance>( this->address + offsets::Seat::Occupant );
}

float engine::vehicle_seat::get_max_speed( ) {
	return memory->read<float>( this->address + offsets::VehicleSeat::MaxSpeed );
}

float engine::vehicle_seat::get_steer_float( ) {
	return memory->read<float>( this->address + offsets::VehicleSeat::SteerFloat );
}

float engine::vehicle_seat::get_throttle_float( ) {
	return memory->read<float>( this->address + offsets::VehicleSeat::ThrottleFloat );
}

float engine::vehicle_seat::get_torque( ) {
	return memory->read<float>( this->address + offsets::VehicleSeat::Torque );
}

float engine::vehicle_seat::get_turn_speed( ) {
	return memory->read<float>( this->address + offsets::VehicleSeat::TurnSpeed );
}

bool engine::spawn_location::get_allow_team_change_on_touch( ) {
	return memory->read<bool>( this->address + offsets::SpawnLocation::AllowTeamChangeOnTouch );
}

bool engine::spawn_location::is_enabled( ) {
	return memory->read<bool>( this->address + offsets::SpawnLocation::Enabled );
}

float engine::spawn_location::get_forcefield_duration( ) {
	return memory->read<float>( this->address + offsets::SpawnLocation::ForcefieldDuration );
}

bool engine::spawn_location::is_neutral( ) {
	return memory->read<bool>( this->address + offsets::SpawnLocation::Neutral );
}

engine::color3 engine::spawn_location::get_team_color( ) {
	return O_o::read_color3_prop( this->address + offsets::SpawnLocation::TeamColor );
}

void engine::spawn_location::set_enabled( bool value ) {
	memory->write<bool>( this->address + offsets::SpawnLocation::Enabled, value );
}

void engine::spawn_location::set_forcefield_duration( float value ) {
	memory->write<float>( this->address + offsets::SpawnLocation::ForcefieldDuration, value );
}

float engine::terrain::get_grass_length( ) {
	return memory->read<float>( this->address + offsets::Terrain::GrassLength );
}

std::uint64_t engine::terrain::get_material_colors( ) {
	return memory->read<std::uint64_t>( this->address + offsets::Terrain::MaterialColors );
}

engine::color3 engine::terrain::get_water_color( ) {
	return O_o::read_color3_prop( this->address + offsets::Terrain::WaterColor );
}

float engine::terrain::get_water_reflectance( ) {
	return memory->read<float>( this->address + offsets::Terrain::WaterReflectance );
}

float engine::terrain::get_water_transparency( ) {
	return memory->read<float>( this->address + offsets::Terrain::WaterTransparency );
}

float engine::terrain::get_water_wave_size( ) {
	return memory->read<float>( this->address + offsets::Terrain::WaterWaveSize );
}

float engine::terrain::get_water_wave_speed( ) {
	return memory->read<float>( this->address + offsets::Terrain::WaterWaveSpeed );
}

engine::cframe engine::tool::get_grip( ) {
	return memory->read<engine::cframe>( this->address + offsets::Tool::Grip );
}

bool engine::tool::can_be_dropped( ) {
	return memory->read<bool>( this->address + offsets::Tool::CanBeDropped );
}

bool engine::tool::is_enabled( ) {
	return memory->read<bool>( this->address + offsets::Tool::Enabled );
}

bool engine::tool::get_manual_activation_only( ) {
	return memory->read<bool>( this->address + offsets::Tool::ManualActivationOnly );
}

bool engine::tool::get_requires_handle( ) {
	return memory->read<bool>( this->address + offsets::Tool::RequiresHandle );
}

std::string engine::tool::get_texture_id( ) {
	return O_o::read_string_prop( this->address + offsets::Tool::TextureId );
}

std::string engine::tool::get_tooltip( ) {
	return O_o::read_string_prop( this->address + offsets::Tool::Tooltip );
}

void engine::tool::set_enabled( bool value ) {
	memory->write<bool>( this->address + offsets::Tool::Enabled, value );
}

void engine::tool::set_grip( const engine::cframe& value ) {
	memory->write<engine::cframe>( this->address + offsets::Tool::Grip, value );
}

engine::instance engine::weld::get_part0( ) {
	return memory->read<engine::instance>( this->address + offsets::Weld::Part0 );
}

engine::instance engine::weld::get_part1( ) {
	return memory->read<engine::instance>( this->address + offsets::Weld::Part1 );
}

engine::instance engine::weld_constraint::get_part0( ) {
	return memory->read<engine::instance>( this->address + offsets::WeldConstraint::Part0 );
}

engine::instance engine::weld_constraint::get_part1( ) {
	return memory->read<engine::instance>( this->address + offsets::WeldConstraint::Part1 );
}

engine::instance engine::animation_track::get_animation( ) {
	return memory->read<engine::instance>( this->address + offsets::AnimationTrack::Animation );
}

engine::instance engine::animation_track::get_animator( ) {
	return memory->read<engine::instance>( this->address + offsets::AnimationTrack::Animator );
}

bool engine::animation_track::is_playing( ) {
	return memory->read<bool>( this->address + offsets::AnimationTrack::IsPlaying );
}

bool engine::animation_track::is_looped( ) {
	return memory->read<bool>( this->address + offsets::AnimationTrack::Looped );
}

float engine::animation_track::get_speed( ) {
	return memory->read<float>( this->address + offsets::AnimationTrack::Speed );
}

float engine::animation_track::get_time_position( ) {
	return memory->read<float>( this->address + offsets::AnimationTrack::TimePosition );
}

void engine::animation_track::set_speed( float value ) {
	memory->write<float>( this->address + offsets::AnimationTrack::Speed, value );
}

void engine::animation_track::set_time_position( float value ) {
	memory->write<float>( this->address + offsets::AnimationTrack::TimePosition, value );
}

float engine::lighting::get_brightness( ) {
	return memory->read<float>( this->address + offsets::Lighting::Brightness );
}

float engine::lighting::get_clock_time( ) {
	return memory->read<float>( this->address + offsets::Lighting::ClockTime );
}

engine::color3 engine::lighting::get_ambient( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::Ambient );
}

engine::color3 engine::lighting::get_color_shift_top( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::ColorShift_Top );
}
// < > //
engine::color3 engine::lighting::get_color_shift_bottom( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::ColorShift_Bottom );
}

engine::color3 engine::lighting::get_outdoor_ambient( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::OutdoorAmbient );
}

engine::color3 engine::lighting::get_fog_color( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::FogColor );
}

float engine::lighting::get_fog_start( ) {
	return memory->read<float>( this->address + offsets::Lighting::FogStart );
}

float engine::lighting::get_fog_end( ) {
	return memory->read<float>( this->address + offsets::Lighting::FogEnd );
}

float engine::lighting::get_geographic_latitude( ) {
	return memory->read<float>( this->address + offsets::Lighting::GeographicLatitude );
}

bool engine::lighting::get_global_shadows( ) {
	return memory->read<bool>( this->address + offsets::Lighting::GlobalShadows );
}

float engine::lighting::get_environment_diffuse_scale( ) {
	return memory->read<float>( this->address + offsets::Lighting::EnvironmentDiffuseScale );
}

float engine::lighting::get_environment_specular_scale( ) {
	return memory->read<float>( this->address + offsets::Lighting::EnvironmentSpecularScale );
}

float engine::lighting::get_exposure_compensation( ) {
	return memory->read<float>( this->address + offsets::Lighting::ExposureCompensation );
}

engine::color3 engine::lighting::get_gradient_top( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::GradientTop );
}
// < > //
engine::color3 engine::lighting::get_gradient_bottom( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::GradientBottom );
}

engine::color3 engine::lighting::get_light_color( ) {
	return O_o::read_color3_prop( this->address + offsets::Lighting::LightColor );
}

engine::vector3 engine::lighting::get_light_direction( ) {
	return memory->read<engine::vector3>( this->address + offsets::Lighting::LightDirection );
}

engine::vector3 engine::lighting::get_moon_position( ) {
	return memory->read<engine::vector3>( this->address + offsets::Lighting::MoonPosition );
}

engine::vector3 engine::lighting::get_sun_position( ) {
	return memory->read<engine::vector3>( this->address + offsets::Lighting::SunPosition );
}

std::string engine::lighting::get_source( ) {
	return O_o::read_string_prop( this->address + offsets::Lighting::Source );
}

engine::instance engine::lighting::get_sky( ) {
	return memory->read<engine::instance>( this->address + offsets::Lighting::Sky );
}

void engine::lighting::set_brightness( float value ) {
	memory->write<float>( this->address + offsets::Lighting::Brightness, value );
}

void engine::lighting::set_clock_time( float value ) {
	memory->write<float>( this->address + offsets::Lighting::ClockTime, value );
}

void engine::lighting::set_fog_end( float value ) {
	memory->write<float>( this->address + offsets::Lighting::FogEnd, value );
}

void engine::lighting::set_fog_start( float value ) {
	memory->write<float>( this->address + offsets::Lighting::FogStart, value );
}

void engine::lighting::set_global_shadows( bool value ) {
	memory->write<bool>( this->address + offsets::Lighting::GlobalShadows, value );
}

std::string engine::sky::get_skybox_bk( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxBk );
}

std::string engine::sky::get_skybox_dn( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxDn );
}

std::string engine::sky::get_skybox_ft( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxFt );
}

std::string engine::sky::get_skybox_lf( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxLf );
}

std::string engine::sky::get_skybox_rt( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxRt );
}

std::string engine::sky::get_skybox_up( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SkyboxUp );
}

std::string engine::sky::get_sun_texture_id( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::SunTextureId );
}

std::string engine::sky::get_moon_texture_id( ) {
	return O_o::read_string_prop( this->address + offsets::Sky::MoonTextureId );
}

float engine::sky::get_skybox_orientation( ) {
	return memory->read<float>( this->address + offsets::Sky::SkyboxOrientation );
}

float engine::sky::get_sun_angular_size( ) {
	return memory->read<float>( this->address + offsets::Sky::SunAngularSize );
}

float engine::sky::get_moon_angular_size( ) {
	return memory->read<float>( this->address + offsets::Sky::MoonAngularSize );
}

int engine::sky::get_star_count( ) {
	return memory->read<int>( this->address + offsets::Sky::StarCount );
}

engine::color3 engine::atmosphere::get_color( ) {
	return O_o::read_color3_prop( this->address + offsets::Atmosphere::Color );
}

float engine::atmosphere::get_decay( ) {
	return memory->read<float>( this->address + offsets::Atmosphere::Decay );
}

float engine::atmosphere::get_density( ) {
	return memory->read<float>( this->address + offsets::Atmosphere::Density );
}

float engine::atmosphere::get_glare( ) {
	return memory->read<float>( this->address + offsets::Atmosphere::Glare );
}

float engine::atmosphere::get_haze( ) {
	return memory->read<float>( this->address + offsets::Atmosphere::Haze );
}

float engine::atmosphere::get_offset( ) {
	return memory->read<float>( this->address + offsets::Atmosphere::Offset );
}

std::string engine::sound::get_sound_id( ) {
	return O_o::read_string_prop( this->address + offsets::Sound::SoundId );
}

float engine::sound::get_volume( ) {
	return memory->read<float>( this->address + offsets::Sound::Volume );
}

float engine::sound::get_playback_speed( ) {
	return memory->read<float>( this->address + offsets::Sound::PlaybackSpeed );
}

bool engine::sound::is_playing( ) {
	return memory->read<bool>( this->address + offsets::Sound::IsPlaying );
}

bool engine::sound::is_looped( ) {
	return memory->read<bool>( this->address + offsets::Sound::Looped );
}

float engine::sound::get_roll_off_max_distance( ) {
	return memory->read<float>( this->address + offsets::Sound::RollOffMaxDistance );
}

float engine::sound::get_roll_off_min_distance( ) {
	return memory->read<float>( this->address + offsets::Sound::RollOffMinDistance );
}

engine::instance engine::sound::get_sound_group( ) {
	return memory->read<engine::instance>( this->address + offsets::Sound::SoundGroup );
}

void engine::sound::set_volume( float value ) {
	memory->write<float>( this->address + offsets::Sound::Volume, value );
}

void engine::sound::set_playback_speed( float value ) {
	memory->write<float>( this->address + offsets::Sound::PlaybackSpeed, value );
}

engine::vector3 engine::attachment::get_position( ) {
	return memory->read<engine::vector3>( this->address + offsets::Attachment::Position );
}

engine::instance engine::beam::get_attachment0( ) {
	return memory->read<engine::instance>( this->address + offsets::Beam::Attachment0 );
}

engine::instance engine::beam::get_attachment1( ) {
	return memory->read<engine::instance>( this->address + offsets::Beam::Attachment1 );
}

float engine::beam::get_brightness( ) {
	return memory->read<float>( this->address + offsets::Beam::Brightness );
}

float engine::beam::get_curve_size0( ) {
	return memory->read<float>( this->address + offsets::Beam::CurveSize0 );
}

float engine::beam::get_curve_size1( ) {
	return memory->read<float>( this->address + offsets::Beam::CurveSize1 );
}

float engine::beam::get_light_emission( ) {
	return memory->read<float>( this->address + offsets::Beam::LightEmission );
}

float engine::beam::get_light_influence( ) {
	return memory->read<float>( this->address + offsets::Beam::LightInfluence );
}

std::string engine::beam::get_texture( ) {
	return O_o::read_string_prop( this->address + offsets::Beam::Texture );
}

float engine::beam::get_texture_length( ) {
	return memory->read<float>( this->address + offsets::Beam::TextureLength );
}

float engine::beam::get_texture_speed( ) {
	return memory->read<float>( this->address + offsets::Beam::TextureSpeed );
}

float engine::beam::get_width0( ) {
	return memory->read<float>( this->address + offsets::Beam::Width0 );
}

float engine::beam::get_width1( ) {
	return memory->read<float>( this->address + offsets::Beam::Width1 );
}

float engine::beam::get_z_offset( ) {
	return memory->read<float>( this->address + offsets::Beam::ZOffset );
}

engine::vector3 engine::particle_emitter::get_acceleration( ) {
	return memory->read<engine::vector3>( this->address + offsets::ParticleEmitter::Acceleration );
}

float engine::particle_emitter::get_brightness( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::Brightness );
}

float engine::particle_emitter::get_drag( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::Drag );
}

float engine::particle_emitter::get_lifetime( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::Lifetime );
}

float engine::particle_emitter::get_light_emission( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::LightEmission );
}

float engine::particle_emitter::get_light_influence( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::LightInfluence );
}

float engine::particle_emitter::get_rate( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::Rate );
}

engine::vector3 engine::particle_emitter::get_rot_speed( ) {
	return memory->read<engine::vector3>( this->address + offsets::ParticleEmitter::RotSpeed );
}

engine::vector3 engine::particle_emitter::get_rotation( ) {
	return memory->read<engine::vector3>( this->address + offsets::ParticleEmitter::Rotation );
}

float engine::particle_emitter::get_speed( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::Speed );
}

engine::vector2 engine::particle_emitter::get_spread_angle( ) {
	return memory->read<engine::vector2>( this->address + offsets::ParticleEmitter::SpreadAngle );
}

std::string engine::particle_emitter::get_texture( ) {
	return O_o::read_string_prop( this->address + offsets::ParticleEmitter::Texture );
}

float engine::particle_emitter::get_time_scale( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::TimeScale );
}

float engine::particle_emitter::get_velocity_inheritance( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::VelocityInheritance );
}

float engine::particle_emitter::get_z_offset( ) {
	return memory->read<float>( this->address + offsets::ParticleEmitter::ZOffset );
}

int engine::surface_appearance::get_alpha_mode( ) {
	return memory->read<int>( this->address + offsets::SurfaceAppearance::AlphaMode );
}

engine::color3 engine::surface_appearance::get_color( ) {
	return O_o::read_color3_prop( this->address + offsets::SurfaceAppearance::Color );
}

std::string engine::surface_appearance::get_color_map( ) {
	return O_o::read_string_prop( this->address + offsets::SurfaceAppearance::ColorMap );
}

std::string engine::surface_appearance::get_emissive_mask_content( ) {
	return O_o::read_string_prop( this->address + offsets::SurfaceAppearance::EmissiveMaskContent );
}

float engine::surface_appearance::get_emissive_strength( ) {
	return memory->read<float>( this->address + offsets::SurfaceAppearance::EmissiveStrength );
}

engine::color3 engine::surface_appearance::get_emissive_tint( ) {
	return O_o::read_color3_prop( this->address + offsets::SurfaceAppearance::EmissiveTint );
}

std::string engine::surface_appearance::get_metalness_map( ) {
	return O_o::read_string_prop( this->address + offsets::SurfaceAppearance::MetalnessMap );
}

std::string engine::surface_appearance::get_normal_map( ) {
	return O_o::read_string_prop( this->address + offsets::SurfaceAppearance::NormalMap );
}

std::string engine::surface_appearance::get_roughness_map( ) {
	return O_o::read_string_prop( this->address + offsets::SurfaceAppearance::RoughnessMap );
}

std::string engine::proximity_prompt::get_action_text( ) {
	return O_o::read_string_prop( this->address + offsets::ProximityPrompt::ActionText );
}

bool engine::proximity_prompt::is_enabled( ) {
	return memory->read<bool>( this->address + offsets::ProximityPrompt::Enabled );
}

int engine::proximity_prompt::get_gamepad_key_code( ) {
	return memory->read<int>( this->address + offsets::ProximityPrompt::GamepadKeyCode );
}

float engine::proximity_prompt::get_hold_duration( ) {
	return memory->read<float>( this->address + offsets::ProximityPrompt::HoldDuration );
}

int engine::proximity_prompt::get_key_code( ) {
	return memory->read<int>( this->address + offsets::ProximityPrompt::KeyCode );
}

float engine::proximity_prompt::get_max_activation_distance( ) {
	return memory->read<float>( this->address + offsets::ProximityPrompt::MaxActivationDistance );
}

std::string engine::proximity_prompt::get_object_text( ) {
	return O_o::read_string_prop( this->address + offsets::ProximityPrompt::ObjectText );
}

bool engine::proximity_prompt::get_requires_line_of_sight( ) {
	return memory->read<bool>( this->address + offsets::ProximityPrompt::RequiresLineOfSight );
}

float engine::click_detector::get_max_activation_distance( ) {
	return memory->read<float>( this->address + offsets::ClickDetector::MaxActivationDistance );
}

std::string engine::click_detector::get_mouse_icon( ) {
	return O_o::read_string_prop( this->address + offsets::ClickDetector::MouseIcon );
}

std::string engine::drag_detector::get_activated_cursor_icon( ) {
	return O_o::read_string_prop( this->address + offsets::DragDetector::ActivatedCursorIcon );
}

std::string engine::drag_detector::get_cursor_icon( ) {
	return O_o::read_string_prop( this->address + offsets::DragDetector::CursorIcon );
}

float engine::drag_detector::get_max_activation_distance( ) {
	return memory->read<float>( this->address + offsets::DragDetector::MaxActivationDistance );
}

float engine::drag_detector::get_max_drag_angle( ) {
	return memory->read<float>( this->address + offsets::DragDetector::MaxDragAngle );
}

engine::vector3 engine::drag_detector::get_max_drag_translation( ) {
	return memory->read<engine::vector3>( this->address + offsets::DragDetector::MaxDragTranslation );
}

float engine::drag_detector::get_max_force( ) {
	return memory->read<float>( this->address + offsets::DragDetector::MaxForce );
}
// < > //
float engine::drag_detector::get_max_torque( ) {
	return memory->read<float>( this->address + offsets::DragDetector::MaxTorque );
}

float engine::drag_detector::get_min_drag_angle( ) {
	return memory->read<float>( this->address + offsets::DragDetector::MinDragAngle );
}

engine::vector3 engine::drag_detector::get_min_drag_translation( ) {
	return memory->read<engine::vector3>( this->address + offsets::DragDetector::MinDragTranslation );
}

engine::instance engine::drag_detector::get_reference_instance( ) {
	return memory->read<engine::instance>( this->address + offsets::DragDetector::ReferenceInstance );
}

float engine::drag_detector::get_responsiveness( ) {
	return memory->read<float>( this->address + offsets::DragDetector::Responsiveness );
}

std::uint64_t engine::script::get_bytecode( ) {
	auto bytecode = memory->read<std::uint64_t>( this->address + offsets::Script::ByteCode );
	if ( !bytecode )
		return 0;

	return memory->read<std::uint64_t>( bytecode + offsets::ByteCode::Pointer );
}

std::uint64_t engine::script::get_bytecode_size( ) {
	auto bytecode = memory->read<std::uint64_t>( this->address + offsets::Script::ByteCode );
	if ( !bytecode )
		return 0;

	return memory->read<std::uint64_t>( bytecode + offsets::ByteCode::Size );
}

std::string engine::script::get_guid( ) {
	return O_o::read_string_prop( this->address + offsets::Script::GUID );
}

std::string engine::script::get_hash( ) {
	return O_o::read_string_prop( this->address + offsets::Script::Hash );
}

std::string engine::module_script::get_hash( ) {
	return O_o::read_string_prop( this->address + offsets::ModuleScript::Hash );
}

engine::vector2 engine::mouse_service::get_mouse_position( ) {
	return memory->read<engine::vector2>( this->address + offsets::MouseService::MousePosition );
}

engine::instance engine::mouse_service::get_input_object( ) {
	return memory->read<engine::instance>( this->address + offsets::MouseService::InputObject );
}

engine::instance engine::mouse_service::get_input_object2( ) {
	return memory->read<engine::instance>( this->address + offsets::MouseService::InputObject2 );
}

std::string engine::player_mouse::get_icon( ) {
	return O_o::read_string_prop( this->address + offsets::PlayerMouse::Icon );
}

engine::instance engine::player_mouse::get_workspace( ) {
	return memory->read<engine::instance>( this->address + offsets::PlayerMouse::Workspace );
}

engine::vector2 engine::gui_base2d::get_absolute_position( ) {
	return memory->read<engine::vector2>( this->address + offsets::GuiBase2D::AbsolutePosition );
}

float engine::gui_base2d::get_absolute_rotation( ) {
	return memory->read<float>( this->address + offsets::GuiBase2D::AbsoluteRotation );
}

engine::vector2 engine::gui_base2d::get_absolute_size( ) {
	return memory->read<engine::vector2>( this->address + offsets::GuiBase2D::AbsoluteSize );
}

engine::color3 engine::gui_object::get_background_color3( ) {
	return O_o::read_color3_prop( this->address + offsets::GuiObject::BackgroundColor3 );
}

float engine::gui_object::get_background_transparency( ) {
	return memory->read<float>( this->address + offsets::GuiObject::BackgroundTransparency );
}

std::string engine::gui_object::get_image( ) {
	return O_o::read_string_prop( this->address + offsets::GuiObject::Image );
}

int engine::gui_object::get_layout_order( ) {
	return memory->read<int>( this->address + offsets::GuiObject::LayoutOrder );
}
// < > //
engine::udim2 engine::gui_object::get_position( ) {
	return memory->read<engine::udim2>( this->address + offsets::GuiObject::Position );
}

bool engine::gui_object::get_rich_text( ) {
	return memory->read<bool>( this->address + offsets::GuiObject::RichText );
}

float engine::gui_object::get_rotation( ) {
	return memory->read<float>( this->address + offsets::GuiObject::Rotation );
}

bool engine::gui_object::is_screen_gui_enabled( ) {
	return memory->read<bool>( this->address + offsets::GuiObject::ScreenGui_Enabled );
}

engine::udim2 engine::gui_object::get_size( ) {
	return memory->read<engine::udim2>( this->address + offsets::GuiObject::Size );
}

std::string engine::gui_object::get_text( ) {
	return O_o::read_string_prop( this->address + offsets::GuiObject::Text );
}

engine::color3 engine::gui_object::get_text_color3( ) {
	return O_o::read_color3_prop( this->address + offsets::GuiObject::TextColor3 );
}

bool engine::gui_object::is_visible( ) {
	return memory->read<bool>( this->address + offsets::GuiObject::Visible );
}

int engine::gui_object::get_z_index( ) {
	return memory->read<int>( this->address + offsets::GuiObject::ZIndex );
}

void engine::gui_object::set_background_transparency( float value ) {
	memory->write<float>( this->address + offsets::GuiObject::BackgroundTransparency, value );
}

void engine::gui_object::set_visible( bool value ) {
	memory->write<bool>( this->address + offsets::GuiObject::Visible, value );
}

engine::instance engine::user_input_service::get_window_input_state( ) {
	return memory->read<engine::instance>( this->address + offsets::UserInputService::WindowInputState );
}

float engine::stats_item::get_value( ) {
	return memory->read<float>( this->address + offsets::StatsItem::Value );
}
// < > //
std::string engine::textures::get_texture( ) {
	return O_o::read_string_prop( this->address + offsets::Textures::Decal_Texture );
}
// < > //
std::uint32_t engine::fast_cluster_entity::get_render_queue_id( ) {
	return memory->read<std::uint32_t>( this->address + offsets::FastClusterEntity::RenderQueueId );
}

void engine::fast_cluster_entity::set_render_queue_id( std::uint32_t value ) {
	memory->write<std::uint32_t>( this->address + offsets::FastClusterEntity::RenderQueueId, value );
}

std::uint8_t engine::fast_cluster_entity::get_alpha( ) {
	return memory->read<std::uint8_t>( this->address + offsets::FastClusterEntity::AlphaByte );
}

void engine::fast_cluster_entity::set_alpha( std::uint8_t value ) {
	memory->write<std::uint8_t>( this->address + offsets::FastClusterEntity::AlphaByte, value );
}

std::uintptr_t engine::fast_cluster_entity::get_technique_array( ) {
	return memory->read<std::uintptr_t>( this->address + offsets::FastClusterEntity::TechniqueArrayPtr );
}
// < >//
std::uintptr_t engine::technique_array::get_begin( ) {
	return memory->read<std::uintptr_t>( this->address + offsets::TechniqueArray::BeginOffset );
}

std::uintptr_t engine::technique_array::get_end( ) {
	return memory->read<std::uintptr_t>( this->address + offsets::TechniqueArray::EndOffset );
}

std::size_t engine::technique_array::get_size( ) {
	auto begin = this->get_begin( );
	auto end = this->get_end( );

	if ( !begin || end <= begin )
		return 0;

	return end - begin;
}

std::size_t engine::technique_array::get_count( ) {
	const auto size = this->get_size( );

	if ( !size )
		return 0;

	return size / offsets::MaterialLayer::Stride;
}
// < > //
std::uint8_t engine::material_layer::get_fill_mode( ) {
	return memory->read<std::uint8_t>( this->address + offsets::MaterialLayer::FillModeByte );
}

void engine::material_layer::set_fill_mode( std::uint8_t value ) {
	memory->write<std::uint8_t>( this->address + offsets::MaterialLayer::FillModeByte, value );
}

std::uint32_t engine::material_layer::get_material_flags( ) {
	return memory->read<std::uint32_t>( this->address + offsets::MaterialLayer::MatFlags );
}

void engine::material_layer::set_material_flags( std::uint32_t value ) {
	memory->write<std::uint32_t>( this->address + offsets::MaterialLayer::MatFlags, value );
}

std::uint32_t engine::material_layer::get_param( ) {
	return memory->read<std::uint32_t>( this->address + offsets::MaterialLayer::Param );
}

void engine::material_layer::set_param( std::uint32_t value ) {
	memory->write<std::uint32_t>( this->address + offsets::MaterialLayer::Param, value );
}

std::uint32_t engine::material_layer::get_flags_2( ) {
	return memory->read<std::uint32_t>( this->address + offsets::MaterialLayer::Flags2 );
}

void engine::material_layer::set_flags_2( std::uint32_t value ) {
	memory->write<std::uint32_t>( this->address + offsets::MaterialLayer::Flags2, value );
}

std::uint32_t engine::material_layer::get_color_data( ) {
	return memory->read<std::uint32_t>( this->address + offsets::MaterialLayer::ColorData );
}

void engine::material_layer::set_color_data( std::uint32_t value ) {
	memory->write<std::uint32_t>( this->address + offsets::MaterialLayer::ColorData, value );
}

std::uintptr_t engine::material_layer::get_owner( ) {
	return memory->read<std::uintptr_t>( this->address + 0x30 );
}