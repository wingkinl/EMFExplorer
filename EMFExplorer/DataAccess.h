#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H

namespace data_access
{
	using byte = uint8_t;

template <typename _Ty>
struct optional_wrapper
{
	using value_type      = _Ty;

	inline operator bool() const { return _enabled; }
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

	inline auto begin() { enable(); return _val.begin(); }
	inline auto begin() const { return _val.begin(); }
	inline auto end() { enable(); return _val.end(); }
	inline auto end() const { return _val.end(); }

	inline auto& operator[](size_t pos) { enable(); return _val[pos]; }
	inline const auto& operator[](size_t pos) const { return _val[pos]; }
protected:
	bool	_enabled = false;
	_Ty		_val;
};

template <typename _Ty>
struct object_wrapper
{
	using value_type      = _Ty;

	inline bool is_attached() const { return _ptr != nullptr; }
	inline void attach(_Ty* ptr) { _ptr = ptr; }

	inline _Ty& get() { return _ptr ? *_ptr : _val; }
	inline const _Ty& get() const { return _ptr ? *_ptr : _val; }

	inline _Ty* operator->() { return &get(); }
	inline const _Ty* operator->() const { return &get(); }

	inline _Ty& operator*() { return get(); }
protected:
	_Ty*	_ptr = nullptr;
	_Ty		_val;
};

template <typename _Ty>
struct vector_wrapper
{
	using value_type      = _Ty;
	using vector_type     = std::vector<_Ty>;

	inline bool is_attached() const { return _ptr != nullptr; }
	inline void attach(_Ty* ptr, size_t size = 0) { _ptr = ptr; _size = size; }

	vector_type& get()
	{
		_ptr = nullptr;
		return _val;
	}

	inline bool empty() const
	{
		if (_ptr)
			return _size == 0;
		return _val.empty();
	}

	inline size_t size() const { return _ptr ? _size : _val.size(); }
	inline void resize(size_t newSize)
	{
		if (_ptr)
			_size = newSize;
		else
			_val.resize(newSize);
	}

	template<typename ValT>
	inline void resize(size_t newSize, ValT newVal)
	{
		if (_ptr)
		{
			_size = newSize;
			std::fill_n(_ptr, _size, newVal);
		}
		else
		{
			_val.resize(newSize, newVal);
		}
	}

	inline void clear()
	{
		if (_ptr)
			_size = 0;
		else
			_val.clear();
	}

	inline auto begin() { return std::begin(_ptr ? _ptr : _val); }
	inline auto begin() const { return std::begin(_ptr ? _ptr : _val); }
	inline auto end() { return std::end(_ptr ? (_ptr+_size) : _val); }
	inline auto end() const { return std::end(_ptr ? (_ptr+_size) : _val); }

	inline auto& operator[](size_t pos) { return _ptr ? _ptr[pos] : _val[pos]; }
	inline const auto& operator[](size_t pos) const { return _ptr ? _ptr[pos] : _val[pos]; }

	inline auto data() { return _ptr ? _ptr : _val.data(); }
	inline auto data() const { return _ptr ? _ptr : _val.data(); }
protected:
	_Ty*				_ptr = nullptr;
	size_t				_size = 0;
	std::vector<_Ty>	_val;
};

template <typename T> struct is_optional_wrapper : public std::false_type {};

template <typename T> struct is_optional_wrapper<optional_wrapper<T>> : public std::true_type {};

template <typename T> struct is_object_wrapper : public std::false_type {};

template <typename T> struct is_object_wrapper<object_wrapper<T>> : public std::true_type {};

template <class _Ty>
constexpr bool is_optional_wrapper_v = is_optional_wrapper<_Ty>::value;

template<typename T> struct is_vector_wrapper : public std::false_type {};

template<typename T> struct is_vector_wrapper<vector_wrapper<T>> : public std::true_type {};

template <class _Ty>
constexpr bool is_vector_wrapper_v = is_vector_wrapper<_Ty>::value;

using memory_wrapper = vector_wrapper<byte>;

class DataReader
{
public:
	DataReader(const byte* p, size_t nSize, bool bAttachMemMode = false)
	{
		m_p = p;
		m_pEnd = m_p + nSize;
		m_pCur = p;
		m_bAttachMemMode = bAttachMemMode;
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
	void ReadBytes(object_wrapper<ValT>* pData, size_t nSize)
	{
		ASSERT(m_pCur + nSize <= m_pEnd);
		ASSERT(nSize == sizeof(object_wrapper<ValT>::value_type));
		if (m_bAttachMemMode)
		{
			pData->attach(m_pCur);
		}
		else
		{
			memcpy(pData, m_pCur, nSize);
		}
		m_pCur += nSize;
	}

	template <typename ValT>
	void ReadBytes(optional_wrapper<ValT>* pData, size_t nSize)
	{
		const size_t nValSize = sizeof(optional_wrapper<ValT>::value_type);
		ASSERT(nSize >= nValSize);
		ASSERT(m_pCur + nValSize <= m_pEnd);
		ReadBytes(&pData->get(), nValSize);
	}

	template <typename ValT>
	void ReadArray(vector_wrapper<ValT>& arr, size_t nCount)
	{
		if (m_bAttachMemMode)
		{
			arr.attach( (typename vector_wrapper<ValT>::value_type*)m_pCur );
			arr.resize(nCount);
			auto nSize = sizeof(vector_wrapper<ValT>::value_type) * nCount;
			ASSERT(m_pCur + nSize <= m_pEnd);
			m_pCur += nSize;
		}
		else
		{
			ReadArray(arr.get(), nCount);
		}
	}


	template <typename ValT>
	inline void ReadArray(optional_wrapper<ValT>& arr, size_t nCount)
	{
		ReadArray(arr.get(), nCount);
	}

	// nElemSize must be either 0 or smaller than sizeof(ValT)
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
	const byte* m_p;
	const byte* m_pEnd;
	const byte* m_pCur;
	bool		m_bAttachMemMode = false;
};

enum SizeType { UNKNOWN_SIZE = SIZE_MAX };

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