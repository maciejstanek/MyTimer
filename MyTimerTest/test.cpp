#include "pch.h"
#include "../../MyTimer/MyTimer/Timer.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

TEST(Timer, OneShotPrecision) {
	constexpr auto runTime = 3ms;
	constexpr auto precision = 1ms;
	auto value = false;
	const auto setValue = [&value]() { value = true; };
	Timer timer;
	timer.callWithDelay(setValue, runTime);
	sleep_for(runTime - precision);
	EXPECT_FALSE(value);
	sleep_for(2 * precision);
	EXPECT_TRUE(value);
}

TEST(Timer, PeriodicalPrecision) {
	constexpr auto oneTick = 200ms;

	// TODO: The timer precision is impacted significantly by the
	//       run time of the callback, leading to increased period.
	constexpr auto expectedCallCount = 10;
	constexpr auto fullTime = (expectedCallCount + 0.5) * oneTick;
	auto counter = 0;
	const auto tickOnce = [&counter]() { ++counter; };
	Timer timer;
	timer.callWithPeriod(tickOnce, oneTick);
	sleep_for(fullTime);
	EXPECT_EQ(counter, expectedCallCount);
}

// TODO: test stop for one shot and for periodical
// TODO: test the one use exception
// TODO: test hasStarted