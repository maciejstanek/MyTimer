#include "pch.h"
#include "../../MyTimer/MyTimer/Timer.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using std::this_thread::sleep_for;

TEST(PlainTimer, Precision) {
	constexpr auto runTime = 3ms;
	constexpr auto precision = 1ms;
	static_assert(precision < runTime, "The timer run time must be longer than the tested timer precision");
	auto value = false;
	const auto setValue = [&value]() { value = true; };
	Timer timer;
	timer.callWithDelay(setValue, runTime);
	sleep_for(runTime - precision);
	EXPECT_FALSE(value);
	sleep_for(2*precision);
	EXPECT_TRUE(value);
}