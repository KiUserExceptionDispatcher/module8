#include "memory.hpp"

bool memory_t::read_buf(uintptr_t addr, void* buf, size_t size)
{
    if (!addr || !buf || !size) return false;
    SIZE_T got = 0;
    NTSTATUS s = NtReadVirtualMemory(h, (PVOID)addr, buf, size, &got);
    return s == 0 && got == size;
}

bool memory_t::write_buf(uintptr_t addr, const void* buf, size_t size)
{
    if (!addr || !buf || !size) return false;
    SIZE_T put = 0;
    NTSTATUS s = NtWriteVirtualMemory(h, (PVOID)addr, (PVOID)buf, size, &put);
    return s == 0 && put == size;
}

bool memory_t::rpm(uintptr_t addr, void* buf, size_t size)
{
    if (!addr || !buf || !size) return false;
    SIZE_T got = 0;
    return ReadProcessMemory(h, (LPCVOID)addr, buf, size, &got) && got == size;
}

bool memory_t::wpm(uintptr_t addr, const void* buf, size_t size)
{
    if (!addr || !buf || !size) return false;
    SIZE_T put = 0;
    return WriteProcessMemory(h, (LPVOID)addr, buf, size, &put) && put == size;
}

bool memory_t::attach(const wchar_t* name)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe =
    {
        sizeof(pe)
    };
    while (Process32NextW(snap, &pe))
        if (!wcscmp(pe.szExeFile, name))
        {
            pid = pe.th32ProcessID;
            break;
        }
    CloseHandle(snap);
    if (!pid)
        return false;
    h = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
    if (!h)
        return false;
    snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    MODULEENTRY32W me =
    {
        sizeof(me)
    };
    if (Module32FirstW(snap, &me))
        base = (uintptr_t)me.modBaseAddr;
    CloseHandle(snap);
    return base != 0;
}

std::string memory_t::read_string(uintptr_t addr)
{
    if (!addr || !is_valid(addr)) return "";
    int size = read<int>(addr + 0x10);
    if (size <= 0 || size > 255) return "";
    uintptr_t str_ptr = ( size >= 16 ) ? read<uintptr_t>(addr) : addr;
    if (!str_ptr || !is_valid(str_ptr)) return "";
    char buf[256] = {0};
    NtReadVirtualMemory(h, (PVOID)str_ptr, buf, size, nullptr);
    return std::string(buf, size);
}

uintptr_t memory_t::allocate(size_t size)
{
    PVOID allocated = nullptr;
    auto result = NtAllocateVirtualMemory(h, &allocated, 0, &size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (result != 0)
    {
        printf("[-] allocation failed ( err code 0x%llx )\n", result);
        return 0;
    }
    return (uintptr_t)( allocated );
}

bool memory_t::is_valid(uintptr_t addr)
{
    if (!addr || addr < 0x10000 || addr > 0x7FFFFFFEFFFF)
        return false;

    return true;
}


bool memory_t::write_string(uintptr_t addr, std::string new_str)
{
    auto str = read<rbx_string>(addr);
    str.length = (uintptr_t)( new_str.length() );

    if (str.length > 15)
    {
        if (new_str.length() > str.capacity)
        {
            while (new_str.length() > str.capacity)
            {
                str.capacity *= 2;
                str.capacity += 1;
            }
            str.data.pointer = allocate((size_t)( str.capacity ));
        }
        write<rbx_string>(addr, str);
        NtWriteVirtualMemory(h, (PVOID)str.data.pointer, (PVOID)new_str.c_str(), new_str.length(), nullptr);
    }
    else
    {
        str.capacity = 15;
        memcpy(str.data.buffer, new_str.c_str(), new_str.length());
        if (new_str.length() < 16)
            str.data.buffer[new_str.length()] = '\0';
        write<rbx_string>(addr, str);
    }
    return true;
}

bool memory_t::protect(uintptr_t addr, size_t size, ULONG new_protect, ULONG* old_protect)
{
    PVOID base_addr = (PVOID)addr;
    SIZE_T region_size = size;
    ULONG old_prot = 0;

    auto result = NtProtectVirtualMemory(h, &base_addr, &region_size, new_protect, &old_prot);

    if (old_protect) {
        *old_protect = old_prot;
    }

    if (result != 0) {
        printf("[-] NtProtectVirtualMemory failed with status 0x%lx\n", result);
    }

    return result == 0;
}