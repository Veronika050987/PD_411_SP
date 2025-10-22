#include<Windows.h>
#include<iostream>
#include<conio.h>
#include<thread>
#include<chrono>
#include<cmath> // Для std::abs
#include<string>
#include <sstream>
using namespace std::chrono_literals;
using std::cin;
using std::cout;
using std::endl;

#define Enter 13
#define Escape 27
#define Forward 'W'
#define Backward 'S'
#define IncreaseSpeed 'D'
#define DecreaseSpeed 'A'
#define StartStop 'I'
#define FillFuel 'F'
#define GetIn 'G'
#define GetOut 'O'

#define MIN_TANK_VOLUME 25
#define MAX_TANK_VOLUME 125

const std::string ANSI_COLOR_RED = "\x1b[31m";
const std::string ANSI_COLOR_GREEN = "\x1b[32m";
const std::string ANSI_COLOR_BLUE = "\x1b[36m";
const std::string ANSI_COLOR_ROSE = "\x1b[35m";
const std::string ANSI_COLOR_RESET = "\x1b[0m";

class Tank
{
	const int VOLUME;
	double fuel_level;
public:
	int get_VOLUME()const
	{
		return VOLUME;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	void fill(double fuel)
	{
		if (fuel < 0)return;
		fuel_level += fuel;
		if (fuel_level > VOLUME)fuel_level = VOLUME;
	}
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
	}
	Tank(int volume):
		VOLUME
		(
			volume < MIN_TANK_VOLUME ? MIN_TANK_VOLUME :
			volume > MAX_TANK_VOLUME ? MAX_TANK_VOLUME :
			volume
		)
	{
		//this->VOLUME = volume;
		this->fuel_level = 0;
		cout << ANSI_COLOR_GREEN << "Tank is ready:\t" << this << ANSI_COLOR_RESET << endl;
	}
	~Tank()
	{
		cout << ANSI_COLOR_RED << "Tank is over:\t" << this << ANSI_COLOR_RESET << endl;
	}
	void info()const
	{
		cout << "Volume:\t" << VOLUME << " liters\n";
		cout << "Fuel:\t" << get_fuel_level() << " liters\n";
	}
};

#define MIN_ENGINE_CONSUMPTION 5
#define MAX_ENGINE_CONSUMPTION 25
class Engine
{
	double consumption_per_second;
	bool is_started;
public:
	const double CONSUMPTION;
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	Engine(double consumption) :
		CONSUMPTION
		(
			consumption < MIN_ENGINE_CONSUMPTION ? MIN_ENGINE_CONSUMPTION :
			consumption > MAX_ENGINE_CONSUMPTION ? MAX_ENGINE_CONSUMPTION :
			consumption
		)
	{
		consumption_per_second = CONSUMPTION * 3e-5;
		is_started = false;
		cout << ANSI_COLOR_GREEN << "Engine is ready:\t" << this << ANSI_COLOR_RESET << endl;
	}
	~Engine()
	{
		cout << ANSI_COLOR_RED << "Engine is over:\t" << this << ANSI_COLOR_RESET << endl;
	}
	void start()
	{
		is_started = true;
	}
	void stop()
	{
		is_started = false;
	}
	bool started()const
	{
		return is_started;
	}
	void info()const
	{
		cout << "Consumption per 100 km:\t" << CONSUMPTION << " liters.\n";
		cout << "Consumption per 1 sec:\t" << get_consumption_per_second() << " liters.\n";
	}
	double get_fuel_consumption(int speed) const 
	{
		double base_consumption = consumption_per_second;
		double speed_factor = speed / 100.0; // Нормализуем скорость
		return base_consumption * (1 + speed_factor);
	}
};

