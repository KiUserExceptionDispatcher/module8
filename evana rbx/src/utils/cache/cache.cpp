#include "cache.hpp"

#include <chrono>
#include <cmath>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <thread>

enum class aim_bone : int {
	head = 0,
	body,
	left_leg,
	right_leg,
	left_arm,
	right_arm,
	closest_part
};

const o::cache::part* o::cache::entity::get_bone( ) const
{
	for ( const o::cache::part& part_item : parts )
		if ( part_item.name == "Head" ) return &part_item;
	for ( const o::cache::part& part_item : parts )
		if ( part_item.name == "HumanoidRootPart" || part_item.name == "Torso" || part_item.name == "UpperTorso" )
			return &part_item;
	return parts.empty( ) ? nullptr : &parts.front( );
}

const o::cache::part* o::cache::entity::get_part( const std::string& part_name ) const
{
	for ( const o::cache::part& part_item : parts )
		if ( part_item.name == part_name ) 
			return &part_item;

	return nullptr;
}

const o::cache::part* o::cache::entity::get_hitbox( int bone ) const
{
	std::function<const o::cache::part* ( std::initializer_list<const char*> )> first_of = [this]( std::initializer_list<const char*> names ) -> const o::cache::part*
		{
			for ( const char* n : names )
			{
				if ( const o::cache::part* part_item = get_part( n ) )
					return part_item;
			}
			return nullptr;
		};

	switch ( static_cast<aim_bone>( bone ) )
	{
	case aim_bone::body:
		if ( const o::cache::part* part_item = first_of( {"UpperTorso", "Torso", "HumanoidRootPart"} ) )
			return part_item;
		break;
	case aim_bone::left_leg:
		if ( const o::cache::part* part_item = first_of( {"LeftUpperLeg", "Left Leg", "LeftLowerLeg", "LeftFoot"} ) )
			return part_item;
		break;
	case aim_bone::right_leg:
		if ( const o::cache::part* part_item = first_of( {"RightUpperLeg", "Right Leg", "RightLowerLeg", "RightFoot"} ) )
			return part_item;
		break;
	case aim_bone::left_arm:
		if ( const o::cache::part* part_item = first_of( {"LeftUpperArm", "Left Arm", "LeftLowerArm", "LeftHand"} ) )
			return part_item;
		break;
	case aim_bone::right_arm:
		if ( const o::cache::part* part_item = first_of( {"RightUpperArm", "Right Arm", "RightLowerArm", "RightHand"} ) )
			return part_item;
		break;
	case aim_bone::closest_part:
		break;
	case aim_bone::head:
	default:
		if ( const o::cache::part* part_item = first_of( {"Head"} ) )
			return part_item;
		break;
	}

	if ( const o::cache::part* part_item = get_bone( ) )
		return part_item;
	if ( const o::cache::part* part_item = get_part( "HumanoidRootPart" ) )
		return part_item;
	if ( const o::cache::part* part_item = get_part( "UpperTorso" ) )
		return part_item;
	if ( const o::cache::part* part_item = get_part( "Torso" ) )
		return part_item;
	return nullptr;
}

o::cache::manager::~manager( )
{
	this->stop( );
}

o::cache::manager& o::cache::manager::get( )
{
	static o::cache::manager instance;
	return instance;
}

bool o::cache::manager::start( )
{
	if ( this->active.load( ) )
		return false;

	this->active.store( true );
	this->statics = std::thread( &o::cache::manager::static_loop, this );
	this->transforms = std::thread( &o::cache::manager::transform, this );

	return true;
}

bool o::cache::manager::stop( )
{
	if ( !this->active.load( ) )
		return false;

	this->active.store( false );

	if ( this->statics.joinable( ) )
		this->statics.join( );

	if ( this->transforms.joinable( ) )
		this->transforms.join( );

	return true;
}

bool o::cache::manager::is_ready( ) const
{
	std::shared_lock<std::shared_mutex> lock( this->mutex_ );
	return !this->players_.empty( );
}

void o::cache::manager::reset( )
{
	this->epoch.fetch_add( 1, std::memory_order_release );

	std::unique_lock<std::shared_mutex> lock( this->mutex_ );
	this->players_.clear( );
	this->render_entities_.clear( );
	this->data_model = 0;
	this->workspace = 0;
	this->camera = 0;
	this->players_service = 0;
	this->mouse_service = 0;
	this->fake_dm = 0;
	this->local_player = 0;
	this->local_prim = 0;
	this->local_team = 0;
}

