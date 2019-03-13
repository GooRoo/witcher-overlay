#pragma once

#include "Windows.h"
#include <detours.h>

#include <type_traits>

enum class CallConvention{
	stdcall_t, cdecl_t
};

template<CallConvention cc, typename retn, typename convention, typename ...args> struct convention;

template<typename retn, typename ...args>
struct convention<CallConvention::stdcall_t, retn, args...> {
	typedef retn(__stdcall *type)(args...);
};

template<typename retn, typename ...args>
struct convention<CallConvention::cdecl_t, retn, args...> {
	typedef retn(__cdecl *type)(args...);
};


template<CallConvention cc, typename retn, typename ...args> class Hook
{
	using type = typename convention<cc, retn, args...>::type;

	type _target;
	type _orig;
	type _detour;

	bool _isApplied;
public:
	Hook() : _isApplied(false), _orig(0), _detour(0) {}

	template<typename T>
	Hook(T pFunc, type detour) : apply<T>(pFunc, detour) {}

	~Hook()
	{
		remove();
	}

	template<typename T>
	void apply(T pFunc, type detour)
	{
		_target = (type)pFunc;
		_detour = detour;

		PVOID *_ = nullptr, *__ = nullptr;

		DetourTransactionBegin();
		DetourAttachEx(&(PVOID&)_target, (PVOID)_detour, &(PDETOUR_TRAMPOLINE&)_orig, _, __);
		_isApplied = true;
		DetourTransactionCommit();
	}

	template <typename T>
	void applyEx(T mod, T func, type detour)
	{
		auto pFunc = GetProcAddress(GetModuleHandle(TEXT(mod)), func);
		apply(pFunc, detour);
	}

	bool remove()
	{
		if (!_isApplied)
			return false;

		DetourTransactionBegin();
		DetourDetach(&(PVOID&)_target, (PVOID)_detour);
		_isApplied = false;
		DetourTransactionCommit();
		return true;
	}

	retn callOrig(args... p)
	{
		return _orig(p...);
	}

	const bool isApplied() const {
		return _isApplied;
	}
};
