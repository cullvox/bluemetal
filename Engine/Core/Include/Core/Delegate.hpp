#pragma once

#include "Core/TypeTraits.hpp"

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cassert>

namespace bl
{


template<typename TReturn, typename... TArgs>
constexpr auto FunctionPointer(TReturn (*)(TArgs...)) -> TReturn (*)(TArgs...);

template<typename TReturn, typename TType, typename... TArgs, typename TOther>
constexpr auto FunctionPointer(TReturn (*)(TType, TArgs...), TOther&&) -> TReturn (*)(TArgs...);

template<typename TClass, typename TReturn, typename... TArgs, typename... TOther>
constexpr auto FunctionPointer(TReturn (TClass::*)(TArgs...), TOther&&...) -> TReturn (*)(TArgs...);

template<typename TClass, typename TReturn, typename... TArgs, typename... TOther>
constexpr auto FunctionPointer(TReturn (TClass::*)(TArgs...) const, TOther&&...) -> TReturn (*)(TArgs...);

template<typename TClass, typename TType, typename TOther>
constexpr auto FunctionPointer(TType TClass::*, TOther&&...) -> TType (*)();

template<typename... TType>
using FunctionPointer_t = decltype(FunctionPointer(std::declval<TType>()...));


template<typename... TClass, typename TReturn, typename... TArgs>
[[nodiscard]] constexpr auto IndexSequenceFor(TReturn (*)(TArgs...))
{
    return std::index_sequence_for<TClass..., TArgs...>{};
}

template<auto>
struct ConnectArg_t
{
    explicit ConnectArg_t() = default;
};

template<auto TCandidate>
inline constexpr ConnectArg_t<TCandidate> connect_arg{};

template<typename>
class Delegate;

template<typename TReturn, typename... TArgs>
class Delegate<TReturn(TArgs...)>
{

    template<auto TCandidate, std::size_t... TIndex>
    [[nodiscard]] auto wrap(std::index_sequence<TIndex...>) noexcept 
    {
        return [](const void*, TArgs... args) -> TReturn
        {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<TArgs>(args)...);
            return static_cast<TReturn>(std::invoke(TCandidate, std::forward<TypeListElement_t<TIndex, TypeList<TArgs...>>>(std::get<TIndex>(arguments))...));
        };
    }

    template<auto TCandidate, typename TType, std::size_t... TIndex>
    [[nodiscard]] auto wrap(TType&, std::index_sequence<TIndex...>) noexcept
    {
        return [](const void* pPayload, TArgs... args) -> TReturn
        {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<TArgs>(args)...);
            TType* pCurrent = static_cast<TType*>(const_cast<ConstnessAs_t<void, TType> *>(pPayload));
            return static_cast<TReturn>(std::invoke(TCandidate, *pCurrent, std::forward<TypeListElement_t<TIndex, TypeList<TArgs...>>>(std::get<TIndex>(arguments))...));
        };
    }

    template<auto TCandidate, typename TType, std::size_t... TIndex>
    [[nodiscard]] auto wrap(TType*, std::index_sequence<TIndex...>) noexcept
    {
        return [](const void* pPayload, TArgs... args) -> TReturn
        {
            [[maybe_unused]] const auto arguments = std::forward_as_tuple(std::forward<TArgs>...);
            TType* pCurrent = static_cast<TType*>(const_cast<ConstnessAs_t<void, TType> *>(pPayload));
            return static_cast<TReturn>(std::invoke(TCandidate, pCurrent, std::forward<TypeListElement_t<TIndex, TypeList<TArgs...>>>(std::get<TIndex>(arguments))...));
        };
    }

