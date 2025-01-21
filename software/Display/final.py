from datetime import datetime, timedelta
import tkinter as tk
from tkinter import ttk, messagebox
from PIL import Image, ImageTk  
import psycopg2
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates
from datetime import datetime

# Configurações da base de dados
DB_HOST = "172.20.10.5"
DB_NAME = "test"
DB_USER = "postgres"
DB_PASSWORD = "nova_senha"

# Função para obter os últimos 6 dados da db
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

# Função para atualizar o gráfico em tempo real
def update_graph(i, fig, ax_temp): 
    timestamps, temperatures1, temperatures3, pressures3, production_order = get_data_from_db()

    if not timestamps:
        print("Aviso: Nenhum dado foi encontrado.")
        return

    ax_temp.clear()

    # pressure_3 no eixo Y esquerdo
    ax_temp.plot(timestamps, pressures3, marker='o', color='blue', label='Pressure (bar)')
    ax_temp.set_ylabel('Pressure (bar)', color='blue')
    ax_temp.tick_params(axis='y', labelcolor='blue')

    # segundo eixo Y para temperature_1 no lado direito
    ax_temp2 = ax_temp.twinx()
    ax_temp2.plot(timestamps, temperatures1, marker='o', color='red', label='Outside Mold Temperature (°C)')
    ax_temp2.set_ylabel('Outside Mold Temperature (°C)', color='red')
    ax_temp2.tick_params(axis='y', labelcolor='red')

    # terceiro eixo Y para temperature_3 no lado direito, deslocado
    ax_temp3 = ax_temp.twinx()
    ax_temp3.spines['right'].set_position(('outward', 60))  # Ajusta a posição do eixo Y direito
    ax_temp3.plot(timestamps, temperatures3, marker='o', color='green', label='Inside Mold Temperature (°C)')
    ax_temp3.set_ylabel('Inside Mold Temperature (°C)', color='green')
    ax_temp3.tick_params(axis='y', labelcolor='green')

    # Configuração do eixo x
    ax_temp.set_xlabel('Timestamp')
    ax_temp.set_xticks(timestamps)
    ax_temp.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))

    # Ajuste da rotação dos ticks e do espaçamento
    plt.xticks(rotation=45, ha='right', rotation_mode='anchor')  # Melhorando a rotação dos timestamps
    plt.subplots_adjust(bottom=0.25)  # Aumentando o espaço inferior para os ticks

    # Título e legenda
    ax_temp.set_title('Real-time Temperature and Pressure Data')
    ax_temp.text(0.5, 0.95, f'Production Order - {production_order}', transform=ax_temp.transAxes, fontsize=10,
                 verticalalignment='bottom', horizontalalignment='center', color='black')

    
    plt.tight_layout()  # Ajuste de layout para evitar sobreposição de texto
    fig.autofmt_xdate()  # Ajustar automaticamente os rótulos das datas


# Função que chama a visualização de dados em tempo real
def show_realtime_data():
    global fig_temp, ax_temp
    fig_temp, ax_temp = plt.subplots()
    ani_temp = animation.FuncAnimation(fig_temp, update_graph, fargs=(fig_temp, ax_temp), interval=1000)  # Passando 'fig' e 'ax_temp'
    plt.show()



