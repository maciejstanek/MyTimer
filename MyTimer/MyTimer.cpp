#include "Timer.h"

#include <iostream>
#include <chrono>
#include <thread>

void exampleCall() {
	std::cout << "Hello from the callback!\n";
}

int main()
{
	using namespace std::chrono_literals;
	using std::this_thread::sleep_for;

	{
		Timer timer;
		std::cout << "Example 1s delay over 2s in the main thread:\n";
		timer.callWithDelay(exampleCall, 1s);
		sleep_for(2s);
		std::cout << "Example from the main thread.\n";
	}

	{
		const auto myLambda = []() {
			std::cout << "Hello from the lambda!\n";
		};
		std::cout << "\nExample 1s delay in a lambda over 2s in the main thread:\n";
		Timer timer;
		timer.callWithDelay(myLambda, 1s);
		sleep_for(2s);
		std::cout << "Example from the main thread.\n";
	}

	{
		std::cout << "\nExample stop:\n";
		Timer timer;
		timer.callWithDelay(exampleCall, 2s);
		sleep_for(1s);
		timer.stop();
		std::cout << "Has anything happened? Waiting two seconds just to make sure.\n";
		sleep_for(2s);
		std::cout << "Still nothing, good.\n";
	}

	{
		Timer timer;
		std::cout << "\nExample short delay:\n";
		timer.callWithDelay(exampleCall, 100ms);
		sleep_for(1s);
		std::cout << "Whoa, that was fast!\n";
	}

	{
		auto counter = 0;
		const auto countedLambda = [&counter]() {
			std::cout << "Hello from the counted lambda!\n";
			++counter;
		};
		std::cout << "\nExample periodical timer:\n";
		Timer timer;
		timer.callWithPeriod(countedLambda, 190ms);
		sleep_for(1s);
		timer.stop();
		std::cout << "Done. Called the lambda " << counter << " times.\n";
	}

	{
		Timer timer;
		std::cout << "\nWhat will happen if I return from main before the scheduled task is done?\n";
		timer.callWithDelay(exampleCall, 1s);
	}

	return 0;
}