#define MAX_SPEED_LOWER_LIMIT 40
#define MAX_SPEED_UPPER_LIMIT 400
class Car
{
	Engine engine;
	Tank tank;
	const int MAX_SPEED;
	int speed;
	bool driver_inside;
	struct //Threads
	{
		std::thread panel_thread;
		std::thread engine_idle_thread;
	}threads;
	std::string movement_message;
	bool is_moving_forward; 
	bool is_moving_backward; 
	bool is_accelerating;
	bool is_decelerating;
public:
	Car(double consumption, int volume, int max_speed) :
		engine(consumption),
		tank(volume),
		speed(0),
		MAX_SPEED
		(
			max_speed < MAX_SPEED_LOWER_LIMIT ? MAX_SPEED_LOWER_LIMIT:
			max_speed < MAX_SPEED_UPPER_LIMIT ? MAX_SPEED_UPPER_LIMIT:
			max_speed
		),
		is_moving_forward(false),
		is_moving_backward(false),
		is_accelerating(false),
		is_decelerating(false),
		driver_inside(false)
	{
		cout << ANSI_COLOR_GREEN << "Your car is ready:-)\t" << this << ANSI_COLOR_RESET << endl;
	}
	~Car()
	{
		cout << ANSI_COLOR_RED << "Your car is over:-(\t" << this << ANSI_COLOR_RESET << endl;

	}
	void get_in()
	{
		driver_inside = true;
		is_moving_forward = false;
		is_moving_backward = false;
		is_accelerating = false;
		is_decelerating = false;
		movement_message = "";
		if (threads.panel_thread.joinable()) threads.panel_thread.join();
		threads.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (threads.panel_thread.joinable())threads.panel_thread.join();
		stop();
		system("CLS");
		cout << ANSI_COLOR_BLUE <<"A LITTLE BIT OF FRESH AIR!!!" << ANSI_COLOR_RESET << endl;
	}
	void start()
	{
		if (driver_inside && tank.get_fuel_level() > 0)
		{
			engine.start();
			if (!threads.engine_idle_thread.joinable()) 
				threads.engine_idle_thread = std::thread(&Car::engine_idle, this);
			cout << "Engine started." << endl;
		}
		else if (!driver_inside)
			cout << ANSI_COLOR_RED << "You need to get in the car first!" << ANSI_COLOR_RESET << endl;
		else
			cout << ANSI_COLOR_RED << "Not enough fuel to start the engine!" << ANSI_COLOR_RESET << endl;
	}
	void stop()
	{
		engine.stop();
		if (threads.engine_idle_thread.joinable())threads.engine_idle_thread.join();
		speed = 0;
		is_moving_forward = false;
		is_moving_backward = false;
		is_accelerating = false;
		is_decelerating = false;
		cout << ANSI_COLOR_RED << "Engine stopped." << ANSI_COLOR_RESET << endl;
		movement_message = "";
	}
	void increase_speed() 
	{
		if (driver_inside && engine.started()) 
		{
			is_accelerating = true;
			is_decelerating = false;
			speed += 5;
			if (speed > MAX_SPEED) speed = MAX_SPEED;
			updateMovementMessage();
			movement_message = "Forward. Speed: " + std::to_string(speed) + " km/h";
		}
		else if (!driver_inside) 
			cout << "You need to get in the car first!" << endl;
		else if (!engine.started()) 
			cout << "Start the engine first!" << endl;
	}

	void decrease_speed() 
	{
		if (driver_inside) 
		{
			is_accelerating = false;
			is_decelerating = true;
			speed -= 5;
			if (speed < 0) speed = 0;
			updateMovementMessage();
			movement_message = "Backward. Speed: " + std::to_string(speed) + " km/h";
		}
		else
			cout << "You need to get in the car first!" << endl;
	}
	void toggle_move_forward() 
	{
		if (driver_inside && engine.started()) 
		{
			is_moving_forward = !is_moving_forward; // Переключение состояния движения
			is_moving_backward = false;
			is_accelerating = false;
			is_decelerating = false;
			if (!is_moving_forward) 
				speed = 0;
			updateMovementMessage();
		}
		else 
		{
			if (!driver_inside) 
				cout << "You need to get in the car first!" << endl;
			else 
				cout << "Start the engine first!" << endl;
		}
	}

