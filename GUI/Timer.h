#pragma once

#include <iostream>
#include <string>
#include <chrono>

namespace GUI {
  class Timer
  {
  public:
    Timer()
    {
      Reset();
    }

    void Reset()
    {
      m_Start = std::chrono::high_resolution_clock::now();
    }

    float Elapsed()
    {
      using namespace std::chrono;
      return duration_cast<nanoseconds>(high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001;
    }

    float ElapsedMillis()
    {
      return Elapsed() * 1000.0f;
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
  };

  class ScopedTimer
  {
  public:
    ScopedTimer(std::string& name)
      : m_Name(name){}
    ~ScopedTimer()
    {
      float time = m_Timer.ElapsedMillis();
      std::cout << "[TIMER] " << m_Name << " - " << time << "ms\n";
    }
  private:
    std::string m_Name;
    Timer m_Timer;
  };
}