#include "node.hpp"

#include <iostream>
#include <functional>
#include <unordered_map>
#include <deque>

#include "datatype.hpp"

struct TypeUnknownException : public std::exception
{
    const char *what() const throw()
    {
        return "Type Unknown!";
    }
};

template <class T, class F>
inline std::pair<const std::type_index, std::function<void(std::any const &, std::ostream &s)>>
toStringVisitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::any const &a, std::ostream &s) {
            g(std::any_cast<T const &>(a), s);
        }};
}

static std::unordered_map<
    std::type_index, std::function<void(std::any const &, std::ostream &s)>>
    streamVisitor{
        toStringVisitor<std::string>([](std::string x, std::ostream &s) {
            s << x;
        }),
        toStringVisitor<int>([](int x, std::ostream &s) { s << x;; }),
        toStringVisitor<double>([](double x, std::ostream &s) { s << x; }),
    };

std::ostream &operator<<(std::ostream &out, const Node &node)
{
    auto &value = node.value;
    auto &children = node.children;

    out << "{";

    if (const auto it = streamVisitor.find(std::type_index(value.type()));
        it != streamVisitor.cend())
    {
        it->second(value, out);
    }
    else
    {
        throw TypeUnknownException();
    }

    if (!node.children.empty())
    {
        out << ",{";
        auto it = node.children.begin();

        out << (*it);
        it++;
        while (it != children.end())
        {
            out << ",";
            out << (*it);
            it++;
        }
        out << "}";
    }
    out << "}";
    return out;
}

template <class T, class F>
inline std::pair<const std::type_index, std::function<void(std::any const &, std::vector<std::byte> &data)>>
toBinaryVisitor(F const &f)
{
    return {
        std::type_index(typeid(T)),
        [g = f](std::any const &a, std::vector<std::byte> &data) {
            g(std::any_cast<T const &>(a), data);
        }};
}


template <class T>
void serilize(T const &t, std::vector<std::byte> &data)
{
    for (size_t i = 0; i < sizeof(T); i++)
        data.push_back(static_cast<std::byte>(*(reinterpret_cast<std::byte *>(&(const_cast<T &>(t))) + i)));
}

static std::unordered_map<
    std::type_index, std::function<void(std::any const &, std::vector<std::byte> &data)>>
    binaryVisitor{
        toBinaryVisitor<std::string>([](std::string x, std::vector<std::byte> &data) {
            serilize<int>(DataType::STRING, data);
            serilize<size_t>(x.length(), data);
            for (auto c : x)
            {
                serilize<char>(c, data);
            }
        }),
        toBinaryVisitor<int>([](int x, std::vector<std::byte> &data) {
            serilize<int>(int(DataType::INT),data); 
            serilize<int>(x,data); }),
        toBinaryVisitor<double>([](double x, std::vector<std::byte> &data) { 
            serilize<int>(DataType::DOUBLE,data);
            serilize<double>(x,data); }),
    };

void Node::serialize(std::vector<std::byte> &data)
{
    if (const auto it = binaryVisitor.find(std::type_index(value.type()));
        it != binaryVisitor.cend())
    {
        it->second(value, data);
    }

    if (!children.empty())
    {
        serilize<int>(LIST, data);
        serilize<size_t>(children.size(), data);

        auto it = children.begin();
        while (it != children.end())
        {
            it->serialize(data);
            it++;
        }
    }

    return;
}

template <class T, typename F>
T deserilize(F &iterator)
{
    T value;
    value = *(reinterpret_cast<T *>(&(*(iterator))));
    iterator = iterator + sizeof(T);
    return value;
}

template <typename F>
void deserializeHelper(Node *n, Node &root, size_t size, std::vector<std::byte>::iterator &it, std::vector<std::byte>::iterator end)
{
    for (int i = 0; (size) && (it != end); i++)
    {
        std::any v;
        DataType t = deserilize<DataType, std::vector<std::byte>::iterator>(it);
        switch (t)
        {
        case DataType::DOUBLE:
            v = deserilize<double, std::vector<std::byte>::iterator>(it);
            break;

        case DataType::INT:
            v = deserilize<int, std::vector<std::byte>::iterator>(it);
            break;
        case DataType::STRING:
        {
            size_t sz = deserilize<size_t, std::vector<std::byte>::iterator>(it);
            std::string str;
            while (sz--)
            {
                str.push_back(deserilize<char, std::vector<std::byte>::iterator>(it));
            }
            v = str;
        }
        break;

        case DataType::LIST:
        {
            size_t sz = deserilize<size_t, std::vector<std::byte>::iterator>(it);

            n->children.emplace_front();
            deserializeHelper<std::vector<std::byte>::iterator>(&(n->children.back()), *n, sz, it, end);
        }
        break;

        default:
            TypeUnknownException();
            break;
        }

        if (v.has_value())
        {
            if (n->value.has_value())
            {
                root.children.emplace_back();

                n = &root.children.back();
                size--;
            }
            n->value = v;
        }
    }
}

void Node::deserialize(std::vector<std::byte> &data)
{
    auto it = data.begin();
    auto end = data.end();
    ::deserializeHelper<std::vector<std::byte>::iterator>(this, *this, 1, it, end);
}