# Função para obter os dados de temperatura e pressão para o intervalo especificado
def fetch_data_for_selected_datetime(selected_datetime):
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
            production_order,
            sensors_data->'samples'->0->>'temperature_1' AS temperature1,
            sensors_data->'samples'->0->>'pressure_1' AS pressure1
        FROM inovam_injection
        WHERE injection_timestamp >= %s
        ORDER BY injection_timestamp ASC
        LIMIT 6;
        """
        cursor.execute(query, (selected_datetime,))
        rows = cursor.fetchall()

        cursor.close()
        conn.close()

        if not rows:
            return [], [], [], []

        # Obter production_orders únicos
        production_orders = list({row[0] for row in rows if row[0]})

        # Gerar timestamps em intervalos de 10 segundos
        start_time = datetime.strptime(selected_datetime, '%Y-%m-%d %H:%M:%S')
        timestamps = [start_time + timedelta(seconds=i * 10) for i in range(len(rows))]

        # Garantir que as leituras sejam consistentes
        temperatures = [float(row[1]) if row[1] else 0.0 for row in rows]
        pressures = [abs(float(row[2])) if row[2] else 0.0 for row in rows]

        return timestamps, temperatures, pressures, production_orders
    except Exception as e:
        print(f"Erro ao conectar com a base de dados: {e}")
        return [], [], [], []




def show_selected_data(month_var, day_var, hour_var, minute_var):
    selected_month = month_var.get()
    selected_day = day_var.get()
    selected_hour = hour_var.get()
    selected_minute = minute_var.get()

    if not all([selected_month, selected_day, selected_hour, selected_minute]):
        messagebox.showinfo("Erro", "Por favor, selecione todos os campos.")
        return

    try:
        months = ["Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
                  "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"]
        month_number = months.index(selected_month) + 1

        selected_date = f"2024-{month_number:02d}-{selected_day.zfill(2)}"
        selected_datetime = f"{selected_date} {selected_hour.zfill(2)}:{selected_minute.zfill(2)}:00"

        # Obter dados e production_orders
        timestamps, temperatures, pressures, production_orders = fetch_data_for_selected_datetime(selected_datetime)

        if timestamps:
            fig, ax1 = plt.subplots()

            ax1.plot(timestamps, temperatures, 'r-o', label='Temperature (°C)')
            ax1.set_xlabel('Timestamp')
            ax1.set_ylabel('Temperature (°C)', color='red')

            ax1.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
            fig.autofmt_xdate(rotation=45)

            ax2 = ax1.twinx()
            ax2.plot(timestamps, pressures, 'b-o', label='Pressure (Bar)')
            ax2.set_ylabel('Pressure (Bar)', color='blue')

            # Título atualizado com todas as production_orders
            production_orders_str = ", ".join(production_orders)
            plt.title(f'Temperature and Pressure Data\nProduction Orders: {production_orders_str}\n{selected_datetime}')

            plt.tight_layout()
            plt.show()
        else:
            messagebox.showinfo("Erro", "Nenhum dado encontrado para o período selecionado.")
    except Exception as e:
        messagebox.showinfo("Erro", f"Ocorreu um erro: {e}")



import tkinter as tk

def show_history_data():
    history_window = tk.Toplevel()
    history_window.title("Histórico de Dados")
    history_window.configure(bg="white")
    history_window.geometry("800x600")

    # Título
    title_label = tk.Label(
        history_window,
        text="Escolha o Mês, Dia, Hora e Minuto",
        font=("Arial", 18, "bold"),
        fg="black",
        bg="white",
    )
    title_label.grid(row=0, column=0, columnspan=2, padx=20, pady=20)

    # Estilo geral
    menu_font = ("Arial", 14)

    # Função para selecionar valor de uma lista
    def select_from_list(var, listbox):
        selection = listbox.curselection()
        if selection:
            var.set(listbox.get(selection[0]))

    # Criar uma lista com barra de rolagem
    def create_scrollable_list(frame, options, var):
        list_frame = tk.Frame(frame, bg="white")
        scrollbar = tk.Scrollbar(list_frame, orient="vertical")
        listbox = tk.Listbox(
            list_frame,
            font=menu_font,
            selectmode="single",
            yscrollcommand=scrollbar.set,
            height=5,  # Número de itens visíveis por vez
            exportselection=False,
        )
        scrollbar.config(command=listbox.yview)
        scrollbar.pack(side="right", fill="y")
        listbox.pack(side="left", fill="both", expand=True)

        for option in options:
            listbox.insert("end", option)

        listbox.bind(
            "<ButtonRelease-1>",
            lambda e: select_from_list(var, listbox),
        )

        return list_frame

    # Mês (Lado esquerdo)
    month_label = tk.Label(history_window, text="Mês:", font=menu_font, bg="white")
    month_label.grid(row=1, column=0, padx=10, pady=10, sticky="e")
    months = [
        "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho",
        "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"
    ]
    month_var = tk.StringVar()
    month_list = create_scrollable_list(history_window, months, month_var)
    month_list.grid(row=1, column=1, padx=10, pady=10, sticky="w")

    # Dia (Lado esquerdo)
    day_label = tk.Label(history_window, text="Dia:", font=menu_font, bg="white")
    day_label.grid(row=2, column=0, padx=10, pady=10, sticky="e")
    days = [f"{i:02d}" for i in range(1, 32)]
    day_var = tk.StringVar()
    day_list = create_scrollable_list(history_window, days, day_var)
    day_list.grid(row=2, column=1, padx=10, pady=10, sticky="w")

    # Hora (Lado direito)
    hour_label = tk.Label(history_window, text="Hora:", font=menu_font, bg="white")
    hour_label.grid(row=1, column=2, padx=10, pady=10, sticky="e")
    hours = [f"{i:02d}" for i in range(24)]
    hour_var = tk.StringVar()
    hour_list = create_scrollable_list(history_window, hours, hour_var)
    hour_list.grid(row=1, column=3, padx=10, pady=10, sticky="w")

    # Minuto (Lado direito)
    minute_label = tk.Label(history_window, text="Minuto:", font=menu_font, bg="white")
    minute_label.grid(row=2, column=2, padx=10, pady=10, sticky="e")
    minutes = [f"{i:02d}" for i in range(60)]
    minute_var = tk.StringVar()
    minute_list = create_scrollable_list(history_window, minutes, minute_var)
    minute_list.grid(row=2, column=3, padx=10, pady=10, sticky="w")

    # Botão Gerar Gráfico
    generate_button = tk.Button(
        history_window,
        text="Gerar Gráfico",
        font=("Arial", 16),
        command=lambda: show_selected_data(month_var, day_var, hour_var, minute_var),
        bg="#4CAF50",
        fg="white",
        width=15,
        height=2,
    )
    generate_button.grid(row=3, column=0, columnspan=4, padx=20, pady=20)





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
