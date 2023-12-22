#include "Crypto.h"

namespace crypto {

std::string Crypto::getPublicKeyAsPEM(RSA *publicKey) {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSA_PUBKEY(bio, publicKey);

  char *bufferPtr;
  long length = BIO_get_mem_data(bio, &bufferPtr);
  std::string result(bufferPtr, length);

  BIO_free(bio);
  return result;
}

RSA *Crypto::createPublicKeyFromPEMString(const std::string &pemString) {
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

RSA *Crypto::readPublicKeyFromPEM(const std::string &publicKeyFilePath) {
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

  // Вывод публичного ключа в std::cout
  //RSA_print_fp(stdout, rsa, 0);

  return rsa;
}

RSA *Crypto::readPrivateKeyFromPEM(const std::string &privateKeyFilePath) {
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

std::vector<unsigned char> Crypto::readFileBytes(const std::string &filePath) {
  std::ifstream file(filePath, std::ios::binary);
  std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
  return fileData;
}

void Crypto::writeFileBytes(const std::string &filePath, const std::vector<unsigned char> &fileBytes) {
  std::ofstream file(filePath, std::ios::binary);
  file.write(reinterpret_cast<const char *>(fileBytes.data()), fileBytes.size());
}

// Функция для шифрования отрезка данных с использованием публичного ключа
std::vector<unsigned char> Crypto::performEncryption(RSA *publicKey, const std::vector<unsigned char> &data) {
  int inputBlockSize = RSA_size(publicKey) - 42; // Adjusting for padding size
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

// Функция для дешифрования отрезка зашифрованных данных с использованием приватного ключа
std::vector<unsigned char> Crypto::performDecryption(RSA *privateKey, const std::vector<unsigned char> &encryptedData) {
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

std::vector<unsigned char> Crypto::encryptWithPublicKey(RSA *publicKey, const std::vector<unsigned char> &data) {
  // Инициализация для обработки данных по частям
  int inputBlockSize = RSA_size(publicKey) - 42; // Adjusting for padding size
  std::vector<unsigned char> encryptedData;
  int dataSize = data.size();
  int dataOffset = 0;
  // Шифрование данных по частям
  while (dataOffset < dataSize) {
    int blockSize = (dataSize - dataOffset > inputBlockSize) ? inputBlockSize : dataSize - dataOffset;
    std::vector<unsigned char> chunkToEncrypt(data.begin() + dataOffset, data.begin() + dataOffset + blockSize);
    std::vector<unsigned char> encryptedChunk = performEncryption(publicKey, chunkToEncrypt);
    encryptedData.insert(encryptedData.end(), encryptedChunk.begin(), encryptedChunk.end());
    dataOffset += blockSize;
  }
  return encryptedData;
}

std::vector<unsigned char> Crypto::decryptWithPrivateKey(RSA *privateKey,
                                                         const std::vector<unsigned char> &encryptedData) {
  // Инициализация для обработки данных по частям
  int encryptedBlockSize = RSA_size(privateKey);
  std::vector<unsigned char> decryptedData;
  int dataSize = encryptedData.size();
  int dataOffset = 0;
  // Дешифрование данных по частям
  while (dataOffset < dataSize) {
    std::vector<unsigned char>
        chunkToDecrypt(encryptedData.begin() + dataOffset, encryptedData.begin() + dataOffset + encryptedBlockSize);
    std::vector<unsigned char> decryptedChunk = performDecryption(privateKey, chunkToDecrypt);
    decryptedData.insert(decryptedData.end(), decryptedChunk.begin(), decryptedChunk.end());
    dataOffset += encryptedBlockSize;
  }
  return decryptedData;
}

Crypto::Crypto(std::string public_key) {
  try {
    public_key_ = createPublicKeyFromPEMString(public_key);
  } catch (std::exception &e) {
    std::cout << "Error during creating public key from string - " << e.what() << std::endl;
  }

  keys_.first = true;
}

Crypto::Crypto(std::string public_key_path, std::string private_key_path) {
  try {
    public_key_ = Crypto::readPublicKeyFromPEM(public_key_path);
  } catch (std::exception &e) {
    std::cout << "Error during reading public key from PEM - " << e.what() << std::endl;
  }
  keys_.first = true;
  try {
    private_key_ = Crypto::readPrivateKeyFromPEM(private_key_path);
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
  std::vector<unsigned char> fileData = Crypto::readFileBytes(file_path.string());
  std::vector<unsigned char> encryptedFileData = Crypto::encryptWithPublicKey(public_key_, fileData);

  // Конструируем путь к папке "tmp"
  std::filesystem::path tmp_path = std::filesystem::temp_directory_path() / "tmp";

  // Если папки "tmp" не существует, то создаем ее
  if (!std::filesystem::exists(tmp_path)) {
    std::filesystem::create_directory(tmp_path);
  }

  // Конструируем путь к зашифрованному файлу в папке "tmp"
  std::string encrypted_file_name = "encrypted_" + file_path.stem().string() + file_path.extension().string();
  std::filesystem::path encrypted_file_path = tmp_path / encrypted_file_name;

  // Записываем зашифрованные данные в файл в папке "tmp"
  Crypto::writeFileBytes(encrypted_file_path.string(), encryptedFileData);

  return encrypted_file_path;  // Возвращаем путь к зашифрованному файлу
}

std::string Crypto::DecryptFile(std::filesystem::path file_path) {
  std::string outputExtension = std::filesystem::path(file_path).extension();

  std::vector<unsigned char> encryptedFileDataFromFile = Crypto::readFileBytes(file_path);
  std::vector<unsigned char> decryptedData = Crypto::decryptWithPrivateKey(private_key_, encryptedFileDataFromFile);
  std::filesystem::path decryptedFileName = "decrypted_" + std::filesystem::path(file_path).stem().string() + outputExtension;

  Crypto::writeFileBytes(decryptedFileName, decryptedData);

  return decryptedFileName;
}

}