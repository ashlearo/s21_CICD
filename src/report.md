## Part 1. Настройка gitlab-runner

**Поднять виртуальную машину Ubuntu Server 20.04 LTS**

![linux](images/11.jpg)

**Скачать и установить на виртуальную машину gitlab-runner**

**Установка GitLab Runner**

В репозиториях доступна версия 11.2.0 gitlab-runner,
поэтому установка по инструкции из GitLab:  
<https://docs.gitlab.com/runner/install/linux-manually.html>

1. Загружаем двоичный файл для нашей системы:  
**`sudo curl -L --output /usr/local/bin/gitlab-runner "https://gitlab-runner-downloads.s3.amazonaws.com/latest/binaries/gitlab-runner-linux-amd64"`**


2. Даем ему разрешения на выполнение:  
**`sudo chmod +x /usr/local/bin/gitlab-runner`**

3. Создаем пользователя GitLab CI:  
**`sudo useradd --comment 'GitLab Runner' --create-home gitlab-runner --shell /bin/bash`**

4. Установка и запуск от имени службы:  
**`sudo gitlab-runner install --user=gitlab-runner --working-directory=/home/gitlab-runner`**

5. Проверяем версию:  
**`sudo gitlab-runner -version`**

![linux](images/12.jpg)

**Запустить gitlab-runner и зарегистрировать его для использования в текущем проекте (DO6_CICD)**  

Запуск службы и добавление её в автозагрузку, выполнив команду:   
**`sudo systemctl enable --now gitlab-runner`**
проверка, что служба запустилась:   
**`sudo systemctl status gitlab-runner`**

![linux](images/13.jpg)

Register a runner, в системе Ubuntu, выполнив команду:   
**`sudo gitlab-runner register`** 
- далее идет запрос данных, вписываем URL и token, остальные поля можно оставить пустыми, в последнее поле записываем  **`shell`**  

на страничке Школы 21 - в проекте CICD, копируем поочередно URL и token  

![linux](images/14.jpg)

![linux](images/15.jpg)

## Предварительно установить make, gcc, и clang-format
sudo apt install make

sudo apt install gcc

sudo apt install clang-format

## Part 2. Сборка

**Написать этап для CI по сборке приложений из проекта C2_SimpleBashUtils:**  
**В файле .gitlab-ci.yml добавить этап запуска сборки через мейк файл из проекта C2**  
**Файлы, полученные после сборки (артефакты), сохранять в произвольную директорию со сроком хранения 30 дней.**  

**Создаём файл .gitlab-ci.yml в корне проекта и вписываем туда первую стадию build, путь и время храннения скомпилированных файлов (артефактов):**

![linux](images/21.jpg)

Добавляем в проект все файлы по cat и grep из проекта SimpleBash  

![linux](images/22.jpg)

делаем в проекте какое нибудь изменение затем коммитим и пушим:   

git add .

git commit -am "..."

git push origin develop

наш Pipeline на паузе висит, ждет команду

![linux](images/23.jpg)

запускаем runner, выполнив команду **`sudo gitlab-runner run`**

видим, что Pipeline заработал меняются статусы

![linux](images/24.jpg)

![linux](images/25.jpg)


### Артефакты сохранились

![linux](images/27.jpg)

![linux](images/28.jpg)

## Part 3. Тест кодстайла

**Написать этап для CI, который запускает скрипт кодстайла (clang-format):**  
**Если кодстайл не прошел, то "зафейлить" пайплайн**  
**В пайплайне отобразить вывод утилиты clang-format**  

Добавляем этап style в заголовке .gitlab-ci.yml , и ставим задачу по кодстайлу:  

![linux](images/32.jpg)

Запушил, проверяю:  

![linux](images/33.jpg)

Теперь добавляем ошибки по кодстайлу в файлы s21_cat s21_grep, пушим и смотрим результат:  

пайплайн - фейлит  

![linux](images/34.jpg)

![linux](images/35.jpg)  


## Part 4. Интеграционные тесты

**Написать этап для CI, который запускает ваши интеграционные тесты из того же проекта:**  
**Запускать этот этап автоматически только при условии, если сборка и тест кодстайла прошли успешно**  
**Если тесты не прошли, то "зафейлить" пайплайн**  
**В пайплайне отобразить вывод, что интеграционные тесты успешно прошли / провалились**  

Дописываем этап test в заголовок .gitlab-ci.yml   

![linux](images/41.jpg)

Ставим задачу по тесту:  