	void toggle_move_backward() 
	{
		if (driver_inside && engine.started()) 
		{
			is_moving_backward = !is_moving_backward;
			is_moving_forward = false; // Ensure we're not going forward and backward at the same time
			is_accelerating = false;
			is_decelerating = false;
			if (!is_moving_backward) 
				speed = 0;
			updateMovementMessage();
		}
		else 
		{
			if (!driver_inside) 
				cout << "You need to get in the car first!" << endl;
			else
				cout << "Start the engine first!" << endl;
		}
	}
	void updateMovementMessage() 
	{
		std::stringstream ss;
		if (is_accelerating) 
		{
			ss << "Accelerating. Speed: " << speed << " km/h";
		}
		else if (is_decelerating) 
		{
			ss << "Decelerating. Speed: " << speed << " km/h";
		}
		else if (is_moving_forward) 
		{
			ss << "Moving Forward. Speed: " << speed << " km/h";
		}
		else if (is_moving_backward) 
		{
			ss << "Moving Backward. Speed: " << speed << " km/h";
		}
		else 
		{
			ss << "Stopped.";
			speed = 0; // When not moving, set speed to 0
		}
		movement_message = ss.str();
	}
	void control()
	{
		cout << ANSI_COLOR_BLUE << "Press 'Enter' to get in" << ANSI_COLOR_RESET << endl;
		char key;
		do
		{
			key = 0;
			if(_kbhit)key = _getch();//ожидает нажатие клавиши и возвращает номер из ascii
			switch (key)
			{
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case FillFuel:
			case 'f':
				double amount;
				cout << ANSI_COLOR_ROSE << "How much do you want? "; cin >> amount;
				cout << ANSI_COLOR_RESET;
				tank.fill(amount);
				cout << "Tank filled. Current level: " << tank.get_fuel_level() << endl;
				break;
			case StartStop://ignition
			case'i':
				if (engine.started())stop();
				else start();
				break;
			case Forward:
			case 'w':
				toggle_move_forward();
				break;
			case Backward:
			case 's':
				toggle_move_backward();
				break;
			case IncreaseSpeed:
			case 'd':
				increase_speed();
				break;
			case DecreaseSpeed:
			case 'a':
				decrease_speed();
				break;
			case Escape:
				stop();
				get_out();
			}
			if (tank.get_fuel_level() == 0 && threads.engine_idle_thread.joinable())stop();
		} while (key != Escape);
	}
	void engine_idle()
	{
		while (engine.started() && tank.give_fuel(engine.get_consumption_per_second()))
			std::this_thread::sleep_for(1s);
	}
	void panel()const
	{
		while (driver_inside)
		{
			system("CLS");
			cout << "Fuel level:\t" << tank.get_fuel_level() << " liters.";
			if (tank.get_fuel_level() < 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x4F);
				cout << " LOW FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			else if (tank.get_fuel_level() > 5)
			{
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 0x2F);
				cout << " ENOUGH FUEL ";
				SetConsoleTextAttribute(hConsole, 0x07);
			}
			cout << endl;
			cout << "Engine is " << (engine.started() ? "WRRROOOOOM-WRRROOOOOM started" : "stopped") << endl;
			cout << movement_message << endl;
			std::this_thread::sleep_for(100ms);
		}
	}
	void info()const
	{
		engine.info();
		tank.info();
		cout << "Max speed:\t" << MAX_SPEED << "km/h\n";
	}
};
//#define TANK_CHECK
//#define ENGINE_CHECK
void main()
{
	setlocale(LC_ALL, "");

#ifdef TANK_CHECK
	Tank tank(-30);
	tank.info();
	double fuel;
	do
	{
		cout << "Введите объём топлива: "; cin >> fuel;
		tank.fill(fuel);
		tank.info();
	} while (fuel > 0);
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHECK

	Car bmw(10, 80, 270);
	bmw.info();
	bmw.control();
}