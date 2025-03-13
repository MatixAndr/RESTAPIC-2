# RESTAPIC

Prosty serwer REST API napisany w C++ z wykorzystaniem WinSock2 do obsługi połączeń sieciowych oraz zintegrowanym frameworkiem testowym z kolorowym podsumowaniem wyników.

| Build Status | Windows Build | License | Last Commit | Issues |
|--------------|---------------|---------|-------------|--------|
| ![Build Status](https://github.com/matixandr/RESTAPIC-2/actions/workflows/cmake-single-platform.yml/badge.svg) | ![Windows Build](https://github.com/matixandr/RESTAPIC-2/actions/workflows/cmake-single-platform.yml/badge.svg   ?branch=master&event=push) | ![License](https://img.shields.io/github/license/MatixAndr09/RESTAPIC) | ![Last Commit](https://img.shields.io/github/last-commit/MatixAndr09/RESTAPIC) | ![Issues](https://img.shields.io/github/issues/MatixAndr09/RESTAPIC) |

## Opis

RESTAPIC to lekki, wydajny serwer REST API napisany w C++ z użyciem WinSock2. Projekt implementuje operacje CRUD (Create, Read, Update, Delete) dla zarządzania użytkownikami, a także posiada zintegrowany framework testowy z kolorowym podsumowaniem wyników.

Główne cechy projektu:
- Serwer HTTP nasłuchujący na porcie **8888**
- Implementacja operacji CRUD dla użytkowników (GET, POST, PATCH, DELETE)
- Obsługa żądań HTTP w formacie JSON
- Zintegrowany framework testowy z kolorowym raportem wyników (zielony dla sukcesu, czerwony dla błędu)
- Kompilacja przy użyciu CMake

## Endpointy API

- `GET /users` – pobranie listy wszystkich użytkowników
- `GET /users/{id}` – pobranie szczegółów konkretnego użytkownika
- `POST /users` – utworzenie nowego użytkownika
- `PATCH /users/{id}` – częściowa aktualizacja danych użytkownika
- `DELETE /users/{id}` – usunięcie użytkownika

## Wymagania

- System Windows
- Kompilator C++ wspierający C++17
- CMake (minimum wersja 3.10)
- WinSock2 (wbudowany w Windows SDK)

## Instalacja i uruchomienie

1. Sklonuj repozytorium:
   ```bash
   git clone https://github.com/MatixAndr09/RESTAPIC.git
   cd RESTAPIC
   ```
2. Skonfiguruj projekt przy użyciu CMake:
   ```bash
    cmake -S . -B build
   ```
3. Skompiluj  projekt: 
   ```bash
   cmake --build build
   ```
4. Aby uruchomić serwer, wejdź do katalogu `build` i uruchom:
   ```bash
    RESTAPIC.exe
    ```
5. Aby uruchomić testy (zintegrowane z głównym plikiem wykonywalnym), użyj:
    ```bash
     RESTAPIC.exe --test [u|i|all]
     ```
    gdzie:
    - `u` – uruchamia testy jednostkowe,
    - `i` – uruchamia testy integracyjne,
    - `all` – uruchamia wszystkie testy.