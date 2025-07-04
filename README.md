# pb
c++ static site generator / генератор статических сайтов на c++

The file "ids.txt" contains a list of page IDs, and the file "props.txt" contains a list of properties for each page.
To set a property "P" for a page ID, you need to create a file with the name "ID-P.txt". For example, "page1-body.txt".
The file "template.txt" contains an HTML template where occurrences of "{P}" are replaced with the contents of "ID-P.txt", and then saved as a file named "ID.html".

Файл «ids.txt» содержит список идентификаторов страниц, файл «props.txt» содержит список свойств для каждой страницы.
Чтобы установить свойство «P» для идентификатора страницы, вам необходимо создать файл с именем «ID-P.txt». Например, «page1-body.txt».
Файл «template.txt» содержит шаблон HTML, в котором вхождения «{P}» заменены содержимым «ID-P.txt», а затем сохранены как файл с именем «ID.html».

# Сборка

Тестовые данные можно получить с помощью makefakedata

```
g++ -std=c++20 makefakedata.cpp -o makefakedata
./makefakedata

```

компиляция генератора
```
g++ -pthread -std=c++20 pb.cpp -o pb
```

тест создания страниц
```
mv pb test_data/
cd test_data/
./pb
```

HTML файлы будут помещены в каталог ./build 

# Производительность

Сгенерируем 100 000 описаний атрибутов страниц и запустим сборку сайта

```
Build completed in 130800 milliseconds
Generated 100000 files in ./build directory
```
Таким образом, примерная скорость сборки 1.3 мс/страница