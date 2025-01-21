import csv
import time
import random
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd

csv_file = 'dados_teste.csv'

# Função para atualizar dados de sensores e gravar no CSV
def generate_and_update_csv():
    with open(csv_file, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['Temperature', 'Pressure', 'Timestamp'])  # Cabeçalhos

        while True:
            temperature = round(random.uniform(20, 30), 2)  # Exemplo: temperatura entre 20 e 30°C
            pressure = round(random.uniform(1, 10), 2)      # Exemplo: pressão entre 1 e 10 unidades
            timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
            writer.writerow([temperature, pressure, timestamp])
            file.flush()  # Garantir que os dados são escritos imediatamente
            time.sleep(1)  # Atualizar a cada 1 segundo

# Função para atualizar o gráfico em tempo real
def update_graph(i):
    try:
        data = pd.read_csv(csv_file)
        x = data['Timestamp']
        y1 = data['Temperature']
        y2 = data['Pressure']

        ax1.clear()
        ax1.plot(x, y1, label='Temperature (°C)')
        ax1.plot(x, y2, label='Pressure')

        plt.xticks(rotation=45, ha='right')
        plt.subplots_adjust(bottom=0.30)
        plt.title('Real-time Sensor Data')
        plt.xlabel('Timestamp')
        plt.ylabel('Sensor Values')
        plt.legend()
    except pd.errors.EmptyDataError:
        pass

# Criar thread para gerar dados em paralelo
data_thread = threading.Thread(target=generate_and_update_csv)
data_thread.daemon = True
data_thread.start()

# Configuração do gráfico
fig, ax1 = plt.subplots()
ani = animation.FuncAnimation(fig, update_graph, interval=1000)  # Atualizar a cada 1 segundo

plt.show()
