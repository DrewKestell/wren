#include "GameTimer.h"
#include <Windows.h>

GameTimer::GameTimer() :
    mSecondsPerCount(0.0),
    mDeltaTime(-1.0),
    mBaseTime(0),
    mPausedTime(0),
    mPrevTime(0),
    mCurrTime(0),
    mStopped(false)
{
    __int64 countsPerSec;
    QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
    mSecondsPerCount = 1.0 / (double)countsPerSec;
}void GameTimer::Tick()
{
    if (mStopped)
    {
        mDeltaTime = 0.0;
        return;
    }
    // Get the time this frame.
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    mCurrTime = currTime;
    // Time difference between this frame and the previous.
    mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;
    // Prepare for next frame.
    mPrevTime = mCurrTime;
    // Force nonnegative. The DXSDK's CDXUTTimer mentions that if the
    // processor goes into a power save mode or we get shuffled to another
    // processor, then mDeltaTime can be negative.
    if (mDeltaTime < 0.0)
    {
        mDeltaTime = 0.0;
    }
}

float GameTimer::DeltaTime()
{
    return (float)mDeltaTime;
}void GameTimer::Reset()
{
    __int64 currTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
    mBaseTime = currTime;
    mPrevTime = currTime;
    mStopTime = 0;
    mStopped = false;
}void GameTimer::Stop()
{
    // If we are already stopped, then don't do anything.
    if (!mStopped)
    {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        // Otherwise, save the time we stopped at, and set
        // the Boolean flag indicating the timer is stopped.
        mStopTime = currTime;
        mStopped = true;
    }
}void GameTimer::Start()
{
    __int64 startTime;
    QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
    // Accumulate the time elapsed between stop and start pairs.
    //
    // |<-------d------->|
    // ---------------*-----------------*------------> time
    // mStopTime startTime
    // If we are resuming the timer from a stopped state...
    if (mStopped)
    {
        // then accumulate the paused time.
        mPausedTime += (startTime - mStopTime);
        // since we are starting the timer back up, the current
        // previous time is not valid, as it occurred while paused.
        // So reset it to the current time.
        mPrevTime = startTime;
        // no longer stopped...
        mStopTime = 0;
        mStopped = false;
    }
}float GameTimer::TotalTime()
{
    // If we are stopped, do not count the time that has passed
    // since we stopped. Moreover, if we previously already had
    // a pause, the distance mStopTime - mBaseTime includes paused
    // time,which we do not want to count. To correct this, we can
    // subtract the paused time from mStopTime:
    //
    // previous paused time
    // |<----------->|
    // ---*------------*-------------*-------*-----------*------> time
    // mBaseTime mStopTime mCurrTime
    if (mStopped)
    {
        return (float)(((mStopTime - mPausedTime) -
            mBaseTime)*mSecondsPerCount);
    }
    // The distance mCurrTime - mBaseTime includes paused time,
    // which we do not want to count. To correct this, we can subtract
    // the paused time from mCurrTime:
    //
    // (mCurrTime - mPausedTime) - mBaseTime
    //
    // |<--paused time-->|
    // ----*---------------*-----------------*------------*------> time
    // mBaseTime mStopTime startTime mCurrTime
    else
    {
        return (float)(((mCurrTime - mPausedTime) -
            mBaseTime)*mSecondsPerCount);
    }
}
