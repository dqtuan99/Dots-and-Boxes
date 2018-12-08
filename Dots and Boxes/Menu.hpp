#include <SFML/Graphics.hpp>
#define number_of_item 3

using namespace sf;

struct Menu{
	Menu(float width, float height);

	void draw(RenderWindow &window);
	Font font;
	Text menu[number_of_item];

};

Menu::Menu(float width, float height){
	font.loadFromFile("arial.ttf");

	menu[0].setFont(font);
	menu[0].setColor(Color::Red);
	menu[0].setString("Player 1 vs Player 2");
	menu[0].setPosition(Vector2f(345,350));

	menu[1].setFont(font);
	menu[1].setColor(Color::Red);
	menu[1].setString("Player vs Computer");
	menu[1].setPosition(Vector2f(345,450));

	menu[2].setFont(font);
	menu[2].setColor(Color::Red);
	menu[2].setString("Exit");
	menu[2].setPosition(Vector2f(450,550));
}

void Menu::draw(RenderWindow &window){
    Texture background;
    background.loadFromFile("1.jpg");
    Sprite back;
    back.setTexture(background);
    window.draw(back);
    for (int i = 0; i < number_of_item; i++){
		window.draw(menu[i]);
	}
}
