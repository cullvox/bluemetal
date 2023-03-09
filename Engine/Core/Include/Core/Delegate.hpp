#pragma once

#include <typeinfo>
#include <typeindex>
#include <functional>


template<typename...TArgs>
class SingleDelegate
{
public:
	using FunctionType = void(TArgs...);
	
	template<typename TClass>
	using FunctionMemberType = void(TClass::*)(TArgs...);

	
	/// @brief Default constructor
	SingleDelegate()
		: m_function()
	{
	}

	/// @brief Default destructor
	~SingleDelegate()
	{
	}

	/// @brief Binds a free function to the delegate.
	/// @param freeFunction Function to use when invoking the delegate.
	void bind(FunctionType freeFunction)
	{
		m_function = freeFunction;
	}

	template<class TClass>
	void bind(TClass& instance, FunctionMemberType<TClass> function)
	{
		auto value = std::bind(instance, function);
	}

	void invoke()
	{
		m_function();
	}

	void invoke(TArgs...args)
	{
		m_function(args...);
	}
	
private:
	/// <summary>
	/// The function 
	/// </summary>
	std::function<void(TArgs...)> m_function;
};

template<>
class SingleDelegate<void>
{
public:
	using FunctionType = void(void);

	template<typename TClass>
	using FunctionMemberType = void(TClass::*)(void);

	/// @brief Default constructor
	SingleDelegate()
		: m_function()
	{
	}

	/// @brief Default destructor
	~SingleDelegate()
	{
	}

	/// @brief Binds a free function to the delegate.
	/// @param freeFunction Function to use when invoking the delegate.
	void bind(FunctionType freeFunction)
	{
		m_function = freeFunction;
	}

	template<class TClass>
	void bind(TClass& instance, FunctionMemberType<TClass> function)
	{
		m_function = std::bind(function, instance);
	}

	void invoke()
	{
		m_function();
	}

private:
	/// <summary>
	/// The function 
	/// </summary>
	std::function<void(void)> m_function;
};

/* What I want
* 
* Delegate<int, int> myDelegate{};
* 
* myDelegate.bind(&someClass, &SomeClass::add); // Member function pointer
* myDelegate.bind(add); // Normal function pointer
* 
* myDelegate.call(a, b);
* 
* MulticastDelegate
* 
*/