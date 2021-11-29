#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H

#include <iostream>
#include <vector>

namespace data_access
{
	using byte = uint8_t;

template <typename _Ty>
struct optional_wrapper
{
	using value_type      = _Ty;

	inline bool is_enabled() const { return _enabled; }
	inline void enable() { _enabled = true; }
	inline _Ty& get() { enable(); return _val; }
	inline const _Ty& get() const { ASSERT(_enabled); return _val; }

	inline _Ty* operator->() { enable(); return &_val; }
	inline const _Ty* operator->() const { ASSERT(_enabled); return &_val; }

	inline _Ty& operator*() { enable(); return _val; }
	inline const _Ty& operator*() const { ASSERT(_enabled); return _val; }

	inline operator _Ty& () { return get(); }
	inline operator const _Ty& () { return get(); }

	inline bool empty() const { return _val.empty(); }

	inline size_t size() const { return _val.size(); }
	inline void resize(size_t newSize) { enable(); _val.resize(newSize); }

	template<typename ValT>
	inline void resize(size_t newSize, ValT newVal)
	{
		enable();
		_val.resize(newSize, newVal);
	}

	inline void clear()
	{
		_enabled = false;
		_val.clear();
	}

	inline void reset()
	{
		_enabled = false;
		_val.reset();
	}

	auto& operator=(const _Ty& other)
	{
		enable();
		_val = other;
		return *this;
	}

	inline auto begin() { enable(); return _val.begin(); }
	inline auto begin() const { return _val.begin(); }
	inline auto end() { enable(); return _val.end(); }
	inline auto end() const { return _val.end(); }

	inline auto& operator[](size_t pos) { enable(); return _val[pos]; }
	inline const auto& operator[](size_t pos) const { return _val[pos]; }
protected:
	_Ty		_val;
	bool	_enabled = false;
};

template <typename _Ty>
inline std::ostream& operator<<(std::ostream& out, const optional_wrapper<_Ty>& v)
{
	out << v.get();
	return out;
}

template <typename _Ty>
inline std::istream& operator>>(std::istream& in, optional_wrapper<_Ty>& v)
{
	in >> v.get();
	return in;
}

template <class _Ty1, class _Ty2>
constexpr bool operator==(const optional_wrapper<_Ty1>& _Left, const optional_wrapper<_Ty2>& _Right) {
	const bool _Left_is_enabled = _Left.is_enabled();
	return _Left_is_enabled == _Right.is_enabled() && (!_Left_is_enabled || *_Left == *_Right);
}

template <class _Ty1, class _Ty2>
constexpr bool operator!=(const optional_wrapper<_Ty1>& _Left, const optional_wrapper<_Ty2>& _Right) {
	const bool _Left_is_enabled = _Left.is_enabled();
	return _Left_is_enabled != _Right.is_enabled() || (_Left_is_enabled && *_Left != *_Right);
}

template <typename T> struct is_optional_wrapper : public std::false_type {};

template <typename T> struct is_optional_wrapper<optional_wrapper<T>> : public std::true_type {};

template <class _Ty>
constexpr bool is_optional_wrapper_v = is_optional_wrapper<_Ty>::value;

template <typename _Ty>
struct array_wrapper
{
	array_wrapper(_Ty* _data = nullptr, size_t _size = 0)
	{
		data = _data;
		size = _size;
	}
	array_wrapper(const std::vector<_Ty>& vec)
	{
		data = vec.data();
		size = vec.size();
	}
	_Ty*	data;
	size_t	size;
};

template <typename T> struct is_vector: public std::false_type {};

template <typename T> struct is_vector<std::vector<T>> : public std::true_type {};

template <typename T> struct is_array_wrapper : public std::false_type {};

template <typename T> struct is_array_wrapper<array_wrapper<T>> : public std::true_type {};

class DataReader
{
public:
	DataReader(byte* p, size_t nSize)
	{
		m_p = p;
		m_pEnd = m_p + nSize;
		m_pCur = p;
	}
public:
	inline const byte* GetPos() const
	{
		return m_pCur;
	}

	void ReadBytes(void* pData, size_t nSize)
	{
		ASSERT(m_pCur + nSize <= m_pEnd);
		memcpy(pData, m_pCur, nSize);
		m_pCur += nSize;
	}

	template <typename ValT, typename std::enable_if_t<std::is_trivial_v<ValT>>* = nullptr>
	void ReadBytes(optional_wrapper<ValT>* pData, size_t nSize)
	{
		const size_t nValSize = sizeof(optional_wrapper<ValT>::value_type);
		if (nValSize > nSize)
		{
			ASSERT(0);
			return;
		}
		ASSERT(m_pCur + nValSize <= m_pEnd);
		ReadBytes(&pData->get(), nValSize);
	}

	template <typename ValT>
	inline void ReadArray(optional_wrapper<ValT>& arr, size_t nCount)
	{
		ReadArray(arr.get(), nCount);
	}

	template <typename ValT, std::enable_if_t<std::is_trivial_v<ValT>, bool> = true>
	void ReadArray(std::vector<ValT>& arr, size_t nCount)
	{
		arr.resize(nCount);
		auto nSize = sizeof(ValT) * nCount;
		ASSERT(m_pCur + nSize <= m_pEnd);
		memcpy((void*)arr.data(), m_pCur, nSize);
		m_pCur += nSize;
	}

	void Skip(size_t nSize)
	{
		m_pCur += nSize;
	}
protected:
	byte*	m_p;
	byte*	m_pEnd;
	byte*	m_pCur;
};

enum SizeType : size_t { UNKNOWN_SIZE = SIZE_MAX };

class ReaderChecker
{
public:
	ReaderChecker(DataReader& reader, size_t nExpectedSize)
		: m_reader(reader)
	{
		m_pStart = reader.GetPos();
		m_nExpectedSize = nExpectedSize;
	}
	~ReaderChecker()
	{
#ifdef _DEBUG
		auto nReadSize = GetReadSize();
#endif // _DEBUG
		ASSERT(UNKNOWN_SIZE == m_nExpectedSize || m_nExpectedSize == nReadSize);
	}
public:
	inline size_t GetReadSize() const
	{
		return  (size_t)(m_reader.GetPos() - m_pStart);
	}

	inline size_t GetLeftoverSize() const
	{
		if (UNKNOWN_SIZE == m_nExpectedSize)
			return UNKNOWN_SIZE;
		return m_nExpectedSize - GetReadSize();
	}
	void SkipAlignmentPadding()
	{
		auto nAlignmentPadding = GetReadSize() % 4;
		if (nAlignmentPadding)
			m_reader.Skip(4 - nAlignmentPadding);
	}
#ifdef _DEBUG
	bool CheckLeftoverSize(size_t nExpectedSize) const
	{
		if (UNKNOWN_SIZE == nExpectedSize)
			return true;
		size_t nLeftover = GetLeftoverSize();
		if (UNKNOWN_SIZE == nLeftover)
			return true;
		return nLeftover == nExpectedSize;
	}
#endif // _DEBUG
protected:
	DataReader& m_reader;
	const byte* m_pStart;
	size_t		m_nExpectedSize;
};

}

#endif // DATA_ACCESS_H