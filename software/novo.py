import psycopg2
import matplotlib.pyplot as plt
from datetime import datetime

# Configurações da base de dados
DB_HOST = "localhost"
DB_NAME = "test"
DB_USER = "postgres"
DB_PASSWORD = "nova_senha"

# Função para buscar dados de temperatura para uma data específica
def get_temperature_data_for_date(date):
    try:
        conn = psycopg2.connect(
            host=DB_HOST, 
            database=DB_NAME, 
            user=DB_USER, 
            password=DB_PASSWORD
        )
        cursor = conn.cursor()

        query = """
        SELECT 
            injection_timestamp, 
            sensors_data->'samples'->0->>'temperature_1' AS temperature1
        FROM inovam_injection
        WHERE TO_CHAR(injection_timestamp, 'YYYY-MM-DD') = %s
        AND sensors_data->'samples'->0->>'temperature_1' IS NOT NULL
        ORDER BY injection_timestamp
        LIMIT 6;
        """
        cursor.execute(query, (date,))
        rows = cursor.fetchall()

        # Fechar a conexão
        cursor.close()
        conn.close()

        # Extrair dados de timestamp e temperatura
        timestamps = [row[0] for row in rows]
        temperatures = [float(row[1]) for row in rows]

        return timestamps, temperatures
    except Exception as e:
        print(f"Erro ao conectar com a base de dados: {e}")
        return [], []

# Função para gerar o gráfico
def plot_temperature_graph(date):
    timestamps, temperatures = get_temperature_data_for_date(date)

    if not timestamps:
        print("Aviso: Nenhum dado foi encontrado.")
        return

    plt.plot(timestamps, temperatures, marker='o', label='Temperature (°C)', color='red')
    plt.xlabel('Timestamp')
    plt.ylabel('Temperature (°C)')
    plt.title(f'Temperature Data for {date}')
    plt.xticks(rotation=45)
    plt.tight_layout()
    plt.show()

# Chama a função com a data desejada
plot_temperature_graph('2024-09-05')
