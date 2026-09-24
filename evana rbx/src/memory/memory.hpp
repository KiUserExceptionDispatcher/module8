#pragma once
#include <windows.h>
#include <winternl.h>
#include <TlHelp32.h>
#include <string>
#include <vector>

typedef NTSTATUS(NTAPI* pNtReadVirtualMemory)( HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesRead );
typedef NTSTATUS(NTAPI* pNtWriteVirtualMemory)( HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T BufferSize, PSIZE_T NumberOfBytesWritten );
typedef NTSTATUS(NTAPI* pNtAllocateVirtualMemory)( HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect );
typedef NTSTATUS(NTAPI* pNtProtectVirtualMemory)( HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG NewProtect, PULONG OldProtect );

class memory_t
{
    HANDLE h;
    DWORD pid;
    pNtReadVirtualMemory NtReadVirtualMemory;
    pNtWriteVirtualMemory NtWriteVirtualMemory;
    pNtAllocateVirtualMemory NtAllocateVirtualMemory;
    pNtProtectVirtualMemory NtProtectVirtualMemory;

    struct rbx_string
    {
        union
        {
            char buffer[16];
            uintptr_t pointer;
        } data;
        uintptr_t length;
        uintptr_t capacity;
    };

public:
    HANDLE get_handle( ) const { return h; }
    DWORD get_pid( ) const { return pid; }
    static memory_t& get( ) {
        static memory_t instance;
        return instance;
    }

    uintptr_t base;

    memory_t( ) : h( 0 ), pid( 0 ), base( 0 )
    {
        HMODULE ntdll = GetModuleHandleA( "ntdll.dll" );
        NtReadVirtualMemory = (pNtReadVirtualMemory)GetProcAddress( ntdll, "NtReadVirtualMemory" );
        NtWriteVirtualMemory = (pNtWriteVirtualMemory)GetProcAddress( ntdll, "NtWriteVirtualMemory" );
        NtAllocateVirtualMemory = (pNtAllocateVirtualMemory)GetProcAddress( ntdll, "NtAllocateVirtualMemory" );
        NtProtectVirtualMemory = (pNtProtectVirtualMemory)GetProcAddress( ntdll, "NtProtectVirtualMemory" );
    }

    ~memory_t( )
    {
        if ( h )
            CloseHandle( h );
    }

    bool attach( const wchar_t* name );

    template<typename T>
    T read( uintptr_t addr )
    {
        T val{};
        if ( !addr ) return val;
        NtReadVirtualMemory( h, (PVOID)addr, &val, sizeof( T ), nullptr );
        return val;
    }

    template<typename T>
    void write( uintptr_t addr, T val )
    {
        NtWriteVirtualMemory( h, (PVOID)addr, &val, sizeof( T ), nullptr );
    }
    template<typename T>
    void read_array( uintptr_t addr, T* buffer, size_t size ) {
        NtReadVirtualMemory( h, (PVOID)addr, buffer, size, nullptr );
    }
    template<typename T>
    void write_array( uintptr_t addr, const T* buffer, size_t size ) {
        NtWriteVirtualMemory( h, (PVOID)addr, (PVOID)buffer, size, nullptr );
    }

    bool read_buf( uintptr_t addr, void* buf, size_t size );
    bool write_buf( uintptr_t addr, const void* buf, size_t size );
    bool rpm( uintptr_t addr, void* buf, size_t size );
    bool wpm( uintptr_t addr, const void* buf, size_t size );
    std::string read_string( uintptr_t addr );
    uintptr_t allocate( size_t size );
    bool write_string( uintptr_t addr, std::string new_str );
    bool is_valid( uintptr_t addr );
    bool protect( uintptr_t addr, size_t size, ULONG new_protect, ULONG* old_protect );
};

inline std::unique_ptr< memory_t > memory = std::make_unique< memory_t >( );