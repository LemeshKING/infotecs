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

void input_thread()
{
   while (!stop)
   {
      std::string input;
      std::getline(std::cin, input);

      if (input.length() > 64 || !std::all_of(input.begin(), input.end(), ::isdigit))
      {
         std::cout << "Ошибка: строка должна состоять только из цифр и быть не длиннее 64 символов.\n";
         continue;
      }

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


      {
         std::unique_lock<std::mutex> lock(buffer_mutex);
         buffer_cv.wait(lock, [] { return !buffer.empty() || stop; });

         if (stop && buffer.empty())
            return;
         
         data = buffer.front();
         buffer.pop();
      }

      std::cout << "Обработанная строка: " << data << std::endl;


      int sum = calculateSum(data);
      std::cout << "Сумма чисел: " << sum << std::endl;


      //Добавить работу с сокетами
   }
}
int main()
{
   setlocale(LC_ALL, "");
   std::thread t1(input_thread);
   std::thread t2(processing_thread);

   buffer_cv.notify_all();
   t1.join();
   t2.join();


   return 0;
}