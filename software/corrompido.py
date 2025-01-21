import csv
import time
import os
from tkinter import Tk, messagebox

csv_file = 'dados_teste.csv'

# Função para mostrar janelas de erro
def show_error_message(title, message):
    root = Tk()
    root.withdraw()  # Esconde a janela principal do Tkinter
    messagebox.showerror(title, message)
    root.destroy()  # Fecha a instância do Tkinter

# Função para gerar dados de sensores e gravar no CSV com valores nulos
def generate_and_update_csv():
    try:
        if not os.path.exists(csv_file):
            # Criar o arquivo CSV com dados nulos
            with open(csv_file, mode='w', newline='') as file:
                writer = csv.writer(file)
                writer.writerow(['Temperature', 'Pressure', 'Timestamp'])
                for _ in range(10):  # Gerar 10 linhas com dados nulos
                    writer.writerow([None, None, None])
                    time.sleep(1)  # Simula um pequeno delay
        else:
            print("Erro: O arquivo já existe.")
    except Exception as e:
        print(f"Erro ao criar ou gravar no arquivo CSV: {e}")

# Função para verificar se o arquivo CSV está vazio ou corrompido
def check_csv_file():
    try:
        if not os.path.exists(csv_file):
            show_error_message("Erro no CSV", "Arquivo CSV não encontrado.")
            return
        
        with open(csv_file, mode='r') as file:
            reader = csv.reader(file)
            data = list(reader)

            # Verificar se o arquivo contém apenas os cabeçalhos ou dados nulos
            if len(data) <= 1 or all(all(cell is None for cell in row) for row in data[1:]):
                show_error_message("Erro no CSV", "Arquivo CSV está vazio ou corrompido.")
    except Exception as e:
        show_error_message("Erro no CSV", f"Erro ao abrir ou ler o arquivo CSV: {e}")

# Gerar o arquivo CSV com dados nulos
generate_and_update_csv()

# Verificar se o CSV está vazio ou corrompido
check_csv_file()
