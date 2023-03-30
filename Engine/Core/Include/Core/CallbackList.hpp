#pragma once

#include <algorithm>
#include <functional>
#include <list>

template<typename TReturn, typename...TArgs>
class CallbackList;

/** \brief Custom doubly linked list containing std::function's with a specific signature. */
template<typename TReturn, typename...TArgs>
class CallbackList<TReturn(TArgs...)>
{

	using _Callback = std::function<TReturn(TArgs...)>;

	struct _Data
	{
		_Callback cb;
	};

	using _Handle = std::list<_Data>::iterator;

	std::list<_Data> _callbacks;

public:
	
	using Callback = _Callback;
	using Handle = _Handle;

	CallbackList() = default;
	~CallbackList() = default;

	void operator()(TArgs...args) const
	{
		for (const auto& d : _callbacks)
			d.cb(std::forward<TArgs>(args)...);
	}
	
	TReturn operator()(Handle h, TArgs... args) const
	{
		return (*h).cb(args);
	}

	Handle append(const Callback& func) noexcept
	{
		const _Data d{ func };
		_callbacks.emplace_back(d);
		auto end = _callbacks.end();
		--end;
		return end;
	}

	/** \brief Removes a callback from the list, must be a valid handle. */
	void remove(Handle handle) noexcept
	{
		_callbacks.erase(handle);
	}

	/** \brief Clears all callbacks, invalidates all handles. */
	void clear() noexcept
	{
		_callbacks.clear();
	}

	size_t size() const noexcept
	{
		return _callbacks.size();
	}
};