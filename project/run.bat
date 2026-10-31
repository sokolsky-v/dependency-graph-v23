@echo off
chcp 65001 >nul

if "%~1"=="build" (
    g++ -std=c++17 -Wall -Wextra -o app.exe src/main.cpp src/config.cpp
    if %errorlevel% neq 0 (
        echo Ошибка компиляции!
        exit /b 1
    )
    echo Сборка успешна.
    exit /b 0
)

if "%~1"=="test" (
    g++ -std=c++17 -Wall -Wextra -o test_app.exe tests/test_config.cpp src/config.cpp
    if %errorlevel% neq 0 (
        echo Ошибка компиляции тестов!
        exit /b 1
    )
    test_app.exe
    exit /b 0
)

if "%~1"=="clean" (
    del /f /q app.exe test_app.exe 2>nul
    echo Очистка завершена.
    exit /b 0
)

if "%~1"=="" (
    echo Использование:
    echo   run.bat build   - собрать приложение
    echo   run.bat test    - собрать и запустить тесты
    echo   run.bat clean   - удалить скомпилированные файлы
    echo   run.bat config.json - запустить приложение с конфигом
    exit /b 0
)

rem Если передан файл - запускаем приложение
if exist app.exe (
    app.exe %~1
) else (
    echo Сначала выполните: run.bat build
    exit /b 1
)