#pragma once

class MapNode{
public:
	int x, y;
	int g, h;
	MapNode* parent;

	MapNode(int x,int y, int g,int h, MapNode* parent) : 
		x(x),y(y), g(g),h(h), parent(parent)
	{}
};