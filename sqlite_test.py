import sqlite3
import uuid
import time

# Определяем класс для хранения данных
class DataObject:
    def __init__(self, name, value):
        self.name = name
        self.value = value

# Функция для создания таблицы в базе данных
def create_table(cursor):
    cursor.execute('''CREATE TABLE IF NOT EXISTS data (
                        name TEXT,
                        value TEXT
                    )''')
    cursor.execute("DELETE FROM data")

# Функция для генерации данных
def generate_data(num_objects):
    data_objects = []
    for _ in range(num_objects):
        name = str(uuid.uuid4())  # Генерация случайного UUID
        value = str(uuid.uuid4())
        data_objects.append(DataObject(name, value))
    return data_objects

# Функция для вставки данных в базу данных
def insert_data(conn, cursor, data_objects):
    start_time = time.time()
    for obj in data_objects:
        cursor.execute("INSERT INTO data (name, value) VALUES (?, ?)", (obj.name, obj.value))
    conn.commit()
    end_time = time.time()
    return end_time - start_time
# Функция для выполнения SQL-запроса с измерением времени
def execute_query(conn,cursor, query):
    start_time = time.time()
    res = cursor.execute(query)
    res.fetchall()
    end_time = time.time()
    return end_time - start_time

# Основная функция
def main():
    # Подключение к базе данных
    conn = sqlite3.connect('History')
    cursor = conn.cursor()

    # Создание таблицы, если её ещё нет
    create_table(cursor)

    # Генерация данных
    data_objects = generate_data(50000)

    # Вставка данных в базу
    execution_time = insert_data(conn, cursor, data_objects)
    print(f"Time taken for query execution: {execution_time} seconds")

    # Выполнение запросов
    queries = [
        "SELECT * FROM data",
    ]
    for query in queries:
        execution_time = execute_query(conn, cursor, query)
        print(f"Time taken for query execution: {execution_time} seconds")

    conn.close()

if __name__ == "__main__":
    main()