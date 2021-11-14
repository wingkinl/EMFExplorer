#ifndef PROPERTY_TREE_H
#define PROPERTY_TREE_H

#include <vector>
#include <sstream>
#include <iomanip>
#include "DataAccess.h"

struct PropertyNode 
{
	enum NodeType
	{
		NodeTypeBranch,
		NodeTypeText,
		NodeTypeRectInt,
		NodeTypeRectFloat,
		NodeTypeSizeInt,
		NodeTypeColor,
		NodeTypeFont,
		NodeTypePointDataArray,
		NodeTypeMatrix,
		NodeTypeRectData,
		NodeTypeArcData,
		NodeTypeArray,
	};
	CStringW	name;
	CStringW	text;
	std::vector<std::shared_ptr<PropertyNode>> sub;

	PropertyNode(LPCWSTR szName = nullptr, LPCWSTR szText = nullptr)
		: name(szName)
		, text(szText)
	{

	}

	virtual NodeType	GetNodeType() const { return NodeTypeText; }

	inline std::shared_ptr<PropertyNode> AddText(LPCWSTR szName, LPCWSTR szText)
	{
		auto ptr = std::make_shared<PropertyNode>(szName, szText);
		sub.push_back(ptr);
		return ptr;
	}

	template <typename T>
	std::shared_ptr<PropertyNode> AddValue(LPCWSTR szName, T val, bool bHex = false)
	{
		std::wstring str;
		if (bHex)
		{
			std::wstringstream ss;
			ss << L"0x" << std::hex << std::setw(8) << std::setfill(L'0') << val;
			str = ss.str();
		}
		else
			str = std::to_wstring(val);
		return AddText(szName, str.c_str());
	}

	inline std::shared_ptr<PropertyNode> AddBranch(LPCWSTR szName, LPCWSTR szText = nullptr);
};

struct PropertyNodeBranch : public PropertyNode
{
	using PropertyNode::PropertyNode;
	NodeType	GetNodeType() const override { return NodeTypeBranch; }
};

std::shared_ptr<PropertyNode> PropertyNode::AddBranch(LPCWSTR szName, LPCWSTR szText)
{
	auto ptr = std::make_shared<PropertyNodeBranch>(szName, szText);
	sub.push_back(ptr);
	return ptr;
}

struct PropertyNodeRectInt : public PropertyNode
{
	template <typename T>
	PropertyNodeRectInt(LPCWSTR szName, const T& rc)
	{
		name = szName;
		rect.left = rc.left;
		rect.top = rc.top;
		rect.right = rc.right;
		rect.bottom = rc.bottom;
	}
	template <typename T>
	PropertyNodeRectInt(LPCWSTR szName, T l, T t, T r, T b)
	{
		name = szName;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
	}

	NodeType	GetNodeType() const override { return NodeTypeRectInt; }
	RECT	rect;
};

struct PropertyNodeSizeInt : public PropertyNode
{
	template <typename T>
	PropertyNodeSizeInt(LPCWSTR szName, const T& sz)
		: PropertyNodeSizeInt(szName, sz.cx, sz.cy)
	{
	}
	template <typename T>
	PropertyNodeSizeInt(LPCWSTR szName, T cx, T cy)
	{
		name = szName;
		size.cx = cx;
		size.cy = cy;
	}

	NodeType	GetNodeType() const override { return NodeTypeSizeInt; }
	SIZE	size;
};

template <typename T, PropertyNode::NodeType _type>
struct PropertyNodeData : public PropertyNode
{
	PropertyNodeData(LPCWSTR szName, T _data)
		: data(_data)

	{
		name = szName;
	}

	NodeType	GetNodeType() const override { return _type; }
	T	data;
};

