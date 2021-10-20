#ifndef PROPERTY_TREE_H
#define PROPERTY_TREE_H

#include <vector>
#include <sstream>
#include <iomanip>

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
	};
	NodeType	type;
	CStringW	name;
	CStringW	text;
	std::vector<std::shared_ptr<PropertyNode>> sub;

	PropertyNode(LPCWSTR szName = nullptr, LPCWSTR szText = nullptr, NodeType t = NodeTypeBranch)
		: name(szName)
		, text(szText)
		, type(t)
	{

	}

	inline std::shared_ptr<PropertyNode> AddText(LPCWSTR szName, LPCWSTR szText, NodeType type = NodeTypeText)
	{
		auto ptr = std::make_shared<PropertyNode>(szName, szText, type);
		sub.push_back(ptr);
		return ptr;
	}

	template <typename T>
	void AddValue(LPCWSTR szName, T val, NodeType type = NodeTypeText, bool bHex = false)
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
		auto ptr = std::make_shared<PropertyNode>(szName, str.c_str(), type);
		sub.push_back(ptr);
	}
};

struct PropertyNodeRectInt : public PropertyNode
{
	template <typename T>
	PropertyNodeRectInt(LPCWSTR szName, const T& rc)
	{
		type = NodeTypeRectInt;
		name = szName;
		rect.left = rc.left;
		rect.top = rc.top;
		rect.right = rc.right;
		rect.bottom = rc.bottom;
	}
	template <typename T>
	PropertyNodeRectInt(LPCWSTR szName, T l, T t, T r, T b)
	{
		type = NodeTypeRectInt;
		name = szName;
		rect.left = l;
		rect.top = t;
		rect.right = r;
		rect.bottom = b;
	}
	RECT	rect;
};

struct PropertyNodeRectFloat : public PropertyNode
{
	PropertyNodeRectFloat(LPCWSTR szName, const emfplus::OEmfPlusRectF& rc)
	{
		type = NodeTypeRectFloat;
		name = szName;
		rect = rc;
	}
	emfplus::OEmfPlusRectF rect;
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
		type = NodeTypeSizeInt;
		name = szName;
		size.cx = cx;
		size.cy = cy;
	}
	SIZE	size;
};

struct PropertyNodeColor : public PropertyNode
{
	emfplus::OEmfPlusARGB	clr;
};

struct PropertyNodePointDataArray : public PropertyNode 
{
	PropertyNodePointDataArray(LPCWSTR szName, const emfplus::OEmfPlusPointDataArray& refData, bool bRelative = false)
	{
		type = NodeTypePointDataArray;
		name = szName;
		pts = &refData;
		relative = bRelative;
	}
	const emfplus::OEmfPlusPointDataArray*	pts;
	bool									relative;
};

#endif // PROPERTY_TREE_H
