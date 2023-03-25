#pragma once

#include <Delegate/Delegate.hpp>

namespace bl
{

	template<typename...TArgs>
	class Delegate;

	template<typename TReturn, typename...TArgs>
	class Delegate<TReturn(TArgs...)>
	{
		using TFunctionStub = TReturn(*)(void*, TArgs&&...);

		Delegate  
	public:

		Delegate();

		~Delegate();



	private:
		std::vector<axl::Delegate> m_delegates;
	};
}