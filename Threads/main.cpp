#include<iostream>
#include<thread>
#include<chrono>
using namespace std::chrono_literals;
using std::cin;
using std::cout;
using std::endl;

bool finish = false;

void Plus()
{
	while (!finish)
	{
		cout << "+ ";
		std::this_thread::sleep_for(1s);
	}
}
void Minus()
{
	while (!finish)
	{
		cout << "- ";
		std::this_thread::sleep_for(1s);

	}
}
void main()
{
	setlocale(LC_ALL, "");
	/*Plus();
	Minus();*/
	std::thread plus_thread(Plus);
	std::thread minus_thread(Minus);
	cin.get();
	finish = true;
	if(plus_thread.joinable())plus_thread.join();
	if (minus_thread.joinable())minus_thread.join();


}