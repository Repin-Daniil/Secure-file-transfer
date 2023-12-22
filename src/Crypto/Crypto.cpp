#include <cstring>
#include "Crypto.h"

namespace crypto {

std::string Crypto::GetPublicKeyAsString() {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSA_PUBKEY(bio, public_key_);

  char *bufferPtr;
  long length = BIO_get_mem_data(bio, &bufferPtr);
  std::string result(bufferPtr, length);

  BIO_free(bio);
  return result;
}

RSA *Crypto::CreatePublicKeyFromString(const std::string &pemString) {
  BIO *bio = BIO_new_mem_buf(pemString.c_str(), pemString.size());
  if (bio == NULL) {
    std::cerr << "Failed to create mem BIO" << std::endl;
    return nullptr;
  }

  RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
  if (rsa == NULL) {
    std::cerr << "Failed to create RSA from PEM string" << std::endl;
  }

  BIO_free(bio);
  return rsa;
}

RSA *Crypto::ReadPublicKeyFromPEM(const std::string &publicKeyFilePath) {
  FILE *publicKeyFile = fopen(publicKeyFilePath.c_str(), "r");
  if (!publicKeyFile) {
    std::cerr << "Failed to open public key file" << std::endl;
    return nullptr;
  }

  RSA *rsa = PEM_read_RSA_PUBKEY(publicKeyFile, NULL, NULL, NULL);

  if (!rsa) {
    std::cerr << "Failed to read public key from PEM file" << std::endl;
    fclose(publicKeyFile);
    return nullptr;
  }

  fclose(publicKeyFile);

  return rsa;
}

RSA *Crypto::ReadPrivateKeyFromPEM(const std::string &privateKeyFilePath) {
  FILE *privateKeyFile = fopen(privateKeyFilePath.c_str(), "r");
  if (!privateKeyFile) {
    std::cerr << "Failed to open private key file" << std::endl;
    return nullptr;
  }

  RSA *rsa = PEM_read_RSAPrivateKey(privateKeyFile, NULL, NULL, NULL);
  if (!rsa) {
    std::cerr << "Failed to read private key from PEM file" << std::endl;
    fclose(privateKeyFile);
    return nullptr;
  }

  fclose(privateKeyFile);
  return rsa;
}

std::vector<unsigned char> Crypto::ReadFileBytes(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  return fileData;
}

void Crypto::WriteFileBytes(const std::string &filePath, const std::vector<unsigned char> &fileBytes) {
  std::ofstream file(filePath, std::ios::binary);
  file.write(reinterpret_cast<const char *>(fileBytes.data()), fileBytes.size());
}

std::vector<unsigned char> Crypto::PerformEncryption(RSA *publicKey, const std::vector<unsigned char> &data) {
  int inputBlockSize = RSA_size(publicKey) - 42;
  std::vector<unsigned char> encryptedData(RSA_size(publicKey));
  int totalEncryptedLength = 0;
  int dataOffset = 0;
  while (dataOffset < data.size()) {
    int remainingData = data.size() - dataOffset;
    int blockSize = (remainingData > inputBlockSize) ? inputBlockSize : remainingData;
    int encryptedLength = RSA_public_encrypt(blockSize,
                                             &data[dataOffset],
                                             &encryptedData[totalEncryptedLength],
                                             publicKey,
                                             RSA_PKCS1_PADDING);
    if (encryptedLength == -1) {
      std::cerr << "Failed to encrypt data" << std::endl;
      return {};
    }
    totalEncryptedLength += encryptedLength;
    dataOffset += blockSize;
  }
  encryptedData.resize(totalEncryptedLength);
  return encryptedData;
}

std::vector<unsigned char> Crypto::PerformDecryption(RSA *privateKey, const std::vector<unsigned char> &encryptedData) {
  int encryptedBlockSize = RSA_size(privateKey);
  std::vector<unsigned char> decryptedData(RSA_size(privateKey));
  int totalDecryptedLength = 0;
  int dataOffset = 0;
  while (dataOffset < encryptedData.size()) {
    int decryptedLength = RSA_private_decrypt(encryptedBlockSize,
                                              &encryptedData[dataOffset],
                                              &decryptedData[totalDecryptedLength],
                                              privateKey,
                                              RSA_PKCS1_PADDING);
    if (decryptedLength == -1) {
      std::cerr << "Failed to decrypt data" << std::endl;
      return {};
    }
    totalDecryptedLength += decryptedLength;
    dataOffset += encryptedBlockSize;
  }
  decryptedData.resize(totalDecryptedLength);
  return decryptedData;
}

std::vector<unsigned char> Crypto::EncryptWithPublicKey(RSA *publicKey, const std::vector<unsigned char> &data) {

  int inputBlockSize = RSA_size(publicKey) - 42;
  std::vector<unsigned char> encryptedData;
  int dataSize = data.size();
  int dataOffset = 0;

  while (dataOffset < dataSize) {
    int blockSize = (dataSize - dataOffset > inputBlockSize) ? inputBlockSize : dataSize - dataOffset;
    std::vector<unsigned char> chunkToEncrypt(data.begin() + dataOffset, data.begin() + dataOffset + blockSize);
    std::vector<unsigned char> encryptedChunk = PerformEncryption(publicKey, chunkToEncrypt);
    encryptedData.insert(encryptedData.end(), encryptedChunk.begin(), encryptedChunk.end());
    dataOffset += blockSize;
  }

  return encryptedData;
}

std::vector<unsigned char> Crypto::DecryptWithPrivateKey(RSA *privateKey,
                                                         const std::vector<unsigned char> &encryptedData) {
  int encryptedBlockSize = RSA_size(privateKey);
  std::vector<unsigned char> decryptedData;
  int dataSize = encryptedData.size();
  int dataOffset = 0;

  while (dataOffset < dataSize) {
    std::vector<unsigned char>
        chunkToDecrypt(encryptedData.begin() + dataOffset, encryptedData.begin() + dataOffset + encryptedBlockSize);
    std::vector<unsigned char> decryptedChunk = PerformDecryption(privateKey, chunkToDecrypt);
    decryptedData.insert(decryptedData.end(), decryptedChunk.begin(), decryptedChunk.end());
    dataOffset += encryptedBlockSize;
  }

  return decryptedData;
}

Crypto::Crypto(std::string public_key) {
  try {
    public_key_ = CreatePublicKeyFromString(public_key);
  } catch (std::exception &e) {
    std::cout << "Error during creating public key from string - " << e.what() << std::endl;
  }

  keys_.first = true;
}

Crypto::Crypto(std::string public_key_path, std::string private_key_path) {
  try {
    public_key_ = Crypto::ReadPublicKeyFromPEM(public_key_path);
  } catch (std::exception &e) {
    std::cout << "Error during reading public key from PEM - " << e.what() << std::endl;
  }
  keys_.first = true;
  try {
    private_key_ = Crypto::ReadPrivateKeyFromPEM(private_key_path);
  } catch (std::exception &e) {
    std::cout << "Error during reading private key from PEM - " << e.what() << std::endl;
    if (keys_.first) {
      RSA_free(public_key_);
    }
  }
  keys_.second = true;
}

Crypto::~Crypto() {
  if (keys_.first) {
    RSA_free(public_key_);
  }
  if (keys_.second) {
    RSA_free(private_key_);
  }
}

std::filesystem::path Crypto::EncryptFile(std::filesystem::path file_path) {
  if(!std::filesystem::exists(file_path)) {
    throw std::runtime_error("Error: File not found");
  }

  std::ifstream fileStream(file_path, std::ios::binary | std::ios::ate);
  if (!fileStream.is_open()) {
    throw std::runtime_error("Error: Unable to open file");
  }
  std::streamsize fileSize = fileStream.tellg();
  fileStream.seekg(0, std::ios::beg);

  std::vector<unsigned char> fileData(fileSize);
  if(!fileStream.read(reinterpret_cast<char*>(fileData.data()), fileSize)) {
    throw std::runtime_error("Error: Unable to read file");
  }

  std::vector<unsigned char> encryptedFileData = EncryptWithPublicKey(public_key_, fileData);

  std::filesystem::path tmp_path = std::filesystem::current_path() / "tmp";
  std::filesystem::create_directory(tmp_path);

  std::filesystem::path encrypted_file_path = tmp_path / ("encrypted_" + file_path.filename().string());

  WriteFileBytes(encrypted_file_path.string(), encryptedFileData);

  return encrypted_file_path;
}

std::string Crypto::DecryptFile(std::filesystem::path file_path) {
  std::string outputExtension = std::filesystem::path(file_path).extension();

  std::vector<unsigned char> encryptedFileDataFromFile = Crypto::ReadFileBytes(file_path);
  std::vector<unsigned char> decryptedData = Crypto::DecryptWithPrivateKey(private_key_, encryptedFileDataFromFile);
  std::string decryptedFileName = std::filesystem::path(file_path).string();
  decryptedFileName = decryptedFileName.erase(decryptedFileName.find("encrypted_"), std::strlen("encrypted_"));
  decryptedFileName = std::filesystem::path(decryptedFileName).stem().string() + outputExtension;

  Crypto::WriteFileBytes(decryptedFileName, decryptedData);

  return decryptedFileName;
}

} // namespace crypto

