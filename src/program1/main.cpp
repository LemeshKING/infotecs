#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../lib/lib.h"

#pragma comment(lib, "Ws2_32.lib")

std::queue<std::string> buffer;
std::mutex buffer_mutex;
std::condition_variable buffer_cv;
bool stop = false;
std::mutex console_mutex;

const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

bool initialize_winsock()
{
   WSADATA wsa_data;
   int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
   if (result != 0) {
      std::lock_guard<std::mutex> console_lock(console_mutex);
      std::cout << "������: �� ������� ���������������� Winsock. ��� ������: " << result << std::endl;
      return false;
   }
   return true;
}


void input_thread()
{
   while (!stop)
   {  
 
      if(!buffer.empty())
         continue;
      console_mutex.lock();
      std::cout << "������� ������" <<std::endl;
      
      std::string input;
      std::getline(std::cin, input);
      
      if (input.length() > 64 || !std::all_of(input.begin(), input.end(), ::isdigit))
      {
         std::cout << "������: ������ ������ �������� ������ �� ���� � ���� �� ������� 64 ��������." << std::endl;
         console_mutex.unlock();
         continue;
      }
      console_mutex.unlock();
      sortAndChange(input);
     
      {
         std::lock_guard<std::mutex> lock(buffer_mutex);
         buffer.push(input);
      }
      buffer_cv.notify_one();
   }

}
void processing_thread()
{

   while (!stop)
   {
      std::string data;
      SOCKET sock = INVALID_SOCKET;
      
      {
         std::unique_lock<std::mutex> lock(buffer_mutex);
   
         buffer_cv.wait(lock, [] {  return !buffer.empty() || stop; });
         console_mutex.lock();
         if (stop && buffer.empty())
         {
            if (sock != INVALID_SOCKET)
               closesocket(sock);
            console_mutex.unlock();
            return;
         }
         
         data = buffer.front();
         buffer.pop();
        
      }

      std::cout << "������������ ������: " << data << std::endl;


      int sum = calculateSum(data);
      std::cout << "����� �����: " << sum << std::endl;



      if (sock == INVALID_SOCKET) 
      {
         sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
         if (sock == INVALID_SOCKET) {

            std::cerr << "������: �� ������� ������� �����. ��� ������: " << WSAGetLastError() << std::endl;
            continue;
         }

         sockaddr_in server_address{};
         server_address.sin_family = AF_INET;
         server_address.sin_port = htons(SERVER_PORT);
         inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

         if (connect(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) 
         {
          
            std::cerr << "������: �� ������� ������������ � �������. ��� ������: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            sock = INVALID_SOCKET;
            console_mutex.unlock();
            continue;
         }
      }
      if (send(sock, data.c_str(), data.length(), 0) == SOCKET_ERROR) 
      {

         std::cerr << "������: �� ������� ��������� ������ ����� �����. ��� ������: " << WSAGetLastError() << std::endl;
         closesocket(sock);
         sock = INVALID_SOCKET;
      }
      else 
         std::cout << "������ ���������� ��������� �2: " << data << std::endl;
      
      if (sock != INVALID_SOCKET) 
         closesocket(sock);
      console_mutex.unlock();
   }
}
int main()
{
   setlocale(LC_ALL, "");
   if (!initialize_winsock()) 
      return 1;
   
   setlocale(LC_ALL, "");
   std::thread t1(input_thread);
   std::thread t2(processing_thread);

   buffer_cv.notify_all();
   t1.join();
   t2.join();

   WSACleanup();
   return 0;
}