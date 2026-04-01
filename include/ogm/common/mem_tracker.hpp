#pragma once

#include "ogm/common/Trace.hpp"
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <string>

#ifdef __3DS__
#include <3ds.h>

inline std::atomic<size_t> g_linear_ram_used{0};
inline std::atomic<size_t> g_heap_ram_used{0};

// Offset size to maintain 0x80 (128 bytes) alignment required by 3DS GPU
constexpr size_t TRACKER_ALIGNMENT = 128;

inline void* tracked_linearAlloc(size_t size) {
    // Allocate extra space to store the size
    void* ptr = linearAlloc(size + TRACKER_ALIGNMENT);
    if (!ptr) return nullptr;

    // Store the size at the beginning of the block
    *reinterpret_cast<size_t*>(ptr) = size;

    // Update tracker
    g_linear_ram_used += size;

    // Return the aligned offset pointer
    return static_cast<char*>(ptr) + TRACKER_ALIGNMENT;
}

inline void tracked_linearFree(void* ptr) {
    if (!ptr) return;

    // Recover the original base pointer
    void* base_ptr = static_cast<char*>(ptr) - TRACKER_ALIGNMENT;

    // Read the size that was stored
    size_t size = *reinterpret_cast<size_t*>(base_ptr);

    // Update tracker
    g_linear_ram_used -= size;

    // Free the original base pointer
    linearFree(base_ptr);
}

inline void* tracked_malloc(size_t size) {
    // Allocate extra space to store the size
    void* ptr = std::malloc(size + sizeof(size_t));
    if (!ptr) return nullptr;

    // Store the size at the beginning of the block
    *reinterpret_cast<size_t*>(ptr) = size;

    // Update tracker
    g_heap_ram_used += size;

    // Return the offset pointer
    return static_cast<char*>(ptr) + sizeof(size_t);
}

inline void tracked_free(void* ptr) {
    if (!ptr) return;

    // Recover the original base pointer
    void* base_ptr = static_cast<char*>(ptr) - sizeof(size_t);

    // Read the size that was stored
    size_t size = *reinterpret_cast<size_t*>(base_ptr);

    // Update tracker
    g_heap_ram_used -= size;

    // Free the original base pointer
    std::free(base_ptr);
}

inline void print_memory_stats() {
    SD_PRINT("Memory Stats - Linear RAM Used: " + std::to_string(g_linear_ram_used.load()) + " bytes, Heap RAM Used: " + std::to_string(g_heap_ram_used.load()) + " bytes");
}

#else

// Fallbacks for non-3DS builds
inline void* tracked_linearAlloc(size_t size) {
    return std::malloc(size);
}

inline void tracked_linearFree(void* ptr) {
    std::free(ptr);
}

inline void* tracked_malloc(size_t size) {
    return std::malloc(size);
}

inline void tracked_free(void* ptr) {
    std::free(ptr);
}

inline void print_memory_stats() {
    // No-op for non-3DS builds
}

#endif
