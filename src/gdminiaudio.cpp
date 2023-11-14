#define MINIAUDIO_IMPLEMENTATION
#include "gdminiaudio.h"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>

using namespace godot;

void GDMiniaudio::_bind_methods() {
}

// Initialize any variables here.
GDMiniaudio::GDMiniaudio() {
    ma_result result;
    ma_engine engine;
}

// Add your cleanup here.
GDMiniaudio::~GDMiniaudio() {
    
}

void GDMiniaudio::_ready() {
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize audio engine.\n");
    }
}

void GDMiniaudio::_process(double delta) {
}