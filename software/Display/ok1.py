import csv
import time
import random
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import pandas as pd
from tkinter import Tk, messagebox

csv_file = 'dados_teste.csv'
max_points = 6  # Número máximo de pontos visíveis no gráfico

# Função para mostrar janelas de erro
def show_error_message(title, message):
    root = Tk()
    root.withdraw()  # Esconde a janela principal do Tkinter
    messagebox.showerror(title, message)
    root.destroy()  # Fecha a instância do Tkinter

# Função para gerar dados de sensores e gravar no CSV
def generate_and_update_csv():
    try:
        with open(csv_file, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(['Temperature', 'Pressure', 'Timestamp']) 

            while True:
                temperature = round(random.uniform(15, 35), 2)  # Exemplo: temperatura entre 15 e 35°C
                pressure = round(random.uniform(500, 1000), 2)  # Exemplo: pressão entre 500 e 1000 unidades
                timestamp = time.strftime('%Y-%m-%d %H:%M:%S')

                # Verificar valores inválidos
                if not (500 <= pressure <= 1000):
                    show_error_message("Erro de Pressão", f"Pressão inválida detectada: {pressure}")
                    continue
                if not (15 <= temperature <= 35):
                    show_error_message("Erro de Temperatura", f"Temperatura inválida detectada: {temperature}")
                    continue

                writer.writerow([temperature, pressure, timestamp])
                file.flush()  # Garantir que os dados são escritos imediatamente
                time.sleep(1)  # Atualizar a cada 1 segundo
    except Exception as e:
        show_error_message("Erro no CSV", f"Erro ao abrir ou gravar no arquivo CSV: {e}")

# Função para atualizar o gráfico de temperatura
def update_temperature_graph(i):
    try:
        data = pd.read_csv(csv_file)
        
        # Verificar se o arquivo está vazio
        if data.empty:
            show_error_message("Aviso", "O arquivo CSV está vazio. Aguardando novos dados.")
            return
        
        # Verificar se as colunas obrigatórias estão presentes
        required_columns = {'Temperature', 'Pressure', 'Timestamp'}
        if not required_columns.issubset(data.columns):
            show_error_message("Erro no CSV", "Colunas obrigatórias ausentes no arquivo CSV.")
            return
        
        # Manter apenas os últimos `max_points` valores
        data = data.tail(max_points)
        
        x = data['Timestamp']
        y1 = data['Temperature']

        ax_temp.clear()
        ax_temp.plot(x, y1, marker='o', label='Temperature (°C)', color='red')

        ax_temp.set_title('Real-time Temperature Data')
        ax_temp.set_xlabel('Timestamp')
        ax_temp.set_ylabel('Temperature (°C)')
        ax_temp.legend()
        plt.xticks(rotation=45, ha='right')
        plt.subplots_adjust(bottom=0.30)

    except pd.errors.EmptyDataError:
        show_error_message("Erro no CSV", "Arquivo CSV está vazio ou corrompido.")
    except FileNotFoundError:
        show_error_message("Erro no CSV", "Arquivo CSV não encontrado.")
    except Exception as e:
        show_error_message("Erro", f"Erro inesperado ao atualizar o gráfico de temperatura: {e}")

# Função para atualizar o gráfico de pressão
def update_pressure_graph(i):
    try:
        data = pd.read_csv(csv_file)
        
        # Verificar se o arquivo está vazio
        if data.empty:
            show_error_message("Aviso", "O arquivo CSV está vazio. Aguardando novos dados.")
            return
        
        # Verificar se as colunas obrigatórias estão presentes
        required_columns = {'Temperature', 'Pressure', 'Timestamp'}
        if not required_columns.issubset(data.columns):
            show_error_message("Erro no CSV", "Colunas obrigatórias ausentes no arquivo CSV.")
            return
        
        # Manter apenas os últimos `max_points` valores
        data = data.tail(max_points)
        
        x = data['Timestamp']
        y2 = data['Pressure']

        ax_pres.clear()
        ax_pres.plot(x, y2, marker='o', label='Pressure', color='blue')

        ax_pres.set_title('Real-time Pressure Data')
        ax_pres.set_xlabel('Timestamp')
        ax_pres.set_ylabel('Pressure')
        ax_pres.legend()
        plt.xticks(rotation=45, ha='right')
        plt.subplots_adjust(bottom=0.30)

    except pd.errors.EmptyDataError:
        show_error_message("Erro no CSV", "Arquivo CSV está vazio ou corrompido.")
    except FileNotFoundError:
        show_error_message("Erro no CSV", "Arquivo CSV não encontrado.")
    except Exception as e:
        show_error_message("Erro", f"Erro inesperado ao atualizar o gráfico de pressão: {e}")

# Criar thread para gerar dados em paralelo
data_thread = threading.Thread(target=generate_and_update_csv)
data_thread.daemon = True
data_thread.start()

# Configuração dos gráficos
fig_temp, ax_temp = plt.subplots()
fig_pres, ax_pres = plt.subplots()

try:
    ani_temp = animation.FuncAnimation(fig_temp, update_temperature_graph, interval=1000)  # Atualizar a cada 1 segundo
    ani_pres = animation.FuncAnimation(fig_pres, update_pressure_graph, interval=1000)  # Atualizar a cada 1 segundo
    plt.show()
except Exception as e:
    show_error_message("Erro no Gráfico", f"Erro ao exibir os gráficos: {e}")
