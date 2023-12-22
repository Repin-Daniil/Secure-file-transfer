#pragma once

#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <vector>
#include <fstream>
#include <filesystem>

namespace crypto {

class Crypto {
 public:
  Crypto(std::string public_key);
  Crypto(std::string public_key_path, std::string private_key_path);
  ~Crypto();

  std::filesystem::path EncryptFile(std::filesystem::path file_path);
  std::string DecryptFile(std::filesystem::path file_path);
  std::string getPublicKeyAsPEM();

 private:
  RSA *public_key_;
  RSA *private_key_;
  std::pair<bool, bool> keys_ = std::pair(false, false);

  // Methods
  RSA *createPublicKeyFromPEMString(const std::string &pemString);
  RSA *readPublicKeyFromPEM(const std::string &publicKeyFilePath);
  RSA *readPrivateKeyFromPEM(const std::string &privateKeyFilePath);
  std::vector<unsigned char> readFileBytes(const std::string &filePath);
  void writeFileBytes(const std::string &filePath, const std::vector<unsigned char> &fileBytes);
  std::vector<unsigned char> performEncryption(RSA *publicKey, const std::vector<unsigned char> &data);
  std::vector<unsigned char> performDecryption(RSA *privateKey, const std::vector<unsigned char> &encryptedData);
  std::vector<unsigned char> encryptWithPublicKey(RSA *publicKey, const std::vector<unsigned char> &data);
  std::vector<unsigned char> decryptWithPrivateKey(RSA *privateKey, const std::vector<unsigned char> &encryptedData);
};

} // namespace crypto

