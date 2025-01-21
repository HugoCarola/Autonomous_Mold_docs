import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk  
import psycopg2
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates
from datetime import datetime

# Configurações da base de dados
DB_HOST = "192.168.1.102"
DB_NAME = "test"
DB_USER = "postgres"
DB_PASSWORD = "nova_senha"

# Função para obter os últimos 6 dados do banco de dados
def get_data_from_db():
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
            sensors_data->'samples'->0->>'temperature_1' AS temperature1,
            sensors_data->'samples'->0->>'temperature_3' AS temperature3,
            sensors_data->'samples'->0->>'pressure_3' AS pressure3,
            production_order
        FROM inovam_injection
        ORDER BY injection_timestamp DESC
        LIMIT 6;
        """
        cursor.execute(query)
        rows = cursor.fetchall()

        # Fechar a conexão
        cursor.close()
        conn.close()

        # Extrair dados
        timestamps = [row[0] for row in rows]
        temperatures1 = [float(row[1]) for row in rows]
        temperatures3 = [float(row[2]) for row in rows]
        pressures3 = [float(row[3]) for row in rows]
        production_order = rows[0][4] if rows else "N/A"

        return timestamps, temperatures1, temperatures3, pressures3, production_order
    except Exception as e:
        print(f"Erro ao conectar com a base de dados: {e}")
        return [], [], [], [], "N/A"

# Função para obter os últimos 6 dados do banco de dados
def get_data_from_db():
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
            sensors_data->'samples'->0->>'temperature_1' AS temperature1,
            sensors_data->'samples'->0->>'temperature_3' AS temperature3,
            sensors_data->'samples'->0->>'pressure_3' AS pressure3,
            production_order
        FROM inovam_injection
        ORDER BY injection_timestamp DESC
        LIMIT 6;
        """
        cursor.execute(query)
        rows = cursor.fetchall()

        # Fechar a conexão
        cursor.close()
        conn.close()

        # Extrair dados
        timestamps = [row[0] for row in rows]
        temperatures1 = [float(row[1]) for row in rows]
        temperatures3 = [float(row[2]) for row in rows]
        pressures3 = [float(row[3]) for row in rows]
        production_order = rows[0][4] if rows else "N/A"

        return timestamps, temperatures1, temperatures3, pressures3, production_order
    except Exception as e:
        print(f"Erro ao conectar com a base de dados: {e}")
        return [], [], [], [], "N/A"


# Função para atualizar o gráfico
def update_graph(i):
    timestamps, temperatures1, temperatures3, pressures3, production_order = get_data_from_db()

    if not timestamps:
        print("Aviso: Nenhum dado foi encontrado.")
        return

    ax_temp.clear()

    # Plotando pressure_3 no eixo Y esquerdo
    ax_temp.plot(timestamps, pressures3, marker='o', color='blue', label='Pressure (bar)')
    ax_temp.set_ylabel('Pressure (bar)', color='blue')
    ax_temp.tick_params(axis='y', labelcolor='blue')

    # Criando um segundo eixo Y para temperature_1 no lado direito
    ax_temp2 = ax_temp.twinx()
    ax_temp2.plot(timestamps, temperatures1, marker='o', color='red', label='Temperature (°C)')
    ax_temp2.set_ylabel('Temperature (°C)', color='red')
    ax_temp2.tick_params(axis='y', labelcolor='red')

    # Criando um terceiro eixo Y para temperature_3 no lado direito, deslocado
    ax_temp3 = ax_temp.twinx()
    ax_temp3.spines['right'].set_position(('outward', 60))  # Ajusta a posição do eixo Y direito
    ax_temp3.plot(timestamps, temperatures3, marker='o', color='green', label='Mold Temperature (°C)')
    ax_temp3.set_ylabel('Mold Temperature (°C)', color='green')
    ax_temp3.tick_params(axis='y', labelcolor='green')

    # Configuração dos eixos X
    ax_temp.set_xlabel('Timestamp')
    ax_temp.set_xticks(timestamps)
    ax_temp.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
    plt.xticks(rotation=45, ha='right')

    # Título e legenda
    ax_temp.set_title('Real-time Temperature and Pressure Data')
    ax_temp.text(0.5, 0.95, f'Production Order - {production_order}', transform=ax_temp.transAxes, fontsize=10,
                 verticalalignment='bottom', horizontalalignment='center', color='black')

    # Ajustando layout
    plt.subplots_adjust(bottom=0.25)

  # Força o ajuste do layout
    plt.tight_layout()




# Função que chama a visualização de dados em tempo real
def show_realtime_data():
    global fig_temp, ax_temp
    fig_temp, ax_temp = plt.subplots()
    ani_temp = animation.FuncAnimation(fig_temp, update_graph, interval=1000)
    plt.show()

