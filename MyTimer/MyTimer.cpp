#include <iostream>

#include "Timer.h"

void exampleCall() {
    std::cout << "Bazinga! Hellow from the timer!\n";
}

int main()
{
    using namespace std::chrono_literals;
    using std::this_thread::sleep_for;
    Timer timer;

    std::cout << "Example 2s delay over 4s in the main thread:\n";
    timer.callWithDelay(exampleCall, 2s);
    sleep_for(3s);
    std::cout << "Example from the main thread.\n";

    std::cout << "\nExample stop:\n";
    timer.callWithDelay(exampleCall, 2s);
    sleep_for(1s);
    timer.stop();
    std::cout << "Has anything happened? Waiting three secs just to make sure.\n";
    sleep_for(3s);
    std::cout << "Still nothing.\n";

    std::cout << "\nExample short period:\n";
    timer.callWithDelay(exampleCall, 100ms);
    sleep_for(1s);
    std::cout << "Whoa, that was fast!\n";

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
