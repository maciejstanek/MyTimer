#include "pch.h"
#include "../../MyTimer/MyTimer/Timer.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

TEST(Timer, OneShotPrecision) {
	constexpr auto runTime = 10ms;
	constexpr auto precision = 3ms;
	auto value = false;
	const auto setValue = [&value]() { value = true; };
	Timer timer;
	timer.callWithDelay(setValue, runTime);
	sleep_for(runTime - precision);
	EXPECT_FALSE(value);
	sleep_for(2 * precision);
	EXPECT_TRUE(value);
}

TEST(Timer, OneShotStop) {
	constexpr auto runTime = 10ms;
	constexpr auto precision = 3ms;
	auto done = false;
	const auto action = [&done]() { done = true; };
	Timer timer;
	timer.callWithDelay(action, runTime);
	sleep_for(runTime - precision);
	timer.stop();
	EXPECT_FALSE(done);
	sleep_for(2 * precision);
	EXPECT_FALSE(done);
}

/*
TEST(Timer, PeriodicalPrecision) {
	constexpr auto oneTick = 200ms;
	// TODO #1: The timer precision is impacted significantly by the
	// run time of the callback, leading to increased period.
	constexpr auto expectedCallCount = 10;
	constexpr auto fullTime = (expectedCallCount + 0.5) * oneTick;
	auto counter = 0;
	const auto tickOnce = [&counter]() { ++counter; };
	Timer timer;
	timer.callWithPeriod(tickOnce, oneTick);
	sleep_for(fullTime);
	EXPECT_EQ(counter, expectedCallCount);
}

TEST(Timer, PeriodicalStop) {
	constexpr auto oneTick = 200ms;
	// TODO #1: The timer precision is impacted significantly by the
	// run time of the callback, leading to increased period.
	constexpr auto expectedCallCount = 10;
	constexpr auto fullTime = (expectedCallCount + 0.5) * oneTick;
	auto counter = 0;
	const auto tickOnce = [&counter]() { ++counter; };
	Timer timer;
	timer.callWithPeriod(tickOnce, oneTick);
	sleep_for(fullTime);
	timer.stop();
	sleep_for(expectedCallCount * oneTick);
	EXPECT_EQ(counter, expectedCallCount);
}
*/

TEST(Timer, IsRunning) {
	constexpr auto runTime = 2ms;
	const auto action = []() {};
	Timer timer;
	EXPECT_FALSE(timer.isRunning());
	timer.callWithPeriod(action, 2 * runTime);
	sleep_for(runTime);
	EXPECT_TRUE(timer.isRunning());
	sleep_for(2 * runTime);
	EXPECT_FALSE(timer.isRunning());
}