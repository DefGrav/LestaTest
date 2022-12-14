# Тестовое задание "Многопоточность"

Если будут вопросы или что-то будет непонятно – обязательно пишите нам.

## Общее описание

Нужно реализовать - "многопоточный движок эффектов".
По клику, из точки клика в разные стороны, с разной скорость вылетает 64 частицы.
Они потихоньку падают вниз.
Через какое-то время (пара секунд) частица с какой-то вероятностью либо гаснет, либо превращается в новый "взрыв".
Для которых тоже выполняется эти правила.
Все расчёты движения частиц должны идти в отдельном потоке (или потоках).
Частота обновления частиц не должна зависит от частоты кадров основного потока.
На один кадр рендера может приходится несколько обновлений эффектов или наоборот, если обновление занимает больше одного кадра - это не должно влиять на частоту рендера.
Примерное время для Client Core (Programmer / Senior Programmer) - 6-10 часов.

## Требования

Задание выполняется на базе предоставленного "фреймворка".
Нужно реализовать функции из файла "test.cpp".
Код, который уже есть в файле "test.cpp" можно менять полностью.
Можно создавать новые файлы.
В процессе работы решение не выделяет и не освобождает память. Вся память должна быть выделена заранее и освобождена в конце.
Максимально количество эффектов - 2048 (по 64 частицы каждый).
При превышении этого бюджета - движок не должен падать или выделять новой памяти.
Частицы и эффекты за экраном должны удаляться.
Разработка сейчас в компании ведётся на VS 2015 (VS 2017), поэтому, тестовые задания принимаются только в ней.
Для многопоточности можно использовать ТОЛЬКО std библиотеки. Такие, как (но не только): std::atomic, std::thread, std::mutex, ...
Нельзя использовать WinApi, pthread, QT, MFC, GLUT, GL, DirectX и другие API и библиотеки.
Нельзя использовать партикловые движки или готовые решения из интернета.
Можно делать на основании каких-то статей (но нужно их указать в документации)

## Для упрощения задачи

Это задача про многопоточность и структуры данных, а не про графику.
Визуальная красота не оценивается.
Используйте функцию platform::drawPoint. Не используйте графические API.
Все параметры для частиц можно оставить в коде. Но нужно это сделать аккуратно. Не нужно делать сериализацию / десериализацию.

## Что будет оцениваться

Эффективность структур данных с точки зрения памяти и скорости.
Эффективность использованных алгоритмов.
Сопровождаемость, читаемость и аккуратность кода.
Минимизация времени блокировок потоков. В идеале, хочется увидеть не блокирующие алгоритмы.
Производительность. Не строго. Просто не должно быть очевидных тормозов.

## Документация

Буквально 3-10 предложений.
Использовались ли какие-то статьи.
Чем руководствовались при принятии решений.
Общее описание реализации.
Если есть интересные решения - описать.
