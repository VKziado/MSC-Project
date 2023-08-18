#pragma once
#include <cstdint>

using IdType = uint64_t;

// template <typename T>
class IdObject {
    IdType _id{};

    IdType generateId() {
        static IdType i = 0;
        return i++;
    }

public:
    IdObject() { _id = generateId(); }
    // move
    IdObject(IdObject &&other) { _id = other._id; }
    // copy
    IdObject(IdObject const &other) { _id = generateId(); }

    // move
    IdObject &operator=(IdObject &&other) {
        _id = other._id;
        return *this;
    }
    // copy
    IdObject &operator=(IdObject const &other) {
        _id = generateId();
        return *this;
    }

    constexpr IdType getId() const { return _id; }
};