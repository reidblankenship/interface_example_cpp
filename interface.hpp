#ifndef INTERFACE_HPP
#define INTERFACE_HPP
#include <iostream>
#include <numbers>

// Shape implementations no longer need to know where they
// should be in the VTable.
//
// They only need to implement the area(...) and draw(...) functions.
struct Square {
    float w;

    static double area(const Square& square) { return square.w * square.w; }

    static void draw(const Square& square) {
        std::cout << "Square(" << square.w << ")\n";
    }
};

struct Circle {
    float r;

    static double area(const Circle& circle) {
        return circle.r * std::numbers::pi * std::numbers::pi;
    }

    static void draw(const Circle& circle) {
        std::cout << "Circle(" << circle.r << ")\n";
    }
};

using AreaFn = double (*)(const void*);
using DrawFn = void (*)(const void*);

template <typename AreaFnType, typename DrawFnType> struct TShapeVTable {
    AreaFnType area;
    DrawFnType draw;
};

struct ShapeVTable {
    AreaFn area;
    DrawFn draw;
};

template <typename T> struct ShapeTable {
    using AsNonPointerType = std::remove_pointer_t<T>;
    static inline auto area = (AreaFn)&AsNonPointerType::area;
    static inline auto draw = (DrawFn)&AsNonPointerType::draw;
    // clang-format off
    static inline ShapeVTable shape_vtable{
        .area = area,
        .draw = draw,
    };
    // clang-format on
};

struct Shape {
    const void* data;
    const ShapeVTable& otable;

    template <typename T>
    Shape(const T* ptr)
        : data((void*)ptr), otable(ShapeTable<T>::shape_vtable) {}

    [[gnu::always_inline]] [[clang::always_inline]] [[nodiscard]]
    constexpr static double area(auto* shape) {
        return ShapeTable<decltype(shape)>::area((void*)shape);
    }

    [[gnu::always_inline]] [[clang::always_inline]]
    constexpr static void draw(auto* shape) {
        ShapeTable<decltype(shape)>::draw((void*)shape);
    }

    [[gnu::always_inline]] [[clang::always_inline]] [[nodiscard]]
    constexpr double area() const {
        return otable.area(data);
    }

    [[gnu::always_inline]] [[clang::always_inline]]
    constexpr void draw() const {
        otable.draw(data);
    }
};

struct Rectangle {
    float w, h;

    // Without these methods, we get diagnostics telling us what's wrong.
    // Look in main(), where I make a Shape out of a Rectangle.
    static double area(const Rectangle& rect) { return rect.w * rect.w; }

    static void draw(const Rectangle& rectangle) {
        std::cout << "Rectangle(" << rectangle.w << ", " << rectangle.h
                  << ")\n";
    }
};

#endif // !INTERFACE_HPP
