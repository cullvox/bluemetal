#pragma once

class GLFW
{
public:
    GLFW();
    ~GLFW();

    static void errorCallback(int error_code, const char* description);
};