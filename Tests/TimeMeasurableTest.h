#ifndef TIME_MEASUREABLE_TEST_H_INCLUDED
#define TIME_MEASUREABLE_TEST_H_INCLUDED
#pragma once

#include <chrono>
#include <iostream>

class TimeMeasurableTest
{
public:
    TimeMeasurableTest()
    {
        start_time_ = std::chrono::high_resolution_clock::now();
    }

    ~TimeMeasurableTest()
    {
        end_time_ = std::chrono::high_resolution_clock::now();
        const auto elapsed_time =
                std::chrono::duration_cast<std::chrono::nanoseconds>(end_time_ - start_time_);
        std::cout << testing::UnitTest::GetInstance()->current_test_info()->name() <<
                ": elapsed time = " << elapsed_time.count() <<
                " nanoseconds" << std::endl << std::endl;
    }

protected:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
};

#endif // TIME_MEASUREABLE_TEST_H_INCLUDED
