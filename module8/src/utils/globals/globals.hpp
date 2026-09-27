#pragma once

struct globals_t {
	engine::data_model data_model;
	engine::workspace workspace;
	engine::visual_engine visual_engine;
	engine::player players;

	bool streamproof;
	bool vsync;
};

inline std::unique_ptr< globals_t > g = std::make_unique< globals_t >( );