std::vector<o::cache::entity> o::cache::manager::get_players( ) const
{
	std::shared_lock<std::shared_mutex> lock( this->mutex_ );
	return this->players_;
}

std::vector<o::cache::entity> o::cache::manager::snapshot_players( )
{
	std::shared_lock<std::shared_mutex> lock( this->mutex_ );
	return this->players_;
}

std::vector<o::cache::render_entity> o::cache::manager::get_render_entities( ) const
{
	std::shared_lock<std::shared_mutex> lock( this->mutex_ );
	return this->render_entities_;
}

std::size_t o::cache::manager::add_render_entities( const std::vector<o::cache::render_entity>& entities )
{
	std::unique_lock<std::shared_mutex> lock( this->mutex_ );
	this->render_entities_.insert( this->render_entities_.end( ), entities.begin( ), entities.end( ) );
	return entities.size( );
}

void o::cache::manager::static_loop( )
{
	std::uintptr_t svc_cache_dm = 0;
	std::uintptr_t svc_cache_players = 0;
	std::uintptr_t svc_cache_mouse = 0;

	while ( this->active.load( ) )
	{
		std::uint64_t s_epoch = this->epoch.load( std::memory_order_acquire );

		std::uint64_t s_base = memory->base;
		std::uintptr_t s_data_model = this->data_model;
		std::uintptr_t s_fake_dm = s_base ? memory->read<std::uintptr_t>( s_base + offsets::FakeDataModel::Pointer ) : 0;

		if ( s_fake_dm )
		{
			std::uintptr_t real_dm = memory->read<std::uintptr_t>( s_fake_dm + offsets::FakeDataModel::RealDataModel );
			if ( real_dm )
				s_data_model = real_dm;
		}

		engine::instance dm( s_data_model );

		std::uintptr_t s_players_service = 0;
		std::uintptr_t s_mouse_service = 0;

		if ( s_data_model && s_data_model == svc_cache_dm && svc_cache_players && svc_cache_mouse )
		{
			s_players_service = svc_cache_players;
			s_mouse_service = svc_cache_mouse;
		}
		else
		{
			engine::instance players_svc_inst{};
			engine::instance players_by_name = dm.find_child( "Players" );

			if ( players_by_name.is_valid( ) )
				players_svc_inst = engine::instance( players_by_name.address );

			if ( !players_svc_inst.address )
			{
				engine::instance of_class = dm.find_class( "Players" );

				if ( of_class.is_valid( ) )
					players_svc_inst = engine::instance( of_class.address );
			}
			s_players_service = players_svc_inst.address;

			engine::instance mouse_svc_inst{};
			engine::instance mouse_by_name = dm.find_child( "MouseService" );
			if ( mouse_by_name.is_valid( ) )
				mouse_svc_inst = engine::instance( mouse_by_name.address );
			if ( !mouse_svc_inst.address )
			{
				engine::instance of_class = dm.find_class( "MouseService" );
				if ( of_class.is_valid( ) )
					mouse_svc_inst = engine::instance( of_class.address );
			}
			s_mouse_service = mouse_svc_inst.address;

			if ( s_players_service && s_mouse_service )
			{
				svc_cache_dm = s_data_model;
				svc_cache_players = s_players_service;
				svc_cache_mouse = s_mouse_service;
			}
		}

		engine::instance players_svc( s_players_service );

		std::uintptr_t s_workspace = s_data_model ? memory->read<std::uintptr_t>( s_data_model + offsets::DataModel::Workspace ) : 0;
		std::uintptr_t s_camera = s_workspace ? memory->read<std::uintptr_t>( s_workspace + offsets::Workspace::CurrentCamera ) : 0;

		std::uintptr_t s_local_player = s_players_service ? memory->read<std::uintptr_t>( s_players_service + offsets::Player::LocalPlayer ) : 0;
		std::uintptr_t s_local_prim = 0;

		if ( s_local_player )
		{
			std::uintptr_t lp_model = memory->read<std::uintptr_t>( s_local_player + offsets::Player::ModelInstance );
			if ( lp_model )
			{
				engine::instance lm( lp_model );
				engine::instance hrp = lm.find_child( "HumanoidRootPart" );
				if ( hrp.is_valid( ) )
					s_local_prim = memory->read<std::uintptr_t>( hrp.address + offsets::BasePart::Primitive );
			}
		}

		std::vector<entity> built;

		if ( s_players_service )
		{
			for ( engine::instance player_inst : players_svc.get_children( ) )
			{
				if ( !this->active.load( ) ) break;
				if ( this->epoch.load( std::memory_order_acquire ) != s_epoch ) break;

				engine::player player_obj( player_inst.address );
				engine::instance model_ptr = player_obj.get_character( );
				if ( !model_ptr.is_valid( ) ) continue;

				engine::instance model( model_ptr.address );
				engine::instance humanoid_inst = model.find_class( "Humanoid" );
				engine::humanoid humanoid = humanoid_inst.is_valid( ) ? engine::humanoid( humanoid_inst.address ) : engine::humanoid{};

				entity pe{};
				pe.address = player_inst.address;
				pe.player = engine::player( player_inst.address );
				pe.humanoid = humanoid;
				pe.team = engine::instance( memory->read<std::uintptr_t>( player_inst.address + offsets::Player::Team ) );
				pe.model = engine::model( model.address );
				pe.name = player_inst.get_name( );
				pe.display_name = pe.player.get_display_name( );
				pe.local_player = ( player_inst.address == s_local_player );
				pe.teammate = ( pe.team.address != 0 && s_local_player && pe.team.address == memory->read<std::uintptr_t>( s_local_player + offsets::Player::Team ) );

				for ( engine::instance part_inst : model.get_children( ) )
				{
					std::string class_name = part_inst.get_class_name( );
					std::vector<engine::instance> part_sources;
					bool is_acc = ( class_name == "Accessory" || class_name == "Hat" || class_name == "Accoutrement" );

					if ( class_name == "Part" || class_name == "MeshPart" )
					{
						part_sources.push_back( part_inst );
					}
					else if ( is_acc )
					{
						engine::instance handle = part_inst.find_child( "Handle" );
						if ( handle.is_valid( ) )
							part_sources.push_back( engine::instance( handle.address ) );
						else
						{
							for ( engine::instance child : part_inst.get_children( ) )
							{
								const std::string child_class = child.get_class_name( );
								if ( child_class == "Part" || child_class == "MeshPart" )
									part_sources.push_back( child );
							}
						}
					}

					for ( engine::instance src : part_sources )
					{
						std::uintptr_t primitive = memory->read<std::uintptr_t>( src.address + offsets::BasePart::Primitive );
						if ( !primitive ) continue;

						part p{};
						p.address = src.address;
						p.primitive = primitive;
						p.name = src.get_name( );
						p.is_accessory = is_acc || ( p.name == "Handle" ) || ( p.name.find( "Accessory" ) != std::string::npos ) || ( p.name.find( "Hat" ) != std::string::npos ) || ( p.name.find( "Hair" ) != std::string::npos );

						const std::string src_class = src.get_class_name( );
						if ( src_class == "MeshPart" )
						{
							p.mesh_id = engine::mesh_part( src.address ).get_mesh_id( );
						}
						else if ( p.name == "Head" || p.name == "Handle" )
						{
							engine::instance special = src.find_class( "SpecialMesh" );
							if ( special.is_valid( ) )
							{
								p.mesh_id = memory->read_string( special.address + offsets::SpecialMesh::MeshId );
								p.mesh_scale = memory->read<engine::vector3>( special.address + offsets::SpecialMesh::Scale );
								p.meshes.emplace_back( engine::special_mesh( special.address ) );
							}
						}

						std::uint8_t xform[60]{};
						if ( memory->read_buf( primitive + offsets::Primitive::Rotation, xform, sizeof( xform ) ) )
						{
							std::memcpy( &p.rotation, xform + 0, sizeof( engine::matrix3x3 ) );
							std::memcpy( &p.position, xform + 36, sizeof( engine::vector3 ) );
							std::memcpy( &p.velocity, xform + 48, sizeof( engine::vector3 ) );
						}
						p.size = memory->read<engine::vector3>( primitive + offsets::Primitive::Size );

						pe.parts.push_back( p );
					}
				}

				if ( humanoid.is_valid( ) )
				{
					pe.health = humanoid.get_health( );
					pe.max_health = humanoid.get_max_health( );
				}

				{
					engine::instance tool_inst = model.find_class( "Tool" );
					pe.tool = tool_inst.is_valid( ) ? tool_inst.get_name( ) : "";
				}

				pe.last_model = model.address;
				{
					std::uintptr_t cptr = memory->read<std::uintptr_t>( model.address + offsets::Instance::ChildrenStart );
					if ( cptr )
					{
						pe.last_children_start = memory->read<std::uintptr_t>( cptr );
						pe.last_children_end = memory->read<std::uintptr_t>( cptr + offsets::Instance::ChildrenEnd );
					}
				}

				if ( !pe.parts.empty( ) )
					built.push_back( std::move( pe ) );
			}
		}

		std::vector<render_entity> built_render;
		for ( const auto& pe : built ) {
			for ( const auto& part_item : pe.parts ) {
				render_entity re{};
				re.address = part_item.address;
				re.primitive = part_item.primitive;
				re.part = engine::base_part( part_item.address );
				re.position = part_item.position;
				re.size = part_item.size;
				re.rotation = part_item.rotation;
				re.meshes = part_item.meshes;
				built_render.push_back( std::move( re ) );
			}
		}

		{
			std::unique_lock<std::shared_mutex> lock( this->mutex_ );
			if ( this->epoch.load( std::memory_order_acquire ) == s_epoch )
			{
				for ( entity& pe : built )
				{
					for ( const entity& old : this->players_ )
					{
						std::uintptr_t new_key = pe.address ? pe.address : pe.last_model;
						std::uintptr_t old_key = old.address ? old.address : old.last_model;
						if ( !new_key || new_key != old_key )
							continue;

						pe.priority = old.priority;
						pe.distance = old.distance;
						break;
					}
				}

				this->base = s_base;
				this->data_model = s_data_model;
				this->fake_dm = s_fake_dm;
				this->players_service = s_players_service;
				this->mouse_service = s_mouse_service;
				this->workspace = s_workspace;
				this->camera = s_camera;
				this->local_player = s_local_player;
				this->local_prim = s_local_prim;
				this->players_.swap( built );
				this->render_entities_.swap( built_render );
			}
		}

		if ( s_data_model && s_workspace && memory->is_valid( s_workspace ) )
		{
			const bool dm_changed = g->data_model.address != s_data_model;
			const bool ws_changed = g->workspace.address != s_workspace;
			if ( dm_changed || ws_changed )
			{
				g->data_model = engine::data_model( s_data_model );
				g->workspace = engine::workspace( s_workspace );
				if ( s_players_service && memory->is_valid( s_players_service ) )
					g->players = engine::player( s_players_service );
			}
		}

		for ( int i = 0; i < 10 && this->active.load( ); i++ )
			std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

void o::cache::manager::transform( )
{
	std::uintptr_t lp_cached_model = 0;
	std::uintptr_t lp_cached_hrp = 0;
	std::uint64_t lp_cached_epoch = 0;

	while ( this->active.load( ) )
	{
		std::uint64_t cur_epoch = this->epoch.load( std::memory_order_acquire );
		if ( cur_epoch != lp_cached_epoch )
		{
			lp_cached_model = 0;
			lp_cached_hrp = 0;
			lp_cached_epoch = cur_epoch;
		}

		std::uintptr_t lp = this->local_player;
		std::uintptr_t new_local_prim = 0;

		if ( lp )
			this->local_team = memory->read<std::uintptr_t>( lp + offsets::Player::Team );
		else
			this->local_team = 0;

		if ( lp )
		{
			std::uintptr_t lp_model = memory->read<std::uintptr_t>( lp + offsets::Player::ModelInstance );
			if ( lp_model )
			{
				if ( lp_model == lp_cached_model && lp_cached_hrp )
				{
					new_local_prim = memory->read<std::uintptr_t>( lp_cached_hrp + offsets::BasePart::Primitive );
				}
				else
				{
					engine::instance lm( lp_model );
					engine::instance hrp = lm.find_child( "HumanoidRootPart" );
					if ( hrp.is_valid( ) )
					{
						new_local_prim = memory->read<std::uintptr_t>( hrp.address + offsets::BasePart::Primitive );
						lp_cached_model = lp_model;
						lp_cached_hrp = hrp.address;
					}
					else
					{
						lp_cached_model = 0;
						lp_cached_hrp = 0;
					}
				}
			}
			else
			{
				lp_cached_model = 0;
				lp_cached_hrp = 0;
			}
		}
		this->local_prim = new_local_prim;

		std::uintptr_t cam_addr = this->camera;
		engine::vector3 cam{};
		bool have_cam = ( cam_addr != 0 );
		if ( have_cam )
			cam = memory->read<engine::vector3>( cam_addr + offsets::Camera::Position );

		std::vector<entity> working;
		std::vector<render_entity> working_render;
		{
			std::shared_lock<std::shared_mutex> lock( this->mutex_ );
			working = this->players_;
			working_render = this->render_entities_;
		}

		for ( entity& player_item : working )
		{
			if ( player_item.humanoid.address )
			{
				player_item.health = memory->read<float>( player_item.humanoid.address + offsets::Humanoid::Health );
				player_item.max_health = memory->read<float>( player_item.humanoid.address + offsets::Humanoid::MaxHealth );
			}

			if ( player_item.address )
				player_item.team = engine::instance( memory->read<std::uintptr_t>( player_item.address + offsets::Player::Team ) );

			if ( player_item.address )
			{
				std::uintptr_t model_ptr = memory->read<std::uintptr_t>( player_item.address + offsets::Player::ModelInstance );
				if ( model_ptr )
				{
					std::uintptr_t cptr = memory->read<std::uintptr_t>( model_ptr + offsets::Instance::ChildrenStart );
					std::uintptr_t cstart = 0, cend = 0;
					if ( cptr )
					{
						cstart = memory->read<std::uintptr_t>( cptr );
						cend = memory->read<std::uintptr_t>( cptr + offsets::Instance::ChildrenEnd );
					}

					if ( model_ptr != player_item.last_model
						|| cstart != player_item.last_children_start
						|| cend != player_item.last_children_end )
					{
						engine::instance model( model_ptr );
						engine::instance tool_inst = model.find_class( "Tool" );
						player_item.tool = tool_inst.is_valid( ) ? tool_inst.get_name( ) : "";

						player_item.last_model = model_ptr;
						player_item.last_children_start = cstart;
						player_item.last_children_end = cend;
					}
				}
			}

			for ( part& part_item : player_item.parts )
			{
				if ( !part_item.primitive )
					part_item.primitive = memory->read<std::uintptr_t>( part_item.address + offsets::BasePart::Primitive );
				if ( !part_item.primitive ) continue;

				std::uint8_t buf[60]{};
				if ( !memory->read_buf( part_item.primitive + offsets::Primitive::Rotation, buf, sizeof( buf ) ) )
					continue;

				std::memcpy( &part_item.rotation, buf + 0, sizeof( engine::matrix3x3 ) );
				std::memcpy( &part_item.position, buf + 36, sizeof( engine::vector3 ) );
				std::memcpy( &part_item.velocity, buf + 48, sizeof( engine::vector3 ) );
			}

			if ( have_cam )
			{
				engine::vector3 root{};
				bool have_root = false;
				for ( const part& part_item : player_item.parts )
				{
					if ( part_item.name == "HumanoidRootPart" )
					{
						root = part_item.position;
						have_root = true;
						break;
					}
				}
				if ( !have_root && !player_item.parts.empty( ) )
					root = player_item.parts[0].position;

				float dx = root.x - cam.x;
				float dy = root.y - cam.y;
				float dz = root.z - cam.z;
				player_item.distance = std::sqrt( dx * dx + dy * dy + dz * dz );
			}
		}

		for ( render_entity& re : working_render )
		{
			if ( !re.primitive )
				re.primitive = memory->read<std::uintptr_t>( re.address + offsets::BasePart::Primitive );
			if ( !re.primitive ) continue;

			std::uint8_t buf[60]{};
			if ( !memory->read_buf( re.primitive + offsets::Primitive::Rotation, buf, sizeof( buf ) ) )
				continue;

			std::memcpy( &re.rotation, buf + 0, sizeof( engine::matrix3x3 ) );
			std::memcpy( &re.position, buf + 36, sizeof( engine::vector3 ) );
		}

		{
			std::unique_lock<std::shared_mutex> lock( this->mutex_ );
			if ( working.size( ) == this->players_.size( ) )
			{
				bool same = true;
				for ( std::size_t i = 0; i < this->players_.size( ); ++i )
				{
					if ( this->players_[i].address != working[i].address )
					{
						same = false;
						break;
					}
				}
				if ( same )
					this->players_.swap( working );
			}

			if ( working_render.size( ) == this->render_entities_.size( ) )
			{
				bool same = true;
				for ( std::size_t i = 0; i < this->render_entities_.size( ); ++i )
				{
					if ( this->render_entities_[i].address != working_render[i].address )
					{
						same = false;
						break;
					}
				}
				if ( same )
					this->render_entities_.swap( working_render );
			}
		}

		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}