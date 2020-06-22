#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <list>
#include <vector>
#include <any>
#include <ostream>
#include <typeindex>
#include <cstddef>

struct Node;
struct Node 
{
    Node() = default;
    std::any value;
    std::list<Node> children;

    void deserialize (std::vector<std::byte>&  data);
    void serialize (std::vector<std::byte>&  data);
};

std::ostream& operator<< (std::ostream& out,const Node&) ;

#endif 