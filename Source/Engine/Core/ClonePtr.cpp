#include "CopyPtr.h"

namespace bl {

CopyPtr::CopyPtr()
{

}

CopyPtr::CopyPtr(T* ptr)
{

}

CopyPtr::CopyPtr(const CopyPtr&)
{

}

CopyPtr::CopyPtr(CopyPtr&&)
{

}

CopyPtr::~CopyPtr()
{

}


CopyPtr& CopyPtr::operator=(const CopyPtr& other)
{

}

CopyPtr& CopyPtr::operator=(CopyPtr&& other)
{

}


T* Get();
const T* Get() const;
T* Release();

}