#pragma once

class StripController
{
    public:
    static StripController& getInstance(void);
    void start(void);
    void handler(void * pvParameter) {}

    private:
    StripController() = default;
};