#pragma once
#include <iostream>
#include <memory>

class Tile {
public:
    static constexpr size_t INVALID_ID = static_cast<size_t>(-1);

protected:
    size_t id; // pozycja w kolekcji kafelków w room

public:
    Tile(size_t id = 0);
    virtual ~Tile() = default;

    // Virtual copy constructor
    virtual std::unique_ptr<Tile> clone() const = 0;

    size_t getId() const noexcept;
    void setId(size_t newId);

    // wirtualne które klasy pochodne muszą zaimplementować
    virtual bool isMoveValid() const = 0;

};

