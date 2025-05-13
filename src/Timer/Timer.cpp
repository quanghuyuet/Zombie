#include "Timer.h"

Timer* Timer::s_Instance = nullptr;

Timer::Timer() {
    m_LastTime = SDL_GetTicks();
    m_DeltaTime = 0.0f;
}

Timer::~Timer() {
    delete s_Instance;
    s_Instance = nullptr;
}

Timer* Timer::GetInstance() {
    if (s_Instance == nullptr) {
        s_Instance = new Timer();
    }
    return s_Instance;
}

void Timer::Tick() {
    Uint32 currentTime = SDL_GetTicks();
    m_DeltaTime = (currentTime - m_LastTime) / 1000.0f;

    if (m_DeltaTime > TARGET_DELTATIME)
        m_DeltaTime = TARGET_DELTATIME;

    m_LastTime = currentTime;
}
