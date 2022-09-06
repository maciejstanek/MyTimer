#pragma once

#include <thread>
#include <chrono>
#include <type_traits>

namespace {
	// Attribution: https://stackoverflow.com/questions/41850985/check-if-a-template-parameter-is-some-type-of-chronoduration
	template<class T>
	struct is_duration : std::false_type {};

	template<class Rep, class Period>
	struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};
}

struct Timer
{
	// I have allowed myself to play around with enable_if.
	// This was not necessary for this task but making such
	// quasi contracts might be a good idea to implement for
	// library-like code.
	template<typename Callable, typename Delay,
		typename std::enable_if < std::is_invocable<Callable>{}, bool > ::type = true,
		typename std::enable_if < is_duration<Delay>{}, bool > ::type = true >
	void callWithDelay(Callable callable, Delay delay) {
		stopped_ = false;
		// We need to pass the callable by value and not by
		// reference since it will be called in another thread.
		// Stop flag is passed by reference since this is passed
		// as a pointer by default.
		std::thread thr([=]() {
			std::this_thread::sleep_for(delay);
			if (!this->stopped_) {
				callable();
				this->stopped_ = true;
			}
			});
		thr.detach();
	}

	template<typename Callable, typename Period,
		typename std::enable_if < std::is_invocable<Callable>{}, bool > ::type = true,
		typename std::enable_if < is_duration<Period>{}, bool > ::type = true >
	void callWithPeriod(Callable callable, Period period) {
		stopped_ = false;
		// The following approach has the unintended consequence
		// that the period is always increased by the execution
		// time of the callback and the following lambda. The
		// best would be relegate the callable call to another
		// detached thread so that we can immediately enter
		// sleep_for again. On the other hand it might be too
		// thread-intense. To investigate.
		std::thread thr([=]() {
			while (!this->stopped_) {
				std::this_thread::sleep_for(period);
				if (this->stopped_) {
					// Just stop in case the task was canceled
					// during the wait.
					return;
				}
				callable();
			}
			});
		thr.detach();
	}

	void stop() {
		// We cannot stop the thread itself because we don't have
		// any reference to it. Instead, we will let it know that
		// it must not perform the requested action.
		stopped_ = true;
	}

private:
	// The stop flag does not have to be an atomic object to protect
	// it from data races. The only possible conflict could happen
	// when the timer is stopped right after starting it.
	bool stopped_ = true;
};