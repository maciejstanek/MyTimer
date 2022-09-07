#include "pch.h"
#include "../../MyTimer/MyTimer/Timer.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

class TimerDelay : public ::testing::Test {
protected:
	auto runTime() { return 20ms; }
	auto precision() { return 10ms; }
};
TEST_F(TimerDelay, Precision) {
	auto value = false;
	const auto setValue = [&value]() { value = true; };
	Timer timer;
	timer.callWithDelay(setValue, runTime());
	sleep_for(runTime() - precision());
	EXPECT_FALSE(value) << "too early";
	sleep_for(2 * precision());
	EXPECT_TRUE(value) << "too late";
}
TEST_F(TimerDelay, Stop) {
	auto done = false;
	const auto action = [&done]() { done = true; };
	Timer timer;
	timer.callWithDelay(action, runTime());
	sleep_for(runTime() - precision());
	timer.stop();
	EXPECT_FALSE(done) << "false start";
	sleep_for(2 * precision());
	EXPECT_FALSE(done) << "stop failed";
}

TEST_F(TimerDelay, IsRunning) {
	const auto action = []() {};
	Timer timer;
	EXPECT_TRUE(timer.isHalted()) << "new timer created running";
	timer.callWithDelay(action, 2 * runTime());
	sleep_for(runTime());
	EXPECT_TRUE(timer.isRunning()) << "start failed";
	sleep_for(2 * runTime());
	EXPECT_TRUE(timer.isHalted()) << "stopped task not marked as stopped";
}

class TimerPeriodical : public ::testing::Test {
protected:
	auto oneTick() { return 20ms; }
	auto expectedCallCount() { return 4; }
	auto fullTime() { return (expectedCallCount() + 0.8) * oneTick(); }
};

TEST_F(TimerPeriodical, Precision) {
	auto counter = 0;
	const auto tickOnce = [&counter]() { ++counter; };
	Timer timer;
	timer.callWithPeriod(tickOnce, oneTick());
	sleep_for(fullTime());
	EXPECT_EQ(counter, expectedCallCount());
}

TEST_F(TimerPeriodical, Stop) {
	auto counter = 0;
	const auto tickOnce = [&counter]() { ++counter; };
	Timer timer;
	timer.callWithPeriod(tickOnce, oneTick());
	sleep_for(fullTime());
	timer.stop();
	sleep_for(2 * oneTick());
	EXPECT_EQ(counter, expectedCallCount());
}