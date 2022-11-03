#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "particle_system.h"

ErrT ParticlesPool::allocate(const size_t capacity) {
	this->memory_buf = std:: make_unique<char[]>(capacity * (2 * sizeof(Vect2f) + 2 * sizeof(int) + sizeof(bool)));
	if (memory_buf == nullptr)
		return MemErr;
	positions = reinterpret_cast<Vect2f*>(memory_buf.get());
	velocities = reinterpret_cast<Vect2f*>(positions + capacity);
	colors = reinterpret_cast<int*>(velocities + capacity);
	lifetimes = reinterpret_cast<int*>(colors + capacity);
	is_alive = reinterpret_cast<bool*>(lifetimes + capacity);
	this->capacity = capacity;
	return OK;
}

void ParticlesPool::update(const int delta) {
	const float dt = delta / 100.f;
	for (size_t i = 0; i < size; ++i) {
		Vect2f& pos = positions[i];
		Vect2f& vel = velocities[i];
		vel.y -= GRAVITY_CONSTANT * dt;
		pos.x += vel.x * dt;
		pos.y += vel.y * dt;
		lifetimes[i] -= delta;
		if (pos.x < 0 || pos.x > test::SCREEN_WIDTH || pos.y < 0 || pos.y > test::SCREEN_HEIGHT ||
			lifetimes[i] <= 0) {
			is_alive[i] = false;
		}
		if (lifetimes[i] <= 0)
			if (std::rand() / static_cast<float>(RAND_MAX) < EXPLOSION_PROBABILITY)
				explode(*this, pos, RED);
	}
}

void ParticlesPool::move_particle(const size_t dst_ind, const size_t src_ind) {
	positions[dst_ind] = positions[src_ind];
	velocities[dst_ind] = velocities[src_ind];
	colors[dst_ind] = colors[src_ind];
	lifetimes[dst_ind] = lifetimes[src_ind];
	is_alive[dst_ind] = is_alive[src_ind];
}

void ParticlesPool::refine() {
	for (size_t i = 0; i < size; ++i) {
		if (!is_alive[i]) {
			this->move_particle(i, size - 1);
			--i;
			--size;
		}
	}
}

void copy_for_rendering(ParticlesPool& dst, const ParticlesPool& src) {
	::memcpy(dst.positions, src.positions, src.size * sizeof(Vect2f));
	::memcpy(dst.colors, src.colors, src.size * sizeof(int));
	dst.size = src.size;
}

void explode(ParticlesPool& buf, const Vect2f& pos, const int color) {
	const size_t points_count = std::min(EFFECT_SIZE, buf.capacity - buf.size);
	for (size_t i = 0; i < points_count; ++i) {
		buf.positions[buf.size + i] = pos;
		buf.velocities[buf.size + i] =
		{ (10.f + 3.f * std::rand() / RAND_MAX) * std::cosf(M_PI / 32 * i), 
		  (10.f + 3.f * std::rand() / RAND_MAX) * std::sinf(M_PI / 32 * i)};
		buf.lifetimes[buf.size + i] = PARTICLE_LIFETIME;
		buf.is_alive[buf.size + i] = true;
		buf.colors[buf.size + i] = color;
	}
	buf.size += points_count;
}

ErrT ParticlesSystem::allocate(const size_t capacity) {
	ErrT alloc_check = physics_pool.allocate(capacity);
	if (alloc_check != OK)
		return MemErr;
	alloc_check = render_pool.allocate(capacity);
	if (alloc_check != OK)
		return MemErr;
	alloc_check = buffer.allocate(capacity);
	if (alloc_check != OK)
		return MemErr;
	return OK;
} 
