#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <cmath>
#include <vector>
#include "AntIcon.c"

enum State { WHITE, BLACK };
enum Direction {UP, DOWN, LEFT, RIGHT };

int gridSize = 30;

class Square {
public:
	Square(State state, sf::Vector2f pos = {0,0}) {
		this->state = state;
		size = { 20, 20 };
		pos = { pos.x * size.x, pos.y * size.y };

		shape.setSize(size);
		shape.setPosition(pos);
		setColor();
	}

	Square() {
		state = State::WHITE;
		Square(this->state);
	}

	void setColor() {
		switch (state) {
		case WHITE:
			shape.setFillColor(sf::Color::White);
			shape.setOutlineColor(sf::Color::Black);
			shape.setOutlineThickness(2);
			break;
		case BLACK:
			shape.setFillColor(sf::Color::Black);
			shape.setOutlineColor(sf::Color::Black);
			shape.setOutlineThickness(2);
			break;
		default:
			break;
		}
	}

	sf::RectangleShape shape;
	State state;
private:
	sf::Vector2f position;
	sf::Vector2f size;

};

class Ant {
public:
	Ant(sf::Vector2i position, Direction direction) {
		this->position = position;
		this->direction = direction;
	}

	Ant() {
		srand((unsigned)time(0));
		position.x = 0 + rand() % gridSize + 1;
		position.y = 0 + rand() % gridSize + 1;
		direction = Direction::UP;
	}

	void Rotate(int angle) {
		if (angle == 90) {
			switch (direction) {
			case UP:
				direction = Direction::RIGHT;
				break;
			case DOWN:
				direction = Direction::LEFT;
				break;
			case LEFT:
				direction = Direction::UP;
				break;
			case RIGHT:
				direction = Direction::DOWN;
				break;
			}
		}
		else if (angle == -90) {
			switch (direction) {
			case UP:
				direction = Direction::LEFT;
				break;
			case DOWN:
				direction = Direction::RIGHT;
				break;
			case LEFT:
				direction = Direction::DOWN;
				break;
			case RIGHT:
				direction = Direction::UP;
				break;
			}
		}
	}

	void Move(int units) {
		int moveValue = (units + gridSize) % gridSize;

		switch (direction) {
		case UP:
			position.y = (position.y - units + gridSize) % gridSize;
			break;
		case DOWN:
			position.y = (position.y + units + gridSize) % gridSize;
			break;
		case LEFT:
			position.x = (position.x - units + gridSize) % gridSize;
			break;
		case RIGHT:
			position.x = (position.x + units + gridSize) % gridSize;
			break;
		}
	}

	Direction direction;
	sf::Vector2i position;
private:
};

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, float zoom)
{
	const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
	sf::View view{ window.getView() };
	view.zoom(zoom);
	window.setView(view);
	const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
	const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
	view.move(offsetCoords);
	window.setView(view);
}

int main() {
	std::cout << "Grid size: ";
	std::cin >> gridSize;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Ant game");
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setIcon(sfml_icon.width, sfml_icon.height, sfml_icon.pixel_data);
	

	sf::View camera(sf::FloatRect(0,0, 1280, 720));
	camera.setSize({ 1280, 720 });

	//Square grid[GRID_SIZE][GRID_SIZE];
	std::vector<std::vector<Square>> grid(gridSize, std::vector<Square>(gridSize));
	Ant ant;

	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = Square(State::WHITE, sf::Vector2f(i, j));
		}
	}

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.delta > 0)
					zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, (1.f / 1.1f));
				else if (event.mouseWheelScroll.delta < 0)
					zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, 1.1f);
			}
		}

		if (grid[ant.position.x][ant.position.y].state == State::WHITE) {
			ant.Rotate(90);
			grid[ant.position.x][ant.position.y].state = State::BLACK;
		}
		else {
			ant.Rotate(-90);
			grid[ant.position.x][ant.position.y].state = State::WHITE;
		}
		ant.Move(1);

		if (window.hasFocus()) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
				camera.move(0, -10);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
				camera.move(0, 10);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
				camera.move(-10, 0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
				camera.move(10, 0);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) {
				camera.zoom(1.f / 1.01f);
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) {
				camera.zoom(1.01f);
			}
		}

		window.setView(camera);
		window.clear(sf::Color(44, 62, 80));

		for (int i = 0; i < gridSize; i++) {
			for (int j = 0; j < gridSize; j++) {
				grid[i][j].setColor();
				window.draw(grid[i][j].shape);
			}
		}

		window.display();
	}

}