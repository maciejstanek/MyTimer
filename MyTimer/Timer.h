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
		// The following approach has the unintended consequence
		// that the period is always increased by the execution
		// time of the callback and the following lambda. The
		// best would be relegate the callable call to another
		// detached thread so that we can immediately enter
		// sleep_for again. On the other hand it might be too
		// thread-intense. To investigate.
		thread_ = std::thread([=]() {
			while (this->isRunning()) {
				std::this_thread::sleep_for(period);
				if (this->isHalted()) {
					// Just stop in case the task was canceled
					// during the wait.
					return;
				}
				callable();
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