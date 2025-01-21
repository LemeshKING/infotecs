#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../lib/lib.h"

#pragma comment(lib, "Ws2_32.lib")

const int SERVER_PORT = 8080;
const int BUFFER_SIZE = 1024;


bool initialize_winsock() {
   WSADATA wsa_data;
   int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
   if (result != 0) {
      std::cerr << "������: �� ������� ���������������� Winsock. ��� ������: " << result << std::endl;
      return false;
   }
   return true;
}

int main()
{
   setlocale(LC_ALL, "");
   if (!initialize_winsock()) {
      return 1;
   }

   SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (server_socket == INVALID_SOCKET) {
      std::cerr << "������: �� ������� ������� �����. ��� ������: " << WSAGetLastError() << std::endl;
      WSACleanup();
      return 1;
   }

   sockaddr_in server_address{};
   server_address.sin_family = AF_INET;
   server_address.sin_port = htons(SERVER_PORT);
   server_address.sin_addr.s_addr = INADDR_ANY;

   if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
      std::cerr << "������: �� ������� ��������� �����. ��� ������: " << WSAGetLastError() << std::endl;
      closesocket(server_socket);
      WSACleanup();
      return 1;
   }

   if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
      std::cerr << "������: �� ������� ������ �������������. ��� ������: " << WSAGetLastError() << std::endl;
      closesocket(server_socket);
      WSACleanup();
      return 1;
   }
   SOCKET client_socket;
   while (true)
   {
      std::cout << "�������� �����������..." << std::endl;

      client_socket = accept(server_socket, nullptr, nullptr);
      if (client_socket == INVALID_SOCKET) {
         std::cerr << "������: �� ������� ������� �����������. ��� ������: " << WSAGetLastError() << std::endl;
         closesocket(server_socket);
         WSACleanup();
         return 1;
      }

      std::cout << "������ ���������. �������� ������..." << std::endl;

      char buffer[BUFFER_SIZE];
      while (true) {
         memset(buffer, 0, BUFFER_SIZE);
         int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
         if (bytes_received == SOCKET_ERROR) {
            std::cerr << "������: �� ������� �������� ������. ��� ������: " << WSAGetLastError() << std::endl;
            break;
         }
         else if (bytes_received == 0) {
            std::cout << "������ ����������.\n";
            break;
         }

         std::string data(buffer);
         std::cout << "���������� ������: " << data << std::endl;


         bool is_valid = analyzeString(data);
         if (is_valid)
            std::cout << "������ ���������: " << data << std::endl;
         else
            std::cout << "������: ������ �����������." << std::endl;

      }
   }
   closesocket(client_socket);
   closesocket(server_socket);
   WSACleanup();

   std::cout << "������ �������� ������." << std::endl;
   return 0;
}