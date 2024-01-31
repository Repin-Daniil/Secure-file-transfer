# Подготовка

### Установка

```shell
mkdir build
cd build
pip3 install conan==1.*
conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --target all        
```

### Создание ключей (для сервера)

```shell
openssl genrsa -out res/private_key.pem 1024
openssl rsa -in res/private_key.pem -outform PEM -pubout -out res/public_key.pem
```

# Запуск

### Справка

```shell
./build/bin/safe_file_transfer --help
```

```
General options:
--mode client/server   Set mode: server/client
--help                 produce help message
--config path          Path to config in JSON

Server options:
--port port number     Set listening port: from 1024 до 49151 without root
--public-key path      Path to RSA Public Key (public_key.pem)
--private-key path     Path to RSA Private Key (private_key.pem)

Client options:
--addresses {ip/port}  Receiver's addresses
--packages path        Set path to files
```

### Запуск Сервера. Пример приема файла

```shell
sudo ./build/bin/safe_file_transfer --mode server --port 3333 --public-key res/public_key.pem --private-key res/private_key.pem
```

### Запуск клиента. Пример отправки файла

```shell
./build/bin/safe_file_transfer --mode client --address 127.0.0.1/3333 --package res/package.txt
```

### Запуск тестов

##### Catch2

```shell
./build/bin/ParserTest
```

##### CTest

```shell
ctest --extra-verbose --test-dir build
```
# Архитектура
### UML-диаграмма приложения

![](res/UML.png)

### Алгоритм работы

![](res/Algo.png)
