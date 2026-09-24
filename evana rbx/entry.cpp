#include <iostream>
#include <src/graphics/render/render.hpp>
#include <src/cheat/features/visuals/chams/engine/engine_chams.hpp>

void init_terminal( const wchar_t* name, short size ) {
    HANDLE console = GetStdHandle( STD_OUTPUT_HANDLE );

    CONSOLE_FONT_INFOEX font{};
    font.cbSize = sizeof( font );
    font.dwFontSize.X = 0;
    font.dwFontSize.Y = size;
    font.FontFamily = FF_DONTCARE;
    font.FontWeight = FW_NORMAL;

    wcscpy_s( font.FaceName, name );

    SetCurrentConsoleFontEx( console, FALSE, &font );
    //
    auto console_window = GetConsoleWindow( );

    auto rect = SMALL_RECT{0, 0, 88, 27};
    SetConsoleWindowInfo( console_window, TRUE, &rect );
}

std::int32_t main() {
    init_terminal( L"MS Mincho", 14 );
	logger->init( "britain" );

	memory->attach( L"RobloxPlayerBeta.exe" );
	logger->log( level::info, "attached to roblox" );

	logger->log( level::info, "process id @ {:d}", memory->get_pid( ) );
	logger->log( level::info, "base @ {:x}", memory->base );

    g->data_model = engine::data_model::get( );
    g->workspace = g->data_model.get_workspace( );
    g->visual_engine = engine::visual_engine::get( );
    g->players = (engine::player)g->data_model.find_class( "Players" ).address;

    logger->log( level::dbg, "data_model @ {:x}", g->data_model.address );
    logger->log( level::dbg, "visual_engine @ {:x}", g->visual_engine.address );
    logger->log( level::dbg, "workspace @ {:x}", g->workspace.address );
    logger->log( level::dbg, "players @ {:x}", g->players.address );

    cheat::engine_chams->init( );
    cache->init( );

    render->create_window( );
    render->create_device( );
    render->create_imgui( );

    while ( true ) {
        render->start_render( );

        render->render_menu( );
        render->render_visuals( );

        render->end_render( );
    }

	std::cin.get( );
}