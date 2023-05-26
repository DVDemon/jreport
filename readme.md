# About
Программа для сбора отчета по стратегическим инициативам из jira и публикации их в confluence

# запуск
docker-compose up

В докере запускается:
- MariaDB - для хранения связок инициатива-эпик-продукт-продуктовый эпик
- MongoDB -для хранения истории изменения инициатив
- robot - робот осуществляющий периодический экспорт

# настройки

clusters.json - хранит перечень кластеров
initiatives.json - хранит перечень инициатив в нем же храняться идентификаторы страниц в confluence куда публиковать отчет

связка инициатив с их эпиками пока захардкожена в xlsx_export (процедура init)

# конфигурация в docker-compose

      - DATABASE_IP=db-node
      - DATABASE_PORT=3306
      - DATABASE_LOGIN=stud
      - DATABASE_PASSWORD=stud
      - DATABASE_NAME=jreport
      - JIRA_ADDRESS=https://jira.mts.ru
      - MONGO_ADDRESS=mongo-1
      - MONGO_PORT=27017
      - JIRA_USER=имя_логина_к_jira
      - JIRA_PASSWORD=пароль_к_jira
      - CONFLUENCE_USER=имя_логина_к_confluence
      - CONFLUENCE_PASSWORD=пароь_к_confluence
      - CONFLUENCE_ADDRESS=https://confluence.mts.ru/rest/api

# Dependency
    * Poco
    * openxls https://github.com/troldal/OpenXLSX.git


