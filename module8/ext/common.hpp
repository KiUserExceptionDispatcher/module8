#pragma once
#define FMT_HEADER_ONLY
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <cstdio>
#include <cstddef>
#include <chrono>
#include <thread>
#include <TlHelp32.h>
#include <Psapi.h>
#include <memory>

#include <src/utils/logger.hpp>
#include <src/memory/memory.hpp>
#include <src/rbx_engine/classes/classes.hpp>
#include <src/utils/globals/globals.hpp>
#include <src/utils/cache/cache.hpp>