public:
    using FunctionType = TReturn(const void*, TArgs...);
    using Type = TReturn(TArgs...);
    using ResultType = TReturn;

    Delegate() noexcept
        : m_pInstance{nullptr}
        , m_pFunction{nullptr} {}
    
    template<auto TCandidate, typename... TType>
    Delegate(ConnectArg_t<TCandidate>, TType&&... valueOrInstance) noexcept
    {
        connect<TCandidate>(std::forward<TType>(valueOrInstance)...);
    }

    Delegate(FunctionType* pFunction, const void* pPayload = nullptr) noexcept
    {
        connect(pFunction, pPayload);
    }

    template<auto TCandidate>
    void connect() noexcept
    {
        m_pInstance = nullptr;

        if constexpr (std::is_invocable_r_v<TReturn, decltype(TCandidate), TArgs...>)
        {
            m_pFunction = [](const void*, TArgs... args) -> TReturn
            {
                return TReturn(std::invoke(TCandidate, std::forward<TArgs>(args)...));
            };
        } 
        else if constexpr (std::is_member_pointer_v<decltype(TCandidate)>)
        {
            m_pFunction = wrap<TCandidate>(IndexSequenceFor<TypeListElement_t<0, TypeList<TArgs...>>>(FunctionPointer_t<decltype(TCandidate)>{}));
        } 
        else
        {
            m_pFunction = wrap<TCandidate>(IndexSequenceFor(FunctionPointer_t<decltype(TCandidate)>{}));
        }
    }

    template<auto TCandidate, typename TType>
    void connect(TType& valueOrInstance) noexcept
    {
        m_pInstance = &valueOrInstance;

        if constexpr (std::is_invocable_r_v<TReturn, decltype(TCandidate), TType&, TArgs...>) {
            m_pFunction = [](const void* pPayload, TArgs... args) -> TReturn
            {
                TType* pCurrent = static_cast<TType*>(const_cast<ConstnessAs_t<void, TType> *>(pPayload));
                return TReturn(std::invoke(TCandidate, *pCurrent, std::forward<TArgs>(args)...));
            };
        }
        else
        {
            m_pFunction = wrap<TCandidate>(valueOrInstance, IndexSequenceFor(FunctionPointer_t<decltype(TCandidate), TType>{}));
        }
    }

    template<auto TCandidate, typename TType>
    void connect(TType* pValueOrInstance) noexcept
    {
        m_pInstance = pValueOrInstance;

        if constexpr (std::is_invocable_r_v<TReturn, decltype(TCandidate), TType*, TArgs...>)
        {
            m_pFunction = [](const void* pPayload, TArgs... args) -> TReturn
            {
                TType* pCurrent = static_cast<TType*>(const_cast<ConstnessAs_t<void, TType> *>(pPayload));
                return TReturn(std::invoke(TCandidate, pCurrent, std::forward<TArgs>(args)...));
            };
        }
        else
        {
            m_pFunction = wrap<TCandidate>(pValueOrInstance, IndexSequenceFor(FunctionPointer_t<decltype(TCandidate), TType>{}));
        }
    }

    void connect(FunctionType* pFunction, const void* pPayload = nullptr) noexcept
    {
        assert(pFunction != nullptr && "Uninitialized function pointer");
        m_pInstance = pPayload;
        m_pFunction = pFunction;
    }

    void reset() noexcept
    {
        m_pInstance = nullptr;
        m_pFunction = nullptr;
    }

    [[nodiscard]] FunctionType* target() const noexcept
    {
        return m_pFunction;
    }

    [[nodiscard]] const void* data() const noexcept
    {
        return m_pInstance;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return !(m_pFunction == nullptr);
    } 

    TReturn operator()(TArgs... args) const
    {
        assert(static_cast<bool>(*this) && "Uninitialized delegate");
        return m_pFunction(m_pInstance, std::forward<TArgs>(args)...);
    }

    [[nodiscard]] bool operator==(const Delegate<TReturn(TArgs...)>& other) const noexcept
    {
        return m_pFunction == other.m_pFunction && m_pInstance == other.m_pInstance; 
    }

private:
    const void* m_pInstance;
    FunctionType* m_pFunction;
};

template<auto TCandidate>
Delegate(ConnectArg_t<TCandidate>) -> Delegate<std::remove_pointer_t<FunctionPointer_t<decltype(TCandidate)>>>;

template<auto TCandidate, typename TType>
Delegate(ConnectArg_t<TCandidate>, TType&&) -> Delegate<std::remove_pointer_t<FunctionPointer_t<decltype(TCandidate), TType>>>;

template<typename TReturn, typename... TArgs>
Delegate(TReturn (*)(const void*, TArgs...), const void* = nullptr) -> Delegate<TReturn(TArgs...)>;

}