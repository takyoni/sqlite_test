#include <iostream>
#include "TestObj.h"
#include "sqlite3.h"
#include <windows.h>
#include <string> 
#include "Timer.h"
using namespace std;

// Функция обратного вызова для вывода данных из запроса
static int my_special_callback(void* unused, int count, char** data, char** columns)
{
    int idx;

    for (idx = 0; idx < count; idx++) {
        printf("The data in column \"%s\" is: %s\n", columns[idx], data[idx]);
    }

    printf("\n");

    return 0;
}

// Генерация нового GUID
static string GetNewGUID()
{
    UUID uuid = { 0 };
    string guid;

    // Создаем uuid
    UuidCreate(&uuid);

    // Конвертируем uuid в строку
    RPC_CSTR szUuid = NULL;
    if (UuidToStringA(&uuid, &szUuid) == RPC_S_OK)
    {
        guid = (char*)szUuid;
        RpcStringFreeA(&szUuid);
    }

    return guid;
}

// Открытие БД SQLite
static sqlite3* OpenDatabase()
{
    sqlite3* Database;
    int openResult = sqlite3_open16(L"../History", &Database);
    if (openResult != SQLITE_OK) {
        wcout << L"Ошибка открытия БД" << endl;
    }
    return Database;
}

// Создание таблицы
static void CreateTable(sqlite3* Database)
{
    char* errmsg;
    char sql[] = "CREATE TABLE IF NOT EXISTS test("
        "name TEXT NOT NULL,"
        "value TEXT NOT NULL);";
    // Создаем таблицу
    int execResult = sqlite3_exec(Database, sql, NULL, NULL, &errmsg);
    if (execResult != SQLITE_OK) {
        cout << errmsg << endl;
        wcout << L"Ошибка выполнения запроса" << endl;
    }
}

static void ExecuteQuery(sqlite3* Database, const char* query)
{
    char* errmsg;
    Timer timer;

    timer.start();
    sqlite3_stmt* pStatement;
    int execResult = sqlite3_prepare_v2(Database, query, -1, &pStatement, NULL);
    if (execResult != SQLITE_OK) {
        wcout << L"Ошибка подготовки запроса" << endl;
        return;
    }

    while (sqlite3_step(pStatement) == SQLITE_ROW) {
        // Чтение данных здесь, если это необходимо
    }
    timer.stop();

    std::cout << "Query time: " << query << std::endl;
    std::cout << "Seconds: " << timer.elapsedSeconds() << std::endl;
}

static void InsertData(sqlite3* Database, int num_records)
{
    // Генерируем и вставляем тестовые данные
    TestObj* rows = new TestObj[num_records];
    for (int i = 0; i < num_records; i++) {
        rows[i] = TestObj();
    }

    Timer timer;
    sqlite3_stmt* res;  // компилируемое выражение
    int rc = sqlite3_prepare_v2(Database, "INSERT INTO test (name, value) VALUES (?, ?)", -1, &res, 0);

    timer.start();
    sqlite3_exec(Database, "BEGIN;", nullptr, nullptr, nullptr);
    for (int i = 0; i < num_records; i++) {
        sqlite3_bind_text(res, 1, rows[i].Name, -1, SQLITE_STATIC);
        sqlite3_bind_text(res, 2, rows[i].Value, -1, SQLITE_STATIC);
        // выполняем выражение
        int step = sqlite3_step(res);
        // если выражение успешно выполнено
        if (step == SQLITE_ERROR)
        {
            printf("Error\n");
        }
        sqlite3_clear_bindings(res);
        sqlite3_reset(res);
    }
    sqlite3_exec(Database, "COMMIT;", nullptr, nullptr, nullptr);
    timer.stop();

    std::cout << "Insert time for " << num_records << " records:" << std::endl;
    std::cout << "Seconds: " << timer.elapsedSeconds() << std::endl;
    delete[] rows;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    // Открываем БД
    sqlite3* Database = OpenDatabase();

    // Создаем таблицу
    CreateTable(Database);

    // Вставляем данные для разного количества записей
    int num_records[] = { 1000, 10000, 20000, 50000, 100000 };
    for (int i = 0; i < sizeof(num_records) / sizeof(num_records[0]); ++i) {
        InsertData(Database, num_records[i]);
    }

    // Выполняем запросы SELECT и измеряем время выполнения
    for (int i = 0; i < sizeof(num_records) / sizeof(num_records[0]); ++i) {
        string query = "SELECT * FROM test LIMIT " + to_string(num_records[i]);
        ExecuteQuery(Database, query.c_str());
    }

    // Удаляем данные из таблицы
    char* errmsg;
    int execResult6 = sqlite3_exec(Database, "DELETE FROM test", NULL, NULL, &errmsg);
    if (execResult6 != SQLITE_OK) {
        cout << errmsg << endl;
        wcout << L"Ошибка выполнения запроса" << endl;
    }

    // Закрываем соединение с БД
    sqlite3_close(Database);
    return 0;
}
