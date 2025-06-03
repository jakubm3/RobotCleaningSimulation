#pragma once
#include <iostream>
#include <memory>

class Tile {
public:
    static constexpr size_t INVALID_ID = SIZE_MAX;

protected:
    size_t id;

public:
    Tile(size_t id = 0);
    virtual ~Tile() = default;

    // Virtual copy constructor
    virtual std::unique_ptr<Tile> clone() const = 0;

    // Getters and setters
    size_t getId() const noexcept;
    void setId(size_t newId);

    // Pure virtual methods
    virtual bool isMoveValid() const = 0;
};

