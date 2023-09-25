#pragma once

template <typename T>
class blHeirarchy {
public:
    blHeirarchy() = default;
    blHeirarchy(const blHeirarchy& rhs) = default;
    blHeirarchy(blHeirarchy&& rhs) = default;
    blHeirarchy(T data)
        : _data(data)
    {
    }
    blHeirarchy(blHeirarchy& parent, T data)
        : _parent(&parent)
        , _data(data)
    {
    }

    blHeirarchy* findInChildren(T data)
    {
        // First search on only this's children
        for (const blHeirarchy& child : children) {
            if (child._data == data)
                return &child;
        }

        // Second search recursively through each child
        for (const blHeirarchy& child : children) {
            blHeirarchy* found = child.findInChildren(data);
            if (found)
                return found;
        }

        return nullptr;
    }

private:
    blHeirarchy* _parent;
    std::vector<blHeirarchy> _children;
    T _data;
};