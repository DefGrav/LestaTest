 
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <algorithm>

#include <thread>
#include <mutex>
#include <atomic>

#include "./nvToolsExt.h"

#include "test.h"

constexpr float GRAVITY_CONSTANT = 0.5f * 9.807f;
constexpr float PROBABILITY = 1.f / 64.f;

constexpr int YELLOW = 0xFFFF0000;
constexpr int RED = 0xFF000000;

constexpr size_t EFFECT_SIZE = 64;
constexpr int EFFECTS_LIMIT = 2024;
constexpr int POINTS_LIMIT = EFFECT_SIZE * EFFECTS_LIMIT;
constexpr int POINT_LIFETIME = 1000;

static std::atomic_int globalTime;
static std::atomic_bool workerMustExit = false;

static struct Vect2f {
	float x, y;
};
static struct Points {
	int *memory_buf;
	Vect2f *position, *velocity;
	int *color, *lifetime;
	bool *is_alive;
	size_t count = 0;
	size_t end = 0;
};

Points points;

static inline void explode(const Vect2f& pos, const int color) {
	const size_t points_count = std::min(EFFECT_SIZE, POINTS_LIMIT - points.end);
	for (size_t i = 0; i < points_count; ++i) {
		points.position[points.end + i] = pos;
		points.velocity[points.end + i] = 
			{ 10.f * std::cosf(M_PI / 32 * i), 10.f * std::sinf(M_PI / 32 * i) };
		points.lifetime[points.end + i] = POINT_LIFETIME;
		points.is_alive[points.end + i] = true;
		points.color[points.end + i] = color;
	}
	points.end += points_count;
}

static inline void replace_point(const size_t dst_ind, const size_t src_ind) {
	points.position[dst_ind] = points.position[src_ind];
	points.velocity[dst_ind] = points.velocity[src_ind];
	points.color[dst_ind] = points.color[src_ind];
	points.lifetime[dst_ind] = points.lifetime[src_ind];
	points.is_alive[dst_ind] = points.is_alive[src_ind];
}

void WorkerThread(void)
{
	while (!workerMustExit)
	{
		nvtxRangePush(__FUNCTION__);

		static int lastTime = 0;
		const int time = globalTime.load();
		const int delta = time - lastTime;
		const float dt = delta / 100.f;
		lastTime = time;

		if (delta > 0)
		{
			for (size_t i = 0; i < points.end; ++i) {
				Vect2f& pos = points.position[i];
				Vect2f& vel = points.velocity[i];
				vel.y -= GRAVITY_CONSTANT * dt;
				pos.x += vel.x * dt;
				pos.y += vel.y * dt;
				points.lifetime[i] -= delta;
				if (pos.x < 0 || pos.x > test::SCREEN_WIDTH || pos.y < 0 || pos.y > test::SCREEN_HEIGHT ||
					points.lifetime[i] <= 0) {
					points.is_alive[i] = false;
				}
				if (points.lifetime[i] <= 0)
					if (std::rand() / static_cast<float>(RAND_MAX) < PROBABILITY)
						explode(pos, RED);
			}
			for (size_t i = 0; i < points.end; ++i) {
				if (!points.is_alive[i]) {
					replace_point(i, points.end - 1);
					--i;
					--points.end;
				}
			}
		}

		//static const int MIN_UPDATE_PERIOD_MS = 10;
		//if (delta < MIN_UPDATE_PERIOD_MS)
		//	std::this_thread::sleep_for(std::chrono::milliseconds(MIN_UPDATE_PERIOD_MS - delta));

		nvtxRangePop();
	}
}


void test::init(void)
{
	points.memory_buf = static_cast<int *>(malloc(POINTS_LIMIT * 6 * sizeof(int) + POINTS_LIMIT));
	points.position = reinterpret_cast<Vect2f *>(points.memory_buf);
	points.velocity = reinterpret_cast<Vect2f *>(points.position + POINTS_LIMIT);
	points.color = reinterpret_cast<int *>(points.velocity + POINTS_LIMIT);
	points.lifetime = reinterpret_cast<int *>(points.color + POINTS_LIMIT);
	points.is_alive = reinterpret_cast<bool*>(points.lifetime + POINTS_LIMIT);
	std::thread workerThread(WorkerThread);
	workerThread.detach(); // Glut + MSVC = join hangs in atexit()
}

void test::term(void)
{
	workerMustExit = true;
	free(points.memory_buf);
}

void test::render(void)
{
	for (size_t i = 0; i < points.end; ++i) {
		Vect2f& pos = points.position[i];
		int r = (points.color[i] & 0xFF000000) >> 24;
		int g = (points.color[i] & 0x00FF0000) >> 16;
		int b = (points.color[i] & 0x0000FF00) >> 8;
		platform::drawPoint(pos.x, pos.y, r, g, b, 1.f);
	}
}

void test::update(int dt)
{
	globalTime.fetch_add(dt);
}

void test::on_click(int x, int y)
{
		explode(Vect2f{static_cast<float>(x), static_cast<float>(SCREEN_HEIGHT - y)}, YELLOW);
}

