#pragma once

#include <functional>


template<typename TReturn, typename...TArgs>
class Delegate;

/// @brief Handles delegates parameter functions.
template<typename TReturn, typename...TArgs>
class Delegate<TReturn(TArgs...)>
{
public:
	/// @brief The type of this delegates function.
	using FunctionType = std::function<TReturn(TArgs...)>;
	
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
	template<typename TObject>
	Delegate(const TObject& object, TReturn(TObject::* method)(TArgs...) const) noexcept
	{
		m_functions.push_back(FunctionType(std::bind_front(method, const_cast<TObject*>(&object))));
	}

	/// @brief Constructs a delegate using a pointer-to-member function.
	/// @param object The object to bind to when calling the function.
	/// @param method The method to call.
	template<typename TObject>
	Delegate(const TObject& object, TReturn(TObject::* method)(TArgs...)) noexcept
	{
		m_functions.push_back(FunctionType(std::bind_front(method, const_cast<TObject*>(&object))));
	}

	/// @brief Default destructor
	~Delegate() = default;

	/// @brief Executes every function added to the delegate.
	/// @return The value of the last function added.
	void operator()(TArgs...args) const
	{
		if (m_functions.size() == 0) return;

		for (size_t i = 0; i < m_functions.size() - 1; i++)
		{
			m_functions[i](args...);
		}

		m_functions.back()(args...);
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
	
	TReturn invoke(TArgs...args) const
	{
		return operator()(args...);
	}


private:

	std::vector<FunctionType> m_functions;
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