![linux](images/42.jpg)

Проверка, если сборка и тест кодстайла не прошли успешно - то этап test не запускается:  

![linux](images/43.jpg)

Другой случай - сборка и тест кодстайла прошли успешно.
Пушим, и смотрим результат, что все этапы успешно отработали:  

![linux](images/44.jpg)

Другой случай - специально портим функции cat и grep - чтоб тесты не прошли.  
Пушим, смотрим результат, что тесты провалились:  

![linux](images/46.jpg)

![linux](images/47.jpg)


## Part 5. Этап деплоя

**Поднять вторую виртуальную машину Ubuntu Server 20.04 LTS**

![linux](images/51.jpg)

Изменяю настройки адаптеров на двух машинах:  

![linux](images/52.jpg)

Изменяю файл etc/netplan/00-installer-config.yaml для каждой машины,     
ip 10.20.0.20  Ubuntu_for_CICD    - рабочая машина с нее пушим проект       
ip 10.20.0.10  Ubuntu_for_deploy  - машина, принимающая развернутый проект, в случае успеха     

![linux](images/53.jpg)

Проверяем соединение между машинами:  

![linux](images/54.jpg)

**Написать этап для CD, который "разворачивает" проект на другой виртуальной машине:**
**Запускать этот этап вручную при условии, что все предыдущие этапы прошли успешно**

Добавляем этап deploy в заголовок .gitlab-ci.yml

![linux](images/55.jpg)

Ставим задачу по deploy:

![linux](images/56.jpg)

**Написать bash-скрипт, который при помощи ssh и scp копирует файлы, полученные после сборки (артефакты), в директорию /usr/local/bin второй виртуальной машины**

![linux](images/57.jpg)

Зададим нужные привилегии пользователю gitlab-runner - добавим его в группу sudo и разрешим запуск команд без пароля, выполнив команду:  
**`sudo usermod -aG sudo gitlab-runner`**

![linux](images/58.jpg)

меняем строки в файле /etc/sudoers, используя команду:   
**`sudo visudo`**

![linux](images/59.jpg)

Делаем подключение ко второй машине через ssh без необходимости ввода пароля:

c Ubuntu_for_CICD захожу на вторую машину Ubuntu_for_deploy через ssh, выполнив команду   
**`ssh 10.20.0.20`**
Далее находясь на второй машине, захожу изменяю настройки в конфиг. файле (ставлю поключение по публичному ключу) выполнив команду   
**`sudo vim /etc/ssh/sshd_config`**

![linux](images/510.jpg)

перезапускаем сервер ssh выполнив команду  
**`sudo service ssh restart`**

Затем, именно на Ubuntu_for_CICD генерируем ключ  **`sudo -u root ssh-keygen`**
((используем "sudo -u root" т.к.от пользователя root выполняется действие, раннер от рута работает, поэтому надо от него ключи создать и перекинуть))

![linux](images/511.jpg)

и скопируем его на вторую машину командой   
**`sudo -u root ssh-copy-id ashlearo@10.20.0.10`**

![linux](images/512.jpg)

Теперь подключаемся на вторую машину Ubuntu_for_deploy через ssh без пароля

![linux](images/513.jpg)

дадим права для папки /usr/local/bin/ на машине Ubuntu_for_deploy выполнив команду   
**`sudo chmod -R 777 /usr/local/bin`**

![linux](images/514.jpg)

Снова пушим и видим, что стадия деплой ждёт ручного запуска:

![linux](images/515.jpg)

Деплой прошёл успешно, файлы появились в нужной папке на второй машине:

![linux](images/516.jpg)

![linux](images/517.jpg)

## Part 6. Дополнительно. Уведомления

Настроить уведомления о успешном/неуспешном выполнении пайплайна через бота с именем "[ваш nickname] DO6 CI/CD" в Telegram

Пишу @botfather, чтобы создать бота и получить его токен

![linux](images/61.jpg)

Пишу @getmyid_bot, чтобы узнать свой ID telegram

![linux](images/62.jpg)

Запускаю бота, и пишу в чате созданому боту "ashlearo_d06_CICD_bot" команду:  
**`/start`**

![linux](images/62_.jpg)

В скрипте notifications.sh  прописываем токен и ID telegram

![linux](images/63.jpg)

После каждой стадии добавляем в .gitlab-ci.yml вызов скрипта ./notifications.sh 

![linux](images/64.jpg)

Пушим, получаем уведомления от бота - всё работает

![linux](images/65.jpg)
