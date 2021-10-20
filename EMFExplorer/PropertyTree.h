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
		NodeTypeSizeInt,
		NodeTypeColor,
		NodeTypeFont,
	};
	NodeType	type;
	CStringW	name;
	CStringW	text;
	std::vector<std::unique_ptr<PropertyNode>> sub;

	inline PropertyNode* AddText(LPCWSTR szName, LPCWSTR szText, NodeType type = NodeTypeText)
	{
		auto pNode = new PropertyNode{ type, szName, szText };
		return Add(pNode);
	}
	template <typename T>
	PropertyNode* AddValue(LPCWSTR szName, T val, NodeType type = NodeTypeText, bool bHex = false)
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
		auto pNode = new PropertyNode{ type, szName, str.c_str() };
		return Add(pNode);
	}
	inline PropertyNode* Add(PropertyNode* pNode)
	{
		std::unique_ptr<PropertyNode> node(pNode);
		sub.push_back(std::move(node));
		return pNode;
	}
};

struct PropertyNodeRectInt : public PropertyNode
{
	template <typename T>
	PropertyNodeRectInt(LPCWSTR szName, const T& rect)
	{
		type = NodeTypeRectInt;
		name = szName;
		this->rect.left = rect.left;
		this->rect.top = rect.top;
		this->rect.right = rect.right;
		this->rect.bottom = rect.bottom;
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

struct PropertyNodeSizeInt : public PropertyNode
{
	SIZE	size;
};

struct PropertyNodeColor : public PropertyNode
{
	Gdiplus::Color	clr;
};

#endif // PROPERTY_TREE_H
