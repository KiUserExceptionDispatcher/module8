#pragma once
#include <fmt/base.h>
#include <fmt/color.h>

enum level {
	info = 0,
	dbg = 1,
	warning = 2,
	error = 3
};

struct logger_t {
	template<typename... Args>
	static void log( level lvl, const char* msg, Args... args ) {
		const auto now = std::chrono::system_clock::now( );
		const auto time = std::chrono::system_clock::to_time_t( now );

		std::tm local_time{};
		localtime_s( &local_time, &time );

		fmt::print( fmt::fg( fmt::rgb( 125, 125, 125 ) ), "[{:02}:{:02}:{:02}] ", local_time.tm_hour, local_time.tm_min, local_time.tm_sec );

		switch ( lvl ) {
		case level::info:
			fmt::print( fmt::fg( fmt::rgb( 186, 95, 118 ) ), "[info] : " );
			break;

		case level::dbg:
			fmt::print( fmt::fg( fmt::rgb( 145, 120, 145 ) ), "[dbg] : " );
			break;

		case level::warning:
			fmt::print( fmt::fg( fmt::rgb( 205, 155, 95 ) ), "[warning] : " );
			break;

		case level::error:
			fmt::print( fmt::fg( fmt::rgb( 215, 90, 105 ) ), "[error] : " );
			break;
		}

		fmt::vprint( msg, fmt::make_format_args( args... ) );
		fmt::print( "\n" );
	}

	static void init( std::string title ) {
		SetConsoleTitleA( title.c_str( ) );

		HANDLE handle = GetStdHandle( STD_OUTPUT_HANDLE );
		if ( handle == INVALID_HANDLE_VALUE )
			return;

		DWORD mode = 0;

		if ( !GetConsoleMode( handle, &mode ) )
			return;

		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		SetConsoleMode( handle, mode );
	}
};

inline std::unique_ptr< logger_t > logger = std::make_unique< logger_t >( );