#include <cstdio>
#include <iostream>

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len] != '\0') {
    ++len;
  }
  return len;
}

void encrypt(char *data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    data[i] = data[i] ^ 0xAA;
  }
}

void decrypt(char *data, size_t length) {
  for (size_t i = 0; i < length; ++i) {
    data[i] = data[i] ^ 0xAA;
  }
}

int main() {
  while (true) {
    unsigned char input;
    std::cin >> input;
    std::cin.ignore();
    switch (input) {
    case 'e': {
      std::cout << "Encryption: ";
      char data[128];
      std::cin.getline(data, 128);
      size_t length = strlen(data);

      encrypt(data, length);

      for (size_t i = 0; i < length; ++i) {
        std::cout << std::hex << (int)(unsigned char)data[i] << " ";
      }
      std::cout << std::endl;
      break;
    }
    case 'd': {
      std::cout << "Decryption: ";
      char data[128];
      std::cin.getline(data, 128);

      char result[128];
      size_t resultLen = 0;
      char *ptr = data;

      while (*ptr != '\0') {
        while (*ptr == ' ')
          ptr++;
        if (*ptr == '\0')
          break;
        int value;
        if (sscanf(ptr, "%x", &value) == 1) {
          result[resultLen++] = (char)(value ^ 0xAA);
        }
        while (*ptr != ' ' && *ptr != '\0')
          ptr++;
      }
      result[resultLen] = '\0';

      std::cout << "Result: " << result << std::endl;
      break;
    }
    case '0':
      return 0;
    }
  }
}