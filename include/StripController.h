#pragma once

class StripController
{
    public:
    static StripController& getInstance(void);
    void start(void);

    private:
    StripController() = default;
    void handler(void * pvParameter);
};