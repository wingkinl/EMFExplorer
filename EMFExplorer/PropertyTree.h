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
		NodeTypeRects,
		NodeTypeRectData,
		NodeTypeArcData,
		NodeTypeUnicodeArray,
		NodeTypePointFArray,
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
	NodeType	GetNodeType() const { return NodeTypeBranch; }
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

	NodeType	GetNodeType() const { return NodeTypeRectInt; }
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

	NodeType	GetNodeType() const { return NodeTypeSizeInt; }
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

	NodeType	GetNodeType() const { return _type; }
	T	data;
};

using PropertyNodePointDataArrayBase = PropertyNodeData<const emfplus::OEmfPlusPointDataArray&, PropertyNode::NodeTypePointDataArray>;

struct PropertyNodePlusPointDataArray : public PropertyNodePointDataArrayBase
{
	PropertyNodePlusPointDataArray(LPCWSTR szName, const emfplus::OEmfPlusPointDataArray& refData, bool bRelative = false)
		: PropertyNodePointDataArrayBase(szName, refData)
	{
		relative = bRelative;
	}

	bool	relative;
};

using PlusUnicodeArrayWrapper = data_access::vector_wrapper<emfplus::u16t>;
using PlusPointArrayWrapper = data_access::vector_wrapper<emfplus::OEmfPlusPoint>;
using PlusPointFArrayWrapper = data_access::vector_wrapper<emfplus::OEmfPlusPointF>;

using PropertyNodePlusTransform = PropertyNodeData<const emfplus::OEmfPlusTransformMatrix&, PropertyNode::NodeTypeMatrix>;
using PropertyNodePlusRectF = PropertyNodeData<const emfplus::OEmfPlusRectF&, PropertyNode::NodeTypeRectFloat>;
using PropertyNodeColor = PropertyNodeData<const emfplus::OEmfPlusARGB&, PropertyNode::NodeTypeColor>;
using PropertyNodePlusRectDataArray = PropertyNodeData<const emfplus::OEmfPlusRectDataArray&, PropertyNode::NodeTypeRects>;
using PropertyNodePlusRectData = PropertyNodeData<const emfplus::OEmfPlusRectData&, PropertyNode::NodeTypeRectData>;
using PropertyNodePlusArcData = PropertyNodeData<const emfplus::OEmfPlusArcData&, PropertyNode::NodeTypeArcData>;
using PropertyNodePlusUnicodeArrayWrapper = PropertyNodeData<const PlusUnicodeArrayWrapper&, PropertyNode::NodeTypeUnicodeArray>;
using PropertyNodePlusPointFArrayWrapper = PropertyNodeData<const PlusPointFArrayWrapper&, PropertyNode::NodeTypePointFArray>;

#endif // PROPERTY_TREE_H
