#include <SFML/Graphics.hpp>

using namespace sf;

CircleShape drawCircle(int x, int y){
    CircleShape Circle(3);
                Circle.setOutlineThickness(1);
                Circle.setOutlineColor(Color::Red);
                Circle.setPosition(x, y);
    return Circle;
}

RectangleShape drawLineHorizontal(int x, int y, int opacity){
    RectangleShape  line(Vector2f(74, 1));
                    line.setPosition(x, y);
                    line.setFillColor(Color::Black);
    if (opacity)    line.setFillColor(Color(0, 0, 0, 150));
    return line;
}

RectangleShape drawLineVertical(int x, int y, int opacity){
    RectangleShape  line(Vector2f(1, 74));
                    line.setPosition(x, y);
                    line.setFillColor(Color::Black);
    if (opacity)    line.setFillColor(Color(0, 0, 0, 150));
    return line;
}

