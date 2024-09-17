/**
 * @file Arcade_Timer.ino
 * @author Радионов Тимофей (rtv2506@yandex.ru)
 * @brief Таймер Ардуино UNO для игрового автомата "Аркада"
 * @version 0.3
 * @date 2024-04-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <GyverMAX7219.h>
#include <RunningGFX.h>
/// @defgroup Назначение пинов Ардуино
#define RF_INCREASE PIN_A0  // Кнопка "+" (Ардуино)
#define RF_PLAYPAUSE PIN_A1 // Кнопка "Play/Pause" (Ардуино)
#define RF_STOP PIN_A2      // Кнопка "Stop" (Ардуино)
#define MATRIX_CLK 2        // Таймер CLK
#define MATRIX_CS 3         // Матрица CS
#define MATRIX_DIO 4        // Таймер DIO
#define R_IN1to6_BUTTONS 5  // Реле IN1-IN6
#define R_IN7_PLAYPAUSE 6   // Реле IN7
#define R_IN8_STOP 7        // Реле IN8
#define LED PIN_SPI_SCK     // LED-индикатор
/// @brief Класс таймера
class Timer
{
public:
    Timer() {}
    /// @brief Проверка тика таймера
    /// @param ms количество мс, которое должно пройти, чтоб произошел тик
    /// @return true / false
    bool isTicked(int ms)
    {
        uint32_t current = millis();
        if (current - _previous >= ms)
        {
            refresh();
            return true;
        }
        return false;
    }
    /// @brief Обновление счетчика таймера
    void refresh()
    {
        _previous = millis();
    }
    /// @brief Проверка сброса таймера
    /// @param ms время до сброса, мс
    /// @return true / false
    bool isReset(int sec)
    {
        uint32_t current = millis();
        float end = sec * 60000;
        if (current - _previous >= end)
            return true;
        return false;
    }

private:
    uint32_t _previous;
};
/// @brief Класс кнопки
class Button
{
public:
    Button(byte pin)
    {
        _pin = pin;
        pinMode(_pin, INPUT);
    }
    /// @brief Проверка нажатия кнопки
    /// @return true / false
    bool isClicked()
    {
        delay(30);
        bool curState = digitalRead(_pin);
        // Нажата
        if (curState && !_prevState)
        {
            _prevState = curState;
            return false;
        }
        // Отпущена
        else if (!curState && _prevState)
        {
            _prevState = curState;
            return true;
        }
        return false;
    }

private:
    byte _pin;
    bool _prevState = false;
};
/// @brief Структура матричных символов
struct MatrixSymbol
{
    int array[8][8];

    static MatrixSymbol convertSymbol(char symbol)
    {
        if (symbol == 0)
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 1)
        {
            return {{
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
            }};
        }
        else if (symbol == 2)
        {
            return {{
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 3)
        {
            return {{
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 4)
        {
            return {{
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
            }};
        }
        else if (symbol == 5)
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 6)
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 0},
                {1, 0, 0, 0, 0},
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 7)
        {
            return {{
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
            }};
        }
        else if (symbol == 8)
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == 9)
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {1, 1, 1, 1, 1},
            }};
        }
        else if (symbol == ':')
        {
            return {{
                {0, 0},
                {1, 1},
                {1, 1},
                {0, 0},
                {0, 0},
                {1, 1},
                {1, 1},
                {0, 0},
            }};
        }
        else if (symbol == '$')
        {
            return {{
                {0, 1, 1, 1, 1, 1},
                {0, 1, 0, 0, 0, 1},
                {0, 1, 0, 0, 0, 1},
                {0, 1, 1, 1, 1, 1},
                {0, 1, 0, 0, 0, 0},
                {1, 1, 1, 1, 0, 0},
                {0, 1, 0, 0, 0, 0},
                {0, 1, 0, 0, 0, 0},
            }};
        }
        else if (symbol == '?')
        {
            return {{
                {1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1},
                {0, 0, 0, 0, 1},
                {0, 0, 1, 1, 1},
                {0, 0, 1, 0, 0},
                {0, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
                {0, 0, 1, 0, 0},
            }};
        }
    }
    static int getHeight()
    {
        return 8;
    }
    static int getWidth()
    {
        return 8;
    }
};
/// @brief Структура матричных символов (на всю матрицу)
struct MatrixText
{
    int array[8][33]; // 33 ???!!! 32 ломается вывод матрицы

    static MatrixText convertText(char *text)
    {
        if (text == "ИГРА")
            return {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
        else if (text == "ПАУЗА")
            return {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
        else if (text == "КОНЕЦ")
            return {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
                     {0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0},
                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
    }
    static int getHeight()
    {
        return 8;
    }
    static int getWidth()
    {
        return 32;
    }
};
/// @brief Области вывода символов на матрице (ось X)
enum class MatrixZone
{
    Number1 = 2,
    Number2 = 9,
    Colon = 15,
    Number3 = 18,
    Number4 = 25,
    Ruble = 1,
    Question = 9
};
/// @defgroup Пользовательская конфигурация
int time[] = {0, 0};                         // Время основное: { мин, сек }
int time_start = 5;                          // Задержка перед началом: сек (0-9)
int time_wait = 15;                          // Время ожидания: сек
int time_reset = 1;                          // Время сброса: мин
int time_step = 1;                           // Шаг добавочного времени: мин
char *text_display = "БТБС т. +79233549295"; // Основной текст дисплея (в простое)
/// @defgroup Переменные
MAX7219<4, 1, MATRIX_CS, MATRIX_DIO, MATRIX_CLK> matrix; // Матрица 4 на 1 плитку
RunningGFX matrix_run(&matrix);                          // Бегущая строка матрицы
Button b_increase(RF_INCREASE);                          // Кнопка "+5"
Button b_playpause(RF_PLAYPAUSE);                        // Кнопка "Play/Pause"
Button b_stop(RF_STOP);                                  // Кнопка "Stop"
Timer timer;                                             // Таймер для тиков
bool relayLow = true;                                    // Флаг: тип активации реле = low level
bool relayHigh = !relayLow;                              // Флаг: тип активации реле = high level
int min = time[0];                                       // Минуты таймера
int sec = time[1];                                       // Секунды таймера
bool start = false;                                      // Флаг: запуск
bool activated = false;                                  // Флаг: активность основного таймера
bool waited = false;                                     // Флаг: активность ожидающего таймера
/// @brief Инициализирующие настройки Ардуино
void setup()
{
    Serial.begin(115200);
    // Инициализация режимов работы пинов
    pinMode(RF_INCREASE, INPUT);
    pinMode(RF_PLAYPAUSE, INPUT);
    pinMode(RF_STOP, INPUT);
    pinMode(MATRIX_CLK, OUTPUT);
    pinMode(MATRIX_DIO, OUTPUT);
    pinMode(MATRIX_CS, OUTPUT);
    pinMode(R_IN1to6_BUTTONS, OUTPUT);
    pinMode(R_IN7_PLAYPAUSE, OUTPUT);
    pinMode(R_IN8_STOP, OUTPUT);
    pinMode(LED, OUTPUT);
    // Деактивация всех реле
    relayDeactivate(R_IN1to6_BUTTONS);
    relayDeactivate(R_IN7_PLAYPAUSE);
    relayDeactivate(R_IN8_STOP);
    // Инициализация матрицы
    matrix.begin();
    matrix.setBright(7);
    // matrix.setFlip(true, false); // для wokwi.com
    // Настройка бегущей строки
    matrix_run.setSpeed(5);
    matrix_run.setText(text_display);
    matrix_run.start();
}
/// @brief Главная функция Ардуино
void loop()
{
    // Отслеживание нажатий кнопок
    if (b_increase.isClicked())
        action(RF_INCREASE);
    else if (b_playpause.isClicked())
        action(RF_PLAYPAUSE);
    else if (b_stop.isClicked())
        action(RF_STOP);
    // Тик таймера
    tick(1000);
}
/// @brief Выполнить действие по заданному сигналу
/// @param signal Номер сигнала
void action(int signal)
{
    switch (signal)
    {
    case RF_INCREASE:
        Serial.println("--- RF_INCREASE ---");
        // Активен таймер ожидания?
        if (waited)
            break;
        min += time_step;
        // Отображение времени на дисплее
        matrixPrintTime();
        // Обновление таймера
        timer.refresh();
        break;
    case RF_PLAYPAUSE:
        Serial.println("--- RF_PLAYPAUSE ---");
        // PLAY (START)
        if (!start)
        {
            Serial.println("START");
            // Таймер не настроен? (00:00)
            if (min == 0 && sec == 0)
                break;
            // Автомат - снимаем с паузы
            relayButtonClick(R_IN7_PLAYPAUSE);
            Serial.println("R_IN7_PLAYPAUSE clicked");
            // Автомат - замыкаем кнопки
            relayActivate(R_IN1to6_BUTTONS);
            Serial.println("R_IN1to6_BUTTONS activated");
            // Отображение задержки перед началом
            matrixPrintStart();
            // Флаги
            start = true;
            activated = true;
            waited = false;
        }
        // PLAY -> PAUSE
        else if (activated && !waited)
        {
            Serial.println("PLAY->PAUSE");
            // Автомат - ставим на паузу
            relayButtonClick(R_IN7_PLAYPAUSE);
            Serial.println("R_IN7_PLAYPAUSE clicked");
            // Автомат - размыкаем кнопки
            relayDeactivate(R_IN1to6_BUTTONS);
            Serial.println("R_IN1to6_BUTTONS deactivated");
            // Вывод текста на дисплей
            matrixPrintText("ПАУЗА");
            // Флаги
            activated = false;
            // Обновление таймера
            timer.refresh();
        }
        // PAUSE -> PLAY
        else if (!activated && !waited)
        {
            Serial.println("PAUSE->PLAY");
            // Автомат - снимаем с паузы
            relayButtonClick(R_IN7_PLAYPAUSE);
            Serial.println("R_IN7_PLAYPAUSE clicked");
            // Автомат - замыкаем кнопки
            relayActivate(R_IN1to6_BUTTONS);
            Serial.println("R_IN1to6_BUTTONS activated");
            // Отображение задержки перед началом
            matrixPrintStart();
            // Флаги
            activated = true;
        }
        // PLAY -> PAUSE if waited
        else if (waited && !activated)
        {
            Serial.println("PLAY->PAUSE and waiter");
            // Автомат - ставим на паузу
            relayButtonClick(R_IN7_PLAYPAUSE);
            Serial.println("R_IN7_PLAYPAUSE clicked");
            // Автомат - размыкаем кнопки
            relayDeactivate(R_IN1to6_BUTTONS);
            Serial.println("R_IN1to6_BUTTONS deactivated");
            // Перевод таймера в ожидающий режим
            activated = true;
            sec = time_wait;
        }
        // PAUSE -> PLAY if waited
        else if (waited && activated)
        {
            Serial.println("PAUSE->PLAY and waiter");
            // Сброс переменных
            start = false;
            activated = false;
            waited = false;
            min = time[0];
            sec = time[1];
        }
        break;
    case RF_STOP:
        Serial.println("--- RF_STOP ---");
        // Автомат - останавливаем
        relayButtonClick(R_IN8_STOP);
        Serial.println("R_IN8_STOP clicked");
        // Автомат - размыкаем кнопки
        relayDeactivate(R_IN1to6_BUTTONS);
        Serial.println("R_IN1to6_BUTTONS deactivated");
        // Вывод текста на дисплей
        matrixPrintText("КОНЕЦ");
        // Сброс переменных
        start = false;
        activated = false;
        waited = false;
        min = time[0];
        sec = time[1];
        // Очистка дисплея матрицы
        delay(5000);
        matrix.clear();
        break;
    }
}
/// @brief Тик таймера и его логика
/// @param delay_ms Задержка тика в миллисекундах
void tick(int delay_ms)
{
    // Таймер активен?
    if (activated)
    {
        // Задержка
        if (!timer.isTicked(delay_ms))
            return;
        // Отображение на дисплее таймера
        if (!waited)
            matrixPrintTime();
        else
            matrixPrintWaitingTime();
        // Счетчик времени
        if (min > 0)
        {
            sec--;
            if (sec < 0)
            {
                min--;
                sec = 59;
            }
        }
        else
        {
            if (sec > 0)
                sec--;
            // Время достигло 00:00
            else
            {
                if (!waited)
                {
                    waited = true;
                    activated = false;
                    action(RF_PLAYPAUSE);
                }
                else
                {
                    activated = false;
                    action(RF_STOP);
                }
            }
        }
    }
    else
    {
        // Таймер в начальном состоянии? (00:00)
        if (min == time[0] && sec == time[1])
            matrix_run.tick();
        // Прошло глобальное время сброса таймера?
        else if (!timer.isReset(time_reset))
            return;
        // Сброс автомата
        else
            action(RF_STOP);
    }
}
/// @brief Активация реле
/// @param relay PIN управления реле
void relayActivate(int relay)
{
    if (relayLow)
        digitalWrite(relay, LOW);
    else if (relayHigh)
        digitalWrite(relay, HIGH);
}
/// @brief Деактивация реле
/// @param relay PIN управления реле
void relayDeactivate(int relay)
{
    if (relayLow)
        digitalWrite(relay, HIGH);
    else if (relayHigh)
        digitalWrite(relay, LOW);
}
/// @brief Имитация нажатия кнопки реле
/// @param relay PIN управления реле
void relayButtonClick(int relay)
{
    relayActivate(relay);
    delay(1000);
    relayDeactivate(relay);
}
/// @brief Вывод текста задержки перед началом
void matrixPrintStart()
{
    MatrixText text_mt = MatrixText::convertText("ИГРА");
    MatrixSymbol number4_md;
    for (int counter = 5; counter > 0; counter--)
    {
        matrix.clear();
        number4_md = MatrixSymbol::convertSymbol(counter);
        for (int i = 0; i < MatrixText::getHeight(); i++)
        {
            for (int j = 0; j < MatrixText::getWidth(); j++)
            {
                if (text_mt.array[i][j] == 1)
                    matrix.dot(j, i);
                if (number4_md.array[i][j] == 1)
                    matrix.dot(j + (int)MatrixZone::Number4, i);
            }
        }
        matrix.update();
        delay(1000);
    }
}
/// @brief Вывод времени на дисплей матрицы
void matrixPrintTime()
{
    matrix.clear();
    // Разделяем время на отдельные цифры
    int number1 = min / 10;
    int number2 = min % 10;
    int number3 = sec / 10;
    int number4 = sec % 10;
    // Конвертируем цифры в их матричный вид
    MatrixSymbol number1_md = MatrixSymbol::convertSymbol(number1);
    MatrixSymbol number2_md = MatrixSymbol::convertSymbol(number2);
    MatrixSymbol colon_md = MatrixSymbol::convertSymbol(':');
    MatrixSymbol number3_md = MatrixSymbol::convertSymbol(number3);
    MatrixSymbol number4_md = MatrixSymbol::convertSymbol(number4);
    // Выводим точки цифр на матрицу
    for (int i = 0; i < MatrixSymbol::getHeight(); i++)
    {
        for (int j = 0; j < MatrixSymbol::getWidth(); j++)
        {
            // Минуты
            if (number1_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number1, i);
            if (number2_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number2, i);
            // Двоеточие
            if (sec % 2 == 0 && colon_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Colon, i);
            // Секунды
            if (number3_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number3, i);
            if (number4_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number4, i);
        }
    }
    matrix.update();
}
/// @brief Вывод времени ожидания на дисплей матрицы
void matrixPrintWaitingTime()
{
    matrix.clear();
    // Разделяем время на отдельные цифры
    int number3 = sec / 10;
    int number4 = sec % 10;
    // Конвертируем цифры в их матричный вид
    MatrixSymbol number1_md = MatrixSymbol::convertSymbol('$');
    MatrixSymbol number2_md = MatrixSymbol::convertSymbol('?');
    MatrixSymbol number3_md = MatrixSymbol::convertSymbol(number3);
    MatrixSymbol number4_md = MatrixSymbol::convertSymbol(number4);
    // Выводим точки цифр на матрицу
    for (int i = 0; i < MatrixSymbol::getHeight(); i++)
    {
        for (int j = 0; j < MatrixSymbol::getWidth(); j++)
        {
            // Минуты
            if (number1_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Ruble, i);
            if (number2_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Question, i);
            // Секунды
            if (number3_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number3, i);
            if (number4_md.array[i][j] == 1)
                matrix.dot(j + (int)MatrixZone::Number4, i);
        }
    }
    matrix.update();
}
/// @brief Вывод текста на дисплей матрицы
/// @param text Выводимый текст
void matrixPrintText(char *text)
{
    matrix.clear();
    // Конвертируем текст в матричный вид
    MatrixText text_mt = MatrixText::convertText(text);
    // Выводим точки текста на матрицу
    for (int i = 0; i < MatrixText::getHeight(); i++)
    {
        for (int j = 0; j < MatrixText::getWidth(); j++)
        {
            if (text_mt.array[i][j] == 1)
                matrix.dot(j, i);
        }
    }
    matrix.update();
}