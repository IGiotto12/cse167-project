#ifndef __PROGRESS_BAR_H__
#define __PROGRESS_BAR_H__

#include <iostream>
#include <chrono>
#include <thread>

class ProgressBar
{
public:
    ProgressBar(
        unsigned int total,
        std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now(),
        unsigned int bar_width = 50) : _total(total), _start_time(start_time), _bar_width(bar_width) {};

    void update(unsigned int progress)
    {
        float percentage = static_cast<float>(progress) / _total;
        unsigned int pos = _bar_width * percentage;

        // Calculate elapsed time
        double elapsed_time = std::chrono::duration<double>(std::chrono::steady_clock::now() - _start_time).count();

        // Calculate estimated remaining time
        double estimated_total_time = elapsed_time / (progress > 0 ? progress : 1) * _total;
        double remaining_time = estimated_total_time - elapsed_time;

        // Prepare elapsed and remaining time strings
        char elapsed_time_str[16];
        char remaining_time_str[16];
        formatTime(elapsed_time, elapsed_time_str);
        formatTime(remaining_time, remaining_time_str);

        std::cout << "Rendering Progress: [";
        for (unsigned int i = 0; i < _bar_width; ++i)
        {
            if (i < pos)
                std::cout << "=";
            else if (i == pos)
                std::cout << ">";
            else
                std::cout << " ";
        }
        std::cout << "] " << int(percentage * 100) << "% ";
        // Print formatted elapsed and remaining time
        std::cout << "Elapsed: " << elapsed_time_str << " Remaining: " << remaining_time_str << "\r";

        std::cout.flush();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start_time;
    unsigned int _total;
    unsigned int _bar_width;

    void formatTime(double seconds, char *buffer)
    {
        /**
         * `bar.update()` function is frequently called between frames,
         * that's why using char buffer instead of string to
         * boost performance
         */
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;

        if (minutes > 0)
        {
            // Format as "Xm Ys"
            int len = 0;
            buffer[len++] = minutes / 10 + '0';
            buffer[len++] = minutes % 10 + '0';
            buffer[len++] = 'm';
            buffer[len++] = ' ';
            buffer[len++] = secs / 10 + '0';
            buffer[len++] = secs % 10 + '0';
            buffer[len++] = 's';
            buffer[len] = '\0'; // Null-terminate the string
        }
        else
        {
            // Format as "Xs"
            int len = 0;
            buffer[len++] = secs / 10 + '0';
            buffer[len++] = secs % 10 + '0';
            buffer[len++] = 's';
            buffer[len] = '\0'; // Null-terminate the string
        }
    }
};

#endif