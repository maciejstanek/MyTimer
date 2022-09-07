#pragma once

#include <thread>
#include <chrono>
#include <type_traits>
#include <stdexcept>
#include <atomic>

namespace {
	// Attribution: https://stackoverflow.com/questions/41850985/check-if-a-template-parameter-is-some-type-of-chronoduration
	template<class T>
	struct is_duration : std::false_type {};

	template<class Rep, class Period>
	struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};
}

// Attribution: https://www.fluentcpp.com/2018/12/28/timer-cpp/
// Attribution: https://stackoverflow.com/questions/30425772/c-11-calling-a-c-function-periodically
struct Timer
{
	Timer() : started_{ false } {}
	~Timer() {
		// This should take care of the timer thread
		// after the timer goes out of scope. Stopping
		// is blocking, so it would be best to store
		// the timer somewhere on the heap.
		stop();
	}

	// I have allowed myself to play around with enable_if.
	// This was not necessary for this task but making such
	// quasi contracts might be a good idea to implement for
	// library-like code.
	template<typename Callable, typename Delay,
		typename std::enable_if < std::is_invocable<Callable>{}, bool > ::type = true,
		typename std::enable_if < is_duration<Delay>{}, bool > ::type = true >
	void callWithDelay(Callable callable, Delay delay) {
		prepareForNewTask();
		// We need to pass the callable by value and not by
		// reference since it will be called in another thread.
		// Stop flag is passed by reference since this is passed
		// as a pointer by default.
		thread_ = std::thread([=]() {
			std::this_thread::sleep_for(delay);
			if (this->isRunning()) {
				callable();
				this->started_.store(false);
			}
			});
	}

	template<typename Callable, typename Period,
		typename std::enable_if < std::is_invocable<Callable>{}, bool > ::type = true,
		typename std::enable_if < is_duration<Period>{}, bool > ::type = true >
	void callWithPeriod(Callable callable, Period period) {
		prepareForNewTask();
		thread_ = std::thread([=]() {
			// Added an extra sleep at the beginning because
			// I would expect this method to behave similarly
			// to callWithDelay. This extra wait could not be
			// cleanly achieved by reversing the actions in
			// the loop (sleep vs callable), as this would require
			// an ugly while(true) loop and an exit condition
			// in the middle (even uglier).
			auto cycle_end_time = std::chrono::steady_clock::now() + period;
			std::this_thread::sleep_until(cycle_end_time);
			while (this->isRunning()) {
				callable();
				std::this_thread::sleep_until(cycle_end_time);
				// Need to calculate the end time before the loop
				// condition is checked, because this check can
				// have a significant run time.
				cycle_end_time = std::chrono::steady_clock::now() + period;
			}
			});
	}

	void stop() {
		// We cannot stop the thread itself because we don't have
		// any reference to it. Instead, we will let it know that
		// it must not perform the requested action.
		started_.store(false);
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	bool isRunning() const {
		return started_.load();
	}
	bool isHalted() const {
		return !isRunning();
	}

private:
	void prepareForNewTask() {
		if (isRunning()) {
			// This safely allows the user to reuse the timer
			// for a different callback or with a different
			// delay.
			stop();
		}
		started_.store(true);
	}

	std::atomic<bool> started_;
	std::thread thread_;
};