## Server

### Сборка 
```
cd server
make
```

### Запуск сервера
```
./main
```

При подключении клиентов в логах сообщения:

```
Controller: got connect from 4
Controller: got connect from 5
```


## Сlient

### Сборка 
```
cd client
make
```

### Использование

Для подключения к серверу:
```
./client 127.0.0.1 9034
```

Примеры доступных команд:

1. Подсчет символов в строке
```
>parse Hello
Awaiting server response...
Server: parsedString: H1e1l2o1
```

2. Число подключений

```
>connectionsNumber
Awaiting server response...
Server: connectionsNumber: 2

```

3. Отправить сообщение для fd=5

```
>put 5 hello1  
Awaiting server response...
Server: message sent to 5
```

4. Прочитать сообщение

```
>get
Awaiting server response...
Server: new msg from 4: hello1
>get
Awaiting server response...
Server: no messages
```

5. Если сервер недорступен, то идут попытки подключиться

```
Error connecting to socket!
Error connecting to socket!
Error connecting to socket!
Error connecting to socket!
Error connecting to socket!
Error connecting to socket!
Connected to the server!
>parse Hello
Awaiting server response...
Server: parsedString: H1e1l2o1
```