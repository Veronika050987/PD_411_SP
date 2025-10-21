#include<iostream>
#include<conio.h>
#include<Windows.h> // Добавляем для system("CLS")
#include<thread>    // Для std::this_thread::sleep_for
#include<chrono>    // Для std::chrono::milliseconds
#include<sstream>

using std::cin;
using std::cout;
using std::endl;

#define Enter 13
#define Escape 27
#define StartStop 'S'
#define FillFuel 'F'
#define GetIn 'I'
#define GetOut 'O'

#define MIN_TANK_VOLUME 25
#define MAX_TANK_VOLUME 125
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
	void set_fuel_level(double fuel) 
	{
		fuel_level = fuel;
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
		cout << "Tank is ready:\t" << this << endl;
	}
	~Tank()
	{
		cout << "Tank is over:\t" << this << endl;
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
	void set_started(bool started) { is_started = started; }
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
		cout << "Engine is ready:\t" << this << endl;
	}
	~Engine()
	{
		cout << "Engine is over:\t" << this << endl;
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
	std::thread fuel_consumption_thread;
	
	void consume_fuel() 
	{
		while (driver_inside && engine.started() && tank.get_fuel_level() > 0) 
		{
			double fuel_to_consume = engine.get_consumption_per_second();
			double current_fuel = tank.get_fuel_level();

			if (current_fuel >= fuel_to_consume) 
			{
				tank.set_fuel_level(current_fuel - fuel_to_consume);
			}
			else 
			{
				tank.set_fuel_level(0);
				engine.stop();
				cout << "No more fuel =(! Engine stopped!!!" << endl;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 100ms
		}
	}
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
		)
	{
		driver_inside = false;
		cout << "Your car is ready:-)\t" << this << endl;
	}
	~Car()
	{
		cout << "Your car is over:-(\t" << this << endl;
		if (fuel_consumption_thread.joinable()) 
		{
			fuel_consumption_thread.join(); // Дожидаемся завершения потока
		}

	}
	void get_in()
	{
		driver_inside = true;
		if (fuel_consumption_thread.joinable()) 
		{
			fuel_consumption_thread.join();
		}
		panel();
	}
	void get_out()
	{
		driver_inside = false;
		if (fuel_consumption_thread.joinable()) 
		{
			fuel_consumption_thread.join();
		}
	}
	void start_engine_if_possible() 
	{
		if (tank.get_fuel_level() > 0) 
		{
			engine.start();
			cout << "Engine started." << endl;
			// Запускаем поток расхода топлива
			if (!fuel_consumption_thread.joinable()) 
				fuel_consumption_thread = std::thread(&Car::consume_fuel, this);
		}
		else 
			cout << "Not enough fuel to start the engine." << endl;
	}

	void stop_engine() 
	{
		engine.stop();
		cout << "Engine stopped." << endl;
		if (fuel_consumption_thread.joinable()) 
			fuel_consumption_thread.join();
	}
	void fill_fuel() 
	{
		if (driver_inside) 
		{
			double amount;
			cout << "Please, enter fuel amount: ";
			cin >> amount;
			tank.fill(amount);
			cout << "Tank is filled. Current fuel level is : " << tank.get_fuel_level() << " liters\t" << endl;
		}
		else 
		{
			cout << "You need to get in the car first." << endl;
		}
	}
	void control()
	{
		cout << "\n--- CONTROL MODE ---" << endl;
		cout << "Commands: [I] Get In, [O] Get Out, [F] Fill Fuel, [S] Start/Stop Engine, [ESC] Exit" << endl;;

		char key;
		bool running = true;
		while(running)
		{
			cout << "\nCar> ";
			key = _getch();
			key = toupper(key); // Convert to uppercase

			switch (key) 
			{
			case GetIn:
				get_in();
				break;
			case GetOut:
				if (driver_inside) 
				{
					get_out();
					cout << "You got out of the car." << endl;
				}
				else 
					cout << "You are already outside." << endl;
				break;
			case FillFuel:
				fill_fuel();
				break;
			case StartStop:
				if (driver_inside) 
				{
					if (engine.started()) 
						stop_engine();
					else 
						start_engine_if_possible();
				}
				else 
					cout << "You need to get in first." << endl;
				break;
			case Escape:
				if (driver_inside) get_out();
				running = false;
				break;
			default:
				cout << "Unknown command." << endl;
				break;
			}

			if (driver_inside && tank.get_fuel_level() == 0 && !engine.started()) 
			{
				cout << "\nTank is empty.  Stopping engine..." << endl;
				stop_engine();
			}

			if (driver_inside && tank.get_fuel_level() == 0) 
			{
				cout << "\nTank is empty. Do you want to fill it? (Y/N): ";
				char fill_choice;
				cin >> fill_choice;
				fill_choice = toupper(fill_choice);
				if (fill_choice == 'Y') 
				{
					fill_fuel(); // Используем функцию fill_fuel()
				}
			}
		}
	}
	void panel()
	{	
		char key;
		while (driver_inside)
		{
			system("CLS");
			cout << "--- DRIVING PANEL ---" << endl;
			cout << "Fuel level:\t" << tank.get_fuel_level() << " liters.\n";
			cout << "Engine is " << (engine.started() ? "started" : "stopped") << endl;
			
			cout << "Actions: [Enter] -> Get Out, [S] -> Start/Stop Engine" << endl;
			char key;
			do
			{
				key = _getch();//ожидает нажатие клавиши и возвращает номер из ascii
				switch (key)
				{
				case Enter:
					get_out();
					cout << "You've got out the car" << endl;
					break;
				case StartStop:
				case 's':
					if (engine.started()) 
					{
						engine.stop();
						cout << "Engine stopped." << endl;
						if (fuel_consumption_thread.joinable()) 
						{
							fuel_consumption_thread.join();
						}
					}
					else 
					{
						if (tank.get_fuel_level() > 0) 
						{
							engine.start();
							cout << "Engine started." << endl;
							fuel_consumption_thread = std::thread(&Car::consume_fuel, this); // Запускаем поток
						}
						else 
							cout << "\nTank is empty. Do you want to fill it? (Y/N): ";
							char fill_choice;
							cin >> fill_choice;
							fill_choice = toupper(fill_choice);
							if (fill_choice == 'Y')
							{
								double amount;
								cout << "Enter fuel amount: ";
								cin >> amount;
								tank.fill(amount);
								cout << "Tank filled. Current fuel level is: " << tank.get_fuel_level() << " liters." << endl;
							}
					}
					break;

				default:
					cout << "Invalid key!" << endl;
					break;
				}
			} while (key != Escape);
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