#pragma once

void stripControllerInit(void);

class StripController
{
    public:
    StripController() = default;
    static StripController& getInstance(void) { return stripController; }
    void handler(void * pvParameter);

    private:
    static StripController stripController;
};