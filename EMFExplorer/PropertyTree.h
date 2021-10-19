#ifndef PROPERTY_TREE_H
#define PROPERTY_TREE_H

#include <vector>

struct PropertyNode 
{
	enum NodeType
	{
		NodeTypeBranch,
		NodeTypeText,
		NodeTypeColor,
		NodeTypeFont,
	};
	NodeType	type;
	CStringW	name;
	CStringW	text;
	std::vector<std::unique_ptr<PropertyNode>> sub;

	inline void Add(LPCWSTR szName, LPCWSTR szText, NodeType type = NodeTypeText)
	{
		std::unique_ptr<PropertyNode> node(new PropertyNode{type, szName, szText});
		sub.push_back(std::move(node));
	}
};

struct ColorPropertyNode : public PropertyNode
{
	Gdiplus::Color	clr;
};

#endif // PROPERTY_TREE_H
