#pragma once

#include <typeinfo>
#include <typeindex>
#include <functional>

template<typename TReturn>
class Delegate;

/// @brief Handles delegates of no parameter functions.
template<typename TReturn>
class Delegate<TReturn()>
{
public:
	/// @brief The type of this delegates function.
	using FunctionType = std::function<TReturn()>;
	
	/// @brief Default constructor
	Delegate() = default;

	/// @brief Constructs a delegate using a normal function.
	/// @param function The free function to bind to the delegate.
	Delegate(const FunctionType& function) noexcept
	{
		m_functions.push_back(function);
	}

	/// @brief Constructs a delegate using a const pointer-to-member function.
	/// @param object The object to bind to when calling the function.
	/// @param method The method to call.
	template<typename TFirstObject, typename TSecondObject>
	Delegate(const TFirstObject& object, TReturn(TSecondObject::* method)() const) noexcept
	{
		m_functions.push_back(FunctionType(std::bind_front(method, const_cast<TFirstObject*>(&object))));
	}

	/// @brief Constructs a delegate using a pointer-to-member function.
	/// @param object The object to bind to when calling the function.
	/// @param method The method to call.
	template<typename TFirstObject, typename TSecondObject>
	Delegate(const TFirstObject& object, TReturn(TSecondObject::* method)()) noexcept
	{
		m_functions.push_back(FunctionType(std::bind_front(method, const_cast<TFirstObject*>(&object))));
	}

	/// @brief Default destructor
	~Delegate() = default;

	/// @brief Executes every function added to the delegate.
	/// @return The value of the last function added.
	TReturn operator()() const
	{
		if (m_functions.size() == 0) return TResult{};

		for (size_t i = 0; i < m_functions.size() - 1; i++)
		{
			m_functions[i]();
		}

		return m_functions.back()();
	}

	Delegate& operator+=(const Delegate& delegate) noexcept
	{
		*this = Delegate::combine(*this, delegate);
		return *this;
	}

	Delegate& operator+=(const FunctionType& function) noexcept
	{
		*this = Delegate::combine(*this, delegate(function));
		return *this;
	}

	static Delegate combine(const std::vector<Delegate>& delegates) noexcept
	{
		Delegate result;
		for (const Delegate& delegate : delegates)
		{
			for (const FunctionType& function : delegate.m_functions)
			{
				result.m_functions.push_back(function);
			}
		}
		return result;
	}

	static Delegate combine(const Delegate& a, const Delegate& b) noexcept
	{
		Delegate result = a;
		for (const FunctionType& function : b.m_functions)
		{
			result.m_functions.push_back(function);
		}
		return result;
	}

	static Delegate remove(const Delegate& source, const Delegate& value) noexcept
	{
		Delegate result = source;
		// Look for functions inside the source and value, compare them and remove ones that exist in both.
		for (const FunctionType& function : value.m_functions)
		{
			if (std::find(result.m_functions.begin(), result.m_functions.end(), function) == result.m_functions.end())
				continue;

			for (auto it = result.m_functions.rbegin(); it != result.m_functions.rend(); it++)
			{
				if (!are_equal(*it, function))
					continue;

				result.m_functions.erase((it + 1).base());
				break;
			}
		}
	}

	bool is_empty() const noexcept
	{
		return m_functions.size() == 0;
	}

	const std::vector<FunctionType>& getFunctions() const
	{
		return m_functions;
	}
	
	TReturn invoke() const
	{
		return operator()();
	}


private:
	void* m_pInstance;

	/// <summary>
	/// The function 
	/// </summary>
	std::vector<FunctionType> m_functions;
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

	void call()
	{
		std::invoke(m_function);
	}

private:


	void* pInstance;

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