#include "Danechka.h"

#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{
    danechka::_2Chainz SFEDU(10, 0, {2, 3, 4}, { 6, 8, 9});

    for (int i = 1; i < 60; i++)
    {
        if (i % 2 == 0)
            SFEDU.pushA(1);
        else
            SFEDU.pushB(2);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
