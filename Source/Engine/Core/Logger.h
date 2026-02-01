#pragma once

namespace bl {

class Logger
{
public:
    Logger();
    ~Logger();

    void Info();
    void Error();
};

} // namespace bl