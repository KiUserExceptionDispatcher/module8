#include "engine_chams.hpp"

#include <algorithm>
#include <cstring>

#include <Windows.h>

#include <src/graphics/framework/settings/variables.h>

std::uint32_t cheat::engine_chams_t::get_color_param( int index ) {
	index = std::clamp<int>( index, 0, 6 );
	return static_cast<std::uint32_t>( index + 1 );
}

std::uint32_t cheat::engine_chams_t::get_style_queue( int style ) {
	switch ( style ) {
	case 1: return offsets::RenderQueue::Glass;
	case 2: return offsets::RenderQueue::GlassTint;
	case 3: return offsets::RenderQueue::Transparent;
	case 4: return offsets::RenderQueue::OnTopWithDepth;
	case 5: return offsets::RenderQueue::Glass;
	case 6: return offsets::RenderQueue::AlwaysOnTop;
	case 7: return offsets::RenderQueue::Transparent;
	case 8: return offsets::RenderQueue::OnTopWithDepth;
	default: return offsets::RenderQueue::AlwaysOnTop;
	}
}

std::uintptr_t cheat::engine_chams_t::get_base_address( ) {
	static std::uintptr_t cached_base = 0;

	if ( cached_base )
		return cached_base;

	auto base_ptr = memory->read<std::uintptr_t>( memory->base + 0x30 );

	if ( base_ptr ) {
		cached_base = base_ptr;
		return cached_base;
	}

	return memory->base;
}

bool cheat::engine_chams_t::is_local_entity( std::uintptr_t address ) {
	if ( !address )
		return false;

	engine::fast_cluster_entity entity( address );

	auto technique_array_address = entity.get_technique_array( );

	if ( !technique_array_address )
		return false;

	engine::technique_array technique_array( technique_array_address );

	auto begin = technique_array.get_begin( );
	auto end = technique_array.get_end( );

	if ( !begin || end <= begin )
		return false;

	auto local_player = (engine::player)g->players.get_local_player( ).address;
	auto local_character = local_player.get_character( ).address;

	if ( !local_character )
		return false;

	for ( auto current = begin; current < end; current += offsets::MaterialLayer::Stride ) {
		engine::material_layer layer( current );

		if ( layer.get_owner( ) == local_character )
			return true;
	}

	return false;
}

void cheat::engine_chams_t::entity_locked( std::uintptr_t address ) {
	std::lock_guard lock( mutex );

	auto entity_backup = entity_backups.find( address );

	if ( entity_backup != entity_backups.end( ) ) {
		engine::fast_cluster_entity entity( address );

		entity.set_render_queue_id( entity_backup->second.render_queue_id );
		entity.set_alpha( entity_backup->second.alpha );
	}

	auto layers = entity_layers.find( address );

	if ( layers != entity_layers.end( ) ) {
		for ( auto layer_address : layers->second ) {
			auto backup = backups.find( layer_address );

			if ( backup == backups.end( ) )
				continue;

			engine::material_layer layer( layer_address );

			layer.set_fill_mode( backup->second.fill_mode );
			layer.set_material_flags( backup->second.material_flags );
			layer.set_param( backup->second.param );
			layer.set_flags_2( backup->second.flags_2 );
			layer.set_color_data( backup->second.color );
		}
	}

	known_entities.erase( address );
}

void cheat::engine_chams_t::material_set( std::uintptr_t address ) {
	if ( !address || !prop->enabled )
		return;

	if ( is_local_entity( address ) ) {
		entity_locked( address );
		return;
	}

	engine::fast_cluster_entity entity( address );
	{
		std::lock_guard lock( mutex );

		if ( !entity_backups.contains( address ) ) {
			entity_backups.emplace( address, entity_backup_t{
				entity.get_render_queue_id( ),
				entity.get_alpha( )
				} );
		}

		auto& backup = entity_backups.at( address );

		entity.set_render_queue_id( backup.render_queue_id );
		entity.set_alpha( backup.alpha );
	}

	const auto style = std::clamp( prop->style, static_cast<int>( type::xray ), static_cast<int>( type::invisible ) );
	const auto color = std::clamp( prop->color, 0, 6 );

	auto technique_array_address = entity.get_technique_array( );

	if ( !technique_array_address )
		return;

	engine::technique_array technique_array( technique_array_address );

	const auto begin = technique_array.get_begin( );
	const auto end = technique_array.get_end( );

	if ( !begin || end <= begin )
		return;

	const auto bytes = end - begin;

	if ( bytes > 64 * 1024 )
		return;

	const auto count = bytes / offsets::MaterialLayer::Stride;

	if ( !count || count > 256 )
		return;

	std::uint8_t fill_mode = 0;
	std::uint32_t material_flags = 0;
	std::uint32_t param = get_color_param( color );
	std::uint32_t flags_2 = 0;

	switch ( style ) {
	case type::xray:
		fill_mode = 0;
		material_flags = 0;
		param = get_color_param( color );
		flags_2 = 0;
		break;

	case type::wireframe:
		fill_mode = 1;
		material_flags = 0;
		param = get_color_param( color );
		flags_2 = 0;
		break;

	case type::glow:
		fill_mode = 1;
		material_flags = 0;
		param = get_color_param( color );
		flags_2 = 7;
		break;

	case type::flat:
		fill_mode = 0;
		material_flags = 0;
		param = get_color_param( color );
		flags_2 = 15;
		break;

	case type::glass:
		fill_mode = 0;
		material_flags = 1;
		param = get_color_param( color );
		flags_2 = 0;
		break;

	case type::metal:
		fill_mode = 0;
		material_flags = 2;
		param = get_color_param( color );
		flags_2 = 1;
		break;

	case type::ghost:
		fill_mode = 0;
		material_flags = 3;
		param = get_color_param( color );
		flags_2 = 16;
		break;

	case type::invisible:
		fill_mode = 0;
		material_flags = 0;
		param = 7;
		flags_2 = 0;
		break;

	default:
		return;
	}

	entity.set_render_queue_id( get_style_queue( style ) );

	if ( prop->alpha ) {
		entity.set_alpha( static_cast<std::uint8_t>( std::clamp( prop->alpha_value, 0, 255 ) ) );
	}

	std::vector<std::uintptr_t> layers;
	layers.reserve( count );

	for ( std::size_t i = 0; i < count; ++i ) {
		const auto layer_address = begin + i * offsets::MaterialLayer::Stride;
		engine::material_layer layer( layer_address );

		{
			std::lock_guard lock( mutex );

			if ( !backups.contains( layer_address ) ) {
				backups.emplace( layer_address, layer_backup_t{
					layer.get_fill_mode( ),
					layer.get_material_flags( ),
					layer.get_param( ),
					layer.get_flags_2( ),
					layer.get_color_data( )
					} );
			}

			const auto& backup = backups.at( layer_address );

			layer.set_fill_mode( backup.fill_mode );
			layer.set_material_flags( backup.material_flags );
			layer.set_param( backup.param );
			layer.set_flags_2( backup.flags_2 );
			layer.set_color_data( backup.color );
		}

		layer.set_fill_mode( fill_mode );
		layer.set_material_flags( material_flags );
		layer.set_param( param );
		layer.set_flags_2( flags_2 );
		layer.set_color_data( 0xFFFFFFFFu );

		layers.push_back( layer_address );
	}

	{
		std::lock_guard lock( mutex );
		entity_layers[address] = std::move( layers );
		known_entities.insert( address );
	}
}

