#pragma once
#include <memory>

#include "test.h"

constexpr int YELLOW = 0xFFFF0000;
constexpr int RED = 0xFF000000;

constexpr size_t EFFECT_SIZE = 64;
constexpr size_t EFFECTS_LIMIT = 2024;
constexpr size_t PARTICLE_LIMIT = EFFECT_SIZE * EFFECTS_LIMIT;
constexpr size_t PARTICLE_LIFETIME = 1000;

constexpr float GRAVITY_CONSTANT = 0.5f * 9.807f;
constexpr float EXPLOSION_PROBABILITY = 2.f / EFFECT_SIZE;
//constexpr float EXPLOSION_PROBABILITY = 1.f;

enum ErrT {
	MemErr = -1,
	OK
};

struct Vect2f {
	float x, y;
};

struct ParticlesPool {
	ErrT allocate(const size_t capacity);
	void update(const int delta);
	void move_particle(const size_t dst_ind, const size_t src_ind);
	void refine();
	std::unique_ptr<char[]> memory_buf;
	Vect2f* positions, * velocities;
	int* colors, * lifetimes;
	bool* is_alive;
	size_t capacity = 0;
	size_t size = 0;
};

struct ParticlesSystem {
	ErrT allocate(const size_t capacity);

	ParticlesPool physics_pool;
	ParticlesPool render_pool;
	ParticlesPool buffer;
};

void copy_for_rendering(ParticlesPool& dst, const ParticlesPool& src);
void explode(ParticlesPool& buf, const Vect2f& pos, const int color);