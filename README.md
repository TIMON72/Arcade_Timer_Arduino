# Arcade_Timer_Arduino
 Таймер для аркадного автомата
## Версии
- 0.2 - Arduino Nano с сегментным таймером + Реле 8-ми канальное + RF-модуль
- 0.3 - Arduino UNO с LED-таймером + Реле 8-ми канальное + RF-модуль
- 1.1 - ESP8266 UNO D1 с LED-таймером + Реле 8-ми канальное + RF-модуль
## Инструкция
Установка времени и запуск таймера по следующему алгоритму при помощи брелка для RF-модуля (433 МГц):
1) Нажать на пульте кнопку «А» для настройки времени. Повторное нажатие кнопки «А» добавляет 5 минут. Нажатие кнопки «С» сбросит таймер и можно будет повторить настройку времени.
2) После настройки времени нажать кнопку «B» для запуска игры. Появится надпись «ИГРА» и обратный отсчет 5 секунд, после чего станет возможным запускать игры на автомате.
3) В процессе игры можно нажать кнопку «А» и будет добавлено 5 минут. Если нажать кнопку «B», то автомат и таймер встанет на паузу, а повторное нажатие снимет паузу. Если нажать кнопу «C», то произойдет выход из игры, таймер сбросится и автомат вернется к исходному состоянию.
4) По истечении времени процесс игры будет поставлен на паузу и появится надпись на таймере «₽?», начнется обратный отсчет 60 секунд. В течение этого времени можно нажать кнопку «B» и тогда на таймере отобразится время «05:00», далее можно добавить время (шаг 1) и запустить автомат (шаг 2). Если ничего не предпринимать, то по истечению времени сработает команда сброса таймера и выход из игры, автомат вернется к исходному состоянию.
Таймер и автомат автоматически сбрасывается через 5 минут, если не будет завершен какой-либо шаг алгоритма

Управление брелком:
* А – добавить 5 минут
* B – пуск / пауза
* C – сброс

