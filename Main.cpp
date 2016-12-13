#ifdef SFML_STATIC
#pragma comment(lib, "glew.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")  
#endif // SFML_STATIC

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <conio.h>
#include <thread>
#include <Windows.h>
#include <string>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;

sf::Image image;
sf::Texture Text;
sf::TcpSocket Client;
sf::IpAddress ServerIP;
sf::Packet packet, packet2;
unsigned short port;
std::size_t received;
vector<sf::Uint8> tmpUint;
int x = 352;
int y = 288;
bool Close = false;
char Dat[100];
cv::Mat Decoded, Corrected;
cv::vector<uchar>buff;

/*sf::Packet& operator >> (sf::Packet& packet, sf::Texture texture)
{
	tmpUint.clear();
	for (int i = 0; i < x*y*4; i++)
	{
		sf::Uint8 tmp;
		packet >> tmp;
		tmpUint.push_back(tmp);
	}
	return packet;
}*/

sf::Packet& operator >> (sf::Packet& packet, sf::Texture texture)
{
	size_t Siz;
	packet >> Siz;
	buff.clear();
	for (size_t i = 0; i < Siz; i++)
	{
		uchar test;
		packet >> test;
		buff.push_back(test);
	}
	return packet;
}

void Command()
{
	while (!Close)
	{
		std::cout << "Enter: ";
		if (!Close)
		{
			char Heal = _getch();
			std::string Real(1, Heal);
			strcpy_s(Dat, Real.c_str());
			Client.send(Dat, 100);
		}
	}
}

int main()
{
	std::cout << "Enter: ";
	std::string Hella;
	std::getline(cin, Hella);
	char Data[100];
	strcpy_s(Data, Hella.c_str());
	std::cout << "Sending..." << endl;
	Client.connect("192.168.0.103", 25566);
	if (Client.send(Data, 100) != sf::Socket::Done)
	{
		std::cout << "Error 1" << endl;
	}
	std::cout << "Sent..." << endl;
	std::cout << "Receiving Images..." << endl;
	sf::Packet Real;
	Client.setBlocking(false);
	sf::RenderWindow window(sf::VideoMode(800, 600), "Webcam");
	Close = false;
	thread GetComms(&Command);
	cv::VideoWriter video("out.avi",  -1 /*CV_FOURCC('X', '2', '6', '4')*/, 10, cv::Size(352, 288), true);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				Client.disconnect();
				window.close();
				Close = true;
			}
		}
		window.clear(sf::Color::Black);
		if (Client.receive(Real) == sf::Socket::Done)
		{
			Real >> Text;
			/*size_t Siz;
			Real >> Siz;
			buff = cv::vector<uchar>(Siz);
			Client.receive(&buff, Siz, Siz);*/
			Decoded = cv::imdecode(cv::Mat(buff), CV_LOAD_IMAGE_COLOR);
			cv::cvtColor(Decoded, Corrected, cv::COLOR_BGR2RGBA);
			imwrite("test.jpg", Corrected);
			sf::Image Ass;
			Ass.loadFromFile("test.jpg");
			video.write(Corrected);
			Text.loadFromImage(Ass);
		}
		sf::RectangleShape rectangle(sf::Vector2f(352, 288));
		rectangle.setTexture(&Text);
		rectangle.move(400, 150);
		window.draw(rectangle);
		window.display();
	}
	video.release();
	std::cout << "Window Closed" << endl;
	GetComms.join();
	return 0;
}