struct PropertyNodeArray : public PropertyNode
{
	template <typename T>
	PropertyNodeArray(LPCWSTR szName, const T* _data, size_t _size)
	{
		name = szName;
		SetData(_data, _size);
	}
	template <typename T>
	PropertyNodeArray(LPCWSTR szName, const std::vector<T>& _data)
	{
		SetData(_data);
		name = szName;
	}
	PropertyNodeArray(LPCWSTR szName)
	{
		name = szName;
	}

	template <typename T>
	void SetData(const T* _data, size_t _size)
	{
		data = _data;
		size = _size;
		elem_type = GetElemType<T>();
	}

	template <typename T>
	void SetData(const std::vector<T>& _data)
	{
		data = _data.data();
		size = _data.size();
		elem_type = GetElemType<T>();
	}

	enum ElemType
	{
		ElemTypeUnknown,
		ElemTypeu8t,
		ElemTypeu16t,
		ElemTypeuFloat,
		ElemTypePlusPoint,
		ElemTypePlusPointF,
		ElemTypePlusRect,
		ElemTypePlusRectF,
		ElemTypePlusCharacterRange,
		ElemTypePlusARGB,
	};

	template <typename T>
	ElemType GetElemType(const T* p = nullptr)
	{
		if constexpr (std::is_same_v<T, emfplus::u8t>)
			return ElemTypeu8t;
		else if constexpr (std::is_same_v<T, emfplus::u16t>)
			return ElemTypeu16t;
		else if constexpr (std::is_same_v<T, emfplus::Float>)
			return ElemTypeuFloat;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusPoint>)
			return ElemTypePlusPoint;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusPointF>)
			return ElemTypePlusPointF;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusRect>)
			return ElemTypePlusRect;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusRectF>)
			return ElemTypePlusRectF;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusCharacterRange>)
			return ElemTypePlusCharacterRange;
		else if constexpr (std::is_same_v<T, emfplus::OEmfPlusARGB>)
			return ElemTypePlusARGB;
		else
			static_assert(false, "Unknown array type");
		return ElemTypeUnknown;
	}

	NodeType	GetNodeType() const override { return NodeTypeArray; }
	const void*	data = nullptr;
	size_t		size = 0;
	ElemType	elem_type;
};

struct PropertyNodePlusPointDataArray : public PropertyNodeArray
{
	PropertyNodePlusPointDataArray(LPCWSTR szName, const emfplus::OEmfPlusPointDataArray& refData, bool bRelative = false)
		: PropertyNodeArray(szName)
	{
		if (refData.size() > 0)
		{
			if (!refData.ivals.empty())
				SetData(*refData.ivals);
			else
				SetData(*refData.fvals);
		}
		relative = bRelative;
	}
	NodeType	GetNodeType() const override { return NodeTypePointDataArray; }
	bool	relative;
};

struct PropertyNodePlusRectDataArray : public PropertyNodeArray
{
	PropertyNodePlusRectDataArray(LPCWSTR szName, const emfplus::OEmfPlusRectDataArray& refData)
		: PropertyNodeArray(szName)
	{
		if (refData.size() > 0)
		{
			if (!refData.ivals.empty())
				SetData(*refData.ivals);
			else
				SetData(*refData.fvals);
		}
	}
};

using PropertyNodePlusTransform = PropertyNodeData<const emfplus::OEmfPlusTransformMatrix&, PropertyNode::NodeTypeMatrix>;
using PropertyNodePlusRectF = PropertyNodeData<const emfplus::OEmfPlusRectF&, PropertyNode::NodeTypeRectFloat>;
using PropertyNodeColor = PropertyNodeData<const emfplus::OEmfPlusARGB&, PropertyNode::NodeTypeColor>;
using PropertyNodePlusRectData = PropertyNodeData<const emfplus::OEmfPlusRectData&, PropertyNode::NodeTypeRectData>;
using PropertyNodePlusArcData = PropertyNodeData<const emfplus::OEmfPlusArcData&, PropertyNode::NodeTypeArcData>;

#endif // PROPERTY_TREE_H
