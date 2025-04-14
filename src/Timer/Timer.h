#ifndef TIMER_H
#define TIMER_H
#include <SDL.h>
const int TARGET_FPS = 60;
const float TARGET_DELTATIME = 1.0f / TARGET_FPS;
class Timer
{
public:
    static Timer* GetInstance();
    void Tick();
    inline float GetDeltaTime() { return m_DeltaTime; }
    ~Timer();

private:
    Timer();
    static Timer* s_Instance;
    float m_DeltaTime;
    Uint32 m_LastTime;
};

#endif
