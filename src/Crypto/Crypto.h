#pragma once

#include <openssl/rsa.h>
#include <openssl/pem.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <utility>
#include <string>

namespace crypto {

class Crypto {
 public:
  explicit Crypto(std::string public_key);
  Crypto(std::string public_key_path, std::string private_key_path);
  ~Crypto();

  std::filesystem::path EncryptFile(std::filesystem::path file_path);
  std::string DecryptFile(std::filesystem::path file_path);
  std::string GetPublicKeyAsString();

 private:
  RSA *public_key_;
  RSA *private_key_;
  std::pair<bool, bool> keys_ = std::pair(false, false);

  // Methods
  RSA *CreatePublicKeyFromString(const std::string &pem_string);
  RSA *ReadPublicKeyFromPEM(const std::string &public_key_file_path);
  RSA *ReadPrivateKeyFromPEM(const std::string &private_key_file_path);

  std::vector<unsigned char> EncryptDataBlock(const std::vector<unsigned char> &data);
  std::vector<unsigned char> DecryptDataBlock(const std::vector<unsigned char> &encrypted_data);
  std::vector<unsigned char> EncryptWithPublicKey(const std::vector<unsigned char> &data);
  std::vector<unsigned char> DecryptWithPrivateKey(const std::vector<unsigned char> &encrypted_data);
};

}  // namespace crypto
