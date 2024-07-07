#pragma once

template<typename TReturn, class... TArgs>
class Event {
public:


private:
    
    std::function<TReturn(TArgs...)> _call;
};