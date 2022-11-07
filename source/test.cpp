#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <vector>

#include <future>
#include <thread>
#include <mutex>
#include <atomic>

#include "./nvToolsExt.h"

#include "particle_system.h"
#include "test.h"

ParticlesSystem particle_system;

std::vector<Vect2f> event_query;
constexpr size_t QUERY_CAPACITY = 10;

static std::atomic_int globalTime;

static std::atomic_bool worker_must_exit = false;
static std::atomic_bool physics_is_copying = false;
static std::atomic_bool render_is_copying = false;
static std::mutex click_lock;

void WorkerThread(void)
{
	try {
		particle_system.allocate(PARTICLE_LIMIT);
	}
	catch (std::bad_alloc) {
		printf("Not enough memory! Effect limit was violated!\n");
		test::term();
	}
	while (!worker_must_exit)
	{
		nvtxRangePush(__FUNCTION__);
		static int lastTime = 0;
		const int time = globalTime.load();
		int delta = time - lastTime;
		lastTime = time;
		if (delta > 0)
		{
			if (!render_is_copying.load()) {
				physics_is_copying = true;
				copy_for_rendering(particle_system.buffer, particle_system.physics_pool);
				physics_is_copying = false;
			 }
			click_lock.lock();
			for (const auto& i : event_query) {
				explode(particle_system.physics_pool, i, YELLOW);
			}
			event_query.clear();
			click_lock.unlock();
			particle_system.physics_pool.update(delta);
			particle_system.physics_pool.refine();       
		}
		if (delta < MIN_UPDATE_PERIOD_MS)
			std::this_thread::sleep_for(std::chrono::milliseconds(MIN_UPDATE_PERIOD_MS - delta));

		nvtxRangePop();
	}
}
 

void test::init(void)
{
	event_query.reserve(QUERY_CAPACITY);
	std::thread workerThread(WorkerThread); 
	workerThread.detach(); 
}

void test::term(void)
{
	worker_must_exit = true;
}

void test::render(void)
{
	if (!physics_is_copying.load()) {
		render_is_copying = true;
		copy_for_rendering(particle_system.render_pool, particle_system.buffer);
		render_is_copying = false;
	} 
	
	for (size_t i = 0; i < particle_system.render_pool.size; ++i) {
		Vect2f& pos = particle_system.render_pool.positions[i];
		const int r = (particle_system.render_pool.colors[i] & 0xFF000000) >> 24;
		const int g = (particle_system.render_pool.colors[i] & 0x00FF0000) >> 16;
		const int b = (particle_system.render_pool.colors[i] & 0x0000FF00) >> 8;
		platform::drawPoint(pos.x, pos.y, r, g, b, 1.f);
	}
}

void test::update(int dt)
{
	globalTime.fetch_add(dt);
}

void test::on_click(int x, int y)
{
	std::lock_guard<std::mutex> lg(click_lock);
	event_query.push_back(std::move(Vect2f{ static_cast<float>(x), static_cast<float>(SCREEN_HEIGHT - y) }));
}