# Função para obter os dados de temperatura para a data e hora selecionados
def fetch_temperature_for_selected_datetime(selected_datetime):
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
        WHERE injection_timestamp = %s;
        """
        cursor.execute(query, (selected_datetime,))
        rows = cursor.fetchall()

        # Fechar a conexão
        cursor.close()
        conn.close()

        # Se não houver dados
        if not rows:
            return [], []

        # Extrair dados
        timestamps = [row[0] for row in rows]
        temperatures = [float(row[1]) for row in rows]

        return timestamps, temperatures
    except Exception as e:
        print(f"Erro ao conectar com a base de dados: {e}")
        return [], []

# Função para exibir o histórico de dados
def show_history_data():
    history_window = tk.Toplevel()
    history_window.title("Histórico de Dados")
    history_window.configure(bg="white")
    history_window.geometry("600x500")
    
    # Título
    title_label = tk.Label(history_window, text="Escolha o Mês, Semana, Dia e Hora", font=("Arial", 14), fg="black", bg="white")
    title_label.pack(pady=20)

    # Escolha de Mês
    month_label = tk.Label(history_window, text="Mês:", font=("Arial", 12), bg="white")
    month_label.pack(pady=5)
    months = ["Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"]
    month_var = tk.StringVar()
    month_menu = tk.OptionMenu(history_window, month_var, *months)
    month_menu.pack(pady=5)

    # Escolha de Semana
    week_label = tk.Label(history_window, text="Semana:", font=("Arial", 12), bg="white")
    week_label.pack(pady=5)
    weeks = ["Semana 1", "Semana 2", "Semana 3", "Semana 4"]
    week_var = tk.StringVar()
    week_menu = tk.OptionMenu(history_window, week_var, *weeks)
    week_menu.pack(pady=5)

    # Escolha de Dia
    day_label = tk.Label(history_window, text="Dia:", font=("Arial", 12), bg="white")
    day_label.pack(pady=5)
    days = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"]
    day_var = tk.StringVar()
    day_menu = tk.OptionMenu(history_window, day_var, *days)
    day_menu.pack(pady=5)

    # Escolha de Hora
    hour_label = tk.Label(history_window, text="Hora:", font=("Arial", 12), bg="white")
    hour_label.pack(pady=5)
    hours = [str(i) for i in range(24)]
    hour_var = tk.StringVar()
    hour_menu = tk.OptionMenu(history_window, hour_var, *hours)
    hour_menu.pack(pady=5)

    # Função para gerar o gráfico com os dados selecionados
    def show_selected_day_data():
        selected_month = month_var.get()
        selected_week = week_var.get()
        selected_day = day_var.get()
        selected_hour = hour_var.get()

        if not selected_month or not selected_week or not selected_day or not selected_hour:
            messagebox.showinfo("Erro", "Por favor, selecione todos os campos.")
            return

        selected_date = f"2024-{months.index(selected_month) + 1:02d}-{selected_day.zfill(2)}"
        selected_datetime = f"{selected_date} {selected_hour}:00:00"

        # Função para buscar os dados de temperatura para o dia e hora selecionados
        timestamps, temperatures = fetch_temperature_for_selected_datetime(selected_datetime)

        if timestamps:
            # Gerar o gráfico com os dados encontrados
            fig, ax = plt.subplots()
            ax.plot(timestamps, temperatures, marker='o', label='Temperature (°C)', color='red')
            ax.set_xlabel('Timestamp')
            ax.set_ylabel('Temperature (°C)', color='red')
            ax.set_title(f'Temperature Data for {selected_datetime}')

            plt.xticks(rotation=45, ha='right')
            plt.tight_layout()
            plt.show()
        else:
            messagebox.showinfo("Erro", "Nenhum dado encontrado para o período selecionado.")

    # Botão para gerar o gráfico
    generate_button = tk.Button(history_window, text="Gerar Gráfico", command=show_selected_day_data)
    generate_button.pack(pady=20)

# Função principal para interface com o usuário
def main():
    root = tk.Tk()
    root.title("Autonomous mold WISEWARE")
    root.configure(bg="white")
    root.geometry("600x400")
    
    # Carregar a imagem do projeto
    project_image = Image.open("wiseware_solutions_logo.jpeg")  # Caminho da imagem
    project_image = project_image.resize((200, 100))  # Redimensionar a imagem
    project_image_tk = ImageTk.PhotoImage(project_image)

    # Exibir a imagem na interface
    image_label = tk.Label(root, image=project_image_tk, bg="white")
    image_label.image = project_image_tk
    image_label.pack(pady=10)

    # Título do layout: "Autonomous Mold Data Sensors"
    title_label = tk.Label(root, text="Autonomous Mold Data Sensors", font=("Arial", 16, "bold"), fg="black", bg="white")
    title_label.pack(pady=20)

    # Subtítulo: "Visualizador de Dados em Tempo Real"
   # subtitle_label = tk.Label(root, text="Visualizador de Dados em Tempo Real", font=("Arial", 16), fg="black", bg="white")
   # subtitle_label.pack(pady=10)

    # Botão para visualizar dados em tempo real
    realtime_button = tk.Button(root, text="Ver Dados em Tempo Real", command=show_realtime_data, font=("Arial", 12))
    realtime_button.pack(pady=10)

    # Botão para consultar histórico de dados
    history_button = tk.Button(root, text="Ver Histórico de Dados", command=show_history_data, font=("Arial", 12))
    history_button.pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    main()
