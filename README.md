
# Подготовка
### Установка библиотек
```shell
cd cmake-build-debug
conan install .. --build=missing
```
### Создание ключей (для клиента)
```shell
openssl genrsa -out private_key.pem 1024
openssl rsa -in private_key.pem -outform PEM -pubout -out public_key.pem
```

# Запуск
### Прием файла. Запуск Сервера
```
./safe_tunnel server  <Port> <Path to public key> <Path to private key>
```
##### Пример
```shell
cmake --build cmake-build-debug --target safe_tunnel
sudo ./cmake-build-debug/bin/safe_tunnel server 3333 public_key.pem private_key.pem
```

### Отправка файла. Запуск клиента

```
./safe_tunnel client <ServerIP>  <Server Port> <Path to file>
```
##### Пример
```shell
cmake --build cmake-build-debug --target safe_tunnel
./cmake-build-debug/bin/safe_tunnel client 127.0.0.1  3333 .gitignore
```