void cheat::engine_chams_t::reload( ) {
	std::vector<std::uintptr_t> entities;

	{
		std::lock_guard lock( mutex );

		entities.reserve( known_entities.size( ) );

		for ( auto address : known_entities )
			entities.push_back( address );
	}

	for ( auto address : entities ) {
		if ( !running )
			break;

		material_set( address );
	}
}

void cheat::engine_chams_t::reset( ) {
	std::lock_guard lock( mutex );

	for ( auto& [address, backup] : backups ) {
		if ( !address )
			continue;

		engine::material_layer layer( address );

		layer.set_fill_mode( backup.fill_mode );
		layer.set_material_flags( backup.material_flags );
		layer.set_param( backup.param );
		layer.set_flags_2( backup.flags_2 );
		layer.set_color_data( backup.color );
	}

	for ( auto& [address, backup] : entity_backups ) {
		if ( !address )
			continue;

		engine::fast_cluster_entity entity( address );

		entity.set_render_queue_id( backup.render_queue_id );
		entity.set_alpha( backup.alpha );
	}

	backups.clear( );
	entity_backups.clear( );
	entity_layers.clear( );
	known_entities.clear( );
}

void cheat::engine_chams_t::loop( ) {
	bool last_enabled = false;

	while ( running ) {
		if ( !prop->enabled ) {
			if ( last_enabled ) {
				reset( );
				last_enabled = false;
			}

			Sleep( 100 );
			continue;
		}

		last_enabled = true;

		auto base_address = get_base_address( );

		if ( !base_address ) {
			Sleep( 500 );
			continue;
		}

		MEMORY_BASIC_INFORMATION mbi{};
		std::uintptr_t address = 0;

		while ( running && VirtualQueryEx( memory->get_handle( ), reinterpret_cast<void*>( address ), &mbi, sizeof( mbi ) ) ) {
			if ( mbi.State == MEM_COMMIT && !( mbi.Protect & PAGE_GUARD ) && !( mbi.Protect & PAGE_NOACCESS ) ) {
				const auto region_size = static_cast<std::size_t>( mbi.RegionSize );

				if ( region_size > 0 && region_size <= 0x20000000 ) {
					std::vector<std::uint8_t> buffer( region_size );
					SIZE_T bytes_read = 0;

					if ( ReadProcessMemory( memory->get_handle( ), mbi.BaseAddress, buffer.data( ), region_size, &bytes_read ) ) {
						const auto vtable = base_address + offsets::FastClusterEntity::VTableRva;

						for ( std::size_t i = 0; i + sizeof( std::uintptr_t ) <= bytes_read; i += sizeof( std::uintptr_t ) ) {
							std::uintptr_t value = 0;

							std::memcpy( &value, buffer.data( ) + i, sizeof( value ) );

							if ( value != vtable )
								continue;

							auto entity_address = reinterpret_cast<std::uintptr_t>( mbi.BaseAddress ) + i;

							if ( entity_address < 0x10000 )
								continue;

							material_set( entity_address );
						}
					}
				}
			}

			address = reinterpret_cast<std::uintptr_t>( mbi.BaseAddress ) + mbi.RegionSize;
		}

		reload( );
		Sleep( 500 );
	}

	reset( );
}

void cheat::engine_chams_t::init( ) {
	if ( running.exchange( true ) )
		return;

	thread = std::thread( &engine_chams_t::loop, this );
}

void cheat::engine_chams_t::denit( ) {
	if ( !running.exchange( false ) )
		return;

	if ( thread.joinable( ) )
		thread.join( );

	reset( );
}