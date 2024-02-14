#include "Crypto.h"

namespace crypto {

using constants::LogTag;

// FIXME

std::string Crypto::GetPublicKeyAsString() {
  BIO *bio = BIO_new(BIO_s_mem());
  PEM_write_bio_RSA_PUBKEY(bio, public_key_);

  char *buffer_ptr;
  int64_t length = BIO_get_mem_data(bio, &buffer_ptr);
  std::string result(buffer_ptr, length);

  BIO_free(bio);

  return result;
}

RSA *Crypto::CreatePublicKeyFromString(const std::string &pem_string) {
  BIO *bio = BIO_new_mem_buf(pem_string.c_str(), pem_string.size());

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

RSA *Crypto::ReadPublicKeyFromPEM(const std::string &public_key_file_path) {
  FILE *public_key_file = fopen(public_key_file_path.c_str(), "r");

  if (!public_key_file) {
    std::cerr << "Failed to open public key file" << std::endl;
    return nullptr;
  }

  RSA *rsa = PEM_read_RSA_PUBKEY(public_key_file, NULL, NULL, NULL);

  if (!rsa) {
    std::cerr << "Failed to read public key from PEM file" << std::endl;
    fclose(public_key_file);

    return nullptr;
  }

  fclose(public_key_file);

  return rsa;
}

RSA *Crypto::ReadPrivateKeyFromPEM(const std::string &private_key_file_path) {
  FILE *private_key_file = fopen(private_key_file_path.c_str(), "r");

  if (!private_key_file) {
    std::cerr << "Failed to open private key file" << std::endl;
    return nullptr;
  }

  RSA *rsa = PEM_read_RSAPrivateKey(private_key_file, NULL, NULL, NULL);

  if (!rsa) {
    std::cerr << "Failed to read private key from PEM file" << std::endl;
    fclose(private_key_file);
    return nullptr;
  }

  fclose(private_key_file);

  return rsa;
}

std::vector<unsigned char> Crypto::EncryptDataBlock(const std::vector<unsigned char> &data) {
  int input_block_size = RSA_size(public_key_) - 42;

  std::vector<unsigned char> encrypted_data(RSA_size(public_key_));

  int total_encrypted_length = 0;
  int data_offset = 0;

  while (data_offset < data.size()) {
    int remaining_data = data.size() - data_offset;
    int block_size = (remaining_data > input_block_size) ? input_block_size : remaining_data;
    int encrypted_length = RSA_public_encrypt(block_size,
                                              &data[data_offset],
                                              &encrypted_data[total_encrypted_length],
                                              public_key_,
                                              RSA_PKCS1_PADDING);

    if (encrypted_length == -1) {
      std::cerr << "Failed to encrypt data" << std::endl;
      return {};
    }

    total_encrypted_length += encrypted_length;
    data_offset += block_size;
  }

  encrypted_data.resize(total_encrypted_length);

  return encrypted_data;
}

std::vector<unsigned char> Crypto::DecryptDataBlock(const std::vector<unsigned char> &encrypted_data) {
  int encrypted_block_size = RSA_size(private_key_);
  std::vector<unsigned char> decrypted_data(RSA_size(private_key_));
  int total_decrypted_length = 0;
  int data_offset = 0;

  while (data_offset < encrypted_data.size()) {
    int decrypted_length = RSA_private_decrypt(encrypted_block_size,
                                               &encrypted_data[data_offset],
                                               &decrypted_data[total_decrypted_length],
                                               private_key_,
                                               RSA_PKCS1_PADDING);
    if (decrypted_length == -1) {
      std::cerr << "Failed to decrypt data" << std::endl;
      return {};
    }

    total_decrypted_length += decrypted_length;
    data_offset += encrypted_block_size;
  }

  decrypted_data.resize(total_decrypted_length);

  return decrypted_data;
}

std::vector<unsigned char> Crypto::EncryptWithPublicKey(const std::vector<unsigned char> &data) {
  int input_block_size = RSA_size(public_key_) - 42;
  std::vector<unsigned char> encrypted_data;
  int data_size = data.size();
  int data_offset = 0;

  while (data_offset < data_size) {
    int block_size = (data_size - data_offset > input_block_size) ? input_block_size : data_size - data_offset;

    std::vector<unsigned char> chunk_to_encrypt(data.begin() + data_offset, data.begin() + data_offset + block_size);
    std::vector<unsigned char> encrypted_chunk = EncryptDataBlock(chunk_to_encrypt);
    encrypted_data.insert(encrypted_data.end(), encrypted_chunk.begin(), encrypted_chunk.end());
    data_offset += block_size;
  }

  return encrypted_data;
}

std::vector<unsigned char> Crypto::DecryptWithPrivateKey(const std::vector<unsigned char> &encrypted_data) {
  int encrypted_block_size = RSA_size(private_key_);
  std::vector<unsigned char> decrypted_data;
  int data_size = encrypted_data.size();
  int data_offset = 0;

  while (data_offset < data_size) {
    std::vector<unsigned char>
        chunk_to_decrypt
        (encrypted_data.begin() + data_offset, encrypted_data.begin() + data_offset + encrypted_block_size);
    std::vector<unsigned char> decrypted_chunk = DecryptDataBlock(chunk_to_decrypt);
    decrypted_data.insert(decrypted_data.end(), decrypted_chunk.begin(), decrypted_chunk.end());
    data_offset += encrypted_block_size;
  }

  return decrypted_data;
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
  if (!std::filesystem::exists(file_path)) {
    throw std::runtime_error("Error: File not found");
  }

  std::ifstream file_stream(file_path, std::ios::binary);

  if (!file_stream.is_open()) {
    throw std::runtime_error("Error: Unable to open file");
  }

  std::filesystem::path output_dir = std::filesystem::temp_directory_path();
  std::filesystem::path output_file_path = output_dir / ("encrypted_" + file_path.filename().string());

  std::ofstream combined_output_file(output_file_path, std::ios::binary);

  if (!combined_output_file.is_open()) {
    throw std::runtime_error("Error: Unable to create combined output file");
  }

  LogInfo(LogTag::CLIENT, "Start encrypting"s);

  boost::timer::progress_display progress_bar(std::filesystem::file_size(file_path));
  auto start = std::chrono::steady_clock::now();

  std::streamsize block_size = 20480;
  std::vector<unsigned char> buffer(block_size);

  while (!file_stream.eof()) {
    file_stream.read(reinterpret_cast<char *>(buffer.data()), block_size);
    auto bytes_read = file_stream.gcount();
    if (bytes_read > 0) {
      std::vector<unsigned char> data(buffer.begin(), buffer.begin() + bytes_read);
      std::vector<unsigned char> encrypted_data = EncryptWithPublicKey(data);
      combined_output_file.write(reinterpret_cast<const char *>(encrypted_data.data()), encrypted_data.size());
    }

    progress_bar += bytes_read;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  LogInfo(LogTag::CLIENT, "Encrypted"sv);
  LogTrace(LogTag::CLIENT, "Encryption time: "s + std::to_string(elapsed_seconds.count()) + " seconds"s);

  return output_file_path;
}

std::string Crypto::DecryptFile(std::filesystem::path file_path) {
  if (!std::filesystem::exists(file_path)) {
    throw std::runtime_error("Error: File not found");
  }

  std::ifstream combined_input_file(file_path, std::ios::binary);
  if (!combined_input_file.is_open()) {
    throw std::runtime_error("Error: Unable to open combined input file");
  }

  std::filesystem::path output_file_path = std::filesystem::current_path() / (file_path.filename().string().erase(
      file_path.filename().string().find("encrypted_"),
      std::strlen("encrypted_")));

  std::ofstream output_file(output_file_path, std::ios::binary);
  if (!output_file.is_open()) {
    throw std::runtime_error("Error: Unable to create output file");
  }

  LogInfo(LogTag::SERVER, "Start decrypting"s);
  auto start = std::chrono::steady_clock::now();

  std::streamsize block_size = 20480;
  std::vector<unsigned char> buffer(block_size);
  boost::timer::progress_display progress_bar(std::filesystem::file_size(file_path));

  while (!combined_input_file.eof()) {
    combined_input_file.read(reinterpret_cast<char *>(buffer.data()), block_size);
    auto bytes_read = combined_input_file.gcount();

    if (bytes_read > 0) {
      std::vector<unsigned char> encrypted_data(buffer.begin(), buffer.begin() + bytes_read);
      std::vector<unsigned char> decrypted_data = DecryptWithPrivateKey(encrypted_data);
      output_file.write(reinterpret_cast<const char *>(decrypted_data.data()), decrypted_data.size());
    }

    progress_bar += bytes_read;
  }

  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;

  LogInfo(LogTag::SERVER, "Decrypted!"sv);
  LogTrace(LogTag::SERVER, "Decryption time: "s + std::to_string(elapsed_seconds.count()) + " seconds"s);

  return output_file_path.string();
}

}  // namespace crypto
