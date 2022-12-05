#include <arpa/inet.h>
#include <stdio.h>

int main() {
  unsigned int a1 = inet_addr("172.17.0.2");
  unsigned int a2 = inet_addr("172.17.0.3");
  unsigned int a3 = inet_addr("172.17.0.4");
  unsigned int a4 = inet_addr("172.17.0.5");
  printf("0x%x 0x%x 0x%x 0x%x\n", a1, a2, a3, a